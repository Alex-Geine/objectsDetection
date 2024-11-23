#include "Signal.h"
#include "Picture.h"
#include "ObjectDetection.h"

const static double cos45   = 0.5;
const static double cos45_n = 0.5;
const static double sin45   = 0.5;
const static double sin45_n = -0.5;

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

    g_loadImage("../Pic/corgi.bmp", width, height, &cartoon);

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

    std::vector<double> err = DetObj::DetectObject(obj, pic, k, x0, y0);
    std::cout << "x0: " << x0 << ", y0: " << y0 << std::endl; 
    DetObj::PaintDetectingObject(obj, pic, x0, y0);

    // Save picture
    std::cout << "save picture!" << std::endl;

    g_toGrayScaleOut(width, height, pic.m_data, cartoon);
    g_safeImage("detected.bmp", width, height, cartoon);

    // Save error picture
    std::cout << "Error picture! " << std::endl;
    
    int errWidth = pic.m_width - obj.m_width;
    int errHeight = pic.m_height - obj.m_height;
    Signal sig(errWidth, errHeight);
 
    std::cout << "SizeErr: " << errWidth * errHeight << ", sizeErrVec: " << err.size() << std::endl;

    noiseData = sig.GetDataArray();
    for (int i = 0; i < errHeight; ++i)
        for (int j = 0; j < errWidth; ++j)
            noiseData[i][j] = {err.at(i * errWidth + j), 0};

    uint8_t* errData = new uint8_t[errWidth * errHeight];
    uint8_t* errDataPic = new uint8_t[errWidth * errHeight * 3];

    sig.GetPicture(errData, true);

    g_toGrayScaleOut(errWidth, errHeight, errData, errDataPic);
    g_safeImage("error.bmp", errWidth, errHeight, errDataPic);

    delete[] cartoon;
    delete[] cartData;
    delete[] object;
    delete[] errData;
    delete[] errDataPic;

    delete noiseSig;
}

void TestDetectionAndRotation()
{
    std::cout << "Test Detection!" << std::endl;

    // Get Image
    std::cout << "Get image! " << std::endl;

    uint32_t height   = 0;
    uint32_t width    = 0;
    uint8_t* cartoon  = nullptr;
    uint8_t* cartData = nullptr;
    uint8_t* object   = nullptr;

    g_loadImage("../Pic/corgi.bmp", width, height, &cartoon);

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

    std::vector<double> err = DetObj::DetectObject(obj, pic, k, x0, y0);
    std::cout << "x0: " << x0 << ", y0: " << y0 << std::endl; 
    DetObj::PaintDetectingObject(obj, pic, x0, y0);

    // Save picture
    std::cout << "save picture!" << std::endl;

    g_toGrayScaleOut(width, height, pic.m_data, cartoon);
    g_safeImage("detected.bmp", width, height, cartoon);

    // Save error picture
    std::cout << "Error picture! " << std::endl;
    
    int errWidth = pic.m_width - obj.m_width;
    int errHeight = pic.m_height - obj.m_height;
    Signal sig(errWidth, errHeight);
 
    std::cout << "SizeErr: " << errWidth * errHeight << ", sizeErrVec: " << err.size() << std::endl;

    noiseData = sig.GetDataArray();
    for (int i = 0; i < errHeight; ++i)
        for (int j = 0; j < errWidth; ++j)
            noiseData[i][j] = {err.at(i * errWidth + j), 0};

    uint8_t* errData = new uint8_t[errWidth * errHeight];
    uint8_t* errDataPic = new uint8_t[errWidth * errHeight * 3];

    sig.GetPicture(errData, true);

    g_toGrayScaleOut(errWidth, errHeight, errData, errDataPic);
    g_safeImage("error.bmp", errWidth, errHeight, errDataPic);

    delete[] cartoon;
    delete[] cartData;
    delete[] object;
    delete[] errData;
    delete[] errDataPic;

    delete noiseSig;
}

template<typename T>
void Rotate(uint8_t* data_in, T h, T w, uint8_t** data_out, T& s_out, bool isPositive)
{
    std::cout << "Rotate!" << std::endl;
    s_out = std::ceil((double)sqrt( (double)h * (double)h + (double)w * (double)w));
    std::cout << "Size: " << s_out << std::endl;
    *data_out = new uint8_t[s_out * s_out];
    int i1, j1;

    int c_k = s_out / 2;
    int c_h = h / 2;
    int c_w = w / 2;

    std::cout << "Start! " << std::endl;
    for (int i = 0; i < h; ++i)
    {
        //std::cout << "i: " << i << std::endl;
        for (int j = 0; j < w; ++j)
        {
            //std::cout << "j: " << j << std::endl;
            if (isPositive)
            {
                // translate and rotate
                i1 = (i - c_h)  - (j - c_w);
                j1 = (i - c_h)  + (j - c_w);

                i1 *= 0.5;
                j1 *= 0.5;
                
                // inverce translate
                i1 += c_k;
                j1 += c_k;
                //std::cout << i1 << " " << j1 << " ";    
            }
            else
            {
                i1 = i * 0.5 - j * (-0.5);
                j1 = i * (-0.5) + j * 0.5;
            }
            if ((i1 >= s_out) || (j1 >= s_out) || (i1 < 0) || (j1 < 0))
                std::cout << "Error: " << i1 << " " << j1 << ". i: " << i << ", j: " << j << std::endl;
            (*data_out)[i1 * s_out + j1] = data_in[i * w + j];
        }
        //std::cout << std::endl;
    }

    return;
}

void TestRotation()
{
    std::cout << "Test Rotation!" << std::endl;

    // Get Image
    std::cout << "Get image! " << std::endl;

    uint32_t height   = 0;
    uint32_t width    = 0;
    uint8_t* cartoon  = nullptr;
    uint8_t* cartData = nullptr;
    uint8_t* cartoonRotate = nullptr;
    uint8_t* cartDataRotate = nullptr; 
    uint8_t* object   = nullptr;

    g_loadImage("../Pic/corgi.bmp", width, height, &cartoon);

    cartData = new uint8_t[height * width];

    g_toGrayScaleIn(width, height, cartoon, cartData);

    uint32_t n_size;

    Rotate(cartData, height, width, &cartDataRotate, n_size, true);

    for (int i = 0; i < n_size * n_size; ++i)
        std::cout << cartDataRotate[i] << " ";
    std::cout << std::endl;

    cartoonRotate = new uint8_t[n_size * n_size * 3];

    g_toGrayScaleOut(n_size, n_size, cartDataRotate, cartoonRotate);
    std::cout << "Gray scale!" << std::endl;
    g_safeImage("Rotate.bmp", n_size, n_size, cartoonRotate);
    std::cout << "Save image!" << std::endl;

    delete[] cartoon;
    delete[] cartData;
    delete[] cartoonRotate;
    delete[] cartDataRotate;
}

int main()
{
    //TestDetection();
    TestRotation();
    return 0;
}