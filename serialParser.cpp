#include "serialParser.h"
#include "ui_mainwindow.h"
#include "QWidget"
#include <QDebug>
#include <QTreeWidgetItem>
#include <QString>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Qtport.setBaudRate(QSerialPort::Baud38400);
    // Qtport.setDataBits(QSerialPort::Data8);
    // Qtport.setStopBits(QSerialPort::OneStop);
    // Qtport.setParity(QSerialPort::NoParity);
    // Qtport.setFlowControl(QSerialPort::NoFlowControl);
    Qtport.setBaudRate(QSerialPort::Baud38400);
    Qtport.setDataBits(QSerialPort::Data8);
    Qtport.setStopBits(QSerialPort::OneStop);
    Qtport.setParity(QSerialPort::NoParity);
    Qtport.setFlowControl(QSerialPort::NoFlowControl);
    connect(&Qtport, SIGNAL(readyRead()),this, SLOT(testSerialPort()));
    QList<QSerialPortInfo> dd = QSerialPortInfo::availablePorts();
    for(int i = 0; i < dd.size(); i++)
    {
        ui->comboBox->addItem(dd[i].portName());
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testSerialPort()
{
    QTreeWidgetItem *Item = new QTreeWidgetItem(ui->treeWidget);
    qDebug() << "before read all";
    data.append(Qtport.readAll());
    // data.append("kkigASDPP");
    //    uchar nas[9] = {1, 2, 253, 17, 99, 100, 236, 2, 2};
    //    data.append((char*)nas, 9);
    qDebug() << "after read all";
    // data = Qtport.readAll();
    qDebug() << data.size() << "razmer";
    if(data.size() < SIZE_OF_PACK)
    {
        return;
    }
    for(;data.size() >= SIZE_OF_PACK;)
    {
        for(int i = 0; i < SIZE_OF_PACK; i++)
        {
            mas[i] = (quint8)data.at(i);
        }
        if ((mas[START_STREAM_FB] != 204) & (mas[START_STREAM_SB] != 204))
        {
            continue;
            data.remove(0, 1);
        }
        qDebug() << "if true(composition of arr)";

        ROT = uint16_t(mas[NUMBER_FByte_ROT] << 8) + mas[NUMBER_SByte_ROT];

        XrowACCL = uint16_t(mas[NUMBER_FByte_XACCL] << 8) + mas[NUMBER_SByte_XACCL];

        YrowACCL = uint16_t(mas[NUMBER_FByte_YACCL] << 8) + mas[NUMBER_SByte_YACCL];

//        qDebug() << ROT;
        ROTS = inclineScale(ROT);
        XACCL = accelScale(XrowACCL);
        YACCL = accelScale(YrowACCL);

        qDebug() << ROTS << "from func";
//        qDebug() << fres << "from Sanya";

        ROTS = filtred_value(ROTS, 0);
        XACCL = filtred_value(XACCL, 1);
        YACCL = filtred_value(YACCL, 2);

        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
        for (int i =0; i<SIZE_OF_PACK; i++){
            for (int j = 0; j < NUMBER_VALUE; j++){
                if (j == 0){
            Item->setText(j, QString::number(ROTS));
                }
                if (j == 1){
                    Item->setText(j, QString::number(XACCL));
                }
                else {
                    Item->setText(j, QString::number(YACCL));
                }
            }
            //            QTableWidgetItem asd("asd"); string for testg
            if(ui->tableWidget->item(ui->tableWidget->rowCount() - 1, i) == nullptr)
            {
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, i , new QTableWidgetItem(QString::number(mas[i])));
            }
            else
            {
                ui->tableWidget->item(ui->tableWidget->rowCount() - 1, i)->setText("until full row" + QString::number((mas[i])));
            }
        }
        data.remove(0, SIZE_OF_PACK);
    }
}

void MainWindow::on_pushButton_clicked()
{
    Qtport.setPortName(ui->comboBox->currentText());
    //Qtport.close();
    qDebug() << "open port";
    qDebug() << "open port 2";
    bool ok = Qtport.open(QIODevice::ReadWrite);
    qDebug() << ok << "status";
    testSerialPort();
}
void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->textEdit_infoComPort->clear();
    QSerialPortInfo info(arg1);
    ui->textEdit_infoComPort->append(info.portName() + ":");
    ui->textEdit_infoComPort->append(info.manufacturer());
    ui->textEdit_infoComPort->append(QString::number(info.productIdentifier(), 16));
    ui->textEdit_infoComPort->append(info.serialNumber());
    ui->textEdit_infoComPort->append(info.description());
}

//float MainWindow::filtred_value(float ROW, int number){
//    Pc[number] = P[number] + varProcess[number];
//    G[number] = Pc[number]/(Pc[number] + varVolt[number]);
//    P[number] = (1-G[number])*Pc[number];
//    Xp[number] = Xe[number];
//    Zp[number] = Xp[number];
//    Xe[number] = G[number]*(ROW-Zp[number])+Xp[number]; // "фильтрованное" значение
//    return(Xe[number]);
//}

float MainWindow::inclineScale(int16_t sensorData)
{
    int signedData = 0;
    sensorData = sensorData & 0x3FFF;
    int isNeg = sensorData & 0x2000;
    if (isNeg == 0x2000)
        signedData = sensorData - 0x3FFF;
    else
        signedData = sensorData;
    float finalData = signedData * 0.025;
    return finalData;
}
// X = 0; Y = 1
float MainWindow::accelScale(int16_t sensorData)
{
    int signedData = 0;
    sensorData = sensorData & 0x3FFF; // Discard upper two bits
    int isNeg = sensorData & 0x2000;
    if (isNeg == 0x2000) // If the number is negative, scale and sign the output
        signedData = sensorData - 0x3FFF;
    else
        signedData = sensorData;
    float finalData = signedData * 0.24414; // Multiply by accel sensitivity (244.14 uG/LSB)
    return finalData;
}
