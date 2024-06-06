#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qdebug.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    test.SendTestMessage();
}

void MainWindow::on_pushButton_clicked()
{
    test.Init(13615, 262);
}

MouseManager::MouseManager()
{
}

MouseManager::~MouseManager()
{
}

void MouseManager::AnalyseRandomData(const DataRandom& recv, int index)
{
    qDebug() << __FUNCTION__;
    qDebug() << recv.param_addr << index;
}

void MouseManager::AnalyseContinuousData(const DataContinuous& recv, int index)
{
    qDebug() << __FUNCTION__;
    qDebug() << recv.param_addr << index;
}

void MouseManager::SendTestMessage()
{
    //组织一个随机地址数据包发送
	DataRandom send;
    QByteArray arr_add;
    arr_add.append(0x34);
    arr_add.append(0x12);

    QByteArray arr_val;

    arr_val.append(0x78);
    arr_val.append(0x56);

    CreateDataRandom(send, 0x4B, 11, 0x00, 0x03, (uint8_t*)arr_add.data(), (uint8_t*)arr_val.data(), CRCType::crc_32);

    this->SendRandom(&send, 10, 0);
}
