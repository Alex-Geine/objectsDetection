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

    delete[] cartoon;
    delete[] cartData;
    delete[] object;
}

int main()
{
    TestDetection();

    return 0;
}