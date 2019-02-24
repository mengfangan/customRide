#ifndef LEFTWIDGET_H
#define LEFTWIDGET_H

#include <QWidget>
#include<QPushButton>
#include<QTextEdit>
#include<QComboBox>
#include<QLabel>
#include<QTableWidget>
#include<qheaderview.h>

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QGroupBox>

#include<QStringList>


#include<QDebug>
class leftWidget : public QWidget
{
    Q_OBJECT
public:
    explicit leftWidget(QWidget *parent = 0);

    //variables
    QLabel *carSelectLabel;
    QComboBox *carSelectCombox;
    QPushButton *carSelectAddBtn;
    QPushButton *carSelectMinusBtn;

    QLabel *PositionSelLable;
    QComboBox *positionSelectCombox;
    QPushButton *positionSelectAddBtn;
    QPushButton *positionSelectMinusBtn;

    QLabel *eventSelectLabel;
    QComboBox *eventSelectCombox;

    QPushButton *importData;
    QPushButton *printPjt;

    QPushButton *rawDataFftBtn;
    QPushButton *filteredDataFfBtn;
    QPushButton *identifyEventBtn;
    QPushButton *rawDatatimeDomainBtn;
    QPushButton *filteredDatatimeDomainBtn;
    QPushButton *calculateBtn;
//    QTextEdit *resultTextEdit;
    QTableWidget *calulateResultTabWideget;
    QTableWidgetItem* tableWidgetItemName[15];

    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;

    int carTypeAddNums=0;
    QStringList carType;
    QVector<QComboBox*> carTypeComboxObjVec;

    int positionTypeAddNums=0;
    QStringList positionType;
    QVector<QComboBox*> positionComboxObjVec;

    QStringList allCurveNames;
    QStringList ValidEventList;

    //functions
    void createWidgetItem();
    void layoutWidgetItem();
    QTableWidgetItem** createTWitemsName(int, QStringList);
    void setPosSelectCombox(QStringList posTypeName);
    void setCarSelectCombox(QStringList carInforList);
    void setEventCombox(QStringList eventList);
    void getAllCurveNamesList(QStringList);
    QString getFullNameofEvent(QString);
    QString getNonFullNameofEvent(QString event);

public slots:
    void carTypeAddSlotFunc();
    void carTypeMinusSlotFunc();
    void positionTypeAddSlotFunc();
    void positionTypeMinusSlotFunc();
    void carTypeComboxChangedSlotFunc();
    void eventComboxChangedSlotFunc();
};

#endif // LEFTWIDGET_H
