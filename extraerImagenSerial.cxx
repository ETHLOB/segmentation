//
// Created by anfto on 24/07/23.
//
#include <iostream>
#include <sstream>
#include <bits/stdc++.h>

#include "itkGDCMImageIO.h"
#include "itkImageFileWriter.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"

int main(int argc, char *argv[]) {
    // Verificar que se ingresó la ruta de la carpeta de entrada
    if(argc < 3) {
        std::cerr << "Uso: " << std::endl;
        std::cerr << "<DirectorioDICOM> <RutaMHASalida>" << std::endl;
        return EXIT_FAILURE;
    }

    // Capturamos información de los datos de entrada y salida
    const char * inputDirectory = argv[1];
    const std::string outputPath = argv[2];

    // Definir el tipo de datos de entrada y salida
    typedef int PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image<PixelType, Dimension> ImageType;

    // Definir el lector de series de imágenes DICOM
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetRecursive(true);
    nameGenerator->SetUseSeriesDetails(true);
    nameGenerator->SetLoadSequences(true);
    nameGenerator->SetLoadPrivateTags(true);
    nameGenerator->SetInputDirectory(inputDirectory);

    /* Obtener los nombres de archivo de las imágenes DICOM en la carpeta de entrada y guardar los resultados
    en un vector de ubicaciones */
    const std::vector<std::string> seriesUID = nameGenerator->GetSeriesUIDs();

    /* En esta parte necesito una recomendacion, pues el metodo GetSeriesUID() me extrae toda la
    informacion de la imagen pero la pone en un objeto itreable que no se como extraer en su
    totalidad para luego usar el metodo GetFileNames() */

    /* Para lidiar con este problema, veré si puedo iterar sobre cada elemento de este vector y aplicar la
    trasformación (guardar archivo). Luego de ello veré si se puede hacer sobre cada tipo de imágen */

    const unsigned char length = seriesUID.size();

    // Definir el lector de la serie de imágenes DICOM
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    // Definir el escritor de la imagen MHA
    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();

    for(int i = 0; i < length; i++){

        const std::vector<std::string> fileNames = nameGenerator->GetFileNames(seriesUID[i]);

        reader->SetImageIO(itk::GDCMImageIO::New());
        reader->SetFileNames(fileNames);

        try {
            reader->Update();
        } catch (itk::ExceptionObject & ex) {
            std::cerr << "Error al leer las imágenes DICOM: " << ex << std::endl;
            continue;
        }

        std::stringstream stream;
        stream << i;

        std::string str;
        stream >> str;

        std::string identificador = "_image_.mha";
        const std::string outputFileName = outputPath + str + identificador;
        std::cout << "Se escribirá el archivo en " << outputFileName << std::endl;

        writer->SetFileName(outputFileName);
        writer->SetInput(reader->GetOutput());

        // Escribir la imagen MHA
        try {
            writer->Update();
        } catch (itk::ExceptionObject &ex) {
            std::cerr << "Error al escribir la imagen MHA: " << ex << std::endl;
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
