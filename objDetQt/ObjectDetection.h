#ifndef _OBJECT_DECECTION_H_
#define _OBJECT_DECECTION_H_

#include "Signal.h"

namespace DetObj
{

// Structure of object on picture 
template <typename T>
struct Object
{
    uint32_t m_width  = 0;                      // width of object
    uint32_t m_height = 0;                      // height of object
    T*       m_data   = nullptr;                // data of object
    bool     m_delete = true;                   // flag for deleting data
    std::vector<std::pair<int, int>> m_idsOfObj;  // vector with ids of object (optional)

    // Default constrtctor
    Object()
    {

    }

    // Constructor
    Object(uint32_t width, uint32_t height) : m_width(width), m_height(height)
    {
        m_data = new T[width * height];
    }

    //destructor
    ~Object()
    {
        if ((m_data != nullptr) && m_delete)
            delete[] m_data;
    }
};

//
// Get detecting object
//
// [in/out] obj     - Input/Output object to detecting on picture
// [in]     picture - Input picture for detecting object on it
// [in]     x       - x coordinate on pic for detecting object
// [in]     y       - y coordinate on pic for detecting object
//
// return - void
//
template <typename T, typename ObjT>
void GetDetectingObject(Object<ObjT>& obj, const Object<ObjT>& picture, T x, T y)
{
    uint32_t height    = obj.m_height;
    uint32_t width     = obj.m_width;
    uint32_t widthPic  = picture.m_width;

    for (T raw = 0; raw < height; ++raw)
    {
        for (T col = 0; col < width; ++col)
        {
            obj.m_data[raw * width + col] = picture.m_data[(y + raw) * widthPic + x + col];
        }
    }

    return;
}

//
// Paint detecting object on picture
//
// [in]     obj     - Input        detected object
// [in/out] picture - Input/Output picture for detecting object on it
// [in]     x       - x coordinate on pic for detecting object
// [in]     y       - y coordinate on pic for detecting object
//
// return - void
//
template <typename T, typename ObjT>
void PaintDetectingObject(const Object<ObjT>& obj, Object<ObjT>& picture, T x, T y)
{
    uint32_t height       = obj.m_height;
    uint32_t width        = obj.m_width;
    uint32_t widthPic     = picture.m_width;

    // raws
    for (T raw = y; raw < y + height; ++raw)
    {
        picture.m_data[raw * widthPic + x]         = 255;
        picture.m_data[raw * widthPic + x + width] = 255;
    }

    // calomns
    for (T col = x; col < x + width; ++col)
    {
        picture.m_data[y * widthPic + col]            = 255;
        picture.m_data[(y + height) * widthPic + col] = 255;
    }

    return;
}

//
// Find Square Error criterin
//
// [in] x       - Input x coordinate on picture to start processing
// [in] y       - Input y coordinate on picture to start processing
// [in] obj     - Input object to detecting on picture
// [in] picture - Input picture for detecting object on it
//
// return - square error criterin
//
template <typename T, typename ObjT>
double SquareErrorCriterion(T x, T y, const Object<ObjT>& obj, const Object<ObjT>& picture)
{
    uint32_t height    = obj.m_height;
    uint32_t width     = obj.m_width;
    uint32_t widthPic  = picture.m_width;
    double   err       = 0;
    double   dVal      = 0;
    uint32_t idx, idy;

    for (T i = 0; i < (T)obj.m_idsOfObj.size(); ++i)
    {
        idy = (uint32_t)obj.m_idsOfObj.at(i).first;
        idx = (uint32_t)obj.m_idsOfObj.at(i).second;
        dVal   = (double)obj.m_data[idy * width + idx] - 
                 (double)picture.m_data[(y + idy) * widthPic + x + idx];

        err += dVal * dVal;
    }

    return err;
}

//
// Function for detection objects on picture
//
// [in] obj         - Input object to detecting on picture
// [in] picture     - Input picture for detecting object on it
// [in] koef        - Input koefficient for celection result
// [out] x          - Output x coordinate on picture of detected object
// [out] y          - Output y coordinate on picture of detecred object
// [out] minErr     - Output min error value
//
// return void
//
template<typename ObjT, typename T>
std::vector<double> DetectObject(const Object<ObjT>& obj, const Object<ObjT>& picture, double koef, T& x, T& y, double& minErr)
{
    std::vector<std::pair<T,T>> resultsCoord;
    std::vector<double>         resultsVal;
    std::vector<double>         results;

    uint32_t height    = picture.m_height - obj.m_height;
    uint32_t width     = picture.m_width - obj.m_width;

    double err = 0;

    double max_en = obj.m_height * obj.m_width * 255. * 255.;

    //std::cout << "errors: " << std::endl;
    for (uint32_t raw = 0; raw < height; ++raw)
    {
        //std::cout << raw << "|" << height << std::endl;
        for (uint32_t col = 0; col < width; ++col)
        {
            //std::cout << col << " ";
            err = SquareErrorCriterion(col, raw, obj, picture) / max_en;
            //std::cout << err << " ";
            if (err <= koef)
            {
                resultsCoord.push_back({raw, col});
                resultsVal.push_back(sqrt(err));
            }
            results.push_back(err);
        }
      //  std::cout << std::endl;
    }
    //std::cout << std::endl;
    minErr = 0;
    x = 0;
    y = 0;

    if (!resultsVal.empty())
       minErr = resultsVal.at(0);

    std::cout << "find best: " << resultsVal.size() << std::endl;
    for (auto i = 0; i < resultsVal.size(); ++i)
    {
     //   std::cout << resultsVal.at(i) << " ";
        if (minErr > resultsVal.at(i))
        {
            minErr = resultsVal.at(i);
            x = resultsCoord.at(i).second;
            y = resultsCoord.at(i).first;
        }
    }
    std::cout << "min:" << minErr << std::endl;

    return results;
}

}   // end of DetObj nsp

#endif // _OBJECT_DECECTION_H_
