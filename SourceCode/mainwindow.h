#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ShowControlPoints(int index);

    void UpdatePoints();

    void PlottingValues();

    double NewtonY(double t);

    double NewtonX(double t);

    double PolynomialCalculatorY(int noOfEq, double t);

    double PolynomialCalculatorX(int noOfEq, double t);
private slots:

    void MouseMovementDetection(QMouseEvent *event);
    void ShowControlPointsOnClick(QMouseEvent* event);

    void on_ResetButton_clicked();

private:
    Ui::MainWindow *ui;

    QCPCurve *curvePlotting;
    QCPCurve *pointsPlotting;
    QCPCurve *shellPlotting;

    QVector<QCPCurve*> shellCurves;
    QVector<QCPCurveData> curvePoints;
    QVector<QCPCurveData> dragPoints;

    QVector<QVector<double>> NewtonPointsX;
    QVector<QVector<double>> NewtonPointsY;

    int PointCounter = 0;

};
#endif // MAINWINDOW_H
