#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setStyleSheet("QLabel { background-color : rgb(192,192,192) ;}");

}

MainWindow::~MainWindow()
{
    delete ui;
}

// Find
void MainWindow::on_pushButton_2_clicked()
{
   return;
}

void MainWindow::on_pushButton3_clicked()
{
    QFileDialog dialog;

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    filename = dialog.getOpenFileName();

    if (!filename.isEmpty())
    {
        //uint8_t* data = nullptr;
        //auto image = create_QImage(object,object.n_rows,object.n_cols);

        //ui->object_image->setPixmap(QPixmap::fromImage(image));
    }

    return;
}
