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

#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QWidget>

#include "myjson.h"

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

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class Ui_ThemeWidgetForm;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

QT_CHARTS_USE_NAMESPACE

class ThemeWidget : public QWidget {
    Q_OBJECT
public:
    explicit ThemeWidget(QJsonObject jsonObject, QWidget *parent = 0);
    void updateScatterChart(QString tagX, QString tagY) const;
    ~ThemeWidget();

private Q_SLOTS:
    void updateUI();

private:
    DataTable getData(QJsonObject jsonObject);
    DataTable generateRandomData(int listCount, int valueMax,
                                 int valueCount) const;
    void populateThemeBox();
    void populateAnimationBox();
    void populateLegendBox();
    void connectSignals();
    QChart *createAreaChart() const;
    QChart *createBarChart(int valueCount) const;
    QChart *createPieChart() const;
    QChart *createLineChart(QString tagX, QString tagY) const;
    QChart *createSplineChart() const;
    QChart *createScatterChart(QString tagX, QString tagY) const;

public slots:
    // 添加json数据
    void addJsonData(QJsonObject *addData);

private:
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    QList<QChartView *> m_charts;
    DataTable m_dataTable;
    QVector<galgame *> m_galgames;

    QChartView *chartView;
    QScatterSeries* scatterSeries;
    QChart *scatterChart;
    Ui_ThemeWidgetForm *m_ui;
};

#endif /* THEMEWIDGET_H */
