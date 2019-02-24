#ifndef PLOTFREQUENCY_H
#define PLOTFREQUENCY_H

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

class plotFrequency : public QWidget
{
    Q_OBJECT
public:
    explicit plotFrequency(QWidget *parent = 0);


    void createPlot(QStringList curveName);
    void plotWidgetLayout();
//    void dataToPlot(QString, QVector< QVector<QVector<double>>>, QVector<int>,QVector<int> );

    void setupDemo(int demoIndex);
    void setupQuadraticDemo(QCustomPlot *customPlot);

    void datatoPlot(QVector<QVector<double>>, QVector<QVector<double>>);
    void datatoPlot1(QVector<double> , QVector<double> );
    void filteredDataToPlot(QVector< QVector<QVector<double>>>, QVector<int>,QVector<QVector<double>>);
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

#endif // PLOTFREQUENCY_H
