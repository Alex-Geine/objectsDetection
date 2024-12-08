#include "Signal.h"
#include "Picture.h"
#include "ObjectDetection.h"
#include <fstream>
#include <random>

// Class for algorithm analisys
class Analyser
{
    private: // variables

    // Data
    uint8_t* picture          = nullptr;
    uint8_t* noise_sig        = nullptr;
    uint8_t* rotl_obj_picture = nullptr;
    uint8_t* rotr_obj_picture = nullptr;

    DetObj::Object<uint8_t>* obj    = nullptr;
    DetObj::Object<uint8_t>* pic    = nullptr;
    DetObj::Object<uint8_t>* objOne = nullptr;
    DetObj::Object<uint8_t>* objTwo = nullptr;

    Signal* noiseSig = nullptr;
    std::complex<double>** noiseData = nullptr;

    // Var
    uint32_t height = 0;
    uint32_t width  = 0;
    uint32_t obj_w  = 20;
    uint32_t obj_h  = 20;
    uint32_t obj_x  = 220;
    uint32_t obj_y  = 80;

    uint32_t picRotateSize;
    uint32_t objRotateSize;

    int i_ref  = 0;
    int j_ref  = 0;
    int i_real = 0;
    int j_real = 0;

    uint32_t x_one, y_one, x_two, y_two, x, y;
    double min1, min2;

    double max_snr, min_snr;

    private: // functions

    // Rotate function
    // [in] data_in - input data to rotate
    // [in] h - height of in data
    // [in] w - width of in data
    // [out] data_out - output data
    // [out] s_out - size of output data (h == w == s_out)
    // [in] isPositive - left or right direction of rotation
    // [in/out] i_ref - y coordinate of reference point after rotation
    // [in/out] j_ref - x coordinate of reference point after rotation
    template<typename T>
    std::vector<std::pair<int, int>> Rotate(uint8_t* data_in, T h, T w, uint8_t** data_out, T& s_out, bool isPositive, int& i_ref, int& j_ref);

    // Find object function
    // [in] snr - signal to noise ratio in Db
    // [in] koef - maximum error value to apply
    // [in] rotDir - diraction of rotation
    void FindObj(double snr, double koef, bool rotDir);

    public:

    // Constructor
    Analyser();

    // Destructor
    ~Analyser();

    // [in] snr_ranges - min and max value of snr in analyse
    // [in] isSquareError - if true - square error algorithm, else - correlation algorithm
    void configure(std::pair<double, double> snr_ranges, bool isSquareError = true);

    // [in] points - number of snr points value
    // [in] num_iter - number of iterations for mean value
    void run(int points, int num_iter);
};


int main()
{
    std::pair<double, double> snr_ranges = {-20, 0};
    bool isSquare = true;
    int points    = 20;
    int num_iter  = 1000;

    int tmp       = 0;

    std::cout << "Configure parameters? (1 - yes, 0 - no)" << std::endl;
    std::cin >> tmp;
    
    if (tmp == 1)
    {
        std::cout << "Type snr ranges: (min, max): " << std::endl;
        std::cin >> snr_ranges.first >> snr_ranges.second;

        std::cout << "Type number of poins and number of iteration: " << std::endl;
    }

    Analyser analiser;
    analiser.configure(snr_ranges, isSquare);
    analiser.run(points, num_iter);

    return 0;
}


 // Rotate function
 // [in] data_in - input data to rotate
 // [in] h - height of in data
 // [in] w - width of in data
 // [out] data_out - output data
 // [out] s_out - size of output data (h == w == s_out)
 // [in] isPositive - left or right direction of rotation
 // [in/out] i_ref - y coordinate of reference point after rotation
 // [in/out] j_ref - x coordinate of reference point after rotation
 template<typename T>
 std::vector<std::pair<int, int>> Analyser::Rotate(uint8_t* data_in, T h, T w, uint8_t** data_out, T& s_out, bool isPositive, int& i_ref, int& j_ref)
  {
     std::vector<std::pair<int, int>> res;
     s_out = (h + w) * 0.5;//diag * cos(betta);

     *data_out = new uint8_t[s_out * s_out];
     memset(*data_out, 0, s_out * s_out);
     int i1, j1;

     int c_h = h / 2;
     int c_w = w / 2;
     int c_k = s_out / 2;

     //std::cout << "Start! " << std::endl;
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

// Find object function
// [in] snr - signal to noise ratio in Db
// [in] koef - maximum error value to apply
// [in] rotDir - diraction of rotation
void Analyser::FindObj(double snr, double koef, bool rotDir)
{
    // noise data
    for (uint32_t i = 0; i < height; ++i)
        for (uint32_t j = 0; j < width; ++j)
            noiseData[i][j] = {(double)picture[i * width + j], 0};

    g_noizeSignal(*noiseSig, snr);
    noiseSig->GetPicture(noise_sig, false);

    DetObj::Object<uint8_t> picNew;
    uint8_t* picRot = nullptr;

    // Rotate picture
    //i_ref = obj_y;
    //j_ref = obj_x;
    Rotate(noise_sig, height, width, &picRot, picRotateSize, rotDir, i_ref, j_ref);

    picNew.m_delete = false;
    picNew.m_data   = picRot;
    picNew.m_height = picRotateSize;
    picNew.m_width  = picRotateSize;

    std::vector<double> err  = DetObj::DetectObject(*objOne, picNew, koef, x_one, y_one, min1);
    std::vector<double> err2 = DetObj::DetectObject(*objTwo, picNew, koef, x_two, y_two, min2);

    j_real = x_two;
    i_real = y_two;

    if (min1 < min2)
    {
        j_real = x_one;
        i_real = y_one;
    }

    if (rotDir)
    {
        j_ref = 149;
        i_ref = 80;

        i_real += 10;
    }
    else
    {
        j_ref = 168;
        i_ref = 149;

        j_real += 10;
    }

    delete[] picRot;
}

// [in] snr_ranges - min and max value of snr in analyse
// [in] isSquareError - if true - square error algorithm, else - correlation algorithm
void Analyser::configure(std::pair<double, double> snr_ranges, bool isSquareError)
{
    min_snr = snr_ranges.first;
    max_snr = snr_ranges.second;

    // Get pic
    uint8_t* cartoon = nullptr;

    g_loadImage("../Pic/corgi.bmp", width, height, &cartoon);
    picture = new uint8_t[height * width];
    g_toGrayScaleIn(width, height, cartoon, picture);

    delete[] cartoon;

    noise_sig = new uint8_t[width * height];
    noiseSig = new Signal(width, height);
    noiseData = noiseSig->GetDataArray();

    pic = new DetObj::Object<uint8_t>();
    obj = new DetObj::Object<uint8_t>(obj_w, obj_h);

    pic->m_delete = false;
    pic->m_data   = picture;
    pic->m_height = height;
    pic->m_width  = width;

    DetObj::GetDetectingObject(*obj, *pic, obj_x, obj_y);

    int i = 0, j = 0;
    std::vector<std::pair<int, int>> objRotOneIds = Rotate(obj->m_data, obj->m_height, obj->m_width, &rotl_obj_picture, objRotateSize, true,i,j);
    std::vector<std::pair<int, int>> objRotTwoIds = Rotate(obj->m_data, obj->m_height, obj->m_width, &rotr_obj_picture, objRotateSize, false,i,j);

    // Get new objects
    objOne = new DetObj::Object<uint8_t>();
    objTwo = new DetObj::Object<uint8_t>();

    objOne->m_delete = false;
    objOne->m_data   = rotl_obj_picture;
    objOne->m_height = objRotateSize;
    objOne->m_width  = objRotateSize;
    objOne->m_idsOfObj = objRotOneIds;

    objTwo->m_delete = false;
    objTwo->m_data   = rotr_obj_picture;
    objTwo->m_height = objRotateSize;
    objTwo->m_width  = objRotateSize;
    objTwo->m_idsOfObj = objRotTwoIds;
}

// [in] points - number of snr points value
// [in] num_iter - number of iterations for mean value
void Analyser::run(int points, int num_iter)
{
    std::ofstream file("../graph.txt");

    double SNR  = min_snr;
    double dSNR = (max_snr - min_snr) / (points - 1);

    double counter = 0;
    int dPixel = 0;
    double p = 0;

    std::uniform_int_distribution<int> dist(0,1);
    std::random_device rd;

    // SNR values
    for (uint32_t i = 0; i < points; ++i)
    {
        std::cout << i + 1 << "/" << points << ", SNR: " << SNR << std::endl;
        for (uint32_t j = 0; j < num_iter; ++j)
        {            
            FindObj(SNR, 0.5, dist(rd));
            dPixel = sqrt((i_real - i_ref) * (i_real - i_ref) + (j_real - j_ref) * (j_real - j_ref));
            if (dPixel < 3)
               ++counter;
        }
        p = counter / (double)num_iter;
        file << SNR << " " << p << std::endl;
        SNR += dSNR;
        counter = 0;
    }
}

// Constructor
Analyser::Analyser(){}

// Destructor
Analyser::~Analyser()
{
    if (picture != nullptr)
        delete[] picture;
    if (noise_sig != nullptr)
        delete[] noise_sig;
    if (rotl_obj_picture != nullptr)
        delete[] rotl_obj_picture;
    if (rotr_obj_picture != nullptr)
        delete[] rotr_obj_picture;
        
    if (obj != nullptr)
        delete obj;
    if (pic != nullptr)
        delete pic;
    if (objOne != nullptr)
        delete objOne;
    if (objTwo != nullptr)
        delete objTwo;
    if (noiseSig != nullptr)
        delete noiseSig;
}