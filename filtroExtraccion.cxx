//
// Created by anfto on 25/07/23.
//

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

int main(int argc, char * argv[]) {
    if (argc <= 4)
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << "<input3DImageFile> <output2DImageFile>" << std::endl;
        std::cerr << "<zDimensionSize> <sliceNumber>" << std::endl;
        return EXIT_FAILURE;
    }

    typedef int PixelType;
    const unsigned char InputDimension = 3;
    const unsigned char OutputDimension = 2;

    typedef itk::Image<PixelType, InputDimension> InputImageType;
    typedef itk::Image<PixelType, OutputDimension> OutputImageType;

    // Here we recover the file names from the command line arguments
    const char *inputFileName = argv[1];
    const char *outputFileName = argv[2];

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

    // set up the extraction region [one slice]
    InputImageType::RegionType inputRegion = inputImage->GetBufferedRegion();

    InputImageType::SizeType size = inputRegion.GetSize();
    const unsigned int zSize = std::stoi(argv[3]); // this value should be always 0
    size[2] = zSize; // we extract along z direction

    InputImageType::IndexType start = inputRegion.GetIndex();
    const unsigned int sliceNumber = std::stoi(argv[4]); // se recibe el indice del que se desea extraer la imagen
    start[2] = sliceNumber;

    InputImageType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    extractFilter->SetExtractionRegion(desiredRegion);
    extractFilter->SetInput(inputImage);

    typedef itk::ThresholdImageFilter<OutputImageType> ThresholdImageFilterType;
    auto thresholdFilter_below = ThresholdImageFilterType::New();
    auto thresholdFilter_above = ThresholdImageFilterType::New();

    const OutputImageType::PixelType lowerThreshold = -1024;
    const OutputImageType::PixelType upperThreshold = 1024;

    thresholdFilter_below->SetInput(extractFilter->GetOutput());
    thresholdFilter_below->ThresholdBelow(lowerThreshold);
    thresholdFilter_below->SetOutsideValue(lowerThreshold);

    thresholdFilter_above->SetInput(thresholdFilter_below->GetOutput());
    thresholdFilter_above->ThresholdAbove(upperThreshold);
    thresholdFilter_above->SetOutsideValue(upperThreshold);

    typedef itk::Image<unsigned char, OutputDimension> RescaledImageType;
    typedef itk::RescaleIntensityImageFilter<OutputImageType, RescaledImageType> RescalerType;
    auto rescaler = RescalerType::New();

    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);

    rescaler->SetInput(thresholdFilter_above->GetOutput());

    try
    {
        itk::WriteImage(rescaler->GetOutput(), outputFileName);
    }
    catch (const itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}