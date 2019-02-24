#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    leftWidgetObj= new leftWidget;
    plotTimeWidgetObj =new plottime;
    plotFreqWidgetObj=new plotFrequency;

    firstRadioBtn_group1= new QRadioButton;
    secondRadioBtn_group1= new QRadioButton;
    thirdRadioBtn_group1= new QRadioButton;
    firstRadioBtn_group1->setChecked(true);

    firstRadioBtn_group2= new QRadioButton;
    secondRadioBtn_group2= new QRadioButton;
    firstRadioBtn_group2->setChecked(true);

    mainWindowLayout();
    createMenus();

    connect(leftWidgetObj->importData,SIGNAL(clicked(bool)),this,SLOT(importDataSlotFunc()));
    connect(leftWidgetObj->rawDatatimeDomainBtn,SIGNAL(clicked(bool)),this,SLOT(rawDatatimeDomainPlotSlotFunc()));
    connect(leftWidgetObj->filteredDatatimeDomainBtn,SIGNAL(clicked(bool)),this,SLOT(filteredDatatimeDomainPlotSlotFunc()));
    connect(leftWidgetObj->rawDataFftBtn,SIGNAL(clicked(bool)),this,SLOT(rawDatafftPlotSlotFunc()));
    connect(leftWidgetObj->filteredDataFfBtn, SIGNAL(clicked(bool)),this,SLOT(filteredDatafftPlotSlotFunc()));
    connect(leftWidgetObj->identifyEventBtn,SIGNAL(clicked(bool)),this,SLOT(identifyEventSlotFunc()));
    connect(leftWidgetObj->calculateBtn,SIGNAL(clicked(bool)),this,SLOT(calculateSlotFunc()));
    setWindowIcon(QIcon(":/customRide.jpg"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::mainWindowLayout()
{
    QWidget *mainWidget= new QWidget;
    this->setCentralWidget(mainWidget);
    QVBoxLayout *vboxLayout= new QVBoxLayout;
    vboxLayout->addWidget(plotTimeWidgetObj);
    vboxLayout->addWidget(plotFreqWidgetObj);

    QHBoxLayout *hboxLayout= new QHBoxLayout;
    hboxLayout->addWidget(leftWidgetObj,2);
    hboxLayout->addLayout(vboxLayout,5);
    mainWidget->setLayout(hboxLayout);
}

void MainWindow::createMenus()
{
    QFont fontMenu("Arial",9,QFont::Normal);

    //createActions
    newPjtAction= new QAction(this);
    newPjtAction->setText("Clear All");
    newPjtAction->setFont(fontMenu);
    connect(newPjtAction,SIGNAL(triggered(bool)),this,SLOT(newPjtSlotFunc()));

    importDataPathAction= new QAction(this);
    importDataPathAction->setText("Imported Data Path");
    importDataPathAction->setFont(fontMenu);
    connect(importDataPathAction,SIGNAL(triggered(bool)),this,SLOT(importedDataPathSlotFunc()));

    importDataAction=new QAction(this);
    importDataAction->setText("Import");
    importDataAction->setFont(fontMenu);
    connect(importDataAction,SIGNAL(triggered()),this,SLOT(importDataSlotFunc()));

    scoreAction= new QAction(this);
    scoreAction->setText("Score");
    scoreAction->setFont(fontMenu);
    connect(scoreAction,SIGNAL(triggered(bool)),this,SLOT(scoreSlotFunc()));

    outputCalculateResult= new QAction(this);
    outputCalculateResult->setText("Save Result");
    outputCalculateResult->setFont(fontMenu);
    connect(outputCalculateResult,SIGNAL(triggered()),this,SLOT(outputCalculateResultSlotFunc()));

    exitAction= new QAction(this);
    exitAction->setText("Exit");
    exitAction->setFont(fontMenu);
    connect(exitAction,SIGNAL(triggered()),this,SLOT(exitPjtSlotFunc()));
    helpAction=new QAction(this);
    helpAction->setText("Help");
    helpAction->setFont(fontMenu);
    connect(helpAction,SIGNAL(triggered()),this,SLOT(helpSlotFunc()));

    fileMenu = menuBar()->addMenu("File");
    fileMenu->setFont(fontMenu);
    fileMenu->addAction(newPjtAction);
    fileMenu->addAction(importDataAction);
    fileMenu->addAction(importDataPathAction);
    fileMenu->addAction(scoreAction);
    fileMenu->addAction(outputCalculateResult);
    fileMenu->addAction(exitAction);

    helpMenu= menuBar()->addMenu("Help");
    helpMenu->setFont(fontMenu);
    helpMenu->addAction(helpAction);
}

//slots functions
void MainWindow::importDataSlotFunc()
{
    QStringList fileNames= QFileDialog::getOpenFileNames (this,tr("Import Data"),"/","EXCEL(*.xlsx *.xls))");
    if(!fileNames.isEmpty())
    {
        QMessageBox msgboxStartImport;
        msgboxStartImport.setText("Reading Data......");
        QFont font("Arial",10,QFont::Normal);
        msgboxStartImport.setFont(font);
        msgboxStartImport.show();
        sleep(800);//1000ms
        msgboxStartImport.hide();

        for(int i=0; i<fileNames.size(); i++)
        {
            importedDataPathList.append(fileNames.at(i));

            QAxObject excel("Excel.Application");
            QAxObject *workBooks= excel.querySubObject("workBooks");
            workBooks->dynamicCall("Open(const QString&)",QString(fileNames.at(i)));
            QAxObject *workBook= excel.querySubObject("ActiveWorkBook");
            QAxObject *workSheets= workBook->querySubObject("Sheets");

            int sheetCount =workSheets->property("Count").toInt();//工作表的数量
            if(sheetCount>0)
            {
                QAxObject *workSheet = workBook->querySubObject("Sheets(int)", 1);
                //EXCEL中已有数据的行列数，直接读取
                QAxObject *rowNumsObj = workSheet->querySubObject("Cells(int, int)", 2,2);//行
                int rowNum = rowNumsObj->property("Value").toInt();
                QAxObject *colNumsObj = workSheet->querySubObject("Cells(int, int)", 3,2);//列
                int colNum = colNumsObj->property("Value").toInt();

                //读取车辆类型和路况
                QAxObject *car_eventTypeObj = workSheet->querySubObject("Cells(int, int)", 5,2);
                QString car_eventTypeName=car_eventTypeObj->property("Value").toString();
                QStringList tempList=car_eventTypeName.split(QRegExp("\\s+"));
                QString tempCarName=tempList.at(0);
                QString tempEventName=tempList.at(1);
                if(tempEventName!="B1" && tempEventName!="B2" && tempEventName!="B3" && tempEventName!="B4" && tempEventName!="C1" && tempEventName!="C2" &&tempEventName!="C3" &&tempEventName!="C4")
                    tempEventName="Custom";
                QStringList tempList1;
                tempList1<<tempCarName<<tempEventName;
                QString car_eventName= tempList1.join("-");
                car_evevtTypeNamesList.append(car_eventName);//可以表征是哪个表

                //读取数据类型名称
                QStringList dataTypeNameList;
                for(int i=2; i<colNum+2; i++)
                {
                    QAxObject *dataTypeObj = workSheet->querySubObject("Cells(int, int)", 14,i);
                    QString dataTypeName = dataTypeObj->property("Value").toString();
                    dataTypeNameList.append(dataTypeName);
                }
                QStringList oneCarandPosTypeNames= getCurveNameFunc(car_eventName,dataTypeNameList);
                allCurveNamesList.append(oneCarandPosTypeNames);//车名+路况+被测位置
                eachExcelNameVec.append(oneCarandPosTypeNames);
                if(indexImportData==0)
                {
                    firstCarandPosName=oneCarandPosTypeNames;
                    indexImportData++;
                }

                //读取各个传感器的数据
                QVector<QVector<double>> vectorOfOneExcelDataVector;
                vectorOfOneExcelDataVector =readExcelValidData(workSheet,colNum);//按列存的excel数据
                vectorOfAllExcelDataVector.append(vectorOfOneExcelDataVector);
            }
            workBook->dynamicCall("Close(Boolen)", false);    // 关闭文件
            excel.dynamicCall("Quit(void)");    // 退出
        }
        //设置carSelectCombox
        leftWidgetObj->setCarSelectCombox(car_evevtTypeNamesList);
        //设置eventCombox
        leftWidgetObj->setEventCombox(car_evevtTypeNamesList);
        //设置postionSelectCombox
        leftWidgetObj->setPosSelectCombox(firstCarandPosName);//只设置第一种车型的 pos

        //将allCurveNamesList发送给leftWidget
        leftWidgetObj->getAllCurveNamesList(allCurveNamesList);

        //数据读取完毕
        leftWidgetObj->rawDataFftBtn->setDisabled(false);
        leftWidgetObj->rawDatatimeDomainBtn->setDisabled(false);
        leftWidgetObj->filteredDatatimeDomainBtn->setDisabled(false);
        leftWidgetObj->filteredDataFfBtn->setDisabled(false);
        leftWidgetObj->calculateBtn->setDisabled(false);

        leftWidgetObj->carSelectAddBtn->setDisabled(false);
        leftWidgetObj->carSelectMinusBtn->setDisabled(false);
        leftWidgetObj->positionSelectAddBtn->setDisabled(false);
        leftWidgetObj->positionSelectMinusBtn->setDisabled(false);
        leftWidgetObj->identifyEventBtn->setDisabled(false);

        QMessageBox msgboxEndImport;
        msgboxEndImport.setText("Done");
        msgboxEndImport.setFont(font);

        msgboxEndImport.show();
        sleep(800);//1000ms
        msgboxEndImport.hide();
    }
    else
    {
        return;
    }
}

QVector<QVector<double>>  MainWindow::readExcelValidData(QAxObject *workSheet,int colCount)
{
    //读取各个传感器的数据
    QVariant var;
    QAxObject *usedRange = workSheet->querySubObject("UsedRange");
    var = usedRange->dynamicCall("Value");
    delete usedRange;

    QVariantList varRows = var.toList();
    int rowCount = varRows.size();
    QVariantList rowData;
    QList<QList<QVariant> > res;
    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
    //提取出有效数据
    QVector<QVector<double>> vectorOfValidDataVector;//按列存储的有效数据  一个列是一个QVector<double>

    for(int i=0; i<=colCount; i++)
    {
        QVector<double> validDataVector;
        for(int j=14; j<rowCount; j++)
        {
            validDataVector.append(res.at(j).at(i).toDouble());
        }
        vectorOfValidDataVector.append(validDataVector);
    }
    return vectorOfValidDataVector;
}

QStringList MainWindow::getCurveNameFunc(QString car_eventName, QStringList dataTypeNameList)
{
    //提取数据名称关键词 作为positionSelect 的下拉菜单内容
    QStringList curveNamesList;
    for(int i=0; i<dataTypeNameList.size(); i++)
    {
        QStringList list;
        QString str=dataTypeNameList.at(i);
        list=str.split(QRegExp("\\s+"));
        list.removeLast();
        list.push_front(car_eventName);
        curveNamesList.append(list.join("-"));
    }
    return curveNamesList;
}

void MainWindow::rawDatatimeDomainPlotSlotFunc()//绘制原始数据时域图
{
    //获取各positionSelectCombox中的字符串 car_pos
    QStringList posSelectComboxStrList;
    posSelectComboxStrList.append(leftWidgetObj->positionSelectCombox->currentText());
    if(leftWidgetObj->positionTypeAddNums>0)
    {
        for(int i=0; i<leftWidgetObj->positionTypeAddNums; i++)
        {
            posSelectComboxStrList.append(leftWidgetObj->positionComboxObjVec.at(i)->currentText());
        }
    }

    //获取car_event   car_event_pos
    QStringList car_eventComboxList;
    QStringList car_event_posComboxList;
    for(int i=0; i<posSelectComboxStrList.size(); i++)
    {
        QStringList tempList= posSelectComboxStrList.at(i).split("-");
        QString tempCarName=tempList.at(0);
        tempList.removeFirst();
        QString tempPosName= tempList.join("-");

        tempList.clear();
        tempList.append(tempCarName);
        tempList.append(getNonFullNameofEvent(leftWidgetObj->eventSelectCombox->currentText()));
        car_eventComboxList.append(tempList.join("-"));
        tempList.append(tempPosName);
        car_event_posComboxList.append(tempList.join("-"));
    }

    plotTimeWidgetObj->createPlot(car_event_posComboxList);

    //获取要显示的曲线的数据
    QVector<int> indexofExcelNum, indexofColNum;
    for(int i=0; i<car_eventComboxList.size(); i++)
    {
        int index = car_evevtTypeNamesList.indexOf( car_eventComboxList.at(i));
        indexofExcelNum.append(index);//表征哪几个表的数据

        int index1= eachExcelNameVec.at(index).indexOf(car_event_posComboxList.at(i));
        indexofColNum.append(index1);
    }
    plotTimeWidgetObj->dataToPlot(vectorOfAllExcelDataVector,indexofExcelNum,indexofColNum);
}

//filter
void MainWindow::filteredDatatimeDomainPlotSlotFunc()
{
    //绘制滤波后数据时域图
    //获取各positionSelectCombox中的字符串 car_pos
    QStringList posSelectComboxStrList;
    posSelectComboxStrList.append(leftWidgetObj->positionSelectCombox->currentText());
    if(leftWidgetObj->positionTypeAddNums>0)
    {
        for(int i=0; i<leftWidgetObj->positionTypeAddNums; i++)
        {
            posSelectComboxStrList.append(leftWidgetObj->positionComboxObjVec.at(i)->currentText());
        }
    }

    //获取car_event   car_event_pos
    QStringList car_eventComboxList;
    QStringList car_event_posComboxList;
    for(int i=0; i<posSelectComboxStrList.size(); i++)
    {
        QStringList tempList= posSelectComboxStrList.at(i).split("-");
        QString tempCarName=tempList.at(0);
        tempList.removeFirst();
        QString tempPosName= tempList.join("-");

        tempList.clear();
        tempList.append(tempCarName);
        tempList.append(getNonFullNameofEvent(leftWidgetObj->eventSelectCombox->currentText()));
        car_eventComboxList.append(tempList.join("-"));
        tempList.append(tempPosName);
        car_event_posComboxList.append(tempList.join("-"));
    }

    plotTimeWidgetObj->createPlot(car_event_posComboxList);

    //获取要显示的曲线的数据
    indexofExcelNumFilterData.clear();
    indexofColNumFilterData.clear();
    for(int i=0; i<car_eventComboxList.size(); i++)
    {
        int index = car_evevtTypeNamesList.indexOf( car_eventComboxList.at(i));
        indexofExcelNumFilterData.append(index);//表征哪几个表的数据

        int index1= eachExcelNameVec.at(index).indexOf(car_event_posComboxList.at(i));
        indexofColNumFilterData.append(index1);
    }

    //频率选择的dialog
    filteDialog= new QDialog(this);
    filterFreqSetDialog();
}

void MainWindow::filterFreqSetDialog()
{
    QVBoxLayout *filterDialogLayout= new QVBoxLayout;

    QPushButton *okBtn= new QPushButton("Ok");
    QPushButton *cancelBtn= new QPushButton("Cancel");
    QFont font("Arial",10,QFont::Normal);
    okBtn->setFont(font);
    cancelBtn->setFont(font);
    okBtn->setFixedSize(50,30);
    cancelBtn->setFixedSize(50,30);
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(filterDialogOkBtnSlotFunc()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(filterDialogCancelBtnSlotFunc()));

    QString eventStr= leftWidgetObj->eventSelectCombox->currentText();//根据路况 确定通带频率
    if("LD ave"==eventStr || "Secondary road"==eventStr || "Smooth road"==eventStr)//B1 C2 C3有三种频率
    {
        if(firstRadioBtn_group1->isChecked())
            firstRadioBtn_group1->setChecked(true);
        if(secondRadioBtn_group1->isChecked())
            secondRadioBtn_group1->setCheckable(true);
        if(thirdRadioBtn_group1->isChecked())
            thirdRadioBtn_group1->setChecked(true);

        filterParaTableWidget= new QTableWidget(this);
        filterParaTableWidget->setColumnCount(4);
        filterParaTableWidget->setRowCount(3);
        filterParaTableWidget->setShowGrid(true);
        QFont tableWidgetFont("Arial",10,QFont::Normal);
        filterParaTableWidget->setFont(tableWidgetFont);

        filterParaTableWidget->verticalHeader()->setVisible(false);
        filterParaTableWidget->horizontalHeader()->setVisible(false);
        filterParaTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        filterParaTableWidget->setColumnWidth(0,50);//第0列设置为固定宽度
        filterParaTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        filterParaTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QStringList namesList;
        namesList<<"Fpass:"<<"Fpass:"<<"Fpass"<<"Fstop"<<"Fstop"<<"Fstop";
        QTableWidgetItem** itemNames= createTWitemsName(6,namesList);
        for(int i=0; i<3; i++)
        {
            filterParaTableWidget->setItem(i,0,*(itemNames+i));
            filterParaTableWidget->setItem(i,2,*(itemNames+i+3));
        }

        if("LD ave"==eventStr)//B1
        {
            QStringList valueList;
            valueList<<"0.1"<<"3"<<"9"<<"100"<<"9"<<"30";
            QTableWidgetItem** itemvalues= createTWitemsName(6,valueList);
            for(int i=0; i<3; i++)
            {
                filterParaTableWidget->setItem(i,1,*(itemvalues+i));
                filterParaTableWidget->setItem(i,3,*(itemvalues+i+3));
            }

        }
        else if("Secondary road"==eventStr)//c2
        {
            QStringList valueList;
            valueList<<"0.1"<<"3"<<"9"<<"100"<<"9"<<"30";
            QTableWidgetItem** itemvalues= createTWitemsName(6,valueList);
            for(int i=0; i<3; i++)
            {
                filterParaTableWidget->setItem(i,1,*(itemvalues+i));
                filterParaTableWidget->setItem(i,3,*(itemvalues+i+3));
            }
        }
        else if("Smooth road"==eventStr)//c3
        {
            QStringList valueList;
            valueList<<"0.1"<<"30"<<"50"<<"100"<<"100"<<"100";
            QTableWidgetItem** itemvalues= createTWitemsName(6,valueList);
            for(int i=0; i<3; i++)
            {
                filterParaTableWidget->setItem(i,1,*(itemvalues+i));
                filterParaTableWidget->setItem(i,3,*(itemvalues+i+3));
            }
        }
        QVBoxLayout *vboxLayout= new QVBoxLayout;
        vboxLayout->addWidget(firstRadioBtn_group1);
        vboxLayout->addWidget(secondRadioBtn_group1);
        vboxLayout->addWidget(thirdRadioBtn_group1);

        QHBoxLayout *hboxLayout= new QHBoxLayout;
        hboxLayout->addLayout(vboxLayout);
        hboxLayout->addWidget(filterParaTableWidget);

        filterDialogLayout->addLayout(hboxLayout);
        filteDialog->setFixedSize(380,180);
        filteDialog->setGeometry((this->width()-filteDialog->width())/2,(this->height()-filteDialog->height())/2,filteDialog->width(),filteDialog->height());//位置坐标 窗口大小

    }

    else if("Step"==eventStr || "Bump"==eventStr || "Primary road"==eventStr) //B2 B3 C1有两种频率
    {
        if(firstRadioBtn_group2->isChecked())
            firstRadioBtn_group2->setChecked(true);
        if(secondRadioBtn_group2->isChecked())
            secondRadioBtn_group2->setCheckable(true);

        filterParaTableWidget= new QTableWidget(this);
        filterParaTableWidget->setColumnCount(4);
        filterParaTableWidget->setRowCount(2);
        filterParaTableWidget->setShowGrid(true);
        QFont tableWidgetFont("Arial",10,QFont::Normal);
        filterParaTableWidget->setFont(tableWidgetFont);

        filterParaTableWidget->verticalHeader()->setVisible(false);
        filterParaTableWidget->horizontalHeader()->setVisible(false);
        filterParaTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        filterParaTableWidget->setColumnWidth(0,50);//第0列设置为固定宽度
        filterParaTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        filterParaTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QStringList namesList;
        namesList<<"Fpass:"<<"Fpass:"<<"Fstop"<<"Fstop";
        QTableWidgetItem** itemNames= createTWitemsName(4,namesList);
        for(int i=0; i<2; i++)
        {
            filterParaTableWidget->setItem(i,0,*(itemNames+i));
            filterParaTableWidget->setItem(i,2,*(itemNames+i+2));
        }

        if("Step"==eventStr)//B2
        {
            QStringList valueList;
            valueList<<"0.1"<<"5"<<"100"<<"40";
            QTableWidgetItem** itemvalues= createTWitemsName(4,valueList);
            for(int i=0; i<2; i++)
            {
                filterParaTableWidget->setItem(i,1,*(itemvalues+i));
                filterParaTableWidget->setItem(i,3,*(itemvalues+i+2));
            }
        }
        else if("Bump"==eventStr)//b3
        {
            QStringList valueList;
            valueList<<"0.1"<<"5"<<"100"<<"40";
            QTableWidgetItem** itemvalues= createTWitemsName(4,valueList);
            for(int i=0; i<2; i++)
            {
                filterParaTableWidget->setItem(i,1,*(itemvalues+i));
                filterParaTableWidget->setItem(i,3,*(itemvalues+i+2));
            }
        }
        else if("Primary road"==eventStr)//c1
        {
            QStringList valueList;
            valueList<<"0.1"<<"0.1"<<"100"<<"100";
            QTableWidgetItem** itemvalues= createTWitemsName(4,valueList);
            for(int i=0; i<2; i++)
            {
                filterParaTableWidget->setItem(i,1,*(itemvalues+i));
                filterParaTableWidget->setItem(i,3,*(itemvalues+i+2));
            }
        }

        QVBoxLayout *vboxLayout= new QVBoxLayout;
        vboxLayout->addWidget(firstRadioBtn_group2);
        vboxLayout->addWidget(secondRadioBtn_group2);

        QHBoxLayout *hboxLayout= new QHBoxLayout;
        hboxLayout->addLayout(vboxLayout);
        hboxLayout->addWidget(filterParaTableWidget);

        filterDialogLayout->addLayout(hboxLayout);
        filteDialog->setFixedSize(350,150);
        filteDialog->setGeometry((this->width()-filteDialog->width())/2,(this->height()-filteDialog->height())/2,filteDialog->width(),filteDialog->height());//位置坐标 窗口大小
    }

    else if("Custom"==eventStr)//custom
    {
        filterParaTableWidget= new QTableWidget(this);
        connect(filterParaTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemsChangedCtrlParaTableWidget(QTableWidgetItem*)));
        filterParaTableWidget->setColumnCount(4);
        filterParaTableWidget->setRowCount(1);
        filterParaTableWidget->setShowGrid(true);
        QFont tableWidgetFont("Arial",10,QFont::Normal);
        filterParaTableWidget->setFont(tableWidgetFont);

        filterParaTableWidget->verticalHeader()->setVisible(false);
        filterParaTableWidget->horizontalHeader()->setVisible(false);
        filterParaTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        filterParaTableWidget->setColumnWidth(0,50);//第0列设置为固定宽度
        filterParaTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        filterParaTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QStringList namesList;
        namesList<<"Fpass:"<<"Fstop:";
        QTableWidgetItem** itemNames= createTWitemsName(2,namesList);
        for(int i=0; i<1; i++)
        {
            filterParaTableWidget->setItem(i,0,*(itemNames+i));
            filterParaTableWidget->setItem(i,2,*(itemNames+i+1));
        }

        QStringList valueList;
        valueList<<"0.1"<<"100";
        QTableWidgetItem** itemvalues= createTWitemsName(2,valueList);
        for(int i=0; i<1; i++)
        {
            filterParaTableWidget->setItem(i,1,*(itemvalues+i));
            filterParaTableWidget->setItem(i,3,*(itemvalues+i+1));
        }

        filterDialogLayout->addWidget(filterParaTableWidget);
        filteDialog->setFixedSize(280,70);
        filteDialog->setGeometry((this->width()-filteDialog->width())/2,(this->height()-filteDialog->height())/2,filteDialog->width(),filteDialog->height());//位置坐标 窗口大小
    }

    else if("Sine Wave"==eventStr)
        return;

    QHBoxLayout *hboxLayout= new QHBoxLayout;
    hboxLayout->addWidget(okBtn);
    hboxLayout->addWidget(cancelBtn);
    filterDialogLayout->addLayout(hboxLayout);

    filteDialog->setLayout(filterDialogLayout);
//    filteDialog->setGeometry(500,300,350,180);//位置坐标 窗口大小
    filteDialog->show();
}

void MainWindow::itemsChangedCtrlParaTableWidget(QTableWidgetItem* item)
{
    QString newItemValue =item->text();
    int row =item->row();
    int col =item->column();
    disconnect(filterParaTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemsChangedCtrlParaTableWidget(QTableWidgetItem*)));
    QTableWidgetItem *newItem= new QTableWidgetItem(newItemValue);
    newItem->setTextAlignment(Qt::AlignCenter);
    newItem->setTextColor(QColor(Qt::black));
    filterParaTableWidget->setItem(row,col,newItem);
    connect(filterParaTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemsChangedCtrlParaTableWidget(QTableWidgetItem*)));
}

void MainWindow::filterDialogOkBtnSlotFunc()
{
    double fln,fhn;
    //获取由Dialog中选择的频率
    QString eventStr= leftWidgetObj->eventSelectCombox->currentText();//根据路况 确定通带频率
    if("LD ave"==eventStr || "Secondary road"==eventStr || "Smooth road"==eventStr)//B1 C2 C3有三种频率
    {
        if(firstRadioBtn_group1->isChecked())
        {
            fln=filterParaTableWidget->item(0,1)->text().toDouble();
            fhn=filterParaTableWidget->item(0,3)->text().toDouble();
        }
        else if(secondRadioBtn_group1->isChecked())
        {
            fln=filterParaTableWidget->item(1,1)->text().toDouble();
            fhn=filterParaTableWidget->item(1,3)->text().toDouble();
        }
        else if(thirdRadioBtn_group1->isChecked())
        {
            fln=filterParaTableWidget->item(2,1)->text().toDouble();
            fhn=filterParaTableWidget->item(2,3)->text().toDouble();
        }
    }

    else if("Step"==eventStr || "Bump"==eventStr || "Primary road"==eventStr) //B2 B3 C1有两种频率
    {
        if(firstRadioBtn_group2->isChecked())
        {
            fln=filterParaTableWidget->item(0,1)->text().toDouble();
            fhn=filterParaTableWidget->item(0,3)->text().toDouble();
        }
        else if(secondRadioBtn_group2->isChecked())
        {
            fln=filterParaTableWidget->item(1,1)->text().toDouble();
            fhn=filterParaTableWidget->item(1,3)->text().toDouble();
        }
    }
    else if("Custom"==eventStr)
    {
        fln=filterParaTableWidget->item(0,1)->text().toDouble();
        fhn=filterParaTableWidget->item(0,3)->text().toDouble();
    }

    filteDialog->close();
    QVector<QVector<double>> vecOfFilteredDataVector;
    vecOfFilteredDataVector=firFilter(vectorOfAllExcelDataVector,indexofExcelNumFilterData,indexofColNumFilterData,fln, fhn);
    plotTimeWidgetObj->filteredDataToPlot(vectorOfAllExcelDataVector,indexofExcelNumFilterData,vecOfFilteredDataVector);
}

QVector<QVector<double>> MainWindow::firFilter(QVector< QVector<QVector<double>>> vectorOfAllExcelDataVector, QVector<int> indexofExcelNum,QVector<int> indexofColNum, double fln, double fhn)//filter 阶数、采样频率、通带频率、阻带频率
{
    int n,n2,mid;
    double delay,fs,wc1,wc2,s;
    n=500; fs=frequency;

    vecOfFilteredDataVec.clear();
    for(int i=0; i<indexofExcelNum.size(); i++)
    {
        QVector<double> x,y;
        x=vectorOfAllExcelDataVector.at(indexofExcelNum.at(i)).at(indexofColNum.at(i)+1);
        QVector<double> h(x.size());
        if((n%2)==0)//偶数
        {
            n2=n/2-1;
            mid=1;
        }
        else//奇数
        {
            n2=n/2;
            mid=0;
        }
        delay=n/2.0;
        wc1=2*PI*fln/fs;
        wc2=2*PI*fhn/fs;
        for(int i=0; i<=n2; i++)
        {
            s=i-delay;
            h[i]=(sin(wc2*s)-sin(wc1*s))/(PI*s);
//            h[i]=h[i]*(0.5*(1.0-cos((2*i*PI/n))));//汉宁窗
            h[i]=h[i]*(0.54-0.46*cos(2*i*PI/(n)));//海明窗
            h[n-i]=h[i];
        }
        if(mid==1)
            h[n/2]=(wc2-wc1)/PI;//h(n)计算结束

        int xsize=x.size();
        //h(n) fft
        QVector<complex<double>> complexHDataVec;

        int log2n=log2(xsize);
        int NFFT=1<<(log2n+1);
        for(int i=0; i<xsize; i++)
        {
            complex<double> data(h[i],0);
            complexHDataVec.append(data);
        }
        for(int i=xsize; i<NFFT; i++)
        {
            complex<double> data(0,0);//补零
            complexHDataVec.append(data);
        }
        //x(n) fft
        QVector<complex<double>> complexXDataVec;
        for(int i=0; i<xsize; i++)
        {
            complex<double> data(x[i],0);
            complexXDataVec.append(data);
        }
        for(int i=xsize; i<NFFT; i++)
        {
            complex<double> data(0,0);//补零
            complexXDataVec.append(data);
        }

        QVector<complex<double>> resultOfHDataVec;
        QVector<complex<double>> resultOfXDataVec;
        resultOfHDataVec=filterFft(complexHDataVec, log2n+1);
        resultOfXDataVec=filterFft(complexXDataVec, log2n+1);

        //计算fft结果的乘积
        QVector<complex<double>> productResult;
        for(int i=0; i<resultOfHDataVec.size(); i++)
        {
            complex<double> data;
            data=resultOfHDataVec.at(i)*resultOfXDataVec.at(i);
            productResult.append(data);
        }
    //    将计算结果进行ifft
        y=ifft(productResult ,log2n+1);

        vecOfFilteredDataVec.append(y);
    }
    return vecOfFilteredDataVec;

}

void MainWindow::filterDialogCancelBtnSlotFunc()
{
    filteDialog->close();
}

QVector<complex<double>> MainWindow::filterFft(QVector<complex<double>> complexDataVec, int log2n)
{
    int dist,p;
    int length=1<<log2n;
    QVector<complex<double>> X2(length);
    QVector<complex<double>> X1(length);
    QVector<complex<double>> X;
    QVector<complex<double>> fftResultVec(length);

    double f=2*PI/length;
    double a=0;
    QVector<complex<double>> wn;
    for(int i=0; i<length/2; i++)
    {
        wn.append(complex<double> (cos(a),-sin(a)));
        a+=f;
    }
    for(int i=0; i<length; i++)
    {
        X1[i]=complexDataVec[i];
    }
    for(int k=0; k<log2n; k++)
    {
        for(int j=0; j<(1<<k); j++)
        {
            dist=1<<(log2n-k);
            for(int i=0; i<dist/2; i++)
            {
                p=j*dist;
                X2[i+p]=X1[i+p]+X1[i+p+dist/2];
                X2[i+p+dist/2]=(X1[i+p]-X1[i+p+dist/2])*wn.at(i * (1 << k));
            }
        }
        X=X1;
        X1=X2;
        X2=X;
    }
    for(int j=0; j<length; j++)
    {
        p=0;
        for(int i=0; i<log2n; i++)
        {
            if(j&(1<<i))
            {
                p+=1<<(log2n-i-1);
            }
        }
        fftResultVec[j]=X1[p];
    }
    return fftResultVec;
}

QVector<double> MainWindow::ifft(QVector<complex<double> > complexDataVec, int log2n)
{
    int dist,p;
    int length=1<<log2n;
    QVector<complex<double>> X2(length);
    QVector<complex<double>> X1(length);
    QVector<complex<double>> X;
    QVector<complex<double>> fftResultVec(length);

    double f=2*PI/length;
    double a=0;
    QVector<complex<double>> wn;
    for(int i=0; i<length/2; i++)
    {
        wn.append(complex<double> (cos(a),sin(a)));
        a+=f;
    }
    for(int i=0; i<length; i++)
    {
        X1[i]=complexDataVec[i];
    }
    for(int k=0; k<log2n; k++)
    {
        for(int j=0; j<(1<<k); j++)
        {
            dist=1<<(log2n-k);
            for(int i=0; i<dist/2; i++)
            {
                p=j*dist;
                X2[i+p]=X1[i+p]+X1[i+p+dist/2];
                X2[i+p+dist/2]=(X1[i+p]-X1[i+p+dist/2])*wn.at(i * (1 << k));
            }
        }
        X=X1;
        X1=X2;
        X2=X;
    }
    for(int j=0; j<length; j++)
    {
        p=0;
        for(int i=0; i<log2n; i++)
        {
            if(j&(1<<i))
            {
                p+=1<<(log2n-i-1);
            }
        }
        fftResultVec[j]=X1[p];
    }

    QVector<double> amplitudeVec;

    for(int i=0;i<length;i++)
    {
        double amplitude=(fftResultVec.at(i).real())/length;
        amplitudeVec.append(amplitude);
    }
    return amplitudeVec;
}
//filter end

//fft part
void MainWindow::rawDatafftPlotSlotFunc()
{
    //获取各positionSelectCombox中的字符串 car_pos
    QStringList posSelectComboxStrList;
    posSelectComboxStrList.append(leftWidgetObj->positionSelectCombox->currentText());
    if(leftWidgetObj->positionTypeAddNums>0)
    {
        for(int i=0; i<leftWidgetObj->positionTypeAddNums; i++)
        {
            posSelectComboxStrList.append(leftWidgetObj->positionComboxObjVec.at(i)->currentText());
        }
    }

    //获取car_event   car_event_pos
    QStringList car_eventComboxList;
    QStringList car_event_posComboxList;
    for(int i=0; i<posSelectComboxStrList.size(); i++)
    {
        QStringList tempList= posSelectComboxStrList.at(i).split("-");
        QString tempCarName=tempList.at(0);
        tempList.removeFirst();
        QString tempPosName= tempList.join("-");

        tempList.clear();
        tempList.append(tempCarName);
        tempList.append(getNonFullNameofEvent(leftWidgetObj->eventSelectCombox->currentText()));
        car_eventComboxList.append(tempList.join("-"));
        tempList.append(tempPosName);
        car_event_posComboxList.append(tempList.join("-"));
    }

    plotFreqWidgetObj->createPlot(car_event_posComboxList);

    //获取要显示的曲线的数据
    QVector<int> indexofExcelNum, indexofColNum;
    for(int i=0; i<car_eventComboxList.size(); i++)
    {
        int index = car_evevtTypeNamesList.indexOf( car_eventComboxList.at(i));
        indexofExcelNum.append(index);//表征哪几个表的数据

        int index1= eachExcelNameVec.at(index).indexOf(car_event_posComboxList.at(i));
        indexofColNum.append(index1);
    }
    //开始进行傅里叶变换
    QVector<QVector<double>> vecofdataVec;
    for(int i=0; i<indexofExcelNum.size(); i++)
    {
        vecofdataVec.append(vectorOfAllExcelDataVector.at(indexofExcelNum.at(i)).at(indexofColNum.at(i)+1));
    }
    QVector<QVector<complex<double>>> vecofcomplexDataVec;
    QVector<int >lengthVec;
    QVector<int >log2nVec;
    for(int i=0; i<vecofdataVec.size(); i++)
    {
        QVector<complex<double>> complexDataVec;
        int length=vecofdataVec.at(i).size();
        for(int j=0; j<length; j++)
        {
            complex<double> data(vecofdataVec.at(i).at(j),0);
            complexDataVec.append(data);
        }

        //判断length是否为2的幂
        int log2n= log2(length);
        int NFFT;
        if((length&(length-1))==0)//是2的次幂
        {
            NFFT=1<<(log2n+0);
            log2nVec.append(log2n+0);
        }

        else
        {
            NFFT=1<<(log2n+1);
            log2nVec.append(log2n+1);

            for(int j=length; j<NFFT; j++)
            {
                complex<double> data(0,0);//补零
                complexDataVec.append(data);
            }
        }
        vecofcomplexDataVec.append(complexDataVec);
        lengthVec.append(length);
    }
    fft(vecofcomplexDataVec, log2nVec, lengthVec);
}

void MainWindow::filteredDatafftPlotSlotFunc()
{
    if(!vecOfFilteredDataVec.isEmpty())
    {
        //获取各positionSelectCombox中的字符串 car_pos
        QStringList posSelectComboxStrList;
        posSelectComboxStrList.append(leftWidgetObj->positionSelectCombox->currentText());
        if(leftWidgetObj->positionTypeAddNums>0)
        {
            for(int i=0; i<leftWidgetObj->positionTypeAddNums; i++)
            {
                posSelectComboxStrList.append(leftWidgetObj->positionComboxObjVec.at(i)->currentText());
            }
        }

        //获取car_event   car_event_pos
        QStringList car_eventComboxList;
        QStringList car_event_posComboxList;
        for(int i=0; i<posSelectComboxStrList.size(); i++)
        {
            QStringList tempList= posSelectComboxStrList.at(i).split("-");
            QString tempCarName=tempList.at(0);
            tempList.removeFirst();
            QString tempPosName= tempList.join("-");

            tempList.clear();
            tempList.append(tempCarName);
            tempList.append(getNonFullNameofEvent(leftWidgetObj->eventSelectCombox->currentText()));
            car_eventComboxList.append(tempList.join("-"));
            tempList.append(tempPosName);
            car_event_posComboxList.append(tempList.join("-"));
        }

        plotFreqWidgetObj->createPlot(car_event_posComboxList);

        //获取要显示的曲线的数据 即滤波后的数据
        QVector<int> indexofExcelNum, indexofColNum;
        for(int i=0; i<car_eventComboxList.size(); i++)
        {
            int index = car_evevtTypeNamesList.indexOf( car_eventComboxList.at(i));
            indexofExcelNum.append(index);//表征哪几个表的数据

            int index1= eachExcelNameVec.at(index).indexOf(car_event_posComboxList.at(i));
            indexofColNum.append(index1);
        }
        //开始进行傅里叶变换
        QVector<QVector<complex<double>>> vecofcomplexDataVec;
        QVector<int >lengthVec;
        QVector<int >log2nVec;
        for(int i=0; i<vecOfFilteredDataVec.size(); i++)
        {
            QVector<complex<double>> complexDataVec;
            int length=vecOfFilteredDataVec.at(i).size();
            for(int j=0; j<length; j++)
            {
                complex<double> data(vecOfFilteredDataVec.at(i).at(j),0);
                complexDataVec.append(data);
            }

            //判断length是否为2的幂
            int log2n= log2(length);
            int NFFT;
            if((length&(length-1))==0)//是2的次幂
            {
                NFFT=1<<(log2n+0);
                log2nVec.append(log2n+0);
            }
            else
            {
                NFFT=1<<(log2n+1);
                log2nVec.append(log2n+1);

                for(int j=length; j<NFFT; j++)
                {
                    complex<double> data(0,0);//补零
                    complexDataVec.append(data);
                }
            }
            vecofcomplexDataVec.append(complexDataVec);
            lengthVec.append(length);
        }

       fft(vecofcomplexDataVec, log2nVec, lengthVec);
    }
    else
    {
        return;
    }

}

void MainWindow::fft(QVector<QVector<complex<double>>> vecofcomplexDataVec, QVector<int >log2nVec, QVector<int >lengthVec)
{
    QVector<QVector<double>> vecofFfvec;
    QVector<QVector<double>> vecofAmplitudeVec;
    for(int k=0; k<vecofcomplexDataVec.size(); k++)
    {
        double fs=frequency;
        int dist,p;
        int log2n=log2nVec.at(k);
        int length=1<<log2n;
        QVector<complex<double>> X2(length);
        QVector<complex<double>> X1(length);
        QVector<complex<double>> X;
        QVector<complex<double>> fftResultVec(length);

        double f=2*PI/length;
        double a=0;
        QVector<complex<double>> wn;
        for(int i=0; i<length/2; i++)
        {
            wn.append(complex<double> (cos(a),-sin(a)));
            a+=f;
        }
        for(int i=0; i<length; i++)
        {
            X1[i]=vecofcomplexDataVec.at(k).at(i);
        }
        for(int k=0; k<log2n; k++)
        {
            for(int j=0; j<(1<<k); j++)
            {
                dist=1<<(log2n-k);
                for(int i=0; i<dist/2; i++)
                {
                    p=j*dist;
                    X2[i+p]=X1[i+p]+X1[i+p+dist/2];
                    X2[i+p+dist/2]=(X1[i+p]-X1[i+p+dist/2])*wn.at(i * (1 << k));
                }
            }
            X=X1;
            X1=X2;
            X2=X;
        }
        for(int j=0; j<length; j++)
        {
            p=0;
            for(int i=0; i<log2n; i++)
            {
                if(j&(1<<i))
                {
                    p+=1<<(log2n-i-1);
                }
            }
            fftResultVec[j]=X1[p];
        }

        QVector<double> ffvec;
        QVector<double> amplitudeVec;

        for(int i=0;i<length;i++)
        {
            double amplitude=2*fabs(fftResultVec[i])/lengthVec.at(k);//fft 后的值*2/N,得到各频率分量的幅值
    //        double amplitude=fabs(fftResultVec[i]);
            amplitudeVec.append(amplitude);
        }

//        double unit=1.0/(length/2+1);
//        for(int i=0; i<length/2+1; i++)
//        {
//            double ffUnit=fs/2.0*unit*i;
//            ffvec.append(ffUnit);
//        }

        double unit=1.0/(length/2+1);
        int size=(length/2+1)*0.4;
        for(int i=0; i<size; i++)
        {
            double ffUnit=fs/2.0*unit*i;//频率只在0-100
            ffvec.append(ffUnit);
        }

        vecofFfvec.append(ffvec);
        vecofAmplitudeVec.append(amplitudeVec);
    }
    plotFreqWidgetObj->datatoPlot(vecofFfvec,vecofAmplitudeVec);
}

//fft part end
double MainWindow::calculateFunc(QVector<QVector<double>> vecOfFilteredDataVec, int xsize)
{
    QVector<double> filteredDataVec;
    filteredDataVec =vecOfFilteredDataVec.at(0);
    double squareSum=0;
    for(int i=0; i<xsize; i++)
    {
        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
    }
    double RMS=sqrt(squareSum/xsize);
    return RMS;
}

//event identify part
void MainWindow::identifyEventSlotFunc()
{  
    if(leftWidgetObj->eventSelectCombox->currentText()=="Bump" || leftWidgetObj->eventSelectCombox->currentText()=="Primary road" || leftWidgetObj->eventSelectCombox->currentText()=="Custom")//辨识路况
    {
        //获取positionSelectCombox中的字符串 car_pos
        QString posSelectComboxStr;
        posSelectComboxStr= leftWidgetObj->positionSelectCombox->currentText();

        //获取car_event   car_event_pos
        QString car_eventComboxStr;
        QString car_event_posComboxStr;

        QStringList tempList= posSelectComboxStr.split("-");
        QString tempCarName=tempList.at(0);
        tempList.removeFirst();
        QString tempPosName= tempList.join("-");

        tempList.clear();
        tempList.append(tempCarName);
        tempList.append(getNonFullNameofEvent(leftWidgetObj->eventSelectCombox->currentText()));
        car_eventComboxStr=tempList.join("-");
        tempList.append(tempPosName);
        car_event_posComboxStr=tempList.join("-");

        QStringList car_event_posComboxList;
        car_event_posComboxList.append(car_event_posComboxStr);
        plotTimeWidgetObj->createPlot(car_event_posComboxList);

        //获取要处理的曲线的数据
        int excelIndex = car_evevtTypeNamesList.indexOf( car_eventComboxStr);
        int columnIndex= eachExcelNameVec.at(excelIndex).indexOf(car_event_posComboxStr);

        QVector<double> timeVec= vectorOfAllExcelDataVector.at(excelIndex).at(0);
        QVector<double> originDataVec= vectorOfAllExcelDataVector.at(excelIndex).at(columnIndex+1);

        //let originData 0.1-100Hz filtered
        QVector< QVector<QVector<double>>> vecvecOfDataVec;//为了符合firFilter函数入口参数
        QVector<QVector<double>> vecOfDataVec;
        vecOfDataVec.append(originDataVec);
        vecvecOfDataVec.append(vecOfDataVec);
        QVector<int> indexOfData, indexOfData1;
        indexOfData.append(0);
        indexOfData1.append(-1);

        QVector<QVector<double>> vectorOfFilteredDataVec;
        vectorOfFilteredDataVec= firFilter(vecvecOfDataVec,indexOfData,indexOfData1,0.1, 100);

        //RMS截取数据
        int eventNum=rmsCutData(timeVec, vectorOfFilteredDataVec.at(0));
        QString eventNumStr= QString::number(eventNum,10);

        if(-1!=eventNum)
        {
            eventTotalNums=new QLabel;
            eventTotalNums->setText(eventNumStr);
            eventTotalNums->setFixedSize(80,25);
            identifyEventDialog=new QDialog(this);
            identifyEventSelectDialog(eventNum);
        }
    }
}

int MainWindow::rmsCutData(QVector<double> timeVec, QVector<double> originDataVec)
{
    QString eventName=leftWidgetObj->eventSelectCombox->currentText();
    QString positionName=leftWidgetObj->positionSelectCombox->currentText();

    if(eventName=="Bump" && positionName.contains("SR-X"))
    {
        int eventNum=0;
        const int point=15;//每15个点 计算RMS
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > beginIndexVex;
        QVector<int > endIndexVex;
        for(int i=0; i<groupNum; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    eventNum++;
                    beginIndexVex.append((i-1)*point);
                    endIndexVex.append((i-1)*point+550);
                    i=i+37;
                }
                else
                {
                    i--;
                }
            }
        }
        vecOfRmsCuttimeVec.clear();
        vecOfRmsCutDataVec.clear();
        for(int i=0; i<beginIndexVex.size(); i++)
        {
            int beginIndex=beginIndexVex.at(i);
            QVector<double> data, time;
            for(int j=beginIndex; j<beginIndex+550; j++)
            {
                data.append(originDataVec.at(j));
                time.append(timeVec.at(j));
            }
            vecOfRmsCutDataVec.append(data);
            vecOfRmsCuttimeVec.append(time);
        }

        vecOfRmsCutBeginIndexVec.append(beginIndexVex);
        QStringList templist=positionName.split("-");
        rmsCutCarNameList.append(templist.at(0));
        eventNumVec.append(eventNum);
        return eventNum;
    }

    else if(eventName=="Bump" && (positionName.contains("SR-Y") || positionName.contains("SR-Z") || positionName.contains("SW-X") || positionName.contains("SW-Y") || positionName.contains("SW-Z")))
    {
        QStringList templist=positionName.split("-");
        QString carName=templist.at(0);

        int indexOfSrx=rmsCutCarNameList.indexOf(carName);
        if(-1==indexOfSrx)
        {
            QMessageBox::information(this," ","Please identify SR-X of this car first");
            return -1;
        }
        else
        {
            vecOfRmsCuttimeVec.clear();
            vecOfRmsCutDataVec.clear();
            QVector<int > indexVec;
            indexVec=vecOfRmsCutBeginIndexVec.at(indexOfSrx);

            for(int i=0; i<indexVec.size(); i++)
            {
                int beginIndex=indexVec.at(i);
                QVector<double> data, time;
                for(int j=beginIndex; j<beginIndex+550; j++)
                {
                    data.append(originDataVec.at(j));
                    time.append(timeVec.at(j));
                }
                vecOfRmsCutDataVec.append(data);
                vecOfRmsCuttimeVec.append(time);
            }
            return eventNumVec.at(indexOfSrx);
        }
    }

    if(eventName=="Custom" && positionName.contains("SR-X"))
    {
        int eventNum=0;
        const int point=15;//每15个点 计算RMS
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > beginIndexVex;
        QVector<int > endIndexVex;
        for(int i=0; i<groupNum; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    eventNum++;
                    beginIndexVex.append((i-1)*point);
                    endIndexVex.append((i-1)*point+550);
                    i=i+37;
                }
                else
                {
                    i--;
                }
            }
        }
        vecOfRmsCuttimeVec.clear();
        vecOfRmsCutDataVec.clear();
        for(int i=0; i<beginIndexVex.size(); i++)
        {
            int beginIndex=beginIndexVex.at(i);
            QVector<double> data, time;
            for(int j=beginIndex; j<beginIndex+550; j++)
            {
                data.append(originDataVec.at(j));
                time.append(timeVec.at(j));
            }
            vecOfRmsCutDataVec.append(data);
            vecOfRmsCuttimeVec.append(time);
        }

        vecOfRmsCutBeginIndexVec.append(beginIndexVex);
        QStringList templist=positionName.split("-");
        rmsCutCarNameList.append(templist.at(0));
        eventNumVec.append(eventNum);
        return eventNum;
    }

    else if(eventName=="Custom" && (positionName.contains("SR-Y") || positionName.contains("SR-Z") || positionName.contains("SW-X") || positionName.contains("SW-Y") || positionName.contains("SW-Z")))
    {
        QStringList templist=positionName.split("-");
        QString carName=templist.at(0);

        int indexOfSrx=rmsCutCarNameList.indexOf(carName);
        if(-1==indexOfSrx)
        {
            QMessageBox::information(this," ","Please identify SR-X of this car first");
            return -1;
        }
        else
        {
            vecOfRmsCuttimeVec.clear();
            vecOfRmsCutDataVec.clear();
            QVector<int > indexVec;
            indexVec=vecOfRmsCutBeginIndexVec.at(indexOfSrx);

            for(int i=0; i<indexVec.size(); i++)
            {
                int beginIndex=indexVec.at(i);
                QVector<double> data, time;
                for(int j=beginIndex; j<beginIndex+550; j++)
                {
                    data.append(originDataVec.at(j));
                    time.append(timeVec.at(j));
                }
                vecOfRmsCutDataVec.append(data);
                vecOfRmsCuttimeVec.append(time);
            }
            return eventNumVec.at(indexOfSrx);
        }
    }


    else if(eventName=="Primary road")
    {
        int eventNum=0;
        const int point=15;//每15个点 计算RMS
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > indexVex;
        for(int i=0; i<groupNum-1; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    i++;
                    for(int j=i*point; j<(i+1)*point; j++)
                    {
                        squareSum +=originDataVec.at(j)*originDataVec.at(j);
                    }
                    double rms=sqrt(squareSum/point);
                    squareSum=0;

                    if(rms>0.5)
                    {
                        eventNum++;
                        indexVex.append((i-2)*point);
                        if(eventNum==1)
                        {
                            i=i+538;//16.2s->8100 data->540*15
                        }
                        if(eventNum==2)
                        {
                            i=i+348;//5245
                        }
                        if(eventNum==3)
                        {
                            i=i+228;//3450
                        }
                        if(eventNum==4)
                        {
                            i=i+93;//1415
                        }
                    }
                    else
                    {
                        i--;
                    }

                }

            }
        }

        vecOfRmsCuttimeVec.clear();
        vecOfRmsCutDataVec.clear();
        for(int i=0; i<indexVex.size(); i++)
        {
            int beginIndex=indexVex.at(i);
            QVector<double> data, time;
            if(i==0)//第一段 16.2s的数据
            {
                for(int j=beginIndex; j<beginIndex+8100; j++)
                {
                    data.append(originDataVec.at(j));
                    time.append(timeVec.at(j));
                }
            }
            if(i==1)
            {
                //do nothing
                for(int j=beginIndex; j<beginIndex+5245; j++)
                {
                    data.append(originDataVec.at(j));
                    time.append(timeVec.at(j));
                }
            }

            if(i==2)
            {
                for(int j=beginIndex; j<beginIndex+3450; j++)
                {
                    data.append(originDataVec.at(j));
                    time.append(timeVec.at(j));
                }
            }
            if(i==3)
            {
                for(int j=beginIndex; j<beginIndex+1415; j++)
                {
                    data.append(originDataVec.at(j));
                    time.append(timeVec.at(j));
                }
            }
            vecOfRmsCutDataVec.append(data);
            vecOfRmsCuttimeVec.append(time);
        }
        return eventNum;
    }
}

void MainWindow::identifyEventSelectDialog(int eventNum)
{
    QVBoxLayout *identifyEventDialogLayout= new QVBoxLayout;

    QPushButton *okBtn= new QPushButton("Ok");
    QPushButton *cancelBtn= new QPushButton("Cancel");
    QFont font("Arial",10,QFont::Normal);
    okBtn->setFont(font);
    cancelBtn->setFont(font);
    okBtn->setFixedSize(50,24);
    cancelBtn->setFixedSize(50,24);
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(identifyEventOkBtnSlotFunc()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(identifyEventCancelSlotFunc()));

    QLabel *eventTotalNumLabel= new QLabel;
    eventTotalNumLabel->setText("Event Total Number：");
    eventTotalNumLabel->setFont(font);
    eventTotalNumLabel->setFixedSize(QSize(80,25));

    QLabel *inputNumLabel= new QLabel;
    inputNumLabel->setText("Select：");
    inputNumLabel->setFont(font);
    inputNumLabel->setFixedSize(QSize(80,25));

    inputEventNumSpinBox= new QSpinBox;
    inputEventNumSpinBox->setMinimum(1);
    inputEventNumSpinBox->setMaximum(eventNum);
    inputEventNumSpinBox->setFixedSize(80,25);

    QHBoxLayout *hboxLayout1= new  QHBoxLayout;
    hboxLayout1->addWidget(eventTotalNumLabel);
    hboxLayout1->addWidget(eventTotalNums);

    QHBoxLayout *hboxLayout2= new  QHBoxLayout;
    hboxLayout2->addWidget(inputNumLabel);
    hboxLayout2->addWidget(inputEventNumSpinBox);

    QHBoxLayout *hboxLayout3= new  QHBoxLayout;
    hboxLayout3->addWidget(okBtn);
    hboxLayout3->addWidget(cancelBtn);

    identifyEventDialogLayout->addLayout(hboxLayout1);
    identifyEventDialogLayout->addLayout(hboxLayout2);
    identifyEventDialogLayout->addLayout(hboxLayout3);

    identifyEventDialog->setLayout(identifyEventDialogLayout);
    identifyEventDialog->setFixedSize(230,130);
    identifyEventDialog->setGeometry((this->width()-identifyEventDialog->width())/2,(this->height()-identifyEventDialog->height())/2,identifyEventDialog->width(),identifyEventDialog->height());//位置坐标 窗口大小
    identifyEventDialog->show();
}

void MainWindow::identifyEventOkBtnSlotFunc()
{
    int inputNum= inputEventNumSpinBox->text().toInt();
    plotTimeWidgetObj->eventDataToPlot(vecOfRmsCuttimeVec.at(inputNum-1),vecOfRmsCutDataVec.at(inputNum-1));
    identifyEventDialog->close();
}

void MainWindow::identifyEventCancelSlotFunc()
{
    identifyEventDialog->close();
}
//event identify part end

//calculate part
void MainWindow::calculateSlotFunc()
{
    QString eventStr= leftWidgetObj->eventSelectCombox->currentText();

    if("Custom"==eventStr)//自定义
    {
        customCalculateDialog= new QDialog(this);
        createCustomCalculateDialog();
    }
    else
    {
        calculateDialog= new QDialog(this);
        createCalculateDialog();
    }
}

QVector<double> MainWindow::rmsCutData_calculate(QString eventName,QVector<double> originDataVec)
{
    if("B3"==eventName)
    {
        int eventNum=0;
        //每15个点 计算RMS
        const int point=15;
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > indexVex;

        for(int i=0; i<groupNum; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    eventNum++;
                    indexVex.append((i-1)*point);
                    i=i+37;
                }
                else
                {
                    i--;
                }
            }
        }

        int beginIndex=indexVex.at(0);
        beginindex=indexVex.at(0);
        endindex=beginindex+550;


        QVector<double> data;
        for(int i=beginIndex; i<beginIndex+550; i++)
        {
            data.append(originDataVec.at(i));
        }
        return data;
    }

    else if("C1_FirstPart"==eventName)
    {
        int eventNum=0;
        const int point=15;//每15个点 计算RMS
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > indexVex;

        for(int i=0; i<groupNum-1; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    eventNum++;
                    indexVex.append((i-1)*point);
                    i=i+539;//16.2s->8100 data->540*15
                }
                else
                {
                    i--;
                }
            }
        }

        int beginIndex=indexVex.at(0);
        QVector<double> data;
        for(int j=beginIndex; j<beginIndex+8100; j++)
        {
            data.append(originDataVec.at(j));
        }
        return data;
    }

    else if("C1_SecondPart"==eventName)
    {
        int eventNum=0;
        const int point=15;//每15个点 计算RMS
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > indexVex;
        for(int i=0; i<groupNum-1; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    i++;
                    for(int j=i*point; j<(i+1)*point; j++)
                    {
                        squareSum +=originDataVec.at(j)*originDataVec.at(j);
                    }
                    double rms=sqrt(squareSum/point);
                    squareSum=0;

                    if(rms>0.5)
                    {
                        eventNum++;
                        indexVex.append((i-2)*point);
                        if(eventNum==1)
                        {
                            i=i+538;//16.2s->8100 data->540*15
                        }
                        if(eventNum==2)
                        {
                            i=i+348;//5245
                        }
                        if(eventNum==3)
                        {
                            i=i+228;//3450
                        }
                        if(eventNum==4)
                        {
                            i=i+93;//1415
                        }
                    }
                    else
                    {
                        i--;
                    }

                }

            }
        }
        int beginIndex=indexVex.at(2);
        QVector<double> data;

        for(int j=beginIndex; j<beginIndex+3450; j++)
        {
            data.append(originDataVec.at(j));
        }
        return data;
    }

    else if("C1_ThirdPart"==eventName)
    {
        int eventNum=0;
        const int point=15;//每15个点 计算RMS
        double squareSum=0;
        int groupNum=originDataVec.size()/point;

        QVector<int > indexVex;
        for(int i=0; i<groupNum-1; i++)
        {
            for(int j=i*point; j<(i+1)*point; j++)
            {
                squareSum +=originDataVec.at(j)*originDataVec.at(j);
            }
            double rms=sqrt(squareSum/point);
            squareSum=0;

            if(rms>0.5)
            {
                i++;
                for(int j=i*point; j<(i+1)*point; j++)
                {
                    squareSum +=originDataVec.at(j)*originDataVec.at(j);
                }
                double rms=sqrt(squareSum/point);
                squareSum=0;

                if(rms>0.5)
                {
                    i++;
                    for(int j=i*point; j<(i+1)*point; j++)
                    {
                        squareSum +=originDataVec.at(j)*originDataVec.at(j);
                    }
                    double rms=sqrt(squareSum/point);
                    squareSum=0;

                    if(rms>0.5)
                    {
                        eventNum++;
                        indexVex.append((i-2)*point);
                        if(eventNum==1)
                        {
                            i=i+538;//16.2s->8100 data->540*15
                        }
                        if(eventNum==2)
                        {
                            i=i+348;//5245
                        }
                        if(eventNum==3)
                        {
                            i=i+228;//3450
                        }
                        if(eventNum==4)
                        {
                            i=i+93;//1415
                        }
                    }
                    else
                    {
                        i--;
                    }

                }

            }
        }
        int beginIndex=indexVex.at(3);
        QVector<double> data;

        for(int j=beginIndex; j<beginIndex+1415; j++)
        {
            data.append(originDataVec.at(j));
        }
        return data;
    }
}

void MainWindow::createCalculateDialog()
{
    QVBoxLayout *calculateDialogLayout= new QVBoxLayout;

    QPushButton *okBtn= new QPushButton("Ok");
    QPushButton *cancelBtn= new QPushButton("Cancel");
    QFont font("Arial",10,QFont::Normal);
    okBtn->setFont(font);
    cancelBtn->setFont(font);
    okBtn->setFixedSize(50,24);
    cancelBtn->setFixedSize(50,24);
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(calculateOkBtnSlotFunc()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(calculateCancelSlotFunc()));

    QLabel *calculateItemSelectLabel= new QLabel;
    calculateItemSelectLabel->setText("Select：");
    calculateItemSelectLabel->setFixedSize(QSize(80,25));

    checkBox1=new QCheckBox;
    checkBox2=new QCheckBox;
    checkBox3=new QCheckBox;
    checkBox4=new QCheckBox;
    checkBox5=new QCheckBox;
    checkBox6=new QCheckBox;
    checkBox7=new QCheckBox;

    checkBox1->setText("LD ave");
    checkBox2->setText("Step");
    checkBox3->setText("Bump");
    checkBox4->setText("Primary road");
    checkBox5->setText("Secondary road");
    checkBox6->setText("Smooth road");
    checkBox7->setText("Sine Wave");

    checkBox1->setFont(font);
    checkBox2->setFont(font);
    checkBox3->setFont(font);
    checkBox4->setFont(font);
    checkBox5->setFont(font);
    checkBox6->setFont(font);
    checkBox7->setFont(font);

    checkBox1->setCheckable(false);
    checkBox2->setCheckable(false);
    checkBox3->setCheckable(false);
    checkBox4->setCheckable(false);
    checkBox5->setCheckable(false);
    checkBox6->setCheckable(false);
    checkBox7->setCheckable(false);

    for(int i=0; i<leftWidgetObj->ValidEventList.size(); i++)//获取eventComBox
    {
        QString eventStr=leftWidgetObj->ValidEventList.at(i);
        if("LD ave"==eventStr)
            checkBox1->setCheckable(true);
        else if("Step"==eventStr)
            checkBox2->setCheckable(true);
        else if("Bump"==eventStr)
            checkBox3->setCheckable(true);
        else if("Primary road"==eventStr)
            checkBox4->setCheckable(true);
        else if("Secondary road"==eventStr)
            checkBox5->setCheckable(true);
        else if("Smooth road"==eventStr)
            checkBox6->setCheckable(true);
        else if("Sine Wave"==eventStr)
            checkBox7->setCheckable(true);

    }


    QHBoxLayout *hboxLayout1= new  QHBoxLayout;
    hboxLayout1->addWidget(checkBox1);
    hboxLayout1->addWidget(checkBox2);

    QHBoxLayout *hboxLayout2= new  QHBoxLayout;
    hboxLayout2->addWidget(checkBox3);
    hboxLayout2->addWidget(checkBox4);

    QHBoxLayout *hboxLayout3= new  QHBoxLayout;
    hboxLayout3->addWidget(checkBox5);
    hboxLayout3->addWidget(checkBox6);

    QHBoxLayout *hboxLayout4= new  QHBoxLayout;
    hboxLayout4->addWidget(checkBox7);

//    calculateDialogLayout->addWidget(calculateItemSelectLabel);
    calculateDialogLayout->addLayout(hboxLayout1);
    calculateDialogLayout->addLayout(hboxLayout2);
    calculateDialogLayout->addLayout(hboxLayout3);
    calculateDialogLayout->addLayout(hboxLayout4);

    QHBoxLayout *hboxLayout= new  QHBoxLayout;
    hboxLayout->addWidget(okBtn);
    hboxLayout->addWidget(cancelBtn);
    calculateDialogLayout->addLayout(hboxLayout);

    calculateDialog->setLayout(calculateDialogLayout);
    calculateDialog->setFixedSize(300,170);
    calculateDialog->setGeometry((this->width()-calculateDialog->width())/2,(this->height()-calculateDialog->height())/2,calculateDialog->width(),calculateDialog->height());//位置坐标 窗口大小
    calculateDialog->show();
}

void MainWindow::calculateOkBtnSlotFunc()
{
    QStringList checkedItemList;

    if(checkBox1->isChecked())
        checkedItemList.append("LD ave");
    if(checkBox2->isChecked())
        checkedItemList.append("Step");
    if(checkBox3->isChecked())
        checkedItemList.append("Bump");
    if(checkBox4->isChecked())
        checkedItemList.append("Primary road");
    if(checkBox5->isChecked())
        checkedItemList.append("Secondary road");
    if(checkBox6->isChecked())
        checkedItemList.append("Smooth road");
    if(checkBox7->isChecked())
        checkedItemList.append("Sine Wave");

    calculateDialog->close();
    calculateSelectedItem(checkedItemList);
}

void MainWindow::calculateSelectedItem(QStringList eventStrList)
{
    for(int i=0; i<eventStrList.size(); i++)
    {
        QString eventStr=eventStrList.at(i);
        if("LD ave"==eventStr)//B1
        {
            //choppiness  filter 3-9Hz SR-Z
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_shake, indexofExcelNum_choppiness, indexofColumn_shake, indexofColumn_choppiness;
            QVector<QVector<double>> VecOfDataToFilteredVec_shake, VecOfDataToFilteredVec_choppiness;
            QStringList validCarNameList_shake, validCarNameList_choppiness;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="B1";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList;
                itemList<<"SR-X"<<"SR-Y"<<"SR-Z";
                QStringList carEventPosList;

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList.at(j));
                    QString carEventPosStr=tempList.join("-");
                    carEventPosList.append(carEventPosStr);
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_shake.append(index1);
                        if(j==2)
                        {
                            indexofExcelNum_choppiness.append(index1);
                        }
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_shake.append(index2);
                            VecOfDataToFilteredVec_shake.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_shake.append(carName);
                            if(j==2)
                            {
                                indexofColumn_choppiness.append(index2);
                                VecOfDataToFilteredVec_choppiness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                                validCarNameList_choppiness.append(carName);
                            }
                        }
                    }
                }
            }

            validCarNameList_choppiness.removeDuplicates();
            validCarNameList_shake.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_choppiness;
            vecOfFilteredDataVector_choppiness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_choppiness,indexofColumn_choppiness,3, 9);//滤波后的数据

            for(int i=0; i<validCarNameList_choppiness.size(); i++)
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_choppiness.at(i);

                int xsize=VecOfDataToFilteredVec_choppiness.at(i).size();//xsize 必须是原始数据的size
                if(xsize>20000)
                    xsize=20000;//前40s的数据
                double squareSum=0;
                for(int i=0; i<xsize; i++)
                {
                    squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                }
                double RMS=sqrt(squareSum/xsize);

                QString carNameStr= validCarNameList_choppiness.at(i);
                QString itemNameStr="Choppiness_B1";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_choppiness.at(i) && calculateItemName=="Choppiness_B1")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            //shake 9-30 SR-XYZ
            QVector<QVector<double>> vecOfFilteredDataVector_shake;
            vecOfFilteredDataVector_shake=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_shake,indexofColumn_shake,9, 30);//滤波后的数据
            for(int i=0; i<validCarNameList_shake.size(); i++)
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_shake.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_shake.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    if(xsize>20000)
                        xsize=20000;//前40s的数据
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsShake=sqrt(rmsSum);

                QString carNameStr= validCarNameList_shake.at(i);
                QString itemNameStr="Shake_B1";
                QString valueStr=QString::number(rmsShake);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_shake.at(i) && calculateItemName=="Shake_B1")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

        }

        if("Step"==eventStr)//B2
        {
            //Impact Harshness: filter 1-100Hz  SR-XYZ  ///Sw Impact:SW-XYZ
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }
            QVector<int> indexofExcelNum_harshness, indexofExcelNum_sw, indexofColumn_harshness, indexofColumn_sw;
            QVector<QVector<double>> VecOfDataToFilteredVec_harshness, VecOfDataToFilteredVec_sw;
            QStringList validCarNameList_harshness, validCarNameList_sw;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="B2";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_harshness;
                itemList_harshness<<"SR-X"<<"SR-Y"<<"SR-Z";

                QStringList itemList_sw;
                itemList_sw<<"SW-X"<<"SW-Y"<<"SW-Z";

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList_harshness.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);

                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_harshness.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);

                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_harshness.append(index2);
                            VecOfDataToFilteredVec_harshness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_harshness.append(carName);
                        }
                    }
                }

                for(int j=0; j<3; j++)
                {
                    tempList.append(itemList_sw.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);

                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_sw.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_sw.append(index2);
                            VecOfDataToFilteredVec_sw.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_sw.append(carName);
                        }
                    }
                }
            }

            validCarNameList_harshness.removeDuplicates();
            validCarNameList_sw.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_harshness;
            vecOfFilteredDataVector_harshness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_harshness,indexofColumn_harshness,1,100);//滤波后的数据

            for(int i=0; i<validCarNameList_harshness.size(); i++)//Impact Harshness vdv
            {
                double totalSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_harshness.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_harshness.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    double squareSquareSum=0;//四次方和
                    for(int i=0; i<xsize; i++)
                    {
                        squareSquareSum +=filteredDataVec.at(i)*filteredDataVec.at(i)*filteredDataVec.at(i)*filteredDataVec.at(i);
                    }

                    totalSum+=squareSquareSum;
                }
                double temp=sqrt(totalSum);
                double vdv=sqrt(temp);

                QString carNameStr= validCarNameList_harshness.at(i);
                QString itemNameStr="Impact Harshness_B2";
                QString valueStr=QString::number(vdv);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_harshness.at(i) && calculateItemName=="Impact Harshness_B2")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            QVector<QVector<double>> vecOfFilteredDataVector_sw;
            vecOfFilteredDataVector_sw=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_sw,indexofColumn_sw,1, 100);//滤波后的数据

            for(int i=0; i<validCarNameList_sw.size(); i++)//Sw Impact vdv
            {
                double totalSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_sw.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_sw.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    double squareSquareSum=0;//四次方和
                    for(int i=0; i<xsize; i++)
                    {
                        squareSquareSum +=filteredDataVec.at(i)*filteredDataVec.at(i)*filteredDataVec.at(i)*filteredDataVec.at(i);
                    }

                    totalSum+=squareSquareSum;
                }
                double temp=sqrt(totalSum);
                double vdv=sqrt(temp);

                QString carNameStr= validCarNameList_sw.at(i);
                QString itemNameStr="Sw Impact_B2";
                QString valueStr=QString::number(vdv);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_sw.at(i) && calculateItemName=="Sw Impact_B2")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

        }

        if("Bump"==eventStr)//B3
        {
            //Impact Harshness: filter 1-100Hz  SR-XYZ  ///Sw Impact:SW-XYZ
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_harshness, indexofExcelNum_sw, indexofColumn_harshness, indexofColumn_sw;
            QVector<QVector<double>> VecOfDataToFilteredVec_harshness, VecOfDataToFilteredVec_sw;
            QStringList validCarNameList_harshness, validCarNameList_sw;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="B3";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_harshness;
                itemList_harshness<<"SR-X"<<"SR-Y"<<"SR-Z";

                QStringList itemList_sw;
                itemList_sw<<"SW-X"<<"SW-Y"<<"SW-Z";

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList_harshness.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_harshness.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_harshness.append(index2);
                            VecOfDataToFilteredVec_harshness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_harshness.append(carName);
                        }

                    }
                }

                for(int j=0; j<3; j++)
                {
                    tempList.append(itemList_sw.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_sw.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_sw.append(index2);
                            VecOfDataToFilteredVec_sw.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_sw.append(carName);
                        }
                    }
                }
            }
            validCarNameList_harshness.removeDuplicates();
            validCarNameList_sw.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_harshness;
            vecOfFilteredDataVector_harshness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_harshness,indexofColumn_harshness,0.1,100);//滤波后的数据

            QVector<int> beginIndexVec;
            QVector<int> endIndexVec;//用于记录在SR-XYZ路况识别时截取的起始点
            for(int i=0; i<validCarNameList_harshness.size(); i++)//Impact Harshness vdv
            {
                double totalSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_harshness.at(i*3+j);
                    QVector<double> rmsCutfilteredDataVec;
                    rmsCutfilteredDataVec=rmsCutData_calculate("B3",filteredDataVec);//路况识别，识别的第一段数据计算vdv
                    beginIndexVec.append(beginindex);
                    endIndexVec.append(endindex);

                    int xsize=rmsCutfilteredDataVec.size();
                    double squareSquareSum=0;//四次方和
                    for(int i=0; i<xsize; i++)
                    {
                        squareSquareSum +=rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i);
                    }

                    totalSum+=squareSquareSum;
                }
                double temp=sqrt(totalSum);
                double vdv=sqrt(temp);

                QString carNameStr= validCarNameList_harshness.at(i);
                QString itemNameStr="Impact Harshness_B3";
                QString valueStr=QString::number(vdv);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_harshness.at(i) && calculateItemName=="Impact Harshness_B3")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            QVector<QVector<double>> vecOfFilteredDataVector_sw;
            vecOfFilteredDataVector_sw=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_sw,indexofColumn_sw,0.1, 100);//滤波后的数据
            for(int i=0; i<validCarNameList_sw.size(); i++)//Sw Impact vdv
            {
                double totalSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_sw.at(i*3+j);

                    QVector<double> rmsCutfilteredDataVec;                    
                    //截取出路况段
                    int beginIndex=beginIndexVec.at(i*3+j);
                    int endIndex=endIndexVec.at(i*3+j);
                    for(int i=beginIndex; i<endIndex; i++)
                    {
                        rmsCutfilteredDataVec.append(filteredDataVec.at(i));
                    }
                    int xsize=rmsCutfilteredDataVec.size();

                    double squareSquareSum=0;//四次方和
                    for(int i=0; i<xsize; i++)
                    {
                        squareSquareSum +=rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i);
                    }

                    totalSum+=squareSquareSum;
                }
                double temp=sqrt(totalSum);
                double vdv=sqrt(temp);

                QString carNameStr= validCarNameList_sw.at(i);
                QString itemNameStr="Sw Impact_B3";
                QString valueStr=QString::number(vdv);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_sw.at(i) && calculateItemName=="Sw Impact_B3")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            QVector<QVector<double>> vecOfFilteredDataVector_afterShake;//先滤波，滤波延时相同，根据计算Impact Harshness时截取的位置，确定该项的截取位置
            vecOfFilteredDataVector_afterShake=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_harshness,indexofColumn_harshness,5,40);
            for(int i=0; i<validCarNameList_harshness.size(); i++)
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_afterShake.at(i*3+j);
                    //截取出路况段
                    int beginIndex=beginIndexVec.at(i*3+j);
                    int endIndex=endIndexVec.at(i*3+j);
                    QVector<double> rmsCutDataVec;
                    for(int i=beginIndex; i<endIndex; i++)
                    {
                        rmsCutDataVec.append(filteredDataVec.at(i));
                    }
                    int xsize=rmsCutDataVec.size();
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=rmsCutDataVec.at(i)*rmsCutDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsAfterShake=sqrt(rmsSum);

                QString carNameStr= validCarNameList_harshness.at(i);
                QString itemNameStr="Aftershake_B3";
                QString valueStr=QString::number(rmsAfterShake);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_harshness.at(i) && calculateItemName=="Aftershake_B3")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        if("Secondary road"==eventStr)//C2
        {
            //choppiness  filter 3-9Hz
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_shake, indexofExcelNum_choppiness, indexofColumn_shake, indexofColumn_choppiness;
            QVector<QVector<double>> VecOfDataToFilteredVec_shake, VecOfDataToFilteredVec_choppiness;
            QStringList validCarNameList_shake, validCarNameList_choppiness;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="C2";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList;
                itemList<<"SR-X"<<"SR-Y"<<"SR-Z";
                QStringList carEventPosList;

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList.at(j));
                    QString carEventPosStr=tempList.join("-");
                    carEventPosList.append(carEventPosStr);
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_shake.append(index1);
                        if(j==2)
                            indexofExcelNum_choppiness.append(index1);

                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_shake.append(index2);
                            VecOfDataToFilteredVec_shake.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_shake.append(carName);
                            if(j==2)
                            {
                                indexofColumn_choppiness.append(index2);
                                VecOfDataToFilteredVec_choppiness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                                validCarNameList_choppiness.append(carName);
                            }
                        }

                    }
                }
            }
            validCarNameList_choppiness.removeDuplicates();
            validCarNameList_shake.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_choppiness;
            vecOfFilteredDataVector_choppiness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_choppiness,indexofColumn_choppiness,3, 9);//滤波后的数据

            for(int i=0; i<validCarNameList_choppiness.size(); i++)
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_choppiness.at(i);

                int xsize=VecOfDataToFilteredVec_choppiness.at(i).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                if(xsize>14000)
                    xsize=14000;

                double squareSum=0;
                for(int i=0; i<xsize; i++)
                {
                    squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                }
                double RMS=sqrt(squareSum/xsize);

                QString carNameStr= validCarNameList_choppiness.at(i);
                QString itemNameStr="Choppiness_C2";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_choppiness.at(i) && calculateItemName=="Choppiness_C2")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            //shake 9-30
            QVector<QVector<double>> vecOfFilteredDataVector_shake;
            vecOfFilteredDataVector_shake=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_shake,indexofColumn_shake,9, 30);//滤波后的数据
            for(int i=0; i<validCarNameList_shake.size(); i++)
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_shake.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_shake.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    if(xsize>14000)
                        xsize=14000;
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsShake=sqrt(rmsSum);

                QString carNameStr= validCarNameList_shake.at(i);
                QString itemNameStr="Shake_C2";
                QString valueStr=QString::number(rmsShake);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_shake.at(i) && calculateItemName=="Shake_C2")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        if("Smooth road"==eventStr)//C3
        {
            //plushness: filter 30-100Hz  SR-XYZ  ///Sw Plushness: 50-100 SW-XYZ
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_plushness, indexofExcelNum_sw, indexofColumn_plushness, indexofColumn_sw;
            QVector<QVector<double>> VecOfDataToFilteredVec_plushness, VecOfDataToFilteredVec_sw;
            QStringList validCarNameList_plushness, validCarNameList_sw;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="C3";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_plushness;
                itemList_plushness<<"SR-X"<<"SR-Y"<<"SR-Z";

                QStringList itemList_sw;
                itemList_sw<<"SW-X"<<"SW-Y"<<"SW-Z";

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList_plushness.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_plushness.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_plushness.append(index2);
                            VecOfDataToFilteredVec_plushness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_plushness.append(carName);
                        }
                    }
                }

                for(int j=0; j<3; j++)
                {
                    tempList.append(itemList_sw.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_sw.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_sw.append(index2);
                            VecOfDataToFilteredVec_sw.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_sw.append(carName);
                        }

                    }
                }
            }
            validCarNameList_plushness.removeDuplicates();
            validCarNameList_sw.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_plushness;
            vecOfFilteredDataVector_plushness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_plushness,indexofColumn_plushness,30,100);//滤波后的数据
            for(int i=0; i<validCarNameList_plushness.size(); i++)//plushness RMS
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_plushness.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_plushness.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    if(xsize>10000)
                        xsize=10000;
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsPlushness=sqrt(rmsSum);

                QString carNameStr= validCarNameList_plushness.at(i);
                QString itemNameStr="Plushness_C3";
                QString valueStr=QString::number(rmsPlushness);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_plushness.at(i) && calculateItemName=="Plushness_C3")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            QVector<QVector<double>> vecOfFilteredDataVector_sw;
            vecOfFilteredDataVector_sw=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_sw,indexofColumn_sw,50, 100);//滤波后的数据
            for(int i=0; i<validCarNameList_sw.size(); i++)//Sw Plushness RMS
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_sw.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_sw.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    if(xsize>10000)
                        xsize=10000;
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsSw=sqrt(rmsSum);

                QString carNameStr= validCarNameList_sw.at(i);
                QString itemNameStr="Sw Plushness_C3";
                QString valueStr=QString::number(rmsSw);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_sw.at(i) && calculateItemName=="Sw Plushness_C3")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

        }

        if("Primary road"==eventStr)//C1
        {
            //aruptness  0.1-10Hz
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }
            qDebug()<<"carList:"<<carNameList;
            QVector<int> indexofExcelNum, indexofColumn;
            QVector<QVector<double>> VecOfDataToFilteredVec;
            QStringList validCarNameList;

            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="C1";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QString itemStr="SP-Z";
                tempList.append(itemStr);
                QString carEventPosStr=tempList.join("-");

                int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                if(-1==index1)
                    continue;
                else
                {
                    indexofExcelNum.append(index1);
                    int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                    if(-1==index2)
                        continue;
                    else
                    {
                        indexofColumn.append(index2);
                        VecOfDataToFilteredVec.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                        validCarNameList.append(carName);
                    }

                }
            }
            validCarNameList.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_aruptness;
            vecOfFilteredDataVector_aruptness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum,indexofColumn,0.1,100);//滤波后的数据
            for(int i=0; i<validCarNameList.size(); i++)//aruptness_c1_1
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_aruptness.at(i);
                QVector<double> rmsCutfilteredDataVec;
                rmsCutfilteredDataVec=rmsCutData_calculate("C1_FirstPart",filteredDataVec);//路况识别，识别的第一段数据计算
                int xsize=rmsCutfilteredDataVec.size();

                //将0.1-100 Hz的数据求导
                QVector<double> diffedDataVec;
                for(int i=0; i<xsize-1; i++)
                {
                    double difData=(rmsCutfilteredDataVec.at(i+1)-rmsCutfilteredDataVec.at(i))/0.002;
                    diffedDataVec.append(difData);
                }

                QVector< QVector<QVector<double>>> vecVecVec; QVector<QVector<double>> vecVec;//为了与firFilter函数入口参数保持一致
                vecVec.append(diffedDataVec);
                vecVecVec.append(vecVec);
                QVector<int> indexVec1,indexVec2;
                indexVec1.append(0);
                indexVec2.append(-1);

                QVector<QVector<double>> vecOfdataFiltered;
                vecOfdataFiltered= firFilter(vecVecVec,indexVec1,indexVec2,0.1,10);//将求导的数据滤波 0.1-10Hz

                QVector<double> dataFiltered= vecOfdataFiltered.at(0);
                double dataSize=diffedDataVec.size();//
                double squareSum=0;//RMS
                for(int i=0; i<dataSize; i++)
                {
                    squareSum +=dataFiltered.at(i)*dataFiltered.at(i);
                }
                double RMS=sqrt(squareSum/dataSize);

                QString carNameStr= validCarNameList.at(i);
                QString itemNameStr="Aruptness-1_C1";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList.at(i) && calculateItemName=="Aruptness-1_C1")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            for(int i=0; i<validCarNameList.size(); i++)//aruptness_c1_2
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_aruptness.at(i);
                QVector<double> rmsCutfilteredDataVec;
                rmsCutfilteredDataVec=rmsCutData_calculate("C1_SecondPart",filteredDataVec);//路况识别，识别的第一段数据计算
                int xsize=rmsCutfilteredDataVec.size();

                //将0.1-100 Hz的数据求导
                QVector<double> diffedDataVec;
                for(int i=0; i<xsize-1; i++)
                {
                    double difData=(rmsCutfilteredDataVec.at(i+1)-rmsCutfilteredDataVec.at(i))/0.002;
                    diffedDataVec.append(difData);
                }

                QVector< QVector<QVector<double>>> vecVecVec; QVector<QVector<double>> vecVec;//为了与firFilter函数入口参数保持一致
                vecVec.append(diffedDataVec);
                vecVecVec.append(vecVec);
                QVector<int> indexVec1,indexVec2;
                indexVec1.append(0);
                indexVec2.append(-1);

                QVector<QVector<double>> vecOfdataFiltered;
                vecOfdataFiltered= firFilter(vecVecVec,indexVec1,indexVec2,0.1,10);//将求导的数据滤波 0.1-10Hz

                QVector<double> dataFiltered= vecOfdataFiltered.at(0);
                double dataSize=diffedDataVec.size();//
                double squareSum=0;//RMS
                for(int i=0; i<dataSize; i++)
                {
                    squareSum +=dataFiltered.at(i)*dataFiltered.at(i);
                }
                double RMS=sqrt(squareSum/dataSize);

                QString carNameStr= validCarNameList.at(i);
                QString itemNameStr="Aruptness-2_C1";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList.at(i) && calculateItemName=="Aruptness-2_C1")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            for(int i=0; i<validCarNameList.size(); i++)//aruptness_c1_3
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_aruptness.at(i);
                QVector<double> rmsCutfilteredDataVec;
                rmsCutfilteredDataVec=rmsCutData_calculate("C1_ThirdPart",filteredDataVec);//路况识别，识别的第一段数据计算
                int xsize=rmsCutfilteredDataVec.size();

                //将0.1-100 Hz的数据求导
                QVector<double> diffedDataVec;
                for(int i=0; i<xsize-1; i++)
                {
                    double difData=(rmsCutfilteredDataVec.at(i+1)-rmsCutfilteredDataVec.at(i))/0.002;
                    diffedDataVec.append(difData);
                }

                QVector< QVector<QVector<double>>> vecVecVec; QVector<QVector<double>> vecVec;//为了与firFilter函数入口参数保持一致
                vecVec.append(diffedDataVec);
                vecVecVec.append(vecVec);
                QVector<int> indexVec1,indexVec2;
                indexVec1.append(0);
                indexVec2.append(-1);

                QVector<QVector<double>> vecOfdataFiltered;
                vecOfdataFiltered= firFilter(vecVecVec,indexVec1,indexVec2,0.1,10);//将求导的数据滤波 0.1-10Hz

                QVector<double> dataFiltered= vecOfdataFiltered.at(0);
                double dataSize=diffedDataVec.size();//
                double squareSum=0;//RMS
                for(int i=0; i<dataSize; i++)
                {
                    squareSum +=dataFiltered.at(i)*dataFiltered.at(i);
                }
                double RMS=sqrt(squareSum/dataSize);

                QString carNameStr= validCarNameList.at(i);
                QString itemNameStr="Aruptness-3_C1";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList.at(i) && calculateItemName=="Aruptness-3_C1")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

        }

        if("Sine Wave"==eventStr)//C4
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_bounce, indexofColumn_bounce;
            QVector<QVector<double>> VecOfDataToFftVec_bounce;
            QStringList validCarNameList;
           //bounce
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName_bounce=carNameList.at(i);
                QString eventName_bounce="C4";
                QStringList tempList_bounce;
                tempList_bounce.append(carName_bounce);
                tempList_bounce.append(eventName_bounce);
                QString carEventStr_bounce= tempList_bounce.join("-");

                QString itemStr_bounce="SP-Z";
                tempList_bounce.append(itemStr_bounce);
                QString carEventPosStr_bounce=tempList_bounce.join("-");

                int index1_bounce=car_evevtTypeNamesList.indexOf(carEventStr_bounce);
                if(-1==index1_bounce)
                    continue;
                else
                {
                    indexofExcelNum_bounce.append(index1_bounce);
                    int index2_bounce= eachExcelNameVec.at(index1_bounce).indexOf(carEventPosStr_bounce);
                    if(-1==index2_bounce)
                        continue;
                    else
                    {
                        indexofColumn_bounce.append(index2_bounce);
                        VecOfDataToFftVec_bounce.append(vectorOfAllExcelDataVector.at(index1_bounce).at(index2_bounce+1));
                        validCarNameList.append(carName_bounce);
                    }
                }

                validCarNameList.removeDuplicates();

                for(int i=0; i<validCarNameList.size(); i++)//bounce_c4
                {
                    QVector<complex<double>> complexDataVec;
                    int length=7000;
                    for(int j=0; j<length; j++)
                    {
                        complex<double> data(VecOfDataToFftVec_bounce.at(i).at(j),0);
                        complexDataVec.append(data);
                    }

                    int log2n= log2(length);
                    int NFFT=1<<(log2n+1);

                    for(int j=length; j<NFFT; j++)
                    {
                        complex<double> data(0,0);//补零
                        complexDataVec.append(data);
                    }
                    QVector<double> bounceResultVec;
                    bounceResultVec= calculateBounceFft(complexDataVec, log2n+1, length);

                    QStringList itemNameList;
                    itemNameList<<"MaxAmp_C4"<<"MaxFre_C4"<<"SecAmp_C4"<<"SecFre_C4";
                    for(int k=0; k<bounceResultVec.size(); k++)//分别是最大值、最大值的频率、次大值、次大值的频率
                    {
                        QString carNameStr= validCarNameList.at(i);
                        QString itemNameStr=itemNameList.at(k);
                        QString valueStr=QString::number(bounceResultVec.at(k));
                        QStringList itemNames;
                        itemNames<<carNameStr<<itemNameStr<<valueStr;

                        QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                        //查看结果表中是否已经有该项计算指标，若有，就不在添加
                        bool hasCalculationItem=false;
                        for(int j=0; j<tableWidgetIndex; j++)
                        {
                            QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                            QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                            if(carName==validCarNameList.at(i) && calculateItemName==itemNameList.at(k))
                                hasCalculationItem=true;
                        }
                        if(!hasCalculationItem)
                        {
                            if(tableWidgetIndex<4)
                            {
                                for(int i=0; i<3; i++)
                                {
                                    leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                                }
                                tableWidgetIndex++;
                            }
                            else    //tableWidgetIndex>=4
                            {
                                leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                                for(int i=0; i<3; i++)
                                {
                                    leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                                }
                                tableWidgetIndex++;
                            }
                        }
                    }
                }
            }
        }

    }
}

void MainWindow::calculateCancelSlotFunc()
{
    calculateDialog->close();
}

QVector<double> MainWindow::calculateBounceFft(QVector<complex<double>> complexDataVec, int log2n, int realLength)
{
    QVector<double> resultVec;

    double fs=frequency;
    int dist,p;
    int length=1<<log2n;
    QVector<complex<double>> X2(length);
    QVector<complex<double>> X1(length);
    QVector<complex<double>> X;
    QVector<complex<double>> fftResultVec(length);

    double f=2*PI/length;
    double a=0;
    QVector<complex<double>> wn;
    for(int i=0; i<length/2; i++)
    {
        wn.append(complex<double> (cos(a),-sin(a)));
        a+=f;
    }
    for(int i=0; i<length; i++)
    {
        X1[i]=complexDataVec.at(i);
    }
    for(int k=0; k<log2n; k++)
    {
        for(int j=0; j<(1<<k); j++)
        {
            dist=1<<(log2n-k);
            for(int i=0; i<dist/2; i++)
            {
                p=j*dist;
                X2[i+p]=X1[i+p]+X1[i+p+dist/2];
                X2[i+p+dist/2]=(X1[i+p]-X1[i+p+dist/2])*wn.at(i * (1 << k));
            }
        }
        X=X1;
        X1=X2;
        X2=X;
    }
    for(int j=0; j<length; j++)
    {
        p=0;
        for(int i=0; i<log2n; i++)
        {
            if(j&(1<<i))
            {
                p+=1<<(log2n-i-1);
            }
        }
        fftResultVec[j]=X1[p];
    }

    QVector<double> ffvec;
    QVector<double> amplitudeVec;

    for(int i=0;i<length;i++)
    {
        double amplitude=2*fabs(fftResultVec[i])/realLength;//fft 后的值*2/N,得到各频率分量的幅值
        amplitudeVec.append(amplitude);
    }

    double unit=1.0/(length/2+1);
    int size=(length/2+1)*0.4;
    for(int i=0; i<size; i++)
    {
        double ffUnit=fs/2.0*unit*i;//频率只在0-100
        ffvec.append(ffUnit);
    }
//    plotTimeWidgetObj->dataPlot(ffvec,amplitudeVec );

    int beginIndex=(int)size*0.01;
    QVector<double> topAmplitudeVec;
    QVector<double> topFreqVec;
    QVector<int> topValueIndexVec;
    for(int i=beginIndex+1; i<size-1; i++)//频率在1-100
    {
        if(amplitudeVec.at(i)-amplitudeVec.at(i-1)>0 && amplitudeVec.at(i)-amplitudeVec.at(i+1)>0)//找出每个尖峰点
        {
            topAmplitudeVec.append(amplitudeVec.at(i));
            topFreqVec.append(ffvec.at(i));
            topValueIndexVec.append(i);
        }
    }

    double maxAmplitude;
    double maxFreq;
    maxAmplitude=topAmplitudeVec.at(0);
    maxFreq=topFreqVec.at(0);
    int maxValueIndex;
    for(int i=1; i<topAmplitudeVec.size(); i++)//找出尖峰的最大值和次大值
    {
        if(topAmplitudeVec.at(i)-maxAmplitude>0)
        {
            maxAmplitude=topAmplitudeVec.at(i);
            maxFreq=topFreqVec.at(i);
            maxValueIndex=i;
        }
    }
    resultVec.append(maxAmplitude);//添加最大的幅值
    resultVec.append(maxFreq);//添加最大幅值对应的频率

    topAmplitudeVec.removeAt(maxValueIndex);
    topFreqVec.removeAt(maxValueIndex);

    maxAmplitude=topAmplitudeVec.at(0);
    maxFreq=topFreqVec.at(0);
    for(int i=1; i<topAmplitudeVec.size()-1; i++)//找出尖峰的最大值和次大值
    {
        if(topAmplitudeVec.at(i)-maxAmplitude>0)
        {
            maxAmplitude=topAmplitudeVec.at(i);
            maxFreq=topFreqVec.at(i);
        }
    }

    resultVec.append(maxAmplitude);//添加次大的幅值
    resultVec.append(maxFreq);//添加次大幅值对应的频率

    return resultVec;
}

//custom caculate part
void MainWindow::createCustomCalculateDialog()
{
    QVBoxLayout *calculateDialogLayout= new QVBoxLayout;

    QPushButton *okBtn= new QPushButton("Ok");
    QPushButton *cancelBtn= new QPushButton("Cancel");
    QFont font("Arial",10,QFont::Normal);
    okBtn->setFont(font);
    cancelBtn->setFont(font);
    okBtn->setFixedSize(50,24);
    cancelBtn->setFixedSize(50,24);
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(customCalculateOkBtnSlotFunc()));
    connect(cancelBtn,SIGNAL(clicked(bool)),this,SLOT(customCalculateCancelSlotFunc()));

    QLabel *calculateItemSelectLabel= new QLabel;
    calculateItemSelectLabel->setText("Select：");
    calculateItemSelectLabel->setFont(font);
    calculateItemSelectLabel->setFixedSize(QSize(80,25));

    customCheckBox1=new QCheckBox;
    customCheckBox2=new QCheckBox;
    customCheckBox3=new QCheckBox;
    customCheckBox4=new QCheckBox;
    customCheckBox5=new QCheckBox;
    customCheckBox6=new QCheckBox;
    customCheckBox7=new QCheckBox;
    customCheckBox8=new QCheckBox;
    customCheckBox9=new QCheckBox;

    customCheckBox1->setText("Bounce");
    customCheckBox2->setText("Aruptness");
    customCheckBox3->setText("Choppiness");
    customCheckBox4->setText("Shake");
    customCheckBox5->setText("Plushness");
    customCheckBox6->setText("Sw Plushness");
    customCheckBox7->setText("Impact Harshness");
    customCheckBox8->setText("Sw Impact");
    customCheckBox9->setText("Aftershake");

    customCheckBox1->setFont(font);
    customCheckBox2->setFont(font);
    customCheckBox3->setFont(font);
    customCheckBox4->setFont(font);
    customCheckBox5->setFont(font);
    customCheckBox6->setFont(font);
    customCheckBox7->setFont(font);
    customCheckBox8->setFont(font);
    customCheckBox9->setFont(font);

    QHBoxLayout *hboxLayout1= new  QHBoxLayout;
    hboxLayout1->addWidget(customCheckBox1);
    hboxLayout1->addWidget(customCheckBox2);

    QHBoxLayout *hboxLayout2= new  QHBoxLayout;
    hboxLayout2->addWidget(customCheckBox3);
    hboxLayout2->addWidget(customCheckBox4);

    QHBoxLayout *hboxLayout3= new  QHBoxLayout;
    hboxLayout3->addWidget(customCheckBox5);
    hboxLayout3->addWidget(customCheckBox6);

    QHBoxLayout *hboxLayout4= new  QHBoxLayout;
    hboxLayout4->addWidget(customCheckBox7);
    hboxLayout4->addWidget(customCheckBox8);

    QHBoxLayout *hboxLayout5= new  QHBoxLayout;
    hboxLayout5->addWidget(customCheckBox9);

//    calculateDialogLayout->addWidget(calculateItemSelectLabel);
    calculateDialogLayout->addLayout(hboxLayout1);
    calculateDialogLayout->addLayout(hboxLayout2);
    calculateDialogLayout->addLayout(hboxLayout3);
    calculateDialogLayout->addLayout(hboxLayout4);
    calculateDialogLayout->addLayout(hboxLayout5);

    QHBoxLayout *hboxLayout= new  QHBoxLayout;
    hboxLayout->addWidget(okBtn);
    hboxLayout->addWidget(cancelBtn);
    calculateDialogLayout->addLayout(hboxLayout);

    customCalculateDialog->setLayout(calculateDialogLayout);
    customCalculateDialog->setFixedSize(280,150);
    customCalculateDialog->setGeometry((this->width()-customCalculateDialog->width())/2,(this->height()-customCalculateDialog->height())/2,customCalculateDialog->width(),customCalculateDialog->height());//位置坐标 窗口大小
    customCalculateDialog->show();
}

void MainWindow::customCalculateOkBtnSlotFunc()
{
    QStringList checkedItemList;

    if(customCheckBox1->isChecked())
        checkedItemList.append("Bounce");
    if(customCheckBox2->isChecked())
        checkedItemList.append("Aruptness");
    if(customCheckBox3->isChecked())
        checkedItemList.append("Choppiness");
    if(customCheckBox4->isChecked())
        checkedItemList.append("Shake");
    if(customCheckBox5->isChecked())
        checkedItemList.append("Plushness");
    if(customCheckBox6->isChecked())
        checkedItemList.append("Sw Plushness");
    if(customCheckBox7->isChecked())
        checkedItemList.append("Impact Harshness");
    if(customCheckBox8->isChecked())
        checkedItemList.append("Sw Impact");
    if(customCheckBox9->isChecked())
        checkedItemList.append("Aftershake");

    customCalculateDialog->close();
    customCalculateSelectedItem(checkedItemList);
}

void MainWindow::customCalculateSelectedItem(QStringList checkedItemList)
{
    for(int i=0; i<checkedItemList.size(); i++)
    {
        QString checkedItem=checkedItemList.at(i);
        if("Bounce"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_bounce, indexofColumn_bounce;
            QVector<QVector<double>> VecOfDataToFftVec_bounce;
            QStringList validCarNameList;
           //bounce
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName_bounce=carNameList.at(i);
                QString eventName_bounce="Custom";
                QStringList tempList_bounce;
                tempList_bounce.append(carName_bounce);
                tempList_bounce.append(eventName_bounce);
                QString carEventStr_bounce= tempList_bounce.join("-");

                QString itemStr_bounce="SP-Z";
                tempList_bounce.append(itemStr_bounce);
                QString carEventPosStr_bounce=tempList_bounce.join("-");

                int index1_bounce=car_evevtTypeNamesList.indexOf(carEventStr_bounce);
                if(-1==index1_bounce)
                    continue;
                else
                {
                    indexofExcelNum_bounce.append(index1_bounce);
                    int index2_bounce= eachExcelNameVec.at(index1_bounce).indexOf(carEventPosStr_bounce);
                    if(-1==index2_bounce)
                        continue;
                    else
                    {
                        indexofColumn_bounce.append(index2_bounce);
                        VecOfDataToFftVec_bounce.append(vectorOfAllExcelDataVector.at(index1_bounce).at(index2_bounce+1));
                        validCarNameList.append(carName_bounce);
                    }
                }

                validCarNameList.removeDuplicates();

                for(int i=0; i<validCarNameList.size(); i++)//bounce_c4
                {
                    QVector<complex<double>> complexDataVec;
                    int length=7000;
                    for(int j=0; j<length; j++)
                    {
                        complex<double> data(VecOfDataToFftVec_bounce.at(i).at(j),0);
                        complexDataVec.append(data);
                    }

                    int log2n= log2(length);
                    int NFFT=1<<(log2n+1);

                    for(int j=length; j<NFFT; j++)
                    {
                        complex<double> data(0,0);//补零
                        complexDataVec.append(data);
                    }
                    QVector<double> bounceResultVec;
                    bounceResultVec= calculateBounceFft(complexDataVec, log2n+1, length);

                    QStringList itemNameList;
                    itemNameList<<"MaxAmp_Custom"<<"MaxFre_Custom"<<"SecAmp_Custom"<<"SecFre_Custom";
                    for(int k=0; k<bounceResultVec.size(); k++)//分别是最大值、最大值的频率、次大值、次大值的频率
                    {
                        QString carNameStr= validCarNameList.at(i);
                        QString itemNameStr=itemNameList.at(k);
                        QString valueStr=QString::number(bounceResultVec.at(k));
                        QStringList itemNames;
                        itemNames<<carNameStr<<itemNameStr<<valueStr;

                        QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                        //查看结果表中是否已经有该项计算指标，若有，就不在添加
                        bool hasCalculationItem=false;
                        for(int j=0; j<tableWidgetIndex; j++)
                        {
                            QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                            QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                            if(carName==validCarNameList.at(i) && calculateItemName==itemNameList.at(k))
                                hasCalculationItem=true;
                        }
                        if(!hasCalculationItem)
                        {
                            if(tableWidgetIndex<4)
                            {
                                for(int i=0; i<3; i++)
                                {
                                    leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                                }
                                tableWidgetIndex++;
                            }
                            else    //tableWidgetIndex>=4
                            {
                                leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                                for(int i=0; i<3; i++)
                                {
                                    leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                                }
                                tableWidgetIndex++;
                            }
                        }
                    }
                }
            }
        }

        else if("Aruptness"==checkedItem)
        {
            //aruptness  0.1-10Hz
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum, indexofColumn;
            QVector<QVector<double>> VecOfDataToFilteredVec;
            QStringList validCarNameList;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QString itemStr="SP-Z";
                tempList.append(itemStr);
                QString carEventPosStr=tempList.join("-");

                int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                if(-1==index1)
                    continue;
                else
                {
                    indexofExcelNum.append(index1);
                    int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                    if(-1==index2)
                        continue;
                    else
                    {
                        indexofColumn.append(index2);
                        VecOfDataToFilteredVec.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                        validCarNameList.append(carName);
                    }

                }
            }
            validCarNameList.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_aruptness;
            vecOfFilteredDataVector_aruptness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum,indexofColumn,0.1,100);//滤波后的数据
            for(int i=0; i<validCarNameList.size(); i++)//aruptness_c1_1
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_aruptness.at(i);
                QVector<double> rmsCutfilteredDataVec;
                rmsCutfilteredDataVec=rmsCutData_calculate("C1_FirstPart",filteredDataVec);//路况识别，识别的第一段数据计算
                int xsize=rmsCutfilteredDataVec.size();

                //将0.1-100 Hz的数据求导
                QVector<double> diffedDataVec;
                for(int i=0; i<xsize-1; i++)
                {
                    double difData=(rmsCutfilteredDataVec.at(i+1)-rmsCutfilteredDataVec.at(i))/0.002;
                    diffedDataVec.append(difData);
                }

                QVector< QVector<QVector<double>>> vecVecVec; QVector<QVector<double>> vecVec;//为了与firFilter函数入口参数保持一致
                vecVec.append(diffedDataVec);
                vecVecVec.append(vecVec);
                QVector<int> indexVec1,indexVec2;
                indexVec1.append(0);
                indexVec2.append(-1);

                QVector<QVector<double>> vecOfdataFiltered;
                vecOfdataFiltered= firFilter(vecVecVec,indexVec1,indexVec2,0.1,10);//将求导的数据滤波 0.1-10Hz

                QVector<double> dataFiltered= vecOfdataFiltered.at(0);
                double dataSize=diffedDataVec.size();//
                double squareSum=0;//RMS
                for(int i=0; i<dataSize; i++)
                {
                    squareSum +=dataFiltered.at(i)*dataFiltered.at(i);
                }
                double RMS=sqrt(squareSum/dataSize);

                QString carNameStr= validCarNameList.at(i);
                QString itemNameStr="Aruptness-1_Custom";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList.at(i) && calculateItemName=="Aruptness-1_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            for(int i=0; i<validCarNameList.size(); i++)//aruptness_c1_2
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_aruptness.at(i);
                QVector<double> rmsCutfilteredDataVec;
                rmsCutfilteredDataVec=rmsCutData_calculate("C1_SecondPart",filteredDataVec);//路况识别，识别的第一段数据计算
                int xsize=rmsCutfilteredDataVec.size();

                //将0.1-100 Hz的数据求导
                QVector<double> diffedDataVec;
                for(int i=0; i<xsize-1; i++)
                {
                    double difData=(rmsCutfilteredDataVec.at(i+1)-rmsCutfilteredDataVec.at(i))/0.002;
                    diffedDataVec.append(difData);
                }

                QVector< QVector<QVector<double>>> vecVecVec; QVector<QVector<double>> vecVec;//为了与firFilter函数入口参数保持一致
                vecVec.append(diffedDataVec);
                vecVecVec.append(vecVec);
                QVector<int> indexVec1,indexVec2;
                indexVec1.append(0);
                indexVec2.append(-1);

                QVector<QVector<double>> vecOfdataFiltered;
                vecOfdataFiltered= firFilter(vecVecVec,indexVec1,indexVec2,0.1,10);//将求导的数据滤波 0.1-10Hz

                QVector<double> dataFiltered= vecOfdataFiltered.at(0);
                double dataSize=diffedDataVec.size();//
                double squareSum=0;//RMS
                for(int i=0; i<dataSize; i++)
                {
                    squareSum +=dataFiltered.at(i)*dataFiltered.at(i);
                }
                double RMS=sqrt(squareSum/dataSize);

                QString carNameStr= validCarNameList.at(i);
                QString itemNameStr="Aruptness-2_Custom";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList.at(i) && calculateItemName=="Aruptness-2_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

            for(int i=0; i<validCarNameList.size(); i++)//aruptness_c1_3
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_aruptness.at(i);
                QVector<double> rmsCutfilteredDataVec;
                rmsCutfilteredDataVec=rmsCutData_calculate("C1_ThirdPart",filteredDataVec);//路况识别，识别的第一段数据计算
                int xsize=rmsCutfilteredDataVec.size();

                //将0.1-100 Hz的数据求导
                QVector<double> diffedDataVec;
                for(int i=0; i<xsize-1; i++)
                {
                    double difData=(rmsCutfilteredDataVec.at(i+1)-rmsCutfilteredDataVec.at(i))/0.002;
                    diffedDataVec.append(difData);
                }

                QVector< QVector<QVector<double>>> vecVecVec; QVector<QVector<double>> vecVec;//为了与firFilter函数入口参数保持一致
                vecVec.append(diffedDataVec);
                vecVecVec.append(vecVec);
                QVector<int> indexVec1,indexVec2;
                indexVec1.append(0);
                indexVec2.append(-1);

                QVector<QVector<double>> vecOfdataFiltered;
                vecOfdataFiltered= firFilter(vecVecVec,indexVec1,indexVec2,0.1,10);//将求导的数据滤波 0.1-10Hz

                QVector<double> dataFiltered= vecOfdataFiltered.at(0);
                double dataSize=diffedDataVec.size();//
                double squareSum=0;//RMS
                for(int i=0; i<dataSize; i++)
                {
                    squareSum +=dataFiltered.at(i)*dataFiltered.at(i);
                }
                double RMS=sqrt(squareSum/dataSize);

                QString carNameStr= validCarNameList.at(i);
                QString itemNameStr="Aruptness-3_Custom";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList.at(i) && calculateItemName=="Aruptness-3_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

        }

        else if("Choppiness"==checkedItem)
        {
            //choppiness  filter 3-9Hz SR-Z
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_choppiness, indexofColumn_choppiness;
            QVector<QVector<double>> VecOfDataToFilteredVec_choppiness;
            QStringList validCarNameList_choppiness;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                tempList.append("SR-Z");
                QString carEventPos=tempList.join("-");

                int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                if(-1==index1)
                    break;
                else
                {
                    indexofExcelNum_choppiness.append(index1);
                    int index2= eachExcelNameVec.at(index1).indexOf(carEventPos);
                    if(-1==index2)
                        break;
                    else
                    {
                        indexofColumn_choppiness.append(index2);
                        VecOfDataToFilteredVec_choppiness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                        validCarNameList_choppiness.append(carName);
                    }
                }
            }
            validCarNameList_choppiness.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_choppiness;
            vecOfFilteredDataVector_choppiness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_choppiness,indexofColumn_choppiness,3, 9);

            for(int i=0; i<validCarNameList_choppiness.size(); i++)
            {
                QVector<double> filteredDataVec;
                filteredDataVec=vecOfFilteredDataVector_choppiness.at(i);

                int xsize=VecOfDataToFilteredVec_choppiness.at(i).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                double squareSum=0;
                for(int i=0; i<xsize; i++)
                {
                    squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                }
                double RMS=sqrt(squareSum/xsize);

                QString carNameStr= validCarNameList_choppiness.at(i);
                QString itemNameStr="Choppiness_Custom";
                QString valueStr=QString::number(RMS);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_choppiness.at(i) && calculateItemName=="Choppiness_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        else if("Shake"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_shake, indexofColumn_shake;
            QVector<QVector<double>> VecOfDataToFilteredVec_shake;
            QStringList validCarNameList_shake;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList;
                itemList<<"SR-X"<<"SR-Y"<<"SR-Z";
                QStringList carEventPosList;

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList.at(j));
                    QString carEventPosStr=tempList.join("-");
                    carEventPosList.append(carEventPosStr);
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_shake.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_shake.append(index2);
                            VecOfDataToFilteredVec_shake.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_shake.append(carName);
                        }
                    }
                }
            }
            validCarNameList_shake.removeDuplicates();

            //shake 9-30 SR-XYZ
            QVector<QVector<double>> vecOfFilteredDataVector_shake;
            vecOfFilteredDataVector_shake=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_shake,indexofColumn_shake,9, 30);
            for(int i=0; i<validCarNameList_shake.size(); i++)
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_shake.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_shake.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsShake=sqrt(rmsSum);

                QString carNameStr= validCarNameList_shake.at(i);
                QString itemNameStr="Shake_Custom";
                QString valueStr=QString::number(rmsShake);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_shake.at(i) && calculateItemName=="Shake_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }

        }

        else if("Plushness"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_plushness, indexofColumn_plushness;
            QVector<QVector<double>> VecOfDataToFilteredVec_plushness;
            QStringList validCarNameList_plushness;

            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_plushness;
                itemList_plushness<<"SR-X"<<"SR-Y"<<"SR-Z";

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList_plushness.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_plushness.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_plushness.append(index2);
                            VecOfDataToFilteredVec_plushness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_plushness.append(carName);
                        }

                    }
                }
            }
            validCarNameList_plushness.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_plushness;
            vecOfFilteredDataVector_plushness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_plushness,indexofColumn_plushness,30,100);//滤波后的数据
            for(int i=0; i<validCarNameList_plushness.size(); i++)//plushness RMS
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_plushness.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_plushness.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsPlushness=sqrt(rmsSum);

                QString carNameStr= validCarNameList_plushness.at(i);
                QString itemNameStr="Plushness_Custom";
                QString valueStr=QString::number(rmsPlushness);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_plushness.at(i) && calculateItemName=="Plushness_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        else if("Sw Plushness"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_sw,indexofColumn_sw;
            QVector<QVector<double>> VecOfDataToFilteredVec_sw;
            QStringList validCarNameList_sw;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_sw;
                itemList_sw<<"SW-X"<<"SW-Y"<<"SW-Z";

                for(int j=0; j<3; j++)
                {
                    tempList.append(itemList_sw.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_sw.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_sw.append(index2);
                            VecOfDataToFilteredVec_sw.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_sw.append(carName);
                        }
                    }
                }
            }
            validCarNameList_sw.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_sw;
            vecOfFilteredDataVector_sw=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_sw,indexofColumn_sw,50, 100);//滤波后的数据
            for(int i=0; i<validCarNameList_sw.size(); i++)//Sw Plushness RMS
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_sw.at(i*3+j);

                    int xsize=VecOfDataToFilteredVec_sw.at(i*3+j).size();//xsize 必须是原始数据的size 不能使滤波后数据的size
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=filteredDataVec.at(i)*filteredDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsSw=sqrt(rmsSum);

                QString carNameStr= validCarNameList_sw.at(i);
                QString itemNameStr="Sw Plushness_Custom";
                QString valueStr=QString::number(rmsSw);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_sw.at(i) && calculateItemName=="Sw Plushness_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        else if("Impact Harshness"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_harshness, indexofColumn_harshness;
            QVector<QVector<double>> VecOfDataToFilteredVec_harshness;
            QStringList validCarNameList_harshness;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_harshness;
                itemList_harshness<<"SR-X"<<"SR-Y"<<"SR-Z";

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList_harshness.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_harshness.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        indexofColumn_harshness.append(index2);
                        VecOfDataToFilteredVec_harshness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                        validCarNameList_harshness.append(carName);
                    }
                }
            }
            validCarNameList_harshness.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_harshness;
            vecOfFilteredDataVector_harshness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_harshness,indexofColumn_harshness,0.1,100);//滤波后的数据

            for(int i=0; i<validCarNameList_harshness.size(); i++)//Impact Harshness vdv
            {
                double totalSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_harshness.at(i*3+j);
                    QVector<double> rmsCutfilteredDataVec;
                    rmsCutfilteredDataVec=rmsCutData_calculate("B3",filteredDataVec);//路况识别，识别的第一段数据计算vdv

                    int xsize=rmsCutfilteredDataVec.size();
                    double squareSquareSum=0;//四次方和
                    for(int i=0; i<xsize; i++)
                    {
                        squareSquareSum +=rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i);
                    }

                    totalSum+=squareSquareSum;
                }
                double temp=sqrt(totalSum);
                double vdv=sqrt(temp);

                QString carNameStr= validCarNameList_harshness.at(i);
                QString itemNameStr="Impact Harshness_Custom";
                QString valueStr=QString::number(vdv);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_harshness.at(i) && calculateItemName=="Impact Harshness_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        else if("Sw Impact"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int>  indexofExcelNum_sw, indexofColumn_sw;
            QVector<QVector<double>>  VecOfDataToFilteredVec_sw;
            QStringList validCarNameList_sw;

            QVector<int>  indexofExcelNum_SRX, indexofColumn_SRX;
            QVector<QVector<double>>  VecOfDataToFilteredVec_SRX;
            QStringList validCarNameList_SRX;

            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_sw;
                itemList_sw<<"SW-X"<<"SW-Y"<<"SW-Z";

                for(int j=0; j<3; j++)
                {
                    tempList.append(itemList_sw.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_sw.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        else
                        {
                            indexofColumn_sw.append(index2);
                            VecOfDataToFilteredVec_sw.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                            validCarNameList_sw.append(carName);
                        }
                    }
                }

                QStringList tempList_SRX;
                tempList_SRX.append(carName);
                tempList_SRX.append(eventName);
                QString carEventStr_SRX= tempList_SRX.join("-");
                tempList_SRX.append("SR-X");
                QString carEventPos_SRX=tempList_SRX.join("-");

                int index1=car_evevtTypeNamesList.indexOf(carEventStr_SRX);
                if(-1==index1)
                    break;
                else
                {
                    indexofExcelNum_SRX.append(index1);
                    int index2= eachExcelNameVec.at(index1).indexOf(carEventPos_SRX);
                    if(-1==index2)
                        break;
                    else
                    {
                        indexofColumn_SRX.append(index2);
                        VecOfDataToFilteredVec_SRX.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                        validCarNameList_SRX.append(carName);
                    }
                }
            }

            validCarNameList_sw.removeDuplicates();
            validCarNameList_SRX.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_sw;
            vecOfFilteredDataVector_sw=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_sw,indexofColumn_sw,0.1, 100);

            QVector<QVector<double>> vecOfFilteredDataVector_SRX;
            vecOfFilteredDataVector_SRX=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_SRX,indexofColumn_SRX,0.1, 100);

            for(int i=0; i<validCarNameList_sw.size(); i++)//Sw Impact vdv
            {
                QVector<double> filteredDataVec_SRX;
                filteredDataVec_SRX=vecOfFilteredDataVector_SRX.at(0);

                double totalSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_sw.at(i*3+j);

                    QVector<double> rmsCutfilteredDataVec;
                    rmsCutData_calculate("B3",filteredDataVec_SRX);//调用函数，获取SR-X的截取位置
                    for(int i=beginindex; i<endindex; i++)
                    {
                        rmsCutfilteredDataVec.append(filteredDataVec.at(i));
                    }

                    int xsize=rmsCutfilteredDataVec.size();
                    double squareSquareSum=0;//四次方和

                    for(int i=0; i<xsize; i++)
                    {
                        squareSquareSum +=rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i)*rmsCutfilteredDataVec.at(i);
                    }
                    totalSum+=squareSquareSum;
                }
                double temp=sqrt(totalSum);
                double vdv=sqrt(temp);

                QString carNameStr= validCarNameList_sw.at(i);
                QString itemNameStr="Sw Impact_Custom";
                QString valueStr=QString::number(vdv);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;
                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_sw.at(i) && calculateItemName=="Sw Impact_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }

        else if("Aftershake"==checkedItem)
        {
            QStringList carNameList;
            carNameList.append(leftWidgetObj->carSelectCombox->currentText());
            if(leftWidgetObj->carTypeAddNums>0)
            {
                for(int i=0; i<leftWidgetObj->carTypeAddNums; i++)
                {
                    carNameList.append(leftWidgetObj->carTypeComboxObjVec.at(i)->currentText());
                }
            }

            QVector<int> indexofExcelNum_harshness, indexofColumn_harshness;
            QVector<QVector<double>> VecOfDataToFilteredVec_harshness;
            QStringList validCarNameList_harshness;
            for(int i=0; i<carNameList.size(); i++)
            {
                QString carName=carNameList.at(i);
                QString eventName="Custom";
                QStringList tempList;
                tempList.append(carName);
                tempList.append(eventName);
                QString carEventStr= tempList.join("-");

                QStringList itemList_harshness;
                itemList_harshness<<"SR-X"<<"SR-Y"<<"SR-Z";

                for(int j=0; j<3; j++)//
                {
                    tempList.append(itemList_harshness.at(j));
                    QString carEventPosStr=tempList.join("-");
                    tempList.removeLast();

                    int index1=car_evevtTypeNamesList.indexOf(carEventStr);
                    if(-1==index1)
                        break;
                    else
                    {
                        indexofExcelNum_harshness.append(index1);
                        int index2= eachExcelNameVec.at(index1).indexOf(carEventPosStr);
                        if(-1==index2)
                            break;
                        indexofColumn_harshness.append(index2);
                        VecOfDataToFilteredVec_harshness.append(vectorOfAllExcelDataVector.at(index1).at(index2+1));
                        validCarNameList_harshness.append(carName);
                    }
                }
            }
            validCarNameList_harshness.removeDuplicates();

            QVector<QVector<double>> vecOfFilteredDataVector_harshness;
            vecOfFilteredDataVector_harshness=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_harshness,indexofColumn_harshness,0.1,100);

            QVector<QVector<double>> vecOfFilteredDataVector_afterShake;//先滤波，滤波延时相同，根据计算Impact Harshness时截取的位置，确定该项的截取位置
            vecOfFilteredDataVector_afterShake=firFilter(vectorOfAllExcelDataVector,indexofExcelNum_harshness,indexofColumn_harshness,5,40);

            for(int i=0; i<validCarNameList_harshness.size(); i++)
            {
                double rmsSum=0;
                for(int j=0; j<3; j++)
                {
                    QVector<double> filteredDataVec;
                    filteredDataVec=vecOfFilteredDataVector_afterShake.at(i*3+j);

                    rmsCutData_calculate("B3",vecOfFilteredDataVector_harshness.at(i*3+j));//路况识别,得到beginIndex  endIndex

                    //截取出路况段
                    int beginIndex=beginindex;
                    int endIndex=endindex;

                    QVector<double> rmsCutDataVec;
                    for(int i=beginIndex; i<endIndex; i++)
                    {
                        rmsCutDataVec.append(filteredDataVec.at(i));
                    }
                    int xsize=rmsCutDataVec.size();
                    double squareSum=0;
                    for(int i=0; i<xsize; i++)
                    {
                        squareSum +=rmsCutDataVec.at(i)*rmsCutDataVec.at(i);
                    }
                    double RMS=sqrt(squareSum/xsize);
                    rmsSum+=(RMS*RMS);
                }
                double rmsAfterShake=sqrt(rmsSum);

                QString carNameStr= validCarNameList_harshness.at(i);
                QString itemNameStr="Aftershake_Custom";
                QString valueStr=QString::number(rmsAfterShake);
                QStringList itemNames;
                itemNames<<carNameStr<<itemNameStr<<valueStr;

                QTableWidgetItem** itemNamesPara= createTWitemsName(3,itemNames);

                //查看结果表中是否已经有该项计算指标，若有，就不在添加
                bool hasCalculationItem=false;
                for(int j=0; j<tableWidgetIndex; j++)
                {
                    QString carName=leftWidgetObj->calulateResultTabWideget->item(j,0)->text();
                    QString calculateItemName=leftWidgetObj->calulateResultTabWideget->item(j,1)->text();
                    if(carName==validCarNameList_harshness.at(i) && calculateItemName=="Aftershake_Custom")
                        hasCalculationItem=true;
                }
                if(!hasCalculationItem)
                {
                    if(tableWidgetIndex<4)
                    {
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                    else    //tableWidgetIndex>=4
                    {
                        leftWidgetObj->calulateResultTabWideget->setRowCount(tableWidgetIndex+1);//增加行
                        for(int i=0; i<3; i++)
                        {
                            leftWidgetObj->calulateResultTabWideget->setItem(tableWidgetIndex,i,*(itemNamesPara+i));
                        }
                        tableWidgetIndex++;
                    }
                }
            }
        }
    }
}

void MainWindow::customCalculateCancelSlotFunc()
{
    customCalculateDialog->close();
}

QTableWidgetItem** MainWindow::createTWitemsName(int itemNum,QStringList itemName)
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

void MainWindow::newPjtSlotFunc()
{
    indexImportData=0;
    vectorOfAllExcelDataVector.clear();
    allCurveNamesList.clear();//包含车辆种类和测量位置种类的字符串信息，用于设置posSelectCombox和legend
    car_evevtTypeNamesList.clear();
    firstCarandPosName.clear();
    eachExcelNameVec.clear();
}

void MainWindow::importedDataPathSlotFunc()
{
    QDialog *dataPathDialog= new QDialog(this);
    dataPathDialog->setFixedSize(350,200);

    QHBoxLayout *hboxLayout= new QHBoxLayout;
    QLabel *pathText= new QLabel;//换行符为：\n
    QString pathStr;
    for(int i=0; i<importedDataPathList.size(); i++)
    {
        pathStr=pathStr+importedDataPathList.at(i)+"\n";
    }
    if(1==importedDataPathList.size())
        dataPathDialog->setFixedSize(350,100);
    else
        dataPathDialog->setFixedSize(350,100+importedDataPathList.size()*15);
    pathText->setText(pathStr);
    QFont font("Arial",10,QFont::Normal);
    pathText->setFont(font);

    hboxLayout->addWidget(pathText,Qt::AlignTop);
    dataPathDialog->setLayout(hboxLayout);

    dataPathDialog->setGeometry((this->width()-dataPathDialog->width())/2,(this->height()-dataPathDialog->height())/2,dataPathDialog->width(),dataPathDialog->height());//位置坐标 窗口大小
    dataPathDialog->show();
}

void MainWindow::scoreSlotFunc()
{
    if(tableWidgetIndex>0)
    {
        QStringList carNameList;
        QStringList calculateItemList;
        QStringList carAndItemList;
        QStringList calculateResultList;
        for(int i=tableWidgetIndex-1; i>=0; i--)
        {
            QString carName=leftWidgetObj->calulateResultTabWideget->item(i,0)->text();
            carNameList.append(carName);

            QString item=leftWidgetObj->calulateResultTabWideget->item(i,1)->text();

            if(item.contains("MaxAmp"))
            {
                //do nothing
            }

            else if(item.contains("MaxFre"))
            {
                //do nothing
            }

            else if(item.contains("SecAmp"))
            {
                //do nothing
            }

            else if(item.contains("SecFre"))
            {
                //do nothing
            }

            else
            {
                QStringList tempList;
                tempList<<carName<<item;
                carAndItemList.append(tempList.join("_"));
                calculateResultList.append(leftWidgetObj->calulateResultTabWideget->item(i,2)->text());
                calculateItemList.append(item);
            }
        }

        carNameList.removeDuplicates();//删除重复的
        calculateItemList.removeDuplicates();

        int barSize=calculateResultList.size();
        int barWidth=25;

        QDialog *barGraphWidget= new QDialog(this);
        if(barSize*barWidth<this->height()-30)
        {
            barGraphWidget->setFixedSize(700,barSize*barWidth);
            heightToSaveGraph=barSize*barWidth;
        }

        if(barSize*barWidth<200)
        {
            barGraphWidget->setFixedSize(700,200);
            heightToSaveGraph=200;
        }

        if(barSize*barWidth>=this->height()-30)
        {
            barGraphWidget->setFixedSize(700,this->height()-30);
            heightToSaveGraph=this->height()-30;
        }


        customBarPlot= new QCustomPlot();

        customBarPlot->xAxis->setVisible(false);
        customBarPlot->xAxis2->setVisible(true);
        customBarPlot->xAxis2->setTickLabels(true);
        customBarPlot->xAxis2->setSubTicks(true);
        customBarPlot->yAxis->setTickPen(QPen(Qt::white));

        QFont labelFont("Arial",10,QFont::Normal);
        customBarPlot->xAxis2->setLabelFont(labelFont);
        customBarPlot->yAxis->setLabelFont(labelFont);
        customBarPlot->xAxis2->setTickLabelFont(QFont("Arial"));
        customBarPlot->yAxis->setTickLabelFont(QFont("Arial"));

        customBarPlot->xAxis->setRange(4,10);
        customBarPlot->xAxis2->setRange(4,10);
        customBarPlot->yAxis->setRange(-0.5,calculateItemList.size()-0.5);

        customBarPlot->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(customBarPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        customBarPlot->yAxis->setTicker(textTicker);
        QVector<double> index(barSize);
        for(int i=0;i<calculateItemList.size();++i)
        {
            index[i]=i;
            textTicker->addTick(double(i), calculateItemList.at(i));
        }

        QVector<QColor> barColor;
        QColor color=QColor(102,102,102);
        barColor.append(color);
        color=QColor(102,102,153);
        barColor.append(color);
        color=QColor(102,102,204);
        barColor.append(color);
        color=QColor(102,102,255);
        barColor.append(color);
        color=QColor(102,255,204);
        barColor.append(color);

        QCPBarsGroup *barGroup = new QCPBarsGroup(customBarPlot);//按车分组
        validScoreVecToExcel.clear();
        validItemListToExcel.clear();
        for(int i=0; i<carNameList.size(); i++)
        {
            QCPBars *barGraph= new QCPBars(customBarPlot->yAxis,customBarPlot->xAxis);
            barGraph->setName(carNameList.at(i));

            QStringList itemToScoreList;
            QVector<double> calculationResultToScoreVec;
            for(int j=0; j<calculateItemList.size(); j++)
            {
                QString carName=carNameList.at(i);
                QString calculateItem=calculateItemList.at(j);
                QStringList tempList;
                tempList<<carName<<calculateItem;
                QString carAndItemStr=tempList.join("_");

                int index=carAndItemList.indexOf(carAndItemStr);
                if(index!=-1)//exist
                {
                    itemToScoreList.append(calculateItem);
                    calculationResultToScoreVec.append(calculateResultList.at(index).toDouble());
                }
                else//index==-1
                {
                    itemToScoreList.append("NULL");
                    calculationResultToScoreVec.append(-1);
                }
            }

            QVector<double> itemScoreVec;
            itemScoreVec= getItemScore(itemToScoreList, calculationResultToScoreVec);//根据各项指标获得分数
            QVector<double> validScoreVec;
            QString carName=carNameList.at(i);
            for(int i=0; i<itemScoreVec.size(); i++)
            {
                validScoreVec.append(itemScoreVec.at(i));
                validScoreVecToExcel.append(itemScoreVec.at(i));
                QString itemName=itemToScoreList.at(i);
                QStringList temp;
                temp<<carName<<itemName;
                validItemListToExcel.append(temp.join("-"));

            }
            barGraph->setWidthType(QCPBars::wtAbsolute);
            barGraph->setWidth(8);

            barGraph->setPen(QPen(barColor.at(i)));
            barGraph->setBrush(barColor.at(i));

            barGraph->setData(index, validScoreVec);
            barGraph->setBarsGroup(barGroup);

        }

        customBarPlot->legend->setVisible(true);

        QCPLayoutGrid *subLayout = new QCPLayoutGrid;
        customBarPlot->plotLayout()->addElement(1, 0, subLayout);
        subLayout->setMargins(QMargins(5, 0, 5, 5));
        subLayout->addElement(0, 0, customBarPlot->legend);
//        subLayout->setRowStretchFactor(0,0.001);

        customBarPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
        customBarPlot->legend->setBorderPen(QPen(QColor(255,255,255)));//white
        QFont legendFont("Arial",10,QFont::Normal);
        customBarPlot->legend->setFont(legendFont);
        customBarPlot->plotLayout()->setRowStretchFactor(1,0.001);

        customBarPlot->replot();

        //legend
        QVBoxLayout *vboxLayoutLegend= new QVBoxLayout;
        QVBoxLayout *vboxLayoutEmpty= new QVBoxLayout;
        QGridLayout *gridLayout= new QGridLayout;
        for(int i=carNameList.size()-1; i>=0; i--)
        {
            QPushButton *legendBtn= new QPushButton;
            QLabel *legendLabel= new QLabel(carNameList.at(i));
            legendBtn->setFixedSize(20,10);
            legendBtn->setFont(QFont("Arial"));

            if(i==0)
                legendBtn->setStyleSheet("background-color: rgb(102,102,102);");
            else if(i==1)
                legendBtn->setStyleSheet("background-color: rgb(102,102,153);");
            else if(i==2)
                legendBtn->setStyleSheet("background-color: rgb(102,102,204);");

            QHBoxLayout *hboxLayoutLegend= new QHBoxLayout;
            hboxLayoutLegend->addWidget(legendBtn);
            hboxLayoutLegend->addWidget(legendLabel);

            vboxLayoutLegend->addLayout(hboxLayoutLegend);
        }
        gridLayout->addLayout(vboxLayoutLegend,0,0,1,1);
        gridLayout->addLayout(vboxLayoutEmpty,1,0,3,1);

        QHBoxLayout *hboxLayout= new QHBoxLayout;
        hboxLayout->addWidget(customBarPlot,10);
//        hboxLayout->addLayout(gridLayout,1);
        barGraphWidget->setLayout(hboxLayout);
        barGraphWidget->setGeometry((this->width()-barGraphWidget->width())/2,(this->height()-barGraphWidget->height())/2+15,barGraphWidget->width(),barGraphWidget->height());

        barGraphWidget->show();
    }
}

QVector<double> MainWindow::getItemScore(QStringList itemToScoreList, QVector<double> calculationResultToScoreVec)
{
    QVector<double> scoreVec;
    for(int i=0; i<itemToScoreList.size(); i++)
    {
        double itemValue=calculationResultToScoreVec.at(i);
        QString itmeName=itemToScoreList.at(i);
        //评分
        if(itmeName=="NULL")
        {
            scoreVec.append(4);
        }

        else if(itmeName=="Choppiness_B1")
        {
            double score;
            if(itemValue>=0 && itemValue<0.223)//8.5-10 段
            {
                score=-12.04*pow(itemValue,2)-4.27*itemValue+10;
            }

            else if(itemValue>=0.223 && itemValue<0.33)//6-8.5线性段
            {
                score=-20.69*itemValue+13.21;
            }

            else if(itemValue>=0.33 && itemValue<1)
            {
                score=6.069*pow(itemValue,2)-11.39*itemValue+9.208;
            }
            else if(itemValue>=1)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Choppiness_C2")
        {
            double score;
            if(itemValue>=0 && itemValue<0.53)//8.5-10 段
            {
                score=-0.908*pow(itemValue,2)-2.327*itemValue+9.994;
            }

            else if(itemValue>=0.53 && itemValue<0.83)//6-8.5线性段
            {
                score=-8.412*itemValue+12.95;
            }

            else if(itemValue>=0.83 && itemValue<2.68)//4-6段
            {
                score=1.208*pow(itemValue,2)-5.12*itemValue+9.047;
            }
            else if(itemValue>=2.68)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Shake_B1")
        {
            double score;
            if(itemValue>=0 && itemValue<0.51)//8.5-10 段
            {
                score=5.251*pow(itemValue,2)-5.6217*itemValue+10;
            }

            else if(itemValue>=0.51 && itemValue<0.76)//6-8.5线性段
            {
                score=-10.25*itemValue+13.81;
            }

            else if(itemValue>=0.76 && itemValue<2)//4-6段
            {
                score=0.5505*pow(itemValue,2)-3.132*itemValue+8.064;
            }
            else if(itemValue>=2)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Shake_C2")
        {
            double score;
            if(itemValue>=0 && itemValue<0.54)//8.5-10 段
            {
                score=-1.841*pow(itemValue,2)-1.832*itemValue+9.992;
            }

            else if(itemValue>=0.54 && itemValue<1.31)//6-8.5线性段
            {
                score=-3.324*itemValue+10.35;
            }

            else if(itemValue>=1.31 && itemValue<3.15)//4-6段
            {
                score=0.2838*pow(itemValue,2)-2.35*itemValue+8.587;
            }
            else if(itemValue>=3.15)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Plushness_C3")
        {
            double score;
            if(itemValue>=0 && itemValue<0.0867)//8.5-10 段
            {
                score=-65.52*pow(itemValue,2)-11.38*itemValue+9.98;
            }

            else if(itemValue>=0.0867 && itemValue<0.15)//6-8.5线性段
            {
                score=-39.93*itemValue+11.98;
            }

            else if(itemValue>=0.15 && itemValue<0.42)//4-6段
            {
                score=14.01*pow(itemValue,2)-15.41*itemValue+7.997;
            }
            else if(itemValue>=0.42)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Sw Plushness_C3")
        {
            double score;
            if(itemValue>=0 && itemValue<0.504)//8.5-10 段
            {
                score=-2.151*pow(itemValue,2)-1.891*itemValue+10;
            }

            else if(itemValue>=0.533 && itemValue<1.77)//6-8.5线性段
            {
                score=-2.006*itemValue+9.522;
            }

            else if(itemValue>=1.77 && itemValue<6)//4-6段
            {
                score=0.04341*pow(itemValue,2)-0.805*itemValue+7.269;
            }

            else if(itemValue>=6)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Impact Harshness_B2")
        {
            double score;
            if(itemValue>=0 && itemValue<8.46)//8.5-10 段
            {
                score=0.01334*pow(itemValue,2) -0.2872*itemValue+9.975;
            }

            else if(itemValue>=8.46 && itemValue<13.53)//6-8.5线性段
            {
                score=-0.4789*itemValue+12.48;
            }

            else if(itemValue>=13.53 && itemValue<35)//4-6段
            {
                score=-0.001148*pow(itemValue,2)-0.03705*itemValue+6.704;
            }
            else if(itemValue>=35)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Impact Harshness_B3")
        {
            double score;
            if(itemValue>=0 && itemValue<9.98)//8.5-10 段
            {
                score=-0.004398*pow(itemValue,2)-0.1074 *itemValue+10.01 ;
            }

            else if(itemValue>=9.98 && itemValue<17.84)//6-8.5线性段
            {
                score=-0.3206*itemValue+11.72;
            }

            else if(itemValue>=17.84 && itemValue<27.04)//4-6段
            {
                score=0.01612 *pow(itemValue,2)-0.9492*itemValue+17.88 ;
            }
            else if(itemValue>=27.04)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Sw Impact_B2")
        {
            double score;
            if(itemValue>=0 && itemValue<27.15)//8.5-10 段
            {
                score=0.0006923*pow(itemValue,2) -0.07386*itemValue+9.995;
            }

            else if(itemValue>=27.15 && itemValue<46.45)//6-8.5线性段
            {
                score=-0.1296*itemValue+12.02;
            }

            else if(itemValue>=46.45 && itemValue<148)//4-6段
            {
                score=0.000189*pow(itemValue,2)-0.05595*itemValue+8.151;
            }
            else if(itemValue>=148)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Sw Impact_B3")
        {
            double score;
            if(itemValue>=0 && itemValue<22.47)//8.5-10 段
            {
                score=-0.000402*pow(itemValue,2)-0.05815*itemValue+10.01 ;
            }

            else if(itemValue>=22.47 && itemValue<58.54)//6-8.5线性段
            {
                score=-0.06969*itemValue+10.08;
            }

            else if(itemValue>=58.54 && itemValue<89.80)//4-6段
            {
                score=0.001204*pow(itemValue,2)-0.2403 *itemValue+15.87;
            }
            else if(itemValue>=89.80)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Aftershake_B3")
        {
            double score;
            if(itemValue>=0 && itemValue<1.12)//8.5-10 段
            {
                score=-0.2327*pow(itemValue,2)-1.071*itemValue+9.998 ;
            }

            else if(itemValue>=1.12 && itemValue<2.43)//6-8.5线性段
            {
                score=-1.916*itemValue+10.65;
            }

            else if(itemValue>=2.43 && itemValue<6.01)//4-6段
            {
                score=0.04443*pow(itemValue,2)-0.9219 *itemValue+7.937;
            }
            else if(itemValue>=6.01)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Aruptness-1_C1")
        {
            double score;
            if(itemValue>=0 && itemValue<10.05)//8.5-10 段
            {
                score=0.01044*pow(itemValue,2)-0.253*itemValue+9.988 ;
            }

            else if(itemValue>=10.05 && itemValue<20.14)//6-8.5线性段
            {
                score=-0.2462*itemValue+10.96;
            }

            else if(itemValue>=20.14 && itemValue<68.73)//4-6段
            {
                score=0.0004488*pow(itemValue,2)-0.08104 *itemValue+7.45;
            }
            else if(itemValue>=68.73)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Aruptness-2_C1")
        {
            double score;
            if(itemValue>=0 && itemValue<16.91)//8.5-10 段
            {
                score=0.00192*pow(itemValue,2)-0.1202*itemValue+9.984 ;
            }

            else if(itemValue>=16.91 && itemValue<28.71)//6-8.5线性段
            {
                score=-0.2097*itemValue+12.02;
            }

            else if(itemValue>=28.71 && itemValue<70.22)//4-6段
            {
                score=0.0005271*pow(itemValue,2)-0.1003 *itemValue+8.444;
            }
            else if(itemValue>=70.22)
            {
                score=4;
            }

            scoreVec.append(score);
        }

        else if(itmeName=="Aruptness-3_C1")
        {
            double score;
            if(itemValue>=0 && itemValue<20.99)//8.5-10 段
            {
                score=-0.002028*pow(itemValue,2)-0.02815*itemValue+9.984 ;
            }

            else if(itemValue>=20.99 && itemValue<33.36)//6-8.5线性段
            {
                score=-0.2026 *itemValue+12.76;
            }

            else if(itemValue>=33.36 && itemValue<55.13)//4-6段
            {
                score=0.001834 *pow(itemValue,2)-0.2542*itemValue+12.44 ;
            }
            else if(itemValue>=55.13)
            {
                score=4;
            }

            scoreVec.append(score);
        }

    }
    return scoreVec;
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("Save Graph", this, SLOT(saveBarGraph()));
    menu->popup(customBarPlot->mapToGlobal(pos));
}

void MainWindow::saveBarGraph()
{
    QString file_path = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/home/jana/untitled.bmp",
                               tr("Images (*.bmp *.xpm *.jpg)"));
    if(!file_path.isEmpty())
    {
        customBarPlot->saveBmp(file_path,750,heightToSaveGraph);
    }
    else
        QMessageBox::warning(this,tr("Path"),"未选择保存文件名");
}

void MainWindow::outputCalculateResultSlotFunc()
{
    QString fileName= QFileDialog::getSaveFileName (this,tr("保存计算结果"),"/","Excel File(*.xlsx *.xls))");
    fileName.replace("/","\\");//必有这句

    QFile file(fileName);
    if(!file.exists())
    {
        QAxObject excel("Excel.Application");
        excel.dynamicCall("SetVisible(bool)",false);
        excel.setProperty("DisplayAlerts",false);
        QAxObject *workBooks= excel.querySubObject("workBooks");
        workBooks->dynamicCall("Add");
        QAxObject *workBook= excel.querySubObject("ActiveWorkBook");

        QAxObject *worksheets = workBook->querySubObject("Sheets");//获取工作表集合
        QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);//获取工作表集合的工作表1，即sheet1

        QAxObject *cellA,*cellB,*cellC,*cellE,*cellF;
        //设置标题
        int cellrow=1;
        QString A="A"+QString::number(cellrow);//设置要操作的单元格，如A1
        QString B="B"+QString::number(cellrow);
        QString C="C"+QString::number(cellrow);
        QString E="E"+QString::number(cellrow);
        QString F="F"+QString::number(cellrow);
        cellA = worksheet->querySubObject("Range(QVariant, QVariant)",A);//获取单元格
        cellB = worksheet->querySubObject("Range(QVariant, QVariant)",B);
        cellC=worksheet->querySubObject("Range(QVariant, QVariant)",C);
        cellE=worksheet->querySubObject("Range(QVariant, QVariant)",E);
        cellF=worksheet->querySubObject("Range(QVariant, QVariant)",F);

        cellA->dynamicCall("SetValue(const QVariant&)",QVariant("车辆名称"));//设置单元格的值
        cellB->dynamicCall("SetValue(const QVariant&)",QVariant("计算指标"));
        cellC->dynamicCall("SetValue(const QVariant&)",QVariant("计算结果"));
        cellE->dynamicCall("SetValue(const QVariant&)",QVariant("指标"));
        cellF->dynamicCall("SetValue(const QVariant&)",QVariant("分数"));
        cellrow++;
        //写入数据

        int rows=tableWidgetIndex;
        for(int i=0;i<rows;i++)
        {
            QString A="A"+QString::number(cellrow);//设置要操作的单元格，如A1
            QString B="B"+QString::number(cellrow);
            QString C="C"+QString::number(cellrow);
            cellA = worksheet->querySubObject("Range(QVariant, QVariant)",A);//获取单元格
            cellB = worksheet->querySubObject("Range(QVariant, QVariant)",B);
            cellC=worksheet->querySubObject("Range(QVariant, QVariant)",C);
            cellA->dynamicCall("SetValue(const QVariant&)",QVariant(leftWidgetObj->calulateResultTabWideget->item(i,0)->text()));//设置单元格的值
            cellB->dynamicCall("SetValue(const QVariant&)",QVariant(leftWidgetObj->calulateResultTabWideget->item(i,1)->text()));
            cellC->dynamicCall("SetValue(const QVariant&)",QVariant(leftWidgetObj->calulateResultTabWideget->item(i,2)->text()));
            cellrow++;
            QString itemName=leftWidgetObj->calulateResultTabWideget->item(i,1)->text();
            if(itemName.contains("SecFre"))
            {
                double maxAmplitude= leftWidgetObj->calulateResultTabWideget->item(i-3,2)->text().toDouble();
                double maxFrequency= leftWidgetObj->calulateResultTabWideget->item(i-2,2)->text().toDouble();
                double bounce=maxAmplitude/(maxFrequency*maxFrequency);
                QString bounceStr=QString::number(bounce);

                QString A="A"+QString::number(cellrow);//设置要操作的单元格，如A1
                QString B="B"+QString::number(cellrow);
                QString C="C"+QString::number(cellrow);
                cellA = worksheet->querySubObject("Range(QVariant, QVariant)",A);//获取单元格
                cellB = worksheet->querySubObject("Range(QVariant, QVariant)",B);
                cellC=worksheet->querySubObject("Range(QVariant, QVariant)",C);
                cellA->dynamicCall("SetValue(const QVariant&)",QVariant(leftWidgetObj->calulateResultTabWideget->item(i,0)->text()));//设置单元格的值
                cellB->dynamicCall("SetValue(const QVariant&)",QVariant("Bounce"));
                cellC->dynamicCall("SetValue(const QVariant&)",QVariant(bounceStr));
                cellrow++;
            }
        }

        if(!validScoreVecToExcel.isEmpty())
        {
            int cellrowScore=2;
            for(int i=validScoreVecToExcel.size()-1;i>=0;i--)
            {
                QString E="E"+QString::number(cellrowScore);
                QString F="F"+QString::number(cellrowScore);
                cellE=worksheet->querySubObject("Range(QVariant, QVariant)",E);
                cellF=worksheet->querySubObject("Range(QVariant, QVariant)",F);
                cellE->dynamicCall("SetValue(const QVariant&)",validItemListToExcel.at(i));
                cellF->dynamicCall("SetValue(const QVariant&)",validScoreVecToExcel.at(i));
                cellrowScore++;
            }
        }

        workBook->dynamicCall("SaveAs(const QString&)",fileName);

        workBook->dynamicCall("emptyHeightose(Boolean)",false);
        excel.dynamicCall("Quit(void)");
    }
    else
        return;
}

void MainWindow::exitPjtSlotFunc()
{
    this->close();
}

void MainWindow::helpSlotFunc()
{
    QDialog *helpDialog= new QDialog(this);
    helpDialog->setFixedSize(400,300);

    QHBoxLayout *hboxLayout= new QHBoxLayout;
    QLabel *helpText= new QLabel;//换行符为：\n
    helpText->setText("Help Information：\n1. \n2. \n3.");
    QFont font("Arial",10,QFont::Normal);
    helpText->setFont(font);
    hboxLayout->addWidget(helpText);
    helpDialog->setLayout(hboxLayout);

    helpDialog->setGeometry((this->width()-helpDialog->width())/2,(this->height()-helpDialog->height())/2,helpDialog->width(),helpDialog->height());//位置坐标 窗口大小
    helpDialog->show();

//    QAxObject word("Word.Application");
//    word.setProperty("Visible",true);
//    QAxObject * documents = word.querySubObject("Documents");
//    documents->dynamicCall("Open(const QString&)",QString("E:/qtHelp.doc"));
//    documents->dynamicCall("Add (void)",QString("E:/test/docbyqt.doc"));
//    QAxObject * document = word.querySubObject("ActiveDocument");
//    document->dynamicCall("Open(const QString&)",QString("E:/test/docbyqt.doc"));

}

void MainWindow::sleep(unsigned int msec)
{
    QTime reachTime= QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}

QString MainWindow::getNonFullNameofEvent(QString event)
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
    else if(event=="Custom")
        eventFullName="Custom";
    return eventFullName;
}

