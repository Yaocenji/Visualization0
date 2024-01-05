/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "themewidget.h"

#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>

#include "ui_themewidget.h"

ThemeWidget::ThemeWidget(QJsonObject jsonObject, QWidget *parent)
    : QWidget(parent),
      m_listCount(3),
      m_valueMax(10),
      m_valueCount(7),
      m_dataTable(generateRandomData(m_listCount, m_valueMax, m_valueCount)),
      m_ui(new Ui_ThemeWidgetForm) {
    myjson *json = new myjson(jsonObject);
    m_galgames = json->galgames;

    m_ui->setupUi(this);
    populateThemeBox();
    populateAnimationBox();
    populateLegendBox();

    // create charts

    scatterChart = new QChart();
    scatterSeries = new QScatterSeries();

    //    chartView = new QChartView(createAreaChart());
    //    m_ui->gridLayout->addWidget(chartView, 1, 0);
    //    m_charts << chartView;

    //    chartView = new QChartView(createPieChart());
    //    // Funny things happen if the pie slice labels do not fit the screen,
    //    so we ignore size policy
    //    chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //    m_ui->gridLayout->addWidget(chartView, 1, 1);
    //    m_charts << chartView;

    //![5]
    //    chartView = new QChartView(createLineChart("length_votes", "rating"));
    //    m_ui->gridLayout->addWidget(chartView, 1, 0);
    //    //![5]
    //    m_charts << chartView;

    //    chartView = new QChartView(createBarChart(m_valueCount));
    //    m_ui->gridLayout->addWidget(chartView, 2, 0);
    //    m_charts << chartView;

    //    chartView = new QChartView(createSplineChart());
    //    m_ui->gridLayout->addWidget(chartView, 2, 1);
    //    m_charts << chartView;

    chartView = new QChartView(createScatterChart("length_votes", "rating"));
    m_ui->gridLayout->addWidget(chartView, 1, 0);
    m_charts << chartView;

    // Set defaults
    m_ui->antialiasCheckBox->setChecked(true);

    // Set the colors from the light theme as default ones
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    updateUI();
}

ThemeWidget::~ThemeWidget() {
    delete m_ui;
}

// DataTable ThemeWidget::getData(QJsonObject jsonObject)
//{

//}

DataTable ThemeWidget::generateRandomData(int listCount, int valueMax,
                                          int valueCount) const {
    DataTable dataTable;

    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + QRandomGenerator::global()->bounded(
                                  valueMax / (qreal)valueCount);
            QPointF value((j + QRandomGenerator::global()->generateDouble()) *
                              ((qreal)m_valueMax / (qreal)valueCount),
                          yValue);
            QString label =
                "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}

void ThemeWidget::populateThemeBox() {
    m_ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    m_ui->themeComboBox->addItem("Blue Cerulean",
                                 QChart::ChartThemeBlueCerulean);
    m_ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    m_ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    m_ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    m_ui->themeComboBox->addItem("High Contrast",
                                 QChart::ChartThemeHighContrast);
    m_ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    m_ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);
}

void ThemeWidget::populateAnimationBox() {
    m_ui->animatedComboBox->addItem("No Animations", QChart::NoAnimation);
    m_ui->animatedComboBox->addItem("GridAxis Animations",
                                    QChart::GridAxisAnimations);
    m_ui->animatedComboBox->addItem("Series Animations",
                                    QChart::SeriesAnimations);
    m_ui->animatedComboBox->addItem("All Animations", QChart::AllAnimations);
}

void ThemeWidget::populateLegendBox() {
    m_ui->legendComboBox->addItem("No Legend ", 0);
    m_ui->legendComboBox->addItem("Legend Top", Qt::AlignTop);
    m_ui->legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
    m_ui->legendComboBox->addItem("Legend Left", Qt::AlignLeft);
    m_ui->legendComboBox->addItem("Legend Right", Qt::AlignRight);
}

QChart *ThemeWidget::createAreaChart() const {
    QChart *chart = new QChart();
    chart->setTitle("Area chart");

    QLineSeries *lowerSeries = 0;
    QString name("Series ");
    int nameIndex = 0;
    for (int i(0); i < m_dataTable.count(); i++) {
        QLineSeries *upperSeries = new QLineSeries(chart);
        for (int j(0); j < m_dataTable[i].count(); j++) {
            Data data = m_dataTable[i].at(j);
            if (lowerSeries) {
                const QVector<QPointF> &points = lowerSeries->pointsVector();
                upperSeries->append(QPointF(j, points[i].y() + data.first.y()));
            } else {
                upperSeries->append(QPointF(j, data.first.y()));
            }
        }
        QAreaSeries *area = new QAreaSeries(upperSeries, lowerSeries);
        area->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(area);
        lowerSeries = upperSeries;
    }

    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, m_valueCount - 1);
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueMax);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}

QChart *ThemeWidget::createBarChart(int valueCount) const {
    Q_UNUSED(valueCount);
    QChart *chart = new QChart();
    chart->setTitle("Bar chart");

    QStackedBarSeries *series = new QStackedBarSeries(chart);
    for (int i(0); i < m_dataTable.count(); i++) {
        QBarSet *set = new QBarSet("Bar set " + QString::number(i));
        for (const Data &data : m_dataTable[i]) *set << data.first.y();
        series->append(set);
    }
    chart->addSeries(series);

    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueMax * 2);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}

QChart *ThemeWidget::createLineChart(QString tagX, QString tagY) const {
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setTitle("Line chart");

    double maxX = -1;
    double maxY = -1;
    QLineSeries *series = new QLineSeries(chart);
    for (int i = 0; i < m_galgames.count(); i++) {
        galgame *gal = m_galgames[i];
        if (gal->getDouble(tagX) > maxX) maxX = gal->getDouble(tagX);
        if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
        series->append(QPointF(gal->getDouble(tagX), gal->getDouble(tagY)));
        series->setName(gal->title);
        series->setUseOpenGL(true);
    }

    chart->addSeries(series);

    chart->createDefaultAxes();
    //    chart->axes(Qt::Horizontal).first()->setRange(0, maxX);
    //    chart->axes(Qt::Vertical).first()->setRange(0, maxY);

    // Add space to label to add space between labels and axis
    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.0f  ");

    return chart;
}

QChart *ThemeWidget::createPieChart() const {
    QChart *chart = new QChart();
    chart->setTitle("Pie chart");

    QPieSeries *series = new QPieSeries(chart);
    for (const Data &data : m_dataTable[0]) {
        QPieSlice *slice = series->append(data.second, data.first.y());
        if (data == m_dataTable[0].first()) {
            // Show the first slice exploded with label
            slice->setLabelVisible();
            slice->setExploded();
            slice->setExplodeDistanceFactor(0.5);
        }
    }
    series->setPieSize(0.4);
    chart->addSeries(series);

    return chart;
}

QChart *ThemeWidget::createSplineChart() const {
    QChart *chart = new QChart();
    chart->setTitle("Spline chart");
    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list : m_dataTable) {
        QSplineSeries *series = new QSplineSeries(chart);
        for (const Data &data : list) series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        nameIndex++;
        chart->addSeries(series);
    }

    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, m_valueMax);
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueCount);

    // Add space to label to add space between labels and axis
    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    return chart;
}

QChart *ThemeWidget::createScatterChart(QString tagX, QString tagY) const {
    // scatter chart
    scatterChart->setTitle("Scatter chart");

    double maxX = -1;
    double maxY = -1;

    for (int i = 0; i < m_galgames.count(); i++) {
        galgame *gal = m_galgames[i];
        if (gal->getDouble(tagX) > maxX) maxX = gal->getDouble(tagX);
        if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
        scatterSeries->append(QPointF(gal->getDouble(tagX), gal->getDouble(tagY)));
    }
    scatterChart->legend()->hide();
    scatterChart->addSeries(scatterSeries);

    scatterChart->createDefaultAxes();
    scatterChart->axes(Qt::Horizontal).first()->setRange(0, 150);
        scatterChart->axes(Qt::Vertical).first()->setRange(0, 100);

    // Add space to label to add space between labels and axis
    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(scatterChart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    return scatterChart;
}

void ThemeWidget::updateScatterChart(QString tagX, QString tagY) const {
    scatterSeries->clear();
    QList<QPointF> replaceData;
    for (int i = 0; i < m_galgames.count(); i++) {
        galgame *gal = m_galgames[i];
        replaceData.append(QPointF(gal->getDouble(tagX), gal->getDouble(tagY)));
    }
    qDebug() << replaceData;
    scatterSeries->replace(replaceData);
    scatterChart->update();
    chartView->update();
}

void ThemeWidget::updateUI() {
    //![6]
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
        m_ui->themeComboBox->itemData(m_ui->themeComboBox->currentIndex())
            .toInt());
    //![6]
    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {
            //![7]
            chartView->chart()->setTheme(theme);
            //![7]
        }

        // Set palette colors based on selected theme
        //![8]
        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
            //![8]
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
        window()->setPalette(pal);
    }

    // Update antialiasing
    //![11]
    bool checked = m_ui->antialiasCheckBox->isChecked();
    for (QChartView *chart : charts)
        chart->setRenderHint(QPainter::Antialiasing, checked);
    //![11]

    // Update animation options
    //![9]
    QChart::AnimationOptions options(
        m_ui->animatedComboBox->itemData(m_ui->animatedComboBox->currentIndex())
            .toInt());
    if (!m_charts.isEmpty() &&
        m_charts.at(0)->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }
    //![9]

    // Update legend alignment
    //![10]
    Qt::Alignment alignment(
        m_ui->legendComboBox->itemData(m_ui->legendComboBox->currentIndex())
            .toInt());

    if (!alignment) {
        for (QChartView *chartView : charts)
            chartView->chart()->legend()->hide();
    } else {
        for (QChartView *chartView : charts) {
            chartView->chart()->legend()->setAlignment(alignment);
            chartView->chart()->legend()->show();
        }
    }
    //![10]
}

void ThemeWidget::addJsonData(QJsonObject *addData) {
    qDebug() << "added";
    QJsonArray results = (*addData)["results"].toArray();
    for (int i = 0; i < results.count(); i++) {
        QJsonObject galData(results.at(i).toObject());
        galgame *gal = new galgame();
        gal->id = galData["id"].toString();
        gal->languages = JsonToStringArray(galData["languages"]);
        gal->length_minutes = galData["length_minutes"].toInt();
        gal->length_votes = galData["length_votes"].toInt();
        gal->platforms = JsonToStringArray(galData["platforms"]);
        gal->rating = galData["rating"].toDouble();
        gal->title = galData["title"].toString();
        gal->votecount = galData["votecount"].toInt();
        m_galgames.append(gal);
    }
    updateScatterChart("length_votes", "rating");
}
