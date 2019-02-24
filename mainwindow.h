#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QGroupBox>

#include<QFile>
#include<QAction>
#include<QMenu>
#include<QMenuBar>
#include<QFileDialog>

#include<QAxObject>
#include<QAxWidget>
#include<QAxSelect>

#include<QString>
#include<QStringList>
#include<QMessageBox>
#include<QTableWidget>
#include<qheaderview.h>

#include<QTime>
#include<QCoreApplication>
#include<QEventLoop>
#include <QDesktopWidget>
#include<QApplication>
#include<QRadioButton>
#include<QTextLine>
#include<QSpinBox>
#include<math.h>
#include<complex>// 使用复数的类模板

#include "plottime.h"
#include "plotfrequency.h"
#include "leftwidget.h"

using namespace std;//在调试使用复数的时候 发现要加上
#define PI 3.14159
#define frequency 500

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //variables
    leftWidget *leftWidgetObj;
    plottime *plotTimeWidgetObj;
    plotFrequency *plotFreqWidgetObj;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *newPjtAction;
    QAction *importDataPathAction;
    QAction *importDataAction;
    QAction *scoreAction;
    QAction *outputCalculateResult;
    QAction *exitAction;

    QAction *helpAction;
//    QWidget *filterParawidget;


    //读取各个传感器的数据
    QStringList allCurveNamesList;//包含车辆种类、路况和测量位置种类的字符串信息，用于设置posSelectCombox和legend
    QVector< QVector<QVector<double>>> vectorOfAllExcelDataVector; //内层到外层的定义分别是：一个表的列、一个表的数据、多个表的数据
    QStringList car_evevtTypeNamesList;
    QStringList firstCarandPosName;
    int indexImportData=0;
    QVector<QStringList> eachExcelNameVec;//每个表的车辆种类、路况、测量位置


//    QDialog *dialog;
//    QTableWidget *filterParaTableWidget;
    QDialog *filteDialog;
    QDialog *identifyEventDialog;
    QDialog *calculateDialog;
    QDialog *customCalculateDialog;

    QDialogButtonBox *dialogBtnBox;
    QTableWidgetItem* tableWidgetItemName[15];
    QStringList *carInforList;

    QVector<QVector<double>> vecOfFilteredDataVec;
    QVector<int> indexofExcelNumFilterData, indexofColNumFilterData;

    QTableWidget *filterParaTableWidget;
    QRadioButton *firstRadioBtn_group1;
    QRadioButton *secondRadioBtn_group1;
    QRadioButton *thirdRadioBtn_group1;

    QRadioButton *firstRadioBtn_group2;
    QRadioButton *secondRadioBtn_group2;

    QLabel *eventTotalNums;
    QSpinBox *inputEventNumSpinBox;

    QVector<QVector<double>> vecOfRmsCutDataVec;
    QVector<QVector<double>> vecOfRmsCuttimeVec;

    int tableWidgetIndex=0;

    QCheckBox *checkBox1;
    QCheckBox *checkBox2;
    QCheckBox *checkBox3;
    QCheckBox *checkBox4;
    QCheckBox *checkBox5;
    QCheckBox *checkBox6;
    QCheckBox *checkBox7;

    QCheckBox *customCheckBox1;
    QCheckBox *customCheckBox2;
    QCheckBox *customCheckBox3;
    QCheckBox *customCheckBox4;
    QCheckBox *customCheckBox5;
    QCheckBox *customCheckBox6;
    QCheckBox *customCheckBox7;
    QCheckBox *customCheckBox8;
    QCheckBox *customCheckBox9;


    int beginindex;
    int endindex;

    QStringList importedDataPathList;

    QCustomPlot *customBarPlot;
    int heightToSaveGraph;
    QVector<double> validScoreVecToExcel;
    QStringList validItemListToExcel;

    QVector<QVector<int>> vecOfRmsCutBeginIndexVec;
    QVector<int> eventNumVec;
    QStringList rmsCutCarNameList;


    //functions
    void mainWindowLayout();
    void createMenus();
    QVector<QVector<double>>  readExcelValidData(QAxObject *workSheet,int);
    QStringList getCurveNameFunc(QString, QStringList);
    QTableWidgetItem** createTWitemsName(int itemNum,QStringList itemName);
    QString getNonFullNameofEvent(QString event);
    void sleep(unsigned int msec);
    QVector<QVector<double>> firFilter(QVector< QVector<QVector<double>>>, QVector<int>,QVector<int>,double, double);
    void fft(QVector<QVector<complex<double>>> , QVector<int >, QVector<int >);
    QVector<complex<double>> filterFft(QVector<complex<double>> , int );
    QVector<double> ifft(QVector<complex<double>> productResult, int log2n );

    double calculateFunc(QVector<QVector<double>>, int);
    void filterFreqSetDialog();
    void identifyEventSelectDialog(int );
    int rmsCutData(QVector<double> timeVec, QVector<double> originDataVec);
    void createCalculateDialog();

    QVector<double> rmsCutData_calculate(QString, QVector<double> originDataVec);

    void customCalculate(QString eventStr);
    void calculateSelectedItem(QStringList eventStrList);

    void createCustomCalculateDialog();
    void customCalculateSelectedItem(QStringList checkedItemList);

    QVector<double> calculateBounceFft(QVector<complex<double>> , int, int );
    QVector<double> getItemScore(QStringList, QVector<double> );

private slots:
    void newPjtSlotFunc();
    void importedDataPathSlotFunc();
    void importDataSlotFunc();
    void exitPjtSlotFunc();
    void helpSlotFunc();
    void rawDatatimeDomainPlotSlotFunc();
    void filteredDatatimeDomainPlotSlotFunc();
    void rawDatafftPlotSlotFunc();
    void filteredDatafftPlotSlotFunc();

    void filterDialogOkBtnSlotFunc();
    void filterDialogCancelBtnSlotFunc();

    void identifyEventSlotFunc();
    void identifyEventOkBtnSlotFunc();
    void identifyEventCancelSlotFunc();

    void calculateSlotFunc();
    void calculateOkBtnSlotFunc();
    void calculateCancelSlotFunc();

    void customCalculateOkBtnSlotFunc();
    void customCalculateCancelSlotFunc();

    void itemsChangedCtrlParaTableWidget(QTableWidgetItem*);
    void outputCalculateResultSlotFunc();
    void scoreSlotFunc();
    void contextMenuRequest(QPoint pos);
    void saveBarGraph();
};

#endif // MAINWINDOW_H

//傅里叶变换
//http://blog.csdn.net/fengyhack/article/details/42780785
//http://blog.csdn.net/zwlforever/article/details/2183049
//http://wlsyzx.yzu.edu.cn/kcwz/szxhcl/kechenneirong/jiaoan/jiaoan3.htm
//http://www.doc88.com/p-6931897171271.html
