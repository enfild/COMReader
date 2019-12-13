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
//            qDebug() << mas[i];
        }
        if ((mas[0] != 204) & (mas[1] != 204))
        {
            continue;
            data.remove(0, 1);
        }

        qDebug() << "if true(composition of arr)";

        ROT = uint16_t(mas[2] << 8) + mas[3];
        qDebug() << ROT;
        //        ROT = ROT << 2;
        //                ROT = ROT > 0x2000 ? ROT - 0x4000 : ROT;
        //        ROT = ROT
        //                ROT = ROT &(1 << 13) ? ROT - (1 << 14) : ROT;

        ROTS =inclineScale(ROT);

        //worked form
        int16_t result = ROT & (1<<13) ? short(ROT) - (1<<14) : short(ROT);
        result = result * 0.0245;
        auto fres = filtred_value(result, ROT_PACK);


//        uint16_t result = ROT & (1 << 13) ? (short)ROT - (1 << 14) : short(ROT);

//        short rotation = ROT&(1 << 13) ? ROT - (1 << 14) : ROT;

//        float rotation_obr = rotation/40;
//        float rotation_fil = filtred_value(rotation_obr, ROT_PACK);

//        qDebug() << b;

        //        ROT &= ~(0b11 << 14);
        //        ROT = ROT << 2;
        //        ROT >>= 2;
        //        ROT = ROT/40;
        //        ROT = filtredROT(ROT);

        qDebug() << ROTS << "from func";
        qDebug() << fres << "from Sanya";

        //                     }
        //                 }


        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);

        for (int i =0; i<SIZE_OF_PACK; i++){
            Item->setText(i, QString::number(ROTS));

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
    // Qtport.open(QIODevice::ReadWrite);
    // qDebug() << Qtport.open(QIODevice::ReadWrite);
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

float MainWindow::filtred_value(float ROW, int number){
    Pc[number] = P[number] + varProcess[number];
    G[number] = Pc[number]/(Pc[number] + varVolt[number]);
    P[number] = (1-G[number])*Pc[number];
    Xp[number] = Xe[number];
    Zp[number] = Xp[number];
    Xe[number] = G[number]*(ROW-Zp[number])+Xp[number]; // "фильтрованное" значение
    return(Xe[number]);
}

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
