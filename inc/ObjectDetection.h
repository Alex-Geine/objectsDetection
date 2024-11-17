#ifndef _OBJECT_DECECTION_H_
#define _OBJECT_DECECTION_H_

#include "Signal.h"

namespace DetObj
{

// Structure of object on picture 
template <typename T>
struct Object
{
    uint32_t m_width  = 0;         // width of object
    uint32_t m_height = 0;         // height of object
    T*       m_data   = nullptr;   // data of object
    bool     m_delete = true;      // flag for deleting data

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

    for (T raw = 0; raw < height; ++raw)
    {
        for (T col = 0; col < width; ++col)
        {
            err += sqrt(
                obj.m_data[raw * width + col] * obj.m_data[raw * width + col] -
                picture.m_data[(y + raw) * widthPic + x + col] * picture.m_data[(y + raw) * widthPic + x + col]);
        }
    }

    return err;
}

//
// Function for detection objects on picture
//
// [in] obj     - Input object to detecting on picture
// [in] picture - Input picture for detecting object on it
// [in] koef    - Input koefficient for celection result
// [out] x       - Output x coordinate on picture of detected object
// [out] y       - Output y coordinate on picture of detecred object
//
// return void
//
template<typename ObjT, typename T>
void DetectObject(const Object<ObjT>& obj, const Object<ObjT>& picture, double koef, T& x, T& y)
{
    std::vector<std::pair<T,T>> resultsCoord;
    std::vector<double>         resultsVal;

    uint32_t height    = picture.m_height - obj.m_height;
    uint32_t width     = picture.m_width - obj.m_width;

    double err = 0;

    for (uint32_t raw = 0; raw < height; ++raw)
    {
        for (uint32_t col = 0; col < width; ++ col)
        {
            err = SquareErrorCriterion(col, raw, obj, picture);
            if (err <= koef)
            {
                resultsCoord.push_back({raw, col});
                resultsVal.push_back(err);
            }
        }
    }

    double min = 0;
    x = 0;
    y = 0;

    if (!resultsVal.empty())
       min = resultsVal.at(0);

    for (auto i = 0; i < resultsVal.size(); ++i)
    {
        if (min > resultsVal.at(i))
        {
            min = resultsVal.at(i);
            x = resultsCoord.at(i).second;
            y = resultsCoord.at(i).first;
        }
    }

    return;
}

}   // end of DetObj nsp

#endif // _OBJECT_DECECTION_H_