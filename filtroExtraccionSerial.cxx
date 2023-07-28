//
// Created by anfto on 26/07/23.
//

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

int main(int argc, char * argv[]) {

    if (argc <= 3)
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << "<input3DImageFile> <output2DImageDirectory>" << std::endl;
        std::cerr << "<zDimensionSize>" << std::endl;
        return EXIT_FAILURE;
    }

    typedef int PixelType;
    const unsigned char InputDimension = 3;
    const unsigned char OutputDimension = 2;

    typedef itk::Image<PixelType, InputDimension> InputImageType;
    typedef itk::Image<PixelType, OutputDimension> OutputImageType;

    // Here we recover the file names from the command line arguments
    const char *inputFileName = argv[1];
    const std::string outputFilePath = argv[2];

    InputImageType::Pointer inputImage;
    try
    {
        inputImage = itk::ReadImage<InputImageType>(inputFileName);
    }
    catch (const itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    typedef itk::ExtractImageFilter<InputImageType, OutputImageType> ExtractFilterType;
    auto extractFilter = ExtractFilterType::New();
    extractFilter->SetDirectionCollapseToSubmatrix();
    extractFilter->SetInput(inputImage);

    InputImageType::RegionType inputRegion = inputImage->GetBufferedRegion(); // set up the extraction region [one slice]

    InputImageType::SizeType size = inputRegion.GetSize();

    const unsigned int zDim = size[2]; // we are getting the dimension of
    const unsigned int zSize = std::stoi(argv[3]); // this value should be always 0

    size[2] = zSize; // we extract along z direction

    InputImageType::IndexType start = inputRegion.GetIndex();

    InputImageType::RegionType desiredRegion;
    desiredRegion.SetSize(size);

    typedef itk::Image<unsigned char, OutputDimension> RescaledImageType;
    typedef itk::RescaleIntensityImageFilter<OutputImageType, RescaledImageType> RescalerType;
    auto rescaler = RescalerType::New();

    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);

    typedef itk::ThresholdImageFilter<OutputImageType> ThresholdImageFilterType;
    auto thresholdFilter_below = ThresholdImageFilterType::New();
    auto thresholdFilter_above = ThresholdImageFilterType::New();

    const OutputImageType::PixelType lowerThreshold = -1024;
    const OutputImageType::PixelType upperThreshold = 1024;

    thresholdFilter_below->ThresholdBelow(lowerThreshold);
    thresholdFilter_below->SetOutsideValue(lowerThreshold);

    thresholdFilter_above->ThresholdAbove(upperThreshold);
    thresholdFilter_above->SetOutsideValue(upperThreshold);

    std::string outputFileName;

    for(int i = 0; i < zDim; i++){

        start[2] = i;

        desiredRegion.SetIndex(start);

        extractFilter->SetExtractionRegion(desiredRegion);

        thresholdFilter_below->SetInput(extractFilter->GetOutput());
        thresholdFilter_above->SetInput(thresholdFilter_below->GetOutput());
        rescaler->SetInput(thresholdFilter_above->GetOutput());

        std::stringstream stream;
        stream << i;

        std::string str;
        stream >> str;

        outputFileName.append(outputFilePath).append("axial_cut_").append(str).append(".png");

        try
        {
            itk::WriteImage(rescaler->GetOutput(), outputFileName);
            outputFileName.clear();
        }
        catch (const itk::ExceptionObject & err)
        {
            std::cerr << "ExceptionObject caught !" << std::endl;
            std::cerr << err << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
