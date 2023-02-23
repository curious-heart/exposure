#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

struct fileInfo_struct
{
    quint16 fileName;
    quint16 packetSize;
    quint32 byteSize;
    QByteArray packetData[65535];
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool openSerialPort(int bandrate);

private slots:

    void writeData(const QByteArray &data);

    QString ByteArrayToHexStr(QByteArray data);

    QByteArray createCrc(QByteArray data);

    bool checkCrc(QByteArray data);

    void readDataSolt();

    void overtimeTimerOut();

    void on_openSerialPort_clicked();

    void on_closeSerialPort_clicked();

    void on_refreshSerialPort_clicked();

    void on_setPushButton_1_clicked();

    void on_setPushButton_2_clicked();

    void on_setPushButton_3_clicked();

    void on_setPushButton_4_clicked();

    void readFile();

    void on_addFilePushButton_clicked();


    void on_fix_clicked();

    void on_pushButton_clicked();


    void on_debugTextBrowser_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void readyRead();

private:
    Ui::MainWindow *ui;

    QSerialPort *serialPort;

    QByteArray serialPortData;

    QTimer *overtimeTimer;

    QList<fileInfo_struct>   fileInfoList;
};

#endif // MAINWINDOW_H
