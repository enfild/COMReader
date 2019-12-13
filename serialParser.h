#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define SIZE_OF_PACK    9
#define ROT_PACK  1
#define XACCL_PACK  2
#define YACCL_PACK  3
#define NUMBER_VALUE    3

#define START_STREAM_FB    0
#define START_STREAM_SB    1

#define NUMBER_FByte_ROT    2
#define NUMBER_SByte_ROT    3
#define NUMBER_FByte_XACCL    5
#define NUMBER_SByte_XACCL    6
#define NUMBER_FByte_YACCL    7
#define NUMBER_SByte_YACCL    8


#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTreeWidgetItem>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort Qtport;

    float filtred_value(float ROW, int pack);
    float inclineScale(int16_t sensorData);
    float accelScale(int16_t sensorData);

    QByteArray data;
    quint8 mas[SIZE_OF_PACK];
    uint16_t ROT;
    uint16_t XrowACCL;
    uint16_t YrowACCL;
    float ROTS;
    float XACCL;
    float YACCL;
    // for filter

    float varProcess[NUMBER_VALUE] = {0.05, 0.05, 0.05};
float varVolt[NUMBER_VALUE] = {11.2, 11.2, 11.2};

float Pc[NUMBER_VALUE] = {0};
float G[NUMBER_VALUE] = {0};
float P[NUMBER_VALUE] = {1.0, 1.0, 1.0};
float Xp[NUMBER_VALUE] = {0};
float Zp[NUMBER_VALUE] = {0};
float Xe[NUMBER_VALUE] = {0};

private slots:
void testSerialPort();
void on_pushButton_clicked();
void on_comboBox_currentIndexChanged(const QString &arg1);
};

#endif // MAINWINDOW_H
