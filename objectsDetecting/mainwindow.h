#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <RadiationPattern.h>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString filename;

private slots:
    void on_pushButton_2_clicked();


    void on_pushButton_3_clicked();

    template<typename T>
    QImage create_QImage(T* data, unsigned w, unsigned h)
    {
        T max = data[0];
        for (auto i = 0; i < w * h; ++i)
             if (max < data[i])
                 max = data[i];

        for (auto i = 0; i < w*h; ++i)
        {
            image[i] = data[i] / max * 255.;
        }

        QImage imageOut(w,h,QImage::Format_Grayscale8);
        for (auto i = 0; i < w; ++i)
            for (auto j =0 ;j < h; ++j)
            {
                uint8_t pixel = image[i * h + j];
                auto c = qRgb(pixel,pixel,pixel);
                imageOut.setPixel(i,j,c);
            }

        return imageOut;
    }

};
#endif // MAINWINDOW_H
