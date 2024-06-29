#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "HID_DeviceManager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//尝试继承一个DeviceManager，看看效果
class MouseManager : public CHID_DeviceManager {
public:
    MouseManager();
    ~MouseManager();

    void AnalyseRandomData(const DataRandom& recv,int index);
    void AnalyseContinuousData(const DataContinuous& recv,int index);

    void SendTestMessage();

    void SendRandomWriteMessage();
    void SendRandomReadMessage();
	void SendContinuousWriteMessage();
	void SendContinuousReadMessage();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    MouseManager test;
    Ui::MainWindow *ui;
private slots:
    void on_btn_init_clicked();
    void on_btn_random_write_clicked();
    void on_btn_random_read_clicked();
    void on_btn_continue_write_clicked();
    void on_btn_continue_read_clicked();
};
#endif // MAINWINDOW_H
