#include "Signal.h"
#include "Picture.h"
#include "ObjectDetection.h"

void TestDetection()
{
    std::cout << "Test Detection!" << std::endl;

    // Get Image
    std::cout << "Get image! " << std::endl;

    uint32_t height   = 0;
    uint32_t width    = 0;
    uint8_t* cartoon  = nullptr;
    uint8_t* cartData = nullptr;
    uint8_t* object   = nullptr;

    g_loadImage("../Pic/cartoon.bmp", width, height, &cartoon);

    cartData = new uint8_t[height * width];

    g_toGrayScaleIn(width, height, cartoon, cartData);

    std::cout << "Height: " << height << ", width: " << width << std::endl;

    // Get Det Obj
    std::cout << "Type x, y, h, w for object: " << std::endl;
    uint32_t x, y, h, w;
    std::cin >> x >> y >> h >> w;

    DetObj::Object<uint8_t> obj(w, h);
    DetObj::Object<uint8_t> pic;

    pic.m_delete = false;
    pic.m_data   = cartData;
    pic.m_height = height;
    pic.m_width  = width;

    DetObj::GetDetectingObject(obj, pic, x, y);

    // Safe det Obj
    std::cout << "Safe Detect object!" << std::endl;
    
    object = new uint8_t[h * w * 3];

    g_toGrayScaleOut(w, h, obj.m_data, object);
    g_safeImage("obj.bmp", w, h, object);

    // Noise picture
    std::cout << "Type snr: " << std::endl;
    double snr;
    std::cin >> snr;
    
    Signal* noiseSig = new Signal(width, height);

    std::complex<double>** noiseData = noiseSig->GetDataArray();

    for (uint32_t i = 0; i < height; ++i)
        for (uint32_t j = 0; j < width; ++j)
            noiseData[i][j] = {cartData[i * width + j], 0}; 

    g_noizeSignal(*noiseSig, snr);
    noiseSig->GetPicture(cartData, false);

    g_toGrayScaleOut(width, height, cartData, cartoon);
    g_safeImage(std::string("NoizeSignal.bmp"), width, height, cartoon);

    // Find object on picture
    std::cout << "Find object on picture!" << std::endl;

    std::cout << "Type coeff for detecting!" << std::endl;
    double k;
    std::cin >> k;

    uint32_t x0, y0;

    DetObj::DetectObject(obj, pic, k, x0, y0);
    std::cout << "x0: " << x0 << ", y0: " << y0 << std::endl; 
    DetObj::PaintDetectingObject(obj, pic, x0, y0);

    // Save picture
    std::cout << "save picture!" << std::endl;

    g_toGrayScaleOut(width, height, pic.m_data, cartoon);
    g_safeImage("detected.bmp", width, height, cartoon);

    delete[] cartoon;
    delete[] cartData;
    delete[] object;

    delete noiseSig;
}

int main()
{
    TestDetection();

    return 0;
}