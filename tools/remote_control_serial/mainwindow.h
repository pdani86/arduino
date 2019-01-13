#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtSerialPort/QtSerialPort>
#include <QElapsedTimer>

#include "hidapi.h"

struct RADIO_CONTROL_PACKET
{
  char steer;
  uchar throttle;
  uchar brake;
  uchar clutch;
  char gear;
  uchar lamps;

  inline RADIO_CONTROL_PACKET() {memset(this,0,sizeof(this));}
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTimer timer;
    QElapsedTimer elapsedTimer;
    QSerialPort serial;

    RADIO_CONTROL_PACKET controlPacket;

    hid_device* hid_device_handle;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initSerial();
    void readController();

private:
    Ui::MainWindow *ui;

private slots:
    void timer_timeout();
};

#endif // MAINWINDOW_H
