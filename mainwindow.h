#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloader.h"
#include "QListWidgetItem"
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QListWidgetItem>
#include <QChart>
#include <QtCharts/QDateTimeAxis>
#include <QTimer>
#include <QSplitter>
#include <QMainWindow>
#include <wiringPi.h>
#include "DistanceSensor.h"
#include <QMainWindow>
#include <QMap>
#include <QList>
#include "CSVParser.h"

#ifdef _version5
using namespace QtCharts;
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct weatherDatapoint
{
    QDateTime validTime;
    double value;
    QDateTime startTime;
    QDateTime endTime;
    double temperature;
    QString windSpeed;
    double probabilityOfPrecipitation;
    double relativeHumidity;
    QString shortForecast;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool on = false;

    QJsonArray forecastArray_quan;
    QJsonArray forecastArray_prob;

private:
    Ui::MainWindow *ui;
    Downloader *downloader_link;
    Downloader *downloaderGetForcast_prob;
    Downloader *downloaderGetForcast_quan;
    QMap<QString, QJsonValue> jsonValues;
    void showDatainList_quan();
    void showDatainList_prob();
    void inigetForecastURL(const QString &url);
    QMap<QString, QList<Location>> m_stateCityMap;

    QJsonArray getForecastArray(const QString &url);
    QString forecastURL_quan;
    QString forecastURL_prob;
    QVector<weatherDatapoint> weatherData_quan;
    QVector<weatherDatapoint> weatherData_prob;
    void configureCoordinateInputs();
    void setupButtonConnections();
    void setupStateCityMapping();
    void setupDistanceSensor();
    void configureWaterControls();
    void setupCharts();


    QChart *chart_quan;
    QChart *chart_prob;
    QChartView *chartView_quan;
    QChartView *chartView_prob;
    QLineSeries *series_quan;
    QLineSeries *series_prob;
    QDateTimeAxis *axisX_quan;
    QDateTimeAxis *axisX_prob;
    QValueAxis *axisY_quan;
    QValueAxis *axisY_prob;
    QTimer *Weatherupdatetimer;
    QTimer *timer1 = nullptr;
    QTimer *timer2 = nullptr;
    QTimer *depthCheckTimer =nullptr;
    bool isRainExpected = false;
    DistanceSensor DS;
    int check_interval = 10000;
    int Highlimit_Day = 3;
    int Lowlimit_Day = 0;
    double X_corredinate;
    double Y_corredinate;
    void setupChart_quan();
    void setupChart_prob();
    void updateChartData_prob(const QVector<QPair<QDateTime, double>> &data);
    void updateChartData_quan(const QVector<QPair<QDateTime, double>> &data);
    double distance;
    DistanceSensor sensor;
    void setupdistanceSensor();
    double X_coordinate;
    double Y_coordinate;
    void on_submitButton_clicked();
    double cumulative_precipitation = 0;

public slots:
    void on_exitButton_clicked();
    void getForecastURL(); 
    void getWeatherPrediction_quan();
    void getWeatherPrediction_prob();
    void plotForecast_quan();
    void plotForecast_prob();
    void on_click();
    void Turnoff();
    void TurnOn();


private slots:
    void updateWeatherData();
    void onStateChanged(const QString &state);
    void onCityChanged(const QString &city);

#ifdef GPIO
    void on_pushButton_Measure_distance_clicked();
#endif


};
#endif
