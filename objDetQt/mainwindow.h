#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <armadillo>
#include "qcustomplot.h"
#include "image_io.hpp"
#include "Signal.h"
#include "ObjectDetection.h"

template<typename T>
std::vector<std::pair<int, int>> Rotate(uint8_t* data_in, T h, T w, uint8_t** data_out, T& s_out, bool isPositive, int& i_ref, int& j_ref)
{
     std::vector<std::pair<int, int>> res;
     s_out = (h + w) * 0.5;//diag * cos(betta);

     *data_out = new uint8_t[s_out * s_out];
     memset(*data_out, 0, s_out * s_out);
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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:

    // Create Q image
    template<typename Image>
    QImage create_QImage(Image img, unsigned w, unsigned h)
    {
        QImage image(w,h,QImage::Format_Grayscale8);
        for (auto i = 0; i < w; ++i)
            for (auto j =0 ;j < h; ++j)
            {
                auto c = qRgb(img(i,j),img(i,j),img(i,j));
                image.setPixel(i,j,c);
            }

        return image;
    }

    // Create image
    template<typename Image>
    QImage create_Image(Image* img, unsigned raw, unsigned col)
    {
        QImage image(raw,col,QImage::Format_Grayscale8);
        for (auto i = 0; i < raw; ++i)
        {
            for (auto j = 0 ; j < col; ++j)
            {
                auto c = qRgb(img[i * raw + j], img[i* raw + j], img[i * raw + j]);
                image.setPixel(i,j,c);
            }
        }
        return image;
    }

    // Init fucntion
    void Init();

    // Find object function
    void FindObj(double snr, double koef, bool rotDir);

    Ui::MainWindow *ui;

    // Data
    uint8_t* picture          = nullptr;
    uint8_t* noise_pic        = nullptr;
    uint8_t* rotl_obj_picture = nullptr;
    uint8_t* rotr_obj_picture = nullptr;

    DetObj::Object<uint8_t>* obj    = nullptr;
    DetObj::Object<uint8_t>* pic    = nullptr;
    DetObj::Object<uint8_t>* objOne = nullptr;
    DetObj::Object<uint8_t>* objTwo = nullptr;

    // Var
    uint32_t height = 0;
    uint32_t width  = 0;
    uint32_t obj_w  = 20;
    uint32_t obj_h  = 20;
    uint32_t obj_x  = 80;
    uint32_t obj_y  = 220;

    uint32_t picRotateSize;
    uint32_t objRotateSize;

    int i_ref = 0;
    int j_ref = 0;
};
#endif // MAINWINDOW_H
