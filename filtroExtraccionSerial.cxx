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

    const unsigned char InputDimension = 3;
    const unsigned char OutputDimension = 2;

    typedef itk::Image<int, InputDimension> InputImageType;
    typedef itk::Image<unsigned short, OutputDimension> OutputImageType;

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

    typedef itk::ThresholdImageFilter<InputImageType> ThresholdImageFilterType;
    auto thresholdFilter_below = ThresholdImageFilterType::New();
    auto thresholdFilter_above = ThresholdImageFilterType::New();

    const InputImageType::PixelType lowerThreshold = -1024;
    const InputImageType::PixelType upperThreshold = 1024;

    thresholdFilter_below->ThresholdBelow(lowerThreshold);
    thresholdFilter_below->SetOutsideValue(lowerThreshold);

    thresholdFilter_above->ThresholdAbove(upperThreshold);
    thresholdFilter_above->SetOutsideValue(upperThreshold);

    thresholdFilter_below->SetInput(inputImage);
    thresholdFilter_above->SetInput(thresholdFilter_below->GetOutput());

    typedef itk::Image<unsigned short, InputDimension> RescaledImageType;
    typedef itk::RescaleIntensityImageFilter<InputImageType, RescaledImageType> RescalerType;
    auto rescaler = RescalerType::New();

    rescaler->SetOutputMinimum(std::numeric_limits<RescaledImageType::PixelType>::lowest());
    rescaler->SetOutputMaximum(std::numeric_limits<RescaledImageType::PixelType>::max());

    rescaler->SetInput(thresholdFilter_above->GetOutput());

    auto RescaledImage = RescaledImageType::New();
    RescaledImage = rescaler->GetOutput();

//    typedef itk::ImageFileWriter<RescaledImageType> WriterType;
//    WriterType::Pointer writer = WriterType::New();
//    writer->SetFileName("rescaled.mha");
//    writer->SetInput(RescaledImage);
//    writer->Update();

    typedef itk::ExtractImageFilter<RescaledImageType, OutputImageType> ExtractFilterType;
    auto extractFilter = ExtractFilterType::New();
    extractFilter->SetDirectionCollapseToSubmatrix();
    extractFilter->SetInput(RescaledImage);

    RescaledImageType::RegionType inputRegion = RescaledImage->GetBufferedRegion(); // set up the extraction region [one slice]

    RescaledImageType::SizeType size = inputRegion.GetSize();

    std::cout << size[2] << std::endl;

    const unsigned int zDim = size[2]; // we are getting the dimension of
    const unsigned int zSize = std::stoi(argv[3]); // this value should be always 0

    size[2] = zSize; // we extract along z direction

    RescaledImageType::IndexType start = inputRegion.GetIndex();

    RescaledImageType::RegionType desiredRegion;
    desiredRegion.SetSize(size);

    std::string outputFileName;

    for(int i = 0; i < zDim; i++){

        start[2] = i;

        desiredRegion.SetIndex(start);

        extractFilter->SetExtractionRegion(desiredRegion);

        std::stringstream stream;
        stream << i;

        std::string str;
        stream >> str;

        outputFileName.append(outputFilePath).append("axial_cut_").append(str).append(".png");

        try
        {
            itk::WriteImage(extractFilter->GetOutput(), outputFileName);
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
