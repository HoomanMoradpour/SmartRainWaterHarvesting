#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "CSVParser.h"
#include <QComboBox>
#include <QLabel>
#include <QMap>
#include <QList>
#include <QStringList>
#include <QDebug>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <wiringPi.h>
#include <iostream>
#include <unistd.h> // for sleep()
#include <QTimer>
#include <QDebug>
#include <string>
//#include "DistanceSensor.h"
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , Weatherupdatetimer(new QTimer(this))
    , timer1(new QTimer(this))
    , timer2(new QTimer(this))
    , depthCheckTimer(new QTimer(this))
{
    // Setup UI
    ui->setupUi(this);

    // Configure input placeholders and styles
    configureCoordinateInputs();

    // Connect button signals to slots
    setupButtonConnections();

    // Parse CSV for state-city mapping and initialize combo boxes
    setupStateCityMapping();

    // Setup weather update timer
    connect(Weatherupdatetimer, &QTimer::timeout, this, &MainWindow::updateWeatherData);

    // Initialize distance sensor
    setupDistanceSensor();

    // Configure water level and valve controls
    configureWaterControls();

#ifdef GPIO
    setupGPIO();
#endif

    // Setup charts
    setupCharts();
}

void MainWindow::configureCoordinateInputs() {
    ui->X_coordinate->setPlaceholderText("Enter X Coordinate");
    ui->Y_coordinate->setPlaceholderText("Enter Y Coordinate");
    QString lineEditStyle = "QLineEdit { color: black; } QLineEdit::placeholder { color: lightgray; }";
    ui->X_coordinate->setStyleSheet(lineEditStyle);
    ui->Y_coordinate->setStyleSheet(lineEditStyle);
}

void MainWindow::setupButtonConnections() {
    connect(ui->submitButton, &QPushButton::clicked, this, &MainWindow::on_submitButton_clicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::on_exitButton_clicked);

    connect(ui->stateComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onStateChanged);
    connect(ui->cityComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onCityChanged);

#ifdef GPIO
    connect(ui->pushButton_Measure_distance, &QPushButton::clicked, this, &MainWindow::on_pushButton_Measure_distance_clicked);
#endif
    connect(ui->Valve, &QPushButton::clicked, this, &MainWindow::on_click);

    connect(timer1, &QTimer::timeout, this, &MainWindow::Turnoff);
    connect(timer2, &QTimer::timeout, this, &MainWindow::TurnOn);
}

void MainWindow::setupStateCityMapping() {
    CSVParser parser("/home/arash/Projects/SmartRainWaterHarvesting/uscities.csv");
    m_stateCityMap = parser.parse();

    ui->stateComboBox->addItems(m_stateCityMap.keys());
    onStateChanged(ui->stateComboBox->currentText());
}

void MainWindow::setupDistanceSensor() {
    setupdistanceSensor();
}

void MainWindow::configureWaterControls() {
    ui->pushButton_Measure_distance->setText("Water Level");
    ui->Valve->setText("Valve on");
}

#ifdef GPIO
void MainWindow::setupGPIO() {
    wiringPiSetupGpio(); // Use BCM pin numbering

    DistanceSensor sensor;
    if (!sensor.initialize()) {
        qDebug() << "Something is wrong with the distance sensor!";
    } else {
        double distance = sensor.getDistance();
        qDebug() << "Measured Distance =" << distance << "cm";
    }

    sensor.cleanup();
    DS.initialize();
}
#endif

void MainWindow::setupCharts() {
    setupChart_prob();
    setupChart_quan();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStateChanged(const QString &state)
{
    ui->cityComboBox->clear();

    if (m_stateCityMap.contains(state)) {
        QList<Location> locations = m_stateCityMap[state];
        QStringList cities;
        for (const Location &location : locations) {
            cities.append(location.city);
        }
        ui->cityComboBox->addItems(cities);
    }
}

void MainWindow::onCityChanged(const QString &city)
{

    QString state = ui->stateComboBox->currentText();
    if (m_stateCityMap.contains(state)) {
        QList<Location> locations = m_stateCityMap[state];
        for (const Location &location : locations) {
            if (location.city == city) {
                //ui->coordinatesLabel->setText(QString("Coordinates: %1, %2")
                                                 // .arg(location.latitude)
                                                  //.arg(location.longitude));
                ui->X_coordinate->setText(QString::number(location.latitude));
                ui->Y_coordinate->setText(QString::number(location.longitude));
                return;
            }
        }
    }

    //ui->coordinatesLabel->setText("City not found!");
    ui->X_coordinate->clear();
    ui->Y_coordinate->clear();
}

void MainWindow::on_submitButton_clicked() {
    bool xOk, yOk;
    X_coordinate = ui->X_coordinate->text().toDouble(&xOk);
    Y_coordinate = ui->Y_coordinate->text().toDouble(&yOk);

    if (xOk && yOk) {
        QString url = QString("https://api.weather.gov/points/%1,%2").arg(X_coordinate).arg(Y_coordinate);
        qDebug() << "Generated URL: " << url;


        inigetForecastURL(url);
    } else {
        QMessageBox::warning(this, "Input Error", "Please enter valid coordinates.");
    }
}



/*void MainWindow::on_click()
{
    if (on)
    {
        on = false;
        digitalWrite(18, HIGH);
        ui->pushButton->setText("Valve on");
        //qDebug()<<"Turned off by the user";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Valve off by the user" );
        timer2->start(10000);
    }
    else
    {
        on = true;
        digitalWrite(18, LOW);
        ui->pushButton->setText("Valve off");
        //qDebug()<<"Turned on by the user";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Valve on by the user" );
        timer1->start(10000);
    }
}

void MainWindow::Turnoff()
{
    if (on)
    {
        timer1->stop();
        on = false;
        digitalWrite(18, HIGH);
        //qDebug() << "Turned off by timer";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Valve off by timer" );
        ui->pushButton->setText("Turn on");
        timer2->start(10000);
    }
}

void MainWindow::TurnOn()
{
    if (!on)
    {   timer2->stop();
        on = true;
        digitalWrite(18, LOW);
        ui->pushButton->setText("Turn off");
        //qDebug() << "Turned on by timer";
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Valve on by timer" );
        timer1->start(10000);
    }
}
*/


/*DistanceSensor sensor;
const double MAX_DEPTH = 50.0; // Max depth to keep valve open (in cm)
const double MIN_DEPTH = 10.0;  // Min depth to close valve (in cm)
*/


void MainWindow::setupdistanceSensor() {
    //sensor.initialize();
    //pinMode(18, OUTPUT); // Valve pin
    // Set up a timer for regular depth checking
    //depthCheckTimer->start(1000); // Check every second
}

/*void MainWindow::checkWaterDepth() {
    double distance = sensor.getDistance();

    if (distance < MIN_DEPTH && on) {
        Turnoff(); // Turn off the valve if water depth is below MIN_DEPTH
    } else if (distance > MAX_DEPTH && !on) {
        TurnOn(); // Turn on the valve if water depth is above MAX_DEPTH
    }
  else if (cumulative_precipitation > 3.0 && distance > 50.0 && !on) {
        TurnOn();
    }
    qdebug() << "valve works"
}*/

void MainWindow::Turnoff() {
    if (on) {
        on = false;
        //digitalWrite(18, HIGH); // Turn off valve
        // Update UI
        //ui->textEdit->append("Valve off by automatic control");
        ui->textEdit->setText(ui->textEdit->toPlainText() + "\n" + "Valve off by automatic control");
        ui->Valve->setText("Valve on");
    }
}


void MainWindow::TurnOn() {
    if (!on) {
        on = true;
        //digitalWrite(18, LOW); // Turn on valve
        // Update UI
        ui->textEdit->append("Valve on by automatic control");
        ui->Valve->setText("Valve off");
    }
}

void MainWindow::on_click() {
    // This function can still be used to manually control the valve
    if (on) {
        Turnoff();
    } else {
        TurnOn();
    }
}



void MainWindow::setupChart_quan()
{
    series_quan = new QLineSeries();

    for (int i = 0; i < weatherData_quan.count(); i++)
    {
       QDateTime momentInTime = weatherData_quan[i].validTime;
       series_quan->append(momentInTime.toMSecsSinceEpoch(),weatherData_quan[i].value);
    }

    chart_quan = new QChart();
    chart_quan->addSeries(series_quan);
    chart_quan->legend()->hide();
    chart_quan->setTitle("Precipitation Intensity");

    axisX_quan = new QDateTimeAxis;
    axisX_quan->setTickCount(10);
    axisX_quan->setFormat("dd-MM-yyyy HH:mm");
    axisX_quan->setTitleText("Date");
    chart_quan->addAxis( axisX_quan, Qt::AlignBottom);
    axisX_quan->setLabelsAngle(-90);
    series_quan->attachAxis(axisX_quan);

    axisY_quan = new QValueAxis;
    axisY_quan->setLabelFormat("%i");
    axisY_quan->setTitleText("Precipitation Intensity (mm)");
    axisY_quan->setRange(0, 100);
    chart_quan->addAxis(axisY_quan, Qt::AlignLeft);
    series_quan->attachAxis(axisY_quan);

    chartView_quan = new QChartView(chart_quan);
    chartView_quan->setRenderHint(QPainter::Antialiasing);

   ui->LayoutQuanPanel->addWidget(chartView_quan);
    resize(800, 1050);

    show();
}



void MainWindow::setupChart_prob()
{
  series_prob = new QLineSeries();

    for (int i = 0; i < weatherData_prob.count(); i++)
    {
        QDateTime momentInTime =weatherData_prob[i].startTime;
        series_prob->append(momentInTime.toMSecsSinceEpoch(), weatherData_prob[i].probabilityOfPrecipitation);
    }

    chart_prob = new QChart();
    chart_prob->addSeries(series_prob);
    chart_prob->legend()->hide();
    chart_prob->setTitle("Probability of Precipitation");

    axisX_prob = new QDateTimeAxis;
    axisX_prob->setTickCount(10);
    axisX_prob->setFormat("dd-MM-yyyy HH:mm");
    axisX_prob->setTitleText("Date");
    chart_prob->addAxis(axisX_prob, Qt::AlignBottom);
    axisX_prob->setLabelsAngle(-90);
    series_prob->attachAxis(axisX_prob);

    axisY_prob = new QValueAxis;
    axisY_prob->setLabelFormat("%i");
    axisY_prob->setTitleText("Probability of Precipitation (%)");
    axisY_prob->setRange(0, 100);
    chart_prob->addAxis(axisY_prob, Qt::AlignLeft);
    series_prob->attachAxis(axisY_prob);

    chartView_prob = new QChartView(chart_prob);
    chartView_prob->setRenderHint(QPainter::Antialiasing);


    ui->LayoutProbPanel->addWidget(chartView_prob);
    resize(800, 1050);

    show();
}



void MainWindow::updateChartData_prob(const QVector<QPair<QDateTime, double>> &data)
{
    series_prob->clear();

    double max_y = 0;
    for (const auto &point : data) {
        qreal time = point.first.toMSecsSinceEpoch();
        series_prob->append(time, point.second);
        max_y = max(max_y, point.second);

    }

    if (!data.isEmpty()) {
        qDebug()<<data.first().first<<":"<<data.last().first;
        axisX_prob->setRange(data.first().first, data.last().first);
        axisY_prob->setRange(0, max_y);
    }
}

void MainWindow::updateChartData_quan(const QVector<QPair<QDateTime, double>> &data)
{
    series_quan->clear();
    double max_y = 0;
    for (const auto &point : data) {
        qreal time = point.first.toMSecsSinceEpoch();
        series_quan->append(time, point.second);
        max_y = max(max_y, point.second);
    }

    if (!data.isEmpty()) {
        qDebug()<<data.first().first<<":"<<data.last().first;
        axisX_quan->setRange(data.first().first, data.last().first);
        axisY_quan->setRange(0, max_y);
    }
}

void MainWindow::plotForecast_quan()
{
    QVector<QPair<QDateTime, double>> forecastData_quan;

    for (const auto &dataPoint : weatherData_quan) {
        forecastData_quan.append(qMakePair(dataPoint.validTime, dataPoint.value));
    }

    updateChartData_quan(forecastData_quan);
}

void MainWindow::plotForecast_prob()
{
    QVector<QPair<QDateTime, double>> forecastData_prob;

    for (const auto &dataPoint : weatherData_prob) {
        forecastData_prob.append(qMakePair(dataPoint.startTime, dataPoint.probabilityOfPrecipitation));
    }

    updateChartData_prob(forecastData_prob);
}


void MainWindow::showDatainList_quan() {
    ui->listWidgetQuan->clear();
    for (const auto &dataPoint :weatherData_quan) {

        QString itemText = QString("ValidTime: %1, Value: %2 mm")
                               .arg(dataPoint.validTime.toString(Qt::ISODate))
                               .arg(dataPoint.value)
            ;

        ui->listWidgetQuan->addItem(itemText);
    }

    cumulative_precipitation = 0;
    for (int i=0; i<weatherData_quan.size(); i++)
    {
        if (weatherData_quan[i].validTime<QDateTime::currentDateTime().addDays(Highlimit_Day) && weatherData_quan[i].validTime>=QDateTime::currentDateTime().addDays(Lowlimit_Day))
            cumulative_precipitation+=weatherData_quan[i].value;

    }
    ui->Cumulative->setText("Raining Amount perdiciton: " + QString::number(cumulative_precipitation, 'f', 2) + " mm ");

    qDebug()<<cumulative_precipitation;

    if (cumulative_precipitation > 3.0) {
       //checkWaterDepth();  // Check water depth as we have significant precipitation
    }
}


void MainWindow::showDatainList_prob() {
    ui->listWidgetProb->clear();
    for (const auto &dataPoint : weatherData_prob) {

        QString itemText = QString("Start Time: %1, End Time: %2, Temperature: %3 F, windSpeed: %4 , Probability of Precipitation: %5%, Relative Humidity: %6%, shortForecast: %7")
                               .arg(dataPoint.startTime.toString(Qt::ISODate))
                               .arg(dataPoint.endTime.toString(Qt::ISODate))
                               .arg(dataPoint.temperature)
                               .arg(dataPoint.windSpeed)
                               .arg(dataPoint.probabilityOfPrecipitation)
                               .arg(dataPoint.relativeHumidity)
                               .arg(dataPoint.shortForecast)
            ;
        ui->listWidgetProb->addItem(itemText);
    }
}

void MainWindow::inigetForecastURL(const QString &url)
{
    downloader_link = new Downloader(url);

    connect(downloader_link, SIGNAL(download_finished_sgnl()), this, SLOT(getForecastURL()));
}

void MainWindow::getForecastURL()
{

    forecastURL_quan = downloader_link->loadedJson.object().value("properties").toObject().value("forecastGridData").toString();
    forecastURL_prob= downloader_link->loadedJson.object().value("properties").toObject().value("forecastHourly").toString();
    //delete downloader;
    downloaderGetForcast_quan = new Downloader(forecastURL_quan, this);
    downloaderGetForcast_prob = new Downloader(forecastURL_prob, this);
    connect(downloaderGetForcast_quan, SIGNAL(download_finished_sgnl()), this, SLOT(getWeatherPrediction_quan()));
    connect(downloaderGetForcast_prob, SIGNAL(download_finished_sgnl()), this, SLOT(getWeatherPrediction_prob()));

}


void MainWindow::getWeatherPrediction_quan()
{
    if (!downloaderGetForcast_quan || downloaderGetForcast_quan->loadedJson.isNull()) {
        qDebug() << "No valid JSON data loaded.";
        return;
    }

    QJsonObject jsonObject = downloaderGetForcast_quan->loadedJson.object();
    qDebug() << jsonObject;

    QJsonObject propObj = jsonObject.value("properties").toObject();
    qDebug() << propObj;

    QJsonArray periodsArray = propObj.value("quantitativePrecipitation").toObject().value("values").toArray();
    qDebug() << periodsArray;


weatherData_quan.clear();
    for (const QJsonValue &value : periodsArray) {
        QJsonObject forecastObj = value.toObject();

        weatherDatapoint dataPoint;

        qDebug()<<forecastObj.value("value");
        qDebug()<<forecastObj.value("validTime").toString().split("+")[0];
        dataPoint.validTime = QDateTime::fromString(forecastObj.value("validTime").toString().split("+")[0], Qt::ISODate);

        dataPoint.value = forecastObj.value("value").toDouble();

        weatherData_quan.append(dataPoint);
       // distance = sensor.getDistance();
        // Get the distance


        // Use the distance and weatherData_quan to decide whether to open the valve or not

        //You don't need timer for distance.
    }

  plotForecast_quan();
  showDatainList_quan();

  // Measure distance
  // Open or close the value
 Weatherupdatetimer->start(check_interval);
}


void MainWindow::getWeatherPrediction_prob()
    {

    if (!downloaderGetForcast_prob || downloaderGetForcast_prob->loadedJson.isNull()) {
        qDebug() << "No valid JSON data loaded.";
        return;
    }

        QJsonObject jsonObject = downloaderGetForcast_prob->loadedJson.object();
        qDebug() << jsonObject;

        QJsonObject propObj = jsonObject.value("properties").toObject();
        qDebug() << propObj;

        QJsonArray periodsArray = propObj.value("periods").toArray();
        qDebug() << periodsArray;
        weatherData_prob.clear();
    for (const QJsonValue &value : periodsArray) {
        QJsonObject forecastObj = value.toObject();

         weatherDatapoint dataPoint;


        qDebug()<<forecastObj.value("startTime").toString().split("+")[0];


        dataPoint.startTime = QDateTime::fromString(forecastObj.value("startTime").toString(), Qt::ISODate);
        dataPoint.endTime = QDateTime::fromString(forecastObj.value("endTime").toString(), Qt::ISODate);
        dataPoint.temperature = forecastObj.value("temperature").toDouble();
        dataPoint.windSpeed = forecastObj.value("windSpeed").toString();
        dataPoint.probabilityOfPrecipitation= forecastObj.value("probabilityOfPrecipitation").toObject().value("value").toDouble();
        dataPoint.relativeHumidity = forecastObj.value("relativeHumidity").toObject().value("value").toDouble();
        dataPoint.shortForecast = forecastObj.value("shortForecast").toString();


        qDebug() << "Start Time:" << dataPoint.startTime;
        qDebug() << "End Time:" << dataPoint.endTime;
        qDebug() << "Temperature:" << dataPoint.temperature;
        qDebug() << "Wind Speed:" << dataPoint.windSpeed;
        qDebug() << "Probability of Precipitation:" << dataPoint.probabilityOfPrecipitation;
        qDebug() << "Relative Humidity:" << dataPoint.relativeHumidity;
        qDebug() << "Short Forecast:" << dataPoint.shortForecast;

      weatherData_prob.append(dataPoint);
    }


    plotForecast_prob();
    showDatainList_prob();

}


void MainWindow::updateWeatherData()
{
    qDebug() << "Updating weather data...";

    downloaderGetForcast_quan->setUrl(forecastURL_quan);
    downloaderGetForcast_prob->setUrl(forecastURL_prob);
    downloaderGetForcast_quan->execute();
    downloaderGetForcast_prob->execute();
    // Measure distance

}

void MainWindow::on_exitButton_clicked() {

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit Confirmation",
                                  "Are you sure you want to quit?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QApplication::quit(); // Closes the application
    }
    // Do nothing if "No"
}



#ifdef GPIO
void MainWindow::on_pushButton_Measure_distance_clicked()
{

    double distance = DS.getDistance();
    ui->pushButton_Measure_distance;
    ui->label->setText("Water Level: "  +  (QString::number(distance) +  " Cm" ));
}
#endif











