#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    ui->GraphContainer->resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    ui->GraphContainer->background().fill(Qt::GlobalColor::red);
    ui->GraphContainer->addGraph();
    ui->GraphContainer->addGraph();

    curvePlotting = new QCPCurve(ui->GraphContainer->xAxis, ui->GraphContainer->yAxis);
    pointsPlotting = new QCPCurve(ui->GraphContainer->xAxis, ui->GraphContainer->yAxis);

    pointsPlotting->setScatterStyle(QCPScatterStyle::ssDisc);
    pointsPlotting->setPen(QPen(Qt::black));

    curvePlotting->setPen(QPen(Qt::blue));


    ui->GraphContainer->setMouseTracking(false);

    connect(ui->GraphContainer, SIGNAL(mouseMove(QMouseEvent*)), SLOT(MouseMovementDetection(QMouseEvent*)));
    connect(ui->GraphContainer, SIGNAL(mousePress(QMouseEvent*)), SLOT(ShowControlPointsOnClick(QMouseEvent*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ShowControlPointsOnClick(QMouseEvent* event)
{
    ++PointCounter;
    if (event->button() == Qt::LeftButton)
    {
        float dragPointX = ui->GraphContainer->xAxis->pixelToCoord(event->pos().x());
        float dragPointY = ui->GraphContainer->yAxis->pixelToCoord(event->pos().y());

        dragPoints.push_back(QCPCurveData(PointCounter, dragPointX, dragPointY));

        UpdatePoints();
    }
}

void MainWindow::UpdatePoints()
{
    curvePoints.clear();
    PlottingValues();
    curvePlotting->data()->set(curvePoints, true);
    pointsPlotting->data()->set(dragPoints, true);
    ui->GraphContainer->replot();
    ui->GraphContainer->update();
}


void MainWindow::MouseMovementDetection(QMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton)
    {
        for (int i = 0; i < dragPoints.count(); i++)
        {
            if ((abs(dragPoints[i].key - ui->GraphContainer->xAxis->pixelToCoord(event->pos().x()))  < 0.2)
                && (abs(dragPoints[i].value - ui->GraphContainer->yAxis->pixelToCoord(event->pos().y()))  < 0.2))
            {
                dragPoints[i].value = ui->GraphContainer->yAxis->pixelToCoord(event->pos().y());
                dragPoints[i].key = ui->GraphContainer->xAxis->pixelToCoord(event->pos().x());
                UpdatePoints();

            }
        }
    }
}

void MainWindow::PlottingValues()
{
    if (dragPoints.count() < 3)
    {
        return;
    }

    // Resizing intial vectors of x and y to 0
    NewtonPointsX.resize(dragPoints.count() + 1);
    NewtonPointsY.resize(dragPoints.count() + 1);

    // Resizing second vectors of x and y to 0
    for (int i = 0; i < dragPoints.count() + 1; i++){NewtonPointsX[i].resize(dragPoints.count() + 1);}
    for (int i = 0; i < dragPoints.count() + 1; i++){NewtonPointsY[i].resize(dragPoints.count() + 1);}

    // Initializing both x and y vector to 0
    for (int i = 0; i < dragPoints.count() + 1; i++)
    {
        for (int j = 0; j < dragPoints.count() + 1; j++)
        {
            NewtonPointsX[i][j] = 0;
        }
    }

    for (int i = 0; i < dragPoints.count() + 1; i++)
    {
        for (int j = 0; j < dragPoints.count() + 1; j++)
        {
            NewtonPointsY[i][j] = 0;
        }
    }

    // Setting the first row values of x and y matrix
    for (int i = 0; i < dragPoints.count() + 1; i++)
    {
        NewtonPointsX[0][i] = i;
    }

    for (int i = 0; i < dragPoints.count() + 1; i++)
    {
        NewtonPointsY[0][i] = i;
    }


    // Setting the second row of x and y matrix
    for (int i = 0; i < dragPoints.count(); i++)
    {
        NewtonPointsX[1][i] = dragPoints[i].key;
    }

    for (int i = 0; i < dragPoints.count(); i++)
    {
        NewtonPointsY[1][i] = dragPoints[i].value;
    }

    // Divided Difference calculation of x and y matrix
    for (int j = 2; j <= dragPoints.count(); j++)
    {
        for (int i = 0; i < dragPoints.count() - 1; i++)
        {
            NewtonPointsX[j][i] = (NewtonPointsX[j-1][i+1] - NewtonPointsX[j-1][i])
                    / (j - 1);
        }
    }

    for (int j = 2; j <= dragPoints.count(); j++)
    {
        for (int i = 0; i < dragPoints.count() - 1; i++)
        {
            NewtonPointsY[j][i] = (NewtonPointsY[j-1][i+1] - NewtonPointsY[j-1][i])
                    / (j - 1);
        }
    }


    // Final points to plot calculation
    for (int temp = 0; temp <= (dragPoints.count() - 1) * 100; temp++)
    {
        double t = (double)temp / 100.0f;

        double finalX = NewtonX(t);
        double finalY = NewtonY(t);

        curvePoints.push_back(QCPCurveData(temp, finalX, finalY));
    }

}

double MainWindow::NewtonY(double t)
{
    double sumY  = NewtonPointsY[1][0];
    for (int i = 0; i < dragPoints.count() - 1; i++)
    {
       sumY += NewtonPointsY[i + 2][0] * PolynomialCalculatorY(i + 1, t);
    }
    return sumY;
}

double MainWindow::NewtonX(double t)
{
    double sumX  = NewtonPointsX[1][0];
    for (int i = 0; i < dragPoints.count() - 1; i++)
    {
       sumX += NewtonPointsX[i + 2][0] * PolynomialCalculatorX(i + 1, t);
    }
    return sumX;
}

double MainWindow::PolynomialCalculatorY(int noOfEq, double t)
{
    double value = 1;

    for (int i = 0; i < noOfEq; i++)
    {
        value *= (t - NewtonPointsY[0][i]);
    }

    return value;
}

double MainWindow::PolynomialCalculatorX(int noOfEq, double t)
{
    double value = 1;

    for (int i = 0; i < noOfEq; i++)
    {
        value *= (t - NewtonPointsX[0][i]);
    }

    return value;
}

void MainWindow::on_ResetButton_clicked()
{
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

