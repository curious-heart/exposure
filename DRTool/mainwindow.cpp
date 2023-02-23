#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QtSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include <QMessageBox>
#include <QAxObject>
#include <QDesktopServices>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/picture/app.ico"));
    this->setWindowTitle("DR配置工具");

    ui->debugTextBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    //初始化串口
    serialPort = new QSerialPort(this);
    ui->openSerialPort->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->closeSerialPort->setEnabled(false);
    on_refreshSerialPort_clicked();
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readDataSolt()));
    overtimeTimer = new QTimer(this);
    connect(overtimeTimer,SIGNAL(timeout()),this,SLOT(overtimeTimerOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::openSerialPort(int bandrate)
{
    serialPort->setPortName(ui->serialPortName->currentText());
    if (!serialPort->open(QIODevice::ReadWrite)) {
        ui->debugTextBrowser->append(QString("无法打开%1, 错误: %2")
                                     .arg(ui->serialPortName->currentText())
                                     .arg(serialPort->errorString()));
        return  false;
    }else{
        ui->debugTextBrowser->append(QString("%1打开成功")
                                     .arg(ui->serialPortName->currentText()));
        serialPort->setBaudRate(bandrate);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        serialPort->clearError();
        serialPort->clear();
        return true;
    }

}

QString MainWindow::ByteArrayToHexStr(QByteArray data)
{
    QString temp="";
    QString hex=data.toHex();
    for (int i=0;i<hex.length();i=i+2)
    {
        temp+=hex.mid(i,2)+" ";
    }
    return temp.trimmed().toUpper();
}

QByteArray MainWindow::createCrc(QByteArray data)
{
    quint8  crcHi,crcLo;
    quint16 temp,crc=0xFFFF;

    for(int i=0;i<data.size();i++)
    {
        temp=((quint16)data.data()[i]) & 0x00FF;
        crc^=temp;
        for(quint8 j=0;j<8;j++)
        {
            if (crc & 0x0001)
            {
                crc>>=1;
                crc^=0xA001;
            }
            else crc>>=1;
        }
    }
    crcHi=(crc & 0x00FF);
    crcLo=(crc & 0xFF00)>>8;
    data.append(crcHi);
    data.append(crcLo);
    return data;
}

bool MainWindow::checkCrc(QByteArray data)
{
    if(data.size()<2) return false;
    QByteArray buf =createCrc(data);
    if((buf.data()[buf.size()-2]==0x00)&&(buf.data()[buf.size()-1]==0x00))
    {
        return true;
    }else{
        return false;
    }
}

void MainWindow::readDataSolt()
{
    overtimeTimer->start(200);
    serialPortData.append(serialPort->readAll());
}

void MainWindow::overtimeTimerOut()
{
    overtimeTimer->stop();
    QByteArray data = serialPortData;
    serialPortData.clear();
    int i = 3;

    ui->logoLabel->setText(("收到数据"));
    //
    if(checkCrc(data)!= true && (data.at(0) != 0x05)){
        ui->debugTextBrowser->append(("crc 校验失败!"));

        ui->debugTextBrowser->append((data ) + "1"); //解析错误显示原文
        ui->logoLabel->setText(("crc 校验失败!"));

        return;
    }
    if((quint8)data[0] == 0x5A){
        switch(data[1])
        {
        case 0x01:
            if(data.size() == 6){
                quint16 nameBuf = (quint8)data[2] * 256 + (quint8)data[3];
                for (int i = 0; i < fileInfoList.size(); i++) {
                    ui->debugTextBrowser->append(QString("%1:%2(%3)(%4)").arg(fileInfoList.at(i).fileName).arg(nameBuf).arg((quint8)data[2]).arg((quint8)data[3]));
                    if (fileInfoList.at(i).fileName == nameBuf){
                        QByteArray cmd;
                        cmd.resize(10);
                        cmd[  0] = 0x5A;
                        cmd[  1] = 0x81;
                        cmd[  2] = fileInfoList.at(i).fileName / 256;
                        cmd[  3] = fileInfoList.at(i).fileName % 256;
                        cmd[  4] = fileInfoList.at(i).packetSize / 256;
                        cmd[  5] = fileInfoList.at(i).packetSize % 256;
                        cmd[  6] = (quint8)(fileInfoList.at(i).byteSize>>24);
                        cmd[  7] = (quint8)(fileInfoList.at(i).byteSize>>16);
                        cmd[  8] = (quint8)(fileInfoList.at(i).byteSize>>  8);
                        cmd[  9] = (quint8)(fileInfoList.at(i).byteSize>>  0);
                        QByteArray cmd2 = createCrc(cmd);
                        writeData(cmd2);
                        break;
                    }
                }
            }
            break;
        case 0x02:
            if(data.size() == 8){
                quint16 nameBuf     = (quint8)data[2] * 256 + (quint8)data[3];
                quint16 numberBuf = (quint8)data[4] * 256 + (quint8)data[5];
                for (int i = 0; i < fileInfoList.size(); i++) {
                    if (fileInfoList.at(i).fileName == nameBuf){
                        QByteArray cmd;
                        cmd.resize(8);
                        cmd[  0] = 0x5A;
                        cmd[  1] = 0x82;
                        cmd[  2] = fileInfoList.at(i).fileName / 256;
                        cmd[  3] = fileInfoList.at(i).fileName % 256;
                        cmd[  4] = numberBuf / 256;
                        cmd[  5] = numberBuf % 256;
                        cmd[  6] = fileInfoList.at(i).packetData[numberBuf].size() / 256;
                        cmd[  7] = fileInfoList.at(i).packetData[numberBuf].size() % 256;
                        cmd.append(fileInfoList.at(i).packetData[numberBuf]);
                        QByteArray cmd2 = createCrc(cmd);
                        writeData(cmd2);
                        break;
                    }
                }
            }
            break;
        }
    }else if(data.size() == 82*2+5){
        ui->logoLabel->setText(("正常解析"));
        ui->debugTextBrowser->append(QString("收到数据[%1字节](hex): %2").arg(data.size()).arg(ByteArrayToHexStr(data))); //正常解析16进制
        //
        i = 3;
        //
        //   ui->dataLineEdit_1 ->setText(("%1").arg);
        ui->dataLineEdit_1->setText("0x" + QString::number(((quint8)data[i+0]*256 + (quint8)data[i+1]), 16)
                +"(" + QString::number(((quint8)data[i+0]*256 + (quint8)data[i+1])) + ")");
        i += 2;
        ui->dataLineEdit_2 ->setText(QString("%1").arg((quint8)data[i+0]*256 + (quint8)data[i+1]));
        i += 2;
        ui->comboBox_baudrate->setCurrentIndex((quint8)data[i+0]*256 + (quint8)data[i+1]);
        i += 2;
        ui->dataLineEdit_4 ->setText(QString("%1").arg((quint8)data[i+0]*256 + (quint8)data[i+1]));
        i += 2;
        ui->dataLineEdit_5 ->setText("0x" + QString().setNum((quint8)data[i+0]*256 + (quint8)data[i+1], 2));
        i += 2;

        //查看特定窗口
        short tmpVal =  (quint8)data[ui->checkRegCM->currentIndex()*2+3]*256
                + (quint8)data[ui->checkRegCM->currentIndex()*2+4];
        ui->checkRegLE->setText(QString("%1").arg((signed short)tmpVal));

    }else if((quint8)data[1] >= 0x80){
        ui->debugTextBrowser->append(QString("收到命令(hex): %1").arg(ByteArrayToHexStr(data)));
    }
}

void MainWindow::writeData(const QByteArray &data)
{
    serialPort->write(data);
    ui->debugTextBrowser->append(QString("发送命令(hex): %1").arg(ByteArrayToHexStr(data)));
    ui->logoLabel->setText(("发送命令"));
}

void MainWindow::on_openSerialPort_clicked()
{
    on_closeSerialPort_clicked();

    if(!openSerialPort(9600))
        return;

    ui->openSerialPort->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->closeSerialPort->setEnabled(true);
}

void MainWindow::on_closeSerialPort_clicked()
{
    serialPort->close();
    ui->openSerialPort->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->closeSerialPort->setEnabled(false);
    ui->debugTextBrowser->append(QString("关闭%1")
                                 .arg(ui->serialPortName->currentText()));
}

void MainWindow::on_refreshSerialPort_clicked()
{
    ui->serialPortName->clear();
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {
        ui->serialPortName->addItem(serialPortInfo.portName());
    }
    ui->debugTextBrowser->append(QString(""));
}

void MainWindow::on_setPushButton_1_clicked() //读取数据
{
    QByteArray cmd;
    cmd.resize(6);
    cmd[ 0] = ui->addrLineEdit->text().toInt();
    cmd[ 1] = 0x03;
    cmd[ 2] = 0x00;
    cmd[ 3] = 0x00;
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x52;
    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}

void MainWindow::on_setPushButton_2_clicked() //OTA设置
{
    QByteArray cmd;
    cmd.resize(9);
    cmd[ 0] = ui->addrLineEdit->text().toInt();
    cmd[ 1] = 0x10;
    cmd[ 2] = 0x00;
    cmd[ 3] = 0x01;
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x01;
    cmd[ 6] = 0x01 * 2;
    //
    cmd[ 7] = ui->dataLineEdit_2  ->text().toInt()/256;
    cmd[ 8] = ui->dataLineEdit_2  ->text().toInt()%256;

    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}

void MainWindow::on_setPushButton_3_clicked() //波特率设置
{
    QByteArray cmd;
    cmd.resize(9);
    cmd[ 0] = ui->addrLineEdit->text().toInt();
    cmd[ 1] = 0x10;
    cmd[ 2] = 0x00;
    cmd[ 3] = 0x02;
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x01;
    cmd[ 6] = 0x01 * 2;
    //
    cmd[ 7] = ui->comboBox_baudrate->currentIndex() / 256;
    cmd[ 8] = ui->comboBox_baudrate->currentIndex() % 256;

    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}

void MainWindow::on_setPushButton_4_clicked() //地址设置
{
    QByteArray cmd;
    cmd.resize(9);
    cmd[ 0] = ui->addrLineEdit->text().toInt();
    cmd[ 1] = 0x10;
    cmd[ 2] = 0x00;
    cmd[ 3] = 0x03;
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x01;
    cmd[ 6] = 0x01 * 2;
    //
    cmd[ 7] = ui->dataLineEdit_4  ->text().toInt()/256;
    cmd[ 8] = ui->dataLineEdit_4  ->text().toInt()%256;

    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}



void MainWindow::readFile()
{
    QString path = QCoreApplication::applicationDirPath();
    path += QString("/otafile");
    ui->debugTextBrowser->append(QString("path:%1").arg(path));

    QDir dir(path);
    foreach(QFileInfo mfi, dir.entryInfoList())
    {
        if(mfi.isFile()&& (mfi.suffix() == "bin")){
            ui->debugTextBrowser->append(QString("foreach:%1").arg(mfi.absoluteFilePath()));
            quint8 flg = 0;
            for (int i = 0; i < fileInfoList.size(); i++) {
                if (fileInfoList.at(i).fileName == (quint16)mfi.baseName().toInt()){
                    ui->debugTextBrowser->append(QString("%1 already read!").arg(mfi.baseName().toInt()));
                    flg = 1;
                    break;
                }
            }
            if(flg == 1){
                break;
            }
            //
            fileInfo_struct fileData;
            fileData.fileName = (quint16)mfi.baseName().toInt();
            ui->debugTextBrowser->append(QString("fileName:%1").arg(fileData.fileName));
            //
            QByteArray data;
            QFile file(mfi.absoluteFilePath());
            file.open(QIODevice::ReadOnly);
            data = file.readAll();
            data.append(0xFF);
            data.append(0xFF);
            data.append(0xFF);
            data.append(0xFF);
            data.append(0x02);
            data.append(0x01);
            data.append(0x04);
            data.append(0x03);
            data.append(0x03);
            data.append(0x07);
            data.append((quint8)(fileData.fileName>>  8));
            data.append((quint8)(fileData.fileName>>  0));
            data = createCrc(data);
            fileData.byteSize = data.size();
            ui->debugTextBrowser->append(QString("byteSize:%1").arg(fileData.byteSize));
            if((fileData.byteSize % 512) == 0){
                fileData.packetSize = fileData.byteSize / 512;
            }else{
                fileData.packetSize = fileData.byteSize / 512 + 1;
            }
            ui->debugTextBrowser->append(QString("packetSize:%1").arg(fileData.packetSize));
            for(quint16 i = 0; i < fileData.packetSize; i++)
            {
                if(data.size() >= 512){
                    fileData.packetData[i] = data.left(512);
                    data.remove(0,512);
                }else{
                    fileData.packetData[i] = data.left(data.size());
                    data.remove(0,data.size());
                }
                ui->debugTextBrowser->append(QString("packetSize%1:%2").arg(i).arg(ByteArrayToHexStr(fileData.packetData[i])));
            }
            fileInfoList.append(fileData);
        }
    }
}

void MainWindow::on_addFilePushButton_clicked() //加载文件
{
    fileInfoList.clear();
    readFile();
}


void MainWindow::on_fix_clicked()
{
    QByteArray cmd;
    float val;
    char charVal[4];
    cmd.resize(14);
    cmd[ 0] = ui->addrLineEdit->text().toInt();
    cmd[ 1] = 0x10;
    cmd[ 2] = 0x00;
    cmd[ 3] = 0x12;
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x03;
    cmd[ 6] = 0x02 * 2;
    //
    cmd[ 7] = (ui->cmdComBox->currentIndex())/256;
    cmd[ 8] = (ui->cmdComBox->currentIndex())%256;
    if((ui->cmdComBox->currentIndex() == 0 )||
            (ui->cmdComBox->currentIndex() == 4 ))
        *(int*)charVal = ui->valLineEdit->text().toInt();
    else{
        val = ui->valLineEdit->text().toFloat();
        memcpy(charVal, &val, 4);
    }
    cmd[9]  = charVal[0];
    cmd[10]  = charVal[1];
    cmd[11]  = charVal[2];
    cmd[12]  = charVal[3];

    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray cmd;
    cmd.resize(9);
    cmd[ 0] = 0;
    cmd[ 1] = 0x10;
    cmd[ 2] = 0x00;
    cmd[ 3] = 0x03;
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x01;
    cmd[ 6] = 0x01 * 2;
    //
    cmd[ 7] = ui->addrLineEdit->text().toInt()/256;
    cmd[ 8] = ui->addrLineEdit->text().toInt()%256;

    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}


void MainWindow::on_debugTextBrowser_customContextMenuRequested(const QPoint &pos)
{
    ui->debugTextBrowser->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    on_closeSerialPort_clicked();
    if(!openSerialPort(115200))
        return;
    ui->openSerialPort->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->closeSerialPort->setEnabled(true);
}

void MainWindow::on_pushButton_3_clicked()
{
    QByteArray cmd;
    cmd.resize(9);
    cmd[ 0] = ui->addrLineEdit->text().toInt();
    cmd[ 1] = 0x10;
    cmd[ 2] = 0x00;
    cmd[ 3] = ui->comboBox->currentIndex();
    cmd[ 4] = 0x00;
    cmd[ 5] = 0x01;
    cmd[ 6] = 0x01 * 2;
    //
    cmd[ 7] = ui->lineEdit->text().toInt()/256;
    cmd[ 8] = ui->lineEdit->text().toInt()%256;

    QByteArray cmd2 = createCrc(cmd);
    writeData(cmd2);
}
