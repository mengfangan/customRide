#include "leftwidget.h"

leftWidget::leftWidget(QWidget *parent) : QWidget(parent)
{
    createWidgetItem();
    layoutWidgetItem();
    connect(carSelectAddBtn,SIGNAL(clicked(bool)),this,SLOT(carTypeAddSlotFunc()));
    connect(carSelectMinusBtn,SIGNAL(clicked(bool)),this,SLOT(carTypeMinusSlotFunc()));
    connect(positionSelectAddBtn,SIGNAL(clicked(bool)),this,SLOT(positionTypeAddSlotFunc()));
    connect(positionSelectMinusBtn,SIGNAL(clicked(bool)),this,SLOT(positionTypeMinusSlotFunc()));
}

void leftWidget::createWidgetItem()
{
    carSelectLabel= new QLabel("Model Select:");
    carSelectLabel->setFixedSize(QSize(80,25));
    QStringList carType;
    carType<<"car1"<<"car2"<<"car3";
    carSelectCombox=new QComboBox;
    carSelectCombox->addItems(carType);
    carSelectCombox->setFixedHeight(25);
    connect(carSelectCombox,SIGNAL(currentTextChanged(QString)),this,SLOT(carTypeComboxChangedSlotFunc()));
    carSelectAddBtn =new QPushButton("+");
    carSelectAddBtn->setFixedSize(QSize(30,25));
    carSelectMinusBtn= new QPushButton("-");
    carSelectMinusBtn->setFixedSize(30,25);
    carSelectAddBtn->setDisabled(true);
    carSelectMinusBtn->setDisabled(true);

    PositionSelLable= new QLabel("Sensor Position:");
    PositionSelLable->setFixedSize(QSize(80,25));
    QStringList positionType;
    positionType<<"pos";
    positionSelectCombox =new QComboBox;
    positionSelectCombox->addItems(positionType);
    positionSelectCombox->setFixedHeight(25);
    positionSelectAddBtn= new QPushButton("+");
    positionSelectAddBtn->setFixedSize(QSize(30,25));
    positionSelectMinusBtn= new QPushButton("-");
    positionSelectMinusBtn->setFixedSize(30,25);
    positionSelectAddBtn->setDisabled(true);
    positionSelectMinusBtn->setDisabled(true);

    eventSelectLabel=new QLabel("Event：");
    eventSelectLabel->setFixedSize(QSize(80,25));
    eventSelectCombox =new QComboBox;
    QStringList eventType;
    eventType<<"event1"<<"event2"<<"event3";
    eventSelectCombox =new QComboBox;
    eventSelectCombox->addItems(eventType);
    eventSelectCombox->setFixedHeight(25);
    connect(eventSelectCombox,SIGNAL(currentIndexChanged(QString)),this,SLOT(eventComboxChangedSlotFunc()));

    importData= new QPushButton("导入数据");
    rawDatatimeDomainBtn= new QPushButton("Raw Data");
    filteredDatatimeDomainBtn= new QPushButton("Filter");

    rawDataFftBtn= new QPushButton("Raw FFT");
    filteredDataFfBtn= new QPushButton("Post FFT");
    identifyEventBtn= new QPushButton("Identification");
    calculateBtn= new QPushButton("Calculation");

    importData->setFixedSize(QSize(80,25));
    rawDatatimeDomainBtn->setFixedSize(QSize(80,25));
    filteredDatatimeDomainBtn->setFixedSize(QSize(80,25));
    rawDataFftBtn->setFixedSize(QSize(80,25));
    filteredDataFfBtn->setFixedSize(QSize(80,25));
    calculateBtn->setFixedSize(QSize(80,25));
    identifyEventBtn->setFixedSize(QSize(80,25));

    rawDataFftBtn->setDisabled(true);
    rawDatatimeDomainBtn->setDisabled(true);
    filteredDatatimeDomainBtn->setDisabled(true);
    filteredDataFfBtn->setDisabled(true);
    calculateBtn->setDisabled(true);
    identifyEventBtn->setDisabled(true);

    //计算结果表格
    calulateResultTabWideget= new QTableWidget(this);
    calulateResultTabWideget->setRowCount(4);
    calulateResultTabWideget->setColumnCount(3);
    calulateResultTabWideget->setShowGrid(true);

    calulateResultTabWideget->verticalHeader()->setVisible(false);
    calulateResultTabWideget->horizontalHeader()->setVisible(false);
    calulateResultTabWideget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    calulateResultTabWideget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);// 根据表格内容设置列宽
    calulateResultTabWideget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    calulateResultTabWideget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
//    calulateResultTabWideget->setColumnWidth(0,20);//第0列设置为固定宽度
    QStringList itemNames;
    itemNames<<"-"<<"-"<<"-"<<"-";
    QTableWidgetItem** itemNamesPara= createTWitemsName(4,itemNames);
    for(int i=0; i<4; i++)
    {
        calulateResultTabWideget->setItem(i,0,*(itemNamesPara+i));
//        calulateResultTabWideget->setFont(tableWidgetFont);
    }
}

QTableWidgetItem** leftWidget::createTWitemsName(int itemNum,QStringList itemName)
{
    for(int i=0; i<itemNum; i++)
    {
        QTableWidgetItem *item= new QTableWidgetItem(itemName.at(i));
        item->setTextAlignment(Qt::AlignLeft);
        item->setFlags(Qt::NoItemFlags);
//        item->setTextColor(QColor(230,110,30));
        item->setTextColor(QColor(Qt::black));
        item->setFont(QFont("Arial",10,QFont::Normal));
        tableWidgetItemName[i]=item;
    }
    return tableWidgetItemName;
}

void leftWidget::layoutWidgetItem()
{
    QFont fontGroupbox("Arial",9,QFont::Normal);
    QGroupBox *groupBox1=new QGroupBox("Model Select");
    groupBox1->setFont(fontGroupbox);
    gridLayout1= new QGridLayout;
    gridLayout1->addWidget(carSelectCombox,0,0);
    gridLayout1->addWidget(carSelectAddBtn,0,1);
    gridLayout1->addWidget(carSelectMinusBtn,0,2);
    groupBox1->setLayout(gridLayout1);

    QGroupBox *groupBox2=new QGroupBox("Sensor Position");
    groupBox2->setFont(fontGroupbox);
    gridLayout2= new QGridLayout;
    gridLayout2->addWidget(positionSelectCombox,0,0);
    gridLayout2->addWidget(positionSelectAddBtn,0,1);
    gridLayout2->addWidget(positionSelectMinusBtn,0,2);
    groupBox2->setLayout(gridLayout2);

    QGroupBox *groupBox3=new QGroupBox("Event");
    groupBox3->setFont(fontGroupbox);
    QHBoxLayout *hboxLayout3=new QHBoxLayout;
    hboxLayout3->addWidget(eventSelectCombox);
    groupBox3->setFixedHeight(70);
    groupBox3->setLayout(hboxLayout3);

    QGroupBox *groupBox4=new QGroupBox("Data Analysis");
    QHBoxLayout *hboxLayout4_1= new QHBoxLayout;
    groupBox4->setFont(fontGroupbox);
//    hboxLayout4_1->addWidget(importData);
    hboxLayout4_1->addWidget(rawDatatimeDomainBtn);
    hboxLayout4_1->addWidget(filteredDatatimeDomainBtn);
    hboxLayout4_1->addWidget(rawDataFftBtn);

    QHBoxLayout *hboxLayout4_2= new QHBoxLayout;
    hboxLayout4_2->addWidget(filteredDataFfBtn);
    hboxLayout4_2->addWidget(identifyEventBtn);
    hboxLayout4_2->addWidget(calculateBtn);

    QVBoxLayout *vboxLayout= new QVBoxLayout;
    vboxLayout->addLayout(hboxLayout4_1);
    vboxLayout->addLayout(hboxLayout4_2);

    groupBox4->setLayout(vboxLayout);

    QGroupBox *groupBox5=new QGroupBox("Reslut");
    groupBox5->setFont(fontGroupbox);
    QHBoxLayout *hboxLayout5=new QHBoxLayout;
    hboxLayout5->addWidget(calulateResultTabWideget);
    groupBox5->setFixedHeight(150);
    groupBox5->setLayout(hboxLayout5);

    QGridLayout *gridLayout= new QGridLayout;
    gridLayout->addWidget(groupBox1,0,0);
//    groupBox1
    gridLayout->addWidget(groupBox2,1,0);
    gridLayout->addWidget(groupBox3,2,0);
    gridLayout->addWidget(groupBox4,3,0);
    gridLayout->addWidget(groupBox5,4,0);

    this->setLayout(gridLayout);
}

//slots
void leftWidget::carTypeAddSlotFunc()
{
    if(carTypeAddNums<5)//最多6种车型
    {
        carTypeAddNums++;
        QComboBox *carSelectCombox1=new QComboBox;
        carSelectCombox1->addItems(carType);
        carSelectCombox1->setFixedHeight(25);
        carTypeComboxObjVec.append(carSelectCombox1);

        //增加车型后，更新posSelectCombox
        connect(carSelectCombox1,SIGNAL(currentTextChanged(QString)),this,SLOT(carTypeComboxChangedSlotFunc()));

        //为增加的combox 布局
        if(carTypeAddNums<3)
        {
            gridLayout1->addWidget(carSelectCombox1,carTypeAddNums,0);

            gridLayout1->removeWidget(carSelectAddBtn);
            gridLayout1->removeWidget(carSelectMinusBtn);
            gridLayout1->addWidget(carSelectAddBtn,carTypeAddNums,1);
            gridLayout1->addWidget(carSelectMinusBtn,carTypeAddNums,2);

        }
        else
        {
            gridLayout1->addWidget(carSelectCombox1,carTypeAddNums-3,1);

            gridLayout1->removeWidget(carSelectAddBtn);
            gridLayout1->removeWidget(carSelectMinusBtn);
            gridLayout1->addWidget(carSelectAddBtn,carTypeAddNums-3,2);
            gridLayout1->addWidget(carSelectMinusBtn,carTypeAddNums-3,3);
        }
    }
}

void leftWidget::carTypeMinusSlotFunc()
{
    if(carTypeAddNums>=1)
    {
        gridLayout1->removeWidget(carTypeComboxObjVec.at(carTypeAddNums-1));//没有这句也可
        delete carTypeComboxObjVec.at(carTypeAddNums-1);
        gridLayout1->removeWidget(carSelectAddBtn);
        gridLayout1->removeWidget(carSelectMinusBtn);
        carTypeAddNums--;
        carTypeComboxObjVec.removeLast();

        carTypeComboxChangedSlotFunc();

        if(carTypeAddNums<=2)
        {
            gridLayout1->addWidget(carSelectAddBtn,carTypeAddNums,1);
            gridLayout1->addWidget(carSelectMinusBtn,carTypeAddNums,2);
        }
        else if(carTypeAddNums>=3 && carTypeAddNums<5)
        {
            gridLayout1->addWidget(carSelectAddBtn,carTypeAddNums-3,2);
            gridLayout1->addWidget(carSelectMinusBtn,carTypeAddNums-3,3);
        }
    }
}

void leftWidget::positionTypeAddSlotFunc()
{
    if(positionTypeAddNums<5)//最多6种
    {
        positionTypeAddNums++;

        QComboBox *positionSelectCombox1=new QComboBox;
        positionSelectCombox1->addItems(positionType);
        positionSelectCombox1->setFixedHeight(25);
        positionComboxObjVec.append(positionSelectCombox1);
        if(positionTypeAddNums<3)
        {
            gridLayout2->addWidget(positionSelectCombox1,positionTypeAddNums,0);

            gridLayout2->removeWidget(positionSelectAddBtn);
            gridLayout2->removeWidget(positionSelectMinusBtn);
            gridLayout2->addWidget(positionSelectAddBtn,positionTypeAddNums,1);
            gridLayout2->addWidget(positionSelectMinusBtn,positionTypeAddNums,2);
        }
        else
        {
            gridLayout2->addWidget(positionSelectCombox1,positionTypeAddNums-3,1);

            gridLayout2->removeWidget(positionSelectAddBtn);
            gridLayout2->removeWidget(positionSelectMinusBtn);
            gridLayout2->addWidget(positionSelectAddBtn,positionTypeAddNums-3,2);
            gridLayout2->addWidget(positionSelectMinusBtn,positionTypeAddNums-3,3);
        }
    }
}

void leftWidget::positionTypeMinusSlotFunc()
{
    if(positionTypeAddNums>=1)
    {
        gridLayout2->removeWidget(positionComboxObjVec.at(positionTypeAddNums-1));
        delete positionComboxObjVec.at(positionTypeAddNums-1);
        gridLayout2->removeWidget(positionSelectAddBtn);
        gridLayout2->removeWidget(positionSelectMinusBtn);
        positionTypeAddNums--;
        positionComboxObjVec.removeLast();
        if(positionTypeAddNums<=2)
        {
            gridLayout2->addWidget(positionSelectAddBtn,positionTypeAddNums,1);
            gridLayout2->addWidget(positionSelectMinusBtn,positionTypeAddNums,2);
        }
        else if(positionTypeAddNums>=3 && positionTypeAddNums<5)
        {
            gridLayout2->addWidget(positionSelectAddBtn,positionTypeAddNums-3,2);
            gridLayout2->addWidget(positionSelectMinusBtn,positionTypeAddNums-3,3);
        }
    }
}

void leftWidget::setPosSelectCombox(QStringList posTypeNames)
{
    QStringList posNameList;
    for(int i=0; i<posTypeNames.size(); i++)
    {
        QStringList tempList= posTypeNames.at(i).split("-");
        tempList.removeAt(1);//移除中间的路况信息
        posNameList.append(tempList.join("-"));
    }

    positionSelectCombox->clear();
    positionSelectCombox->addItems(posNameList);
    positionType=posNameList;//用在positionTypeAddSlotFunc()
}

void leftWidget::setCarSelectCombox(QStringList car_evevtTypeNamesList)
{
    QStringList carNameList;
    for(int i=0; i<car_evevtTypeNamesList.size(); i++)
    {
        QStringList carList= car_evevtTypeNamesList.at(i).split("-");
        carNameList.append(carList.at(0));
    }

    carNameList.removeDuplicates();//移除重复的

    disconnect(carSelectCombox,SIGNAL(currentTextChanged(QString)),this,SLOT(carTypeComboxChangedSlotFunc()));
    carSelectCombox->clear();
    carSelectCombox->addItems(carNameList);
    carType=carNameList;
    connect(carSelectCombox,SIGNAL(currentTextChanged(QString)),this,SLOT(carTypeComboxChangedSlotFunc()));
}

void leftWidget::setEventCombox(QStringList car_evevtTypeNamesList)
{
    QStringList EventList;
    ValidEventList.clear();

    for(int i=0; i<car_evevtTypeNamesList.size(); i++)
    {
        QStringList event= car_evevtTypeNamesList.at(i).split("-");
        EventList.append(event.at(1));
    }
    EventList.removeDuplicates();//移除重复的
    for(int i=0; i<EventList.size();i++)
    {
        ValidEventList.append(getFullNameofEvent(EventList.at(i)));
    }
//    ValidEventList.append("Custom");
    disconnect(eventSelectCombox,SIGNAL(currentIndexChanged(QString)),this,SLOT(eventComboxChangedSlotFunc()));
    eventSelectCombox->clear();
    eventSelectCombox->addItems(ValidEventList);
    connect(eventSelectCombox,SIGNAL(currentIndexChanged(QString)),this,SLOT(eventComboxChangedSlotFunc()));
}

void leftWidget::getAllCurveNamesList(QStringList allCurveNamesList)
{
    allCurveNames=allCurveNamesList;
}

void leftWidget::carTypeComboxChangedSlotFunc()
{
    //获取所有carSelectcombox内容
    QStringList carSelectComboxText;
    QStringList posTypeNames,validPosTypeNames;

    carSelectComboxText.append(carSelectCombox->currentText());
    if(!carTypeComboxObjVec.isEmpty())
    {
        for(int i=0; i<carTypeComboxObjVec.size(); i++)
        {
            carSelectComboxText.append(carTypeComboxObjVec.at(i)->currentText());
        }
    }
    for(int i=0; i<carSelectComboxText.size();i++)
    {
        QString carName= carSelectComboxText.at(i);
        for(int j=0; j<allCurveNames.size(); j++)
        {
            QString curveName= allCurveNames.at(j);
            if(curveName.contains(carName) && curveName.contains(getNonFullNameofEvent(eventSelectCombox->currentText())))
            {
                posTypeNames.append(curveName);
            }
        }
    }
    for(int i=0; i<posTypeNames.size(); i++)
    {
        QStringList tempList= posTypeNames.at(i).split("-");
        tempList.removeAt(1);
        validPosTypeNames.append(tempList.join("-"));
    }

    positionSelectCombox->clear();
    positionSelectCombox->addItems(validPosTypeNames);

    positionType=validPosTypeNames;//用在positionTypeAddSlotFunc()
}

void leftWidget::eventComboxChangedSlotFunc()
{
    carTypeComboxChangedSlotFunc();
}

QString leftWidget::getNonFullNameofEvent(QString event)
{
    QString eventFullName;
    if(event=="LD ave")
        eventFullName="B1";
    else if(event=="Step")
        eventFullName="B2";
    else if(event=="Bump")
        eventFullName="B3";
    else if(event=="Primary road")
        eventFullName="C1";
    else if(event=="Secondary road")
        eventFullName="C2";
    else if(event=="Smooth road")
        eventFullName="C3";
    else if(event=="Sine Wave")
        eventFullName="C4";
    return eventFullName;
}

QString leftWidget::getFullNameofEvent(QString event)
{
    QString eventFullName;
    if(event=="B1")
        eventFullName="LD ave";
    else if(event=="B2")
        eventFullName="Step";
    else if(event=="B3")
        eventFullName="Bump";
    else if(event=="C1")
        eventFullName="Primary road";
    else if(event=="C2")
        eventFullName="Secondary road";
    else if(event=="C3")
        eventFullName="Smooth road";
    else if(event=="C4")
        eventFullName="Sine Wave";
    else
        eventFullName="Custom";
    return eventFullName;
}







