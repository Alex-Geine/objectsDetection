#include "Signal.h"
#include "Picture.h"
#include "ObjectDetection.h"

template<typename T>
std::vector<std::pair<int, int>> Rotate(uint8_t* data_in, T h, T w, uint8_t** data_out, T& s_out, bool isPositive, int& i_ref, int& j_ref)
{
    std::vector<std::pair<int, int>> res;
    std::cout << "Rotate!" << std::endl;
    s_out = (h + w) * 0.5;//diag * cos(betta);

    std::cout << "Size: " << s_out << std::endl;
    *data_out = new uint8_t[s_out * s_out];
    int i1, j1;

    int c_h = h / 2;
    int c_w = w / 2;
    int c_k = s_out / 2;

    std::cout << "Start! " << std::endl;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            if (isPositive)
            {
                i1 = (i - c_h) - (j - c_w);
                j1 = (i - c_h) + (j - c_w);

                i1 *= 0.5;
                j1 *= 0.5;
                
                // inverce translate
                i1 += c_k;
                j1 += c_k;
            }
            else
            {
                i1 = (i - c_h) + (j - c_w);
                j1 = -(i - c_h) + (j - c_w);

                i1 *= 0.5;
                j1 *= 0.5;

                // inverce translate
                i1 += c_k;
                j1 += c_k;
            }
            (*data_out)[i1 * s_out + j1] = data_in[i * w + j];
            res.push_back({i1, j1});
            if ((i == i_ref) && (j == j_ref))
            {
               i_ref = i1;
               j_ref = j1;
            }
        }
    }

    return res;
}

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

    double min;

    std::vector<double> err = DetObj::DetectObject(obj, pic, k, x0, y0, min);
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
    uint8_t* cartoonRotate = nullptr;
    uint8_t* cartDataRotate = nullptr;
    uint8_t* object   = nullptr;
    uint8_t* objectRotateOne = nullptr;
    uint8_t* objectRotateTwo = nullptr;

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

    // Reverce data and obj
    std::cout << "Rotate data: " << std::endl;
    uint32_t picRotateSize;
    uint32_t objRotateSize;

    int i = 0,j = 0,i_ref = y, j_ref = x;
    Rotate(cartData, height, width, &cartDataRotate, picRotateSize, true, i_ref, j_ref);
    std::vector<std::pair<int, int>> objRotOneIds = Rotate(obj.m_data, obj.m_height, obj.m_width, &objectRotateOne, objRotateSize, true,i,j);
    std::vector<std::pair<int, int>> objRotTwoIds = Rotate(obj.m_data, obj.m_height, obj.m_width, &objectRotateTwo, objRotateSize, false,i,j);

    cartoonRotate = new uint8_t[picRotateSize * picRotateSize * 3];

    g_toGrayScaleOut(picRotateSize, picRotateSize, cartDataRotate, cartoonRotate);
    g_safeImage("RotatePicture.bmp", picRotateSize, picRotateSize, cartoonRotate);

    // Get new objects
    DetObj::Object<uint8_t> objOne;
    DetObj::Object<uint8_t> objTwo;
    DetObj::Object<uint8_t> picNew;

    picNew.m_delete = false;
    picNew.m_data   = cartDataRotate;
    picNew.m_height = picRotateSize;
    picNew.m_width  = picRotateSize;

    objOne.m_delete = false;
    objOne.m_data   = objectRotateOne;
    objOne.m_height = objRotateSize;
    objOne.m_width  = objRotateSize;
    objOne.m_idsOfObj = objRotOneIds;

    objTwo.m_delete = false;
    objTwo.m_data   = objectRotateTwo;
    objTwo.m_height = objRotateSize;
    objTwo.m_width  = objRotateSize;
    objTwo.m_idsOfObj = objRotTwoIds;

    uint8_t* objRotPic = new uint8_t[objRotateSize * objRotateSize * 3];
    
    g_toGrayScaleOut(objRotateSize, objRotateSize, objOne.m_data, objRotPic);
    g_safeImage("objRotate.bmp", objRotateSize, objRotateSize, objRotPic);

    g_toGrayScaleOut(objRotateSize, objRotateSize, objTwo.m_data, objRotPic);
    g_safeImage("objRotateTwo.bmp", objRotateSize, objRotateSize, objRotPic);

    // Safe det Obj
    std::cout << "Safe Detect object!" << std::endl;
    
    object = new uint8_t[h * w * 3];

    g_toGrayScaleOut(w, h, obj.m_data, object);
    g_safeImage("obj.bmp", w, h, object);

    // Noise picture
    std::cout << "Type snr: " << std::endl;
    double snr;
    std::cin >> snr;
    
    Signal* noiseSig = new Signal(picRotateSize, picRotateSize);

    std::complex<double>** noiseData = noiseSig->GetDataArray();

    for (uint32_t i = 0; i < picRotateSize; ++i)
        for (uint32_t j = 0; j < picRotateSize; ++j)
            noiseData[i][j] = {cartDataRotate[i * picRotateSize + j], 0}; 

    g_noizeSignal(*noiseSig, snr);
    noiseSig->GetPicture(cartDataRotate, false);

    g_toGrayScaleOut(picRotateSize, picRotateSize, cartDataRotate, cartoonRotate);
    g_safeImage(std::string("NoizeSignal.bmp"), picRotateSize, picRotateSize, cartoonRotate);

    // Find object on picture
    std::cout << "Find object on picture!" << std::endl;

    std::cout << "Type coeff for detecting!" << std::endl;
    double k;
    std::cin >> k;

    uint32_t x_one, y_one, x_two, y_two;
    double min1, min2;

    std::vector<double> err  = DetObj::DetectObject(objOne, picNew, k, x_one, y_one, min1);
    std::vector<double> err2 = DetObj::DetectObject(objTwo, picNew, k, x_two, y_two, min2);

    std::cout << "x_one: " << x_one << ", y_one: " << y_one << std::endl; 
    std::cout << "x_two: " << x_two << ", y_two: " << y_two << std::endl;
    std::cout << "x_ref: " << j_ref << ", y_ref: " << i_ref << std::endl;
    std::cout << "min1: " << min1 << ", min2: " << min2 << std::endl;

    if (min1 < min2)
        DetObj::PaintDetectingObject(objOne, picNew, x_one, y_one);
    else
        DetObj::PaintDetectingObject(objOne, picNew, x_two, y_two);

    // Save picture
    std::cout << "save picture!" << std::endl;

    g_toGrayScaleOut(picRotateSize, picRotateSize, picNew.m_data, cartoonRotate);
    g_safeImage("detected.bmp", picRotateSize, picRotateSize, cartoonRotate);

    // Save error picture
    std::cout << "Error picture! " << std::endl;
    
    int errWidth = picNew.m_width - objOne.m_width;
    int errHeight = picNew.m_height - objOne.m_height;
    Signal sig(errWidth, errHeight);
 
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
    delete[] cartoonRotate;
    delete[] cartDataRotate;
    delete[] object;
    delete[] objectRotateOne;
    delete[] objectRotateTwo;
    delete[] errData;
    delete[] errDataPic;

    delete noiseSig;
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
    int i = 0,j = 0;
    Rotate(cartData, height, width, &cartDataRotate, n_size, false,i,j);
    cartoonRotate = new uint8_t[n_size * n_size * 3];

    g_toGrayScaleOut(n_size, n_size, cartDataRotate, cartoonRotate);
    g_safeImage("Rotate.bmp", n_size, n_size, cartoonRotate);

    delete[] cartoon;
    delete[] cartData;
    delete[] cartoonRotate;
    delete[] cartDataRotate;
}

int main()
{
    //TestDetection();
    //TestRotation();
    TestDetectionAndRotation();
    return 0;
}