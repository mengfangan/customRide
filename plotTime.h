#ifndef PLOTTIME_H
#define PLOTTIME_H

#include <QWidget>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QGroupBox>

#include <QMouseEvent>
#include<QPixmap>
#include<QFileDialog>
#include<QAction>
#include<QLabel>

#include<qcustomplot.h>


class plottime : public QWidget
{
    Q_OBJECT
public:
    explicit plottime(QWidget *parent = 0);


    void createPlot(QStringList curveName);
    void plotWidgetLayout();
    void dataToPlot(QVector< QVector<QVector<double>>>, QVector<int>,QVector<int> );

    void setupDemo(int demoIndex);
    void setupQuadraticDemo(QCustomPlot *customPlot);

    void allDataToPlot(QVector<double>, QVector<double>, QVector<double>);
    void filteredDataToPlot(QVector< QVector<QVector<double>>>, QVector<int>,QVector<QVector<double>>);

    void hPlot(QVector<double> time, QVector<double>h);

    void ifftFiltereddatatoPlot(QVector<double> ffvec, QVector<double> amplitude);

    void eventDataToPlot(QVector<double> time, QVector<double> dataVec);
    void dataPlot(QVector<double> time, QVector<double> dataVec);

    QCustomPlot *customPlot1;
//    QCustomPlot *customPlot2;

    QString demoName;
    QTimer dataTimer;
    QCPItemTracer *itemDemoPhaseTracer;
    int currentDemoIndex;
    QLabel *cursorPosLabel;
    QAction *cursorMeasuAct;
    QAction *arrowCursorAct;
    QAction *zoomXAct;
    QAction *zoomYAct;
    QAction *zoomXYAct;


signals:

private slots:
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void saveGraph();
    void cursorMeasurement();
    void arrowCursorSlotFunc();
    void zoomX();
    void zoomY();
    void zoomXY();

    void my_mouseMove(QMouseEvent* event);
    void titleDoubleClick(QMouseEvent *event);
};

#endif // PLOTTIME_H
