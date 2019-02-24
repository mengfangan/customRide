#include "plottime.h"

plottime::plottime(QWidget *parent) : QWidget(parent)
{
    customPlot1= new QCustomPlot(this);
//    customPlot2= new QCustomPlot(this);
    cursorPosLabel = new QLabel(this);
    cursorPosLabel->setFixedSize(150,30);

    //右键弹出菜单
    cursorMeasuAct= new QAction(this);
    cursorMeasuAct->setText("Cursor measurement");
    arrowCursorAct= new QAction(this);
    arrowCursorAct->setText("Arrow cursor");

    zoomXAct= new QAction(this);
    zoomYAct= new QAction(this);
    zoomXYAct= new QAction(this);
    zoomXAct->setText("zoom x");
    zoomYAct->setText("zoom y");
    zoomXYAct->setText("zoom x/y");

    connect(cursorMeasuAct,SIGNAL(triggered(bool)),this,SLOT(cursorMeasurement()));
    connect(arrowCursorAct,SIGNAL(triggered(bool)),this,SLOT(arrowCursorSlotFunc()));

    connect(zoomXAct,SIGNAL(triggered(bool)),this,SLOT(zoomX()));
    connect(zoomYAct,SIGNAL(triggered(bool)),this,SLOT(zoomY()));
    connect(zoomXYAct,SIGNAL(triggered(bool)),this,SLOT(zoomXY()));

    customPlot1->plotLayout()->insertRow(0);//设置标题
    QCPTextElement *title = new QCPTextElement(customPlot1, "Title", QFont("Arial", 13, QFont::Normal));
    customPlot1->plotLayout()->addElement(0, 0, title);
    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));

    customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    connect(customPlot1, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(customPlot1, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(customPlot1, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    customPlot1->setContextMenuPolicy(Qt::CustomContextMenu);// setup policy and connect slot for context menu popup:
    connect(customPlot1, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    plotWidgetLayout();
}

void plottime::createPlot(QStringList curveName)
{
    QStringList colorlist= QColor::colorNames();
    QVector <int> indexOfColor;
    indexOfColor <<colorlist.indexOf("red") <<colorlist.indexOf("blue") <<colorlist.indexOf("green")
                <<colorlist.indexOf("magenta") <<colorlist.indexOf("cyan") <<colorlist.indexOf("darksalmon")
               << colorlist.indexOf("black") <<colorlist.indexOf("red") <<colorlist.indexOf("blue") <<colorlist.indexOf("green");

    customPlot1->xAxis->setLabel("Time(s)");
    customPlot1->yAxis->setLabel("Acceleration(m^2/s)");
    QFont labelFont("Arial",11,QFont::Normal);
    customPlot1->xAxis->setLabelFont(labelFont);
    customPlot1->yAxis->setLabelFont(labelFont);
    customPlot1->xAxis->setTickLabelFont(QFont("Arial"));
    customPlot1->yAxis->setTickLabelFont(QFont("Arial"));
    customPlot1->legend->setVisible(true);
    customPlot1->legend->setFont(QFont("Arial"));

    customPlot1->clearGraphs();//clear all graphs before add graph
    for(int i=0; i<curveName.size(); i++)
    {
        customPlot1->addGraph();
        customPlot1->graph(i)->setName(curveName.at(i));
        customPlot1->graph(i)->setLineStyle(QCPGraph::lsLine);
        QPen linePen;
        linePen.setColor(QColor::colorNames().at(indexOfColor.at(i)));
        customPlot1->graph(i)->setPen(linePen);
    }
}

void plottime::plotWidgetLayout()
{
    QVBoxLayout *vboxLayout= new QVBoxLayout;
    vboxLayout->addWidget(customPlot1);
//    vboxLayout->addWidget(customPlot2);
    this->setLayout(vboxLayout);

    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window,QBrush((Qt::white)));
    this->setPalette(palette);
}

void plottime::dataToPlot(QVector< QVector<QVector<double>>> vectorOfAllExcelDataVector, QVector<int> indexofExcelNum,QVector<int> indexofColNum)//原始数据plot
{
    for(int i=0; i<indexofExcelNum.size(); i++)
    {
        customPlot1->graph(i)->setData(vectorOfAllExcelDataVector.at(indexofExcelNum.at(i)).at(0),vectorOfAllExcelDataVector.at(indexofExcelNum.at(i)).at(indexofColNum.at(i)+1));
        customPlot1->rescaleAxes(true);
    }
    customPlot1->replot();
}

//slots
void plottime::selectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot1->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot1->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customPlot1->graphCount(); ++i)
  {
    QCPGraph *graph = customPlot1->graph(i);
    QCPPlottableLegendItem *item =customPlot1->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void plottime::mousePress()
{
  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot1->axisRect()->setRangeDrag(customPlot1->xAxis->orientation());
  else if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot1->axisRect()->setRangeDrag(customPlot1->yAxis->orientation());
  else
    customPlot1->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void plottime::mouseWheel()
{
  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot1->axisRect()->setRangeZoom(customPlot1->xAxis->orientation());
  else if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot1->axisRect()->setRangeZoom(customPlot1->yAxis->orientation());
  else
    customPlot1->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void plottime::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (customPlot1->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
  }
  else  // general context menu on graphs requested
  {
    menu->addAction("Save graph", this, SLOT(saveGraph()));
    menu->addAction(cursorMeasuAct);
    menu->addAction(arrowCursorAct);

    cursorMeasuAct->setCheckable(true);
    arrowCursorAct->setCheckable(true);
    if(cursorMeasuAct->isChecked())
        cursorMeasuAct->setChecked(true);
    if(arrowCursorAct->isChecked())
        arrowCursorAct->setChecked(true);
    QActionGroup *actGroup1 =new QActionGroup(this);
    actGroup1->addAction(cursorMeasuAct);
    actGroup1->addAction(arrowCursorAct);

    menu->addSeparator();

    menu->addAction(zoomXAct);
    menu->addAction(zoomYAct);
    menu->addAction(zoomXYAct);

    zoomXAct->setCheckable(true);
    zoomYAct->setCheckable(true);
    zoomXYAct->setCheckable(true);

    if(zoomXAct->isChecked())
        zoomXAct->setChecked(true);
    if(zoomYAct->isChecked())
        zoomYAct->setChecked(true);
    if(zoomXYAct->isChecked())
        zoomXYAct->setChecked(true);

    QActionGroup *actGroup2 =new QActionGroup(this);
    actGroup2->addAction(zoomXAct);
    actGroup2->addAction(zoomYAct);
    actGroup2->addAction(zoomXYAct);
  }
  menu->popup(customPlot1->mapToGlobal(pos));
}

void plottime::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      customPlot1->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      customPlot1->replot();
    }
  }
}

void plottime::saveGraph()
{
    QPixmap exportImage(customPlot1->frameGeometry().width()+10,customPlot1->frameGeometry().height()+20);
    exportImage.fill(Qt::white);
    QPainter painter(&exportImage);
    QPixmap axisPixmap=QWidget::grab(QRect(customPlot1->x(),customPlot1->y(),customPlot1->frameGeometry().width(),customPlot1->frameGeometry().height()+20));
    painter.drawPixmap(10,10,axisPixmap);
    painter.end();

    QString file_path = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/home/jana/untitled.png",
                               tr("Images (*.png *.xpm *.jpg)"));
    if(!file_path.isEmpty())
    {
        if(QFileInfo(file_path).suffix().isEmpty())//如果没有写后缀就自动加上
            file_path.append(".png");
        exportImage.save(file_path);
    }
    else
        QMessageBox::warning(this,tr("Path"),"未选择保存文件名");
}

void plottime::cursorMeasurement()
{
    cursorMeasuAct->setChecked(true);
    arrowCursorAct->setChecked(false);
    setCursor(Qt::CrossCursor);
    connect(customPlot1, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(my_mouseMove(QMouseEvent*)));
}

void plottime::arrowCursorSlotFunc()
{
    arrowCursorAct->setChecked(true);
    cursorMeasuAct->setChecked(false);
    setCursor(Qt::ArrowCursor);
    disconnect(customPlot1, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(my_mouseMove(QMouseEvent*)));
    cursorPosLabel->clear();
}

void plottime::zoomX()
{
    zoomXAct->setChecked(true);
    zoomYAct->setChecked(false);
    zoomXYAct->setChecked(false);

    customPlot1->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot1->yAxis->setSelectedParts(QCPAxis::spNone);
}

void plottime::zoomY()
{
    zoomYAct->setChecked(true);
    zoomXAct->setChecked(false);
    zoomXYAct->setChecked(false);

    customPlot1->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot1->xAxis->setSelectedParts(QCPAxis::spNone);
}

void plottime::zoomXY()
{
    zoomXYAct->setChecked(true);
    zoomYAct->setChecked(false);
    zoomXAct->setChecked(false);

    customPlot1->xAxis->setSelectedParts(QCPAxis::spNone);
    customPlot1->yAxis->setSelectedParts(QCPAxis::spNone);
}
//http://www.cnblogs.com/swarmbees/p/6058942.html

void plottime::my_mouseMove(QMouseEvent* event)
{
    int x_pos = event->pos().x();//获取鼠标坐标点
    int y_pos = event->pos().y();

    float x_val = customPlot1->xAxis->pixelToCoord(x_pos);// 把鼠标坐标点 转换为 QCustomPlot 内部坐标值 （pixelToCoord 函数）
    float y_val = customPlot1->yAxis->pixelToCoord(y_pos);// coordToPixel 函数与之相反 是把内部坐标值 转换为外部坐标点

    cursorPosLabel->setText(tr("(%1 %2)").arg(x_val).arg(y_val));
    //http://www.cnblogs.com/ele-eye/p/6931126.html
}

void plottime::titleDoubleClick(QMouseEvent* event)
{
  Q_UNUSED(event)
  if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
  {
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      customPlot1->replot();
    }
  }
}

void plottime::filteredDataToPlot(QVector< QVector<QVector<double>>> vectorOfAllExcelDataVector, QVector<int> indexofExcelNum,QVector<QVector<double>> vecOfFilteredDataVec)
{
    for(int i=0; i<indexofExcelNum.size(); i++)
    {
        customPlot1->graph(i)->setData(vectorOfAllExcelDataVector.at(indexofExcelNum.at(i)).at(0),vecOfFilteredDataVec.at(i));
        customPlot1->rescaleAxes(true);
        customPlot1->replot();
    }
}

void plottime::eventDataToPlot(QVector<double> time, QVector<double> dataVec)
{
    customPlot1->graph(0)->setData(time,dataVec);
    customPlot1->rescaleAxes(true);
    customPlot1->replot();
}

void plottime::dataPlot(QVector<double> time, QVector<double> dataVec)
{
    customPlot1->addGraph();
    customPlot1->graph(0)->setName("123");
    customPlot1->graph(0)->setLineStyle(QCPGraph::lsLine);

    customPlot1->graph(0)->setData(time,dataVec);
    customPlot1->rescaleAxes(true);
    customPlot1->replot();
}
