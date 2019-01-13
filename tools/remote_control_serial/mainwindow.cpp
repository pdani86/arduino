#include "mainwindow.h"
#include "ui_mainwindow.h"


static void myEncode(const uchar* src,int srclen,uchar* dst) {
    // dst must be 2 times the size of src
    for(int i=0;i<srclen;i++) {
        dst[2*i] = src[i]<<4;
        dst[2*i+1] = src[i]&0xf0;
    }
}

static void myDecode(const uchar* src,int srclen,uchar* dst) {
    for(int i=0;i<srclen/2;i++) {
        dst[i] = (src[i*2]>>4) | (src[i*2+1]&0xf0);
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&timer,SIGNAL(timeout()),this,SLOT(timer_timeout()));
    timer.setInterval(100);
    timer.start();

    initSerial();
    elapsedTimer.start();

    hid_device_handle = 0;

    hid_device_handle = hid_open(0x046d,0xc21f,NULL);
    if(0==hid_device_handle) qDebug("HID Device Not Found");
    hid_set_nonblocking(hid_device_handle, 1);

    // test
    char buf[128];
    char buf2[128];
    myEncode((uchar*)"abc",3,(uchar*)buf);
    buf[6] = 0;
    qDebug(buf);
    myDecode((uchar*)buf,6,(uchar*)buf2);
    buf2[3] = 0;
    qDebug(buf2);
}

MainWindow::~MainWindow()
{
    if(hid_device_handle) hid_close(hid_device_handle);
    if(serial.isOpen()) serial.close();
    delete ui;
}

void MainWindow::initSerial() {
    ui->serialConsole->clear();
    serial.setPortName("\\\\.\\COM5");
    serial.setBaudRate(115200);

    serial.open(QIODevice::ReadWrite);
}

void MainWindow::readController() {
    if(0==hid_device_handle) return;
    uchar buf[1024];
    memset(buf,0,sizeof(buf));
        buf[0]=0x11;
        buf[1]=0x08;
        buf[2]=0x10;
        buf[3]=0x80;
    int nread = hid_read(hid_device_handle,buf,1023);
    //qDebug(QString("nread: %1").arg(nread).toLatin1().data());
    if(nread<=0) {return;}
    int _nread = 0;
    while((_nread=hid_read(hid_device_handle,buf,1023)) > 0) {nread=_nread;}

    //if(nread<12) {return;}

    ui->controllerConsole->clear();

    QString str;

    for(int i=0;i<nread;i++)
    {
        str.append(QString("%1 ").arg((int)buf[i],2,16,QChar('0')));
        if(0== ((i+1)%4)) str.append("\n");
    }

    controlPacket.throttle = 0;
    if(buf[9]<0x80) {
        controlPacket.throttle = (0xff-buf[9]*2);
        controlPacket.gear = 1;
    } else {
        if(buf[9]>0x80) controlPacket.throttle  = (buf[9] - 0x80)*2;
        controlPacket.gear = -1;
    }

    controlPacket.steer = (255-buf[0]-128);
    controlPacket.lamps = buf[10]; // logitech F710 -> 10: 'A' button
    ui->controllerConsole->appendPlainText(str);
}

void MainWindow::timer_timeout() {
    readController();
    if(!serial.isOpen()) {initSerial();}
    if(!serial.isOpen()) {qDebug("Couldn't open serial port!"); return;}
    if(serial.bytesAvailable()) ui->serialConsole->appendPlainText(QString(serial.readAll()));
    /*const long periodTimeMs = 6000;
    float t = (elapsedTimer.elapsed() % periodTimeMs)/(float)(periodTimeMs-1);
    char steer = 127*sin(t*2*M_PI);
    qDebug(QString("Steer: %1").arg((int)steer).toLatin1().data());
    controlPacket.steer = steer;*/
    uchar buf[sizeof(controlPacket)*2];
    myEncode((uchar*)&controlPacket,sizeof(controlPacket),buf);
    serial.write("s");
    serial.write((char*)buf,sizeof(controlPacket)*2);
    serial.write("\n");
}
