#ifndef PICTURE_H
#define PICTURE_H

#include <Magick++.h> 
#include <iostream>
#include <cstdint>
#include <cstring>

using namespace Magick;

void g_initMagick(char **argv)
{
    InitializeMagick(*argv);
};

// Save image
bool g_safeImage(std::string name, uint32_t width, uint32_t height, uint8_t* data)
{
    Image image;

    image.read(width, height, "BRG", CharPixel, data);
    image.write(name.data());

    return true;
};

bool g_toGrayScaleIn(uint32_t width, uint32_t height, uint8_t* data, uint8_t* outData)
{
    if (data == nullptr || width == 0 || height == 0) return false;

    uint64_t counter = 0;
    uint64_t size = width * height;
    double sum = 0;
    for (uint32_t i = 0; i < size; ++i)
    {
        sum = 0;
        sum = (double)data[counter] * 0.299D + (double)data[counter + 1] * 0.587D + (double)data[counter + 2] * 0.114D;
        outData[i] = sum;

        counter += 3;
    }

    return true;
};

bool g_toGrayScaleOut(uint32_t width, uint32_t height, uint8_t* data, uint8_t* outData)
{
    if (data == nullptr || width == 0 || height == 0) return false;

    uint64_t counter = 0;

    for (uint32_t i = 0; i < width * height; ++i)
    {
        outData[counter]   = uint8_t((double)data[i]);
        outData[counter+1] = uint8_t((double)data[i]);
        outData[counter+2] = uint8_t((double)data[i]);
        counter += 3;
    }

    return true;
};

// Load image
bool g_loadImage(std::string name, uint32_t& width, uint32_t& height, uint8_t** data)
{
    Image image;
    image.read(name.data());

    width  = image.columns();
    height = image.rows();

    char* pic = new char[width * height * 3];

    image.write(0, 0, image.columns(), image.rows(), "GRB", CharPixel, pic);

    std::cout << "Width: " << width << ", height: " << height << ", data: " << pic << std::endl;
    *data = (uint8_t*)pic;

    return true;
};

#endif    // PICTURE_H
