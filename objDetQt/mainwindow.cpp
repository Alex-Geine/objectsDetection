#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Init();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete[] picture;
    delete[] rotl_obj_picture;
    delete[] rotr_obj_picture;

    delete obj;
    delete pic;
    delete objOne;
    delete objTwo;
}

// Init fucntion
void MainWindow::Init()
{
    // Get pic
    arma::mat pic_array = gray_image_from_file<arma::mat>("../Pic/corgi.bmp");
    auto      image     = create_QImage(pic_array, pic_array.n_rows, pic_array.n_cols);
    ui->init_pic->setPixmap(QPixmap::fromImage(image));

    // Get uint8_t pic
    height = pic_array.n_rows;
    width  = pic_array.n_cols;
    picture = new uint8_t[height * width];

    for (uint32_t raw = 0; raw < height; ++raw)
        for (uint32_t col = 0; col < width; ++col)
            picture[raw * width + col] = static_cast<uint8_t>(pic_array(raw,col));

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

    // Print objects
    auto obj_img  = create_Image(obj->m_data, obj_h, obj_w);
    auto objl_img = create_Image(rotl_obj_picture, objRotateSize, objRotateSize);
    auto objr_img = create_Image(rotr_obj_picture, objRotateSize, objRotateSize);

    ui->object_pic->setPixmap(QPixmap::fromImage(obj_img));
    ui->obj_rot_l_pic->setPixmap(QPixmap::fromImage(objl_img));
    ui->obj_rot_r_pic->setPixmap(QPixmap::fromImage(objr_img));
}

// Start button
void MainWindow::on_pushButton_clicked()
{
    FindObj(ui->snr_box->value(), ui->koeff->value(), ui->rot_box->isChecked());

    return;
}

// Find object function
void MainWindow::FindObj(double snr, double koef, bool rotDir)
{
    noise_pic = new uint8_t[height * width];

    Signal* noiseSig = new Signal(width, height);
    std::complex<double>** noiseData = noiseSig->GetDataArray();

     for (uint32_t i = 0; i < height; ++i)
        for (uint32_t j = 0; j < width; ++j)
            noiseData[i][j] = {(double)picture[i * width + j], 0};

    g_noizeSignal(*noiseSig, snr);
    noiseSig->GetPicture(noise_pic, false);

    DetObj::Object<uint8_t> picNew;
    uint8_t* picRot = nullptr;

    // Rotate picture
    i_ref = obj_y;
    j_ref = obj_x;
    Rotate(noise_pic, height, width, &picRot, picRotateSize, rotDir, i_ref, j_ref);

    delete[] noise_pic;

    ui->x_ref_box->setValue(j_ref);
    ui->y_ref_box->setValue(i_ref);

    picNew.m_delete = false;
    picNew.m_data   = picRot;
    picNew.m_height = picRotateSize;
    picNew.m_width  = picRotateSize;


    uint32_t x_one, y_one, x_two, y_two, x, y;
    double min1, min2;

    std::vector<double> err  = DetObj::DetectObject(*objOne, picNew, koef, x_one, y_one, min1);
    std::vector<double> err2 = DetObj::DetectObject(*objTwo, picNew, koef, x_two, y_two, min2);

    x = x_two;
    y = y_two;

    std::vector<double>* errPtr = &err2;

    if (min1 < min2)
    {
        DetObj::PaintDetectingObject(*objOne, picNew, x_one, y_one);
        x = x_one;
        y = y_one;
        errPtr = &err;
    }
    else
    {
        DetObj::PaintDetectingObject(*objOne, picNew, x_two, y_two);
    }

    if (rotDir)
        y += 10;
    else
        x += 10;

    ui->x_box->setValue(x);
    ui->y_box->setValue(y);

    int errWidth = picNew.m_width - objOne->m_width;
    int errHeight = picNew.m_height - objOne->m_height;
    Signal sig(errWidth, errHeight);

    noiseData = sig.GetDataArray();
    for (int i = 0; i < errHeight; ++i)
        for (int j = 0; j < errWidth; ++j)
            noiseData[i][j] = {(*errPtr).at(i * errWidth + j), 0};

    uint8_t* errData = new uint8_t[errWidth * errHeight];

    sig.GetPicture(errData, true);

    // Print pictures
    auto picErr = create_Image(errData, errHeight, errWidth);
    auto picRes = create_Image(picRot, picRotateSize, picRotateSize);

    ui->error_pic->setPixmap(QPixmap::fromImage(picErr));
    ui->result_pic->setPixmap(QPixmap::fromImage(picRes));

    delete[] picRot;
    delete[] errData;
}
