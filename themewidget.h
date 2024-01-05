#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartGlobal>
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
#include <QtWidgets/QWidget>

#include "myjson.h"
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
    void sortByTag(QString tag, QVector<galgame *> &galgames);
    QChart *updateBarChart(QString tag);
    QChart *updatePieChart(QString tag);
    QChart *updateLineChart(QString tagX, QString tagY);
    QChart *updateSplineChart(QString tagX, QString tagY);
    QChart *updateScatterChart(QString tagX, QString tagY);
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
    //    QChart *createAreaChart() const;
    //    QChart *createBarChart(QString tag) const;
    //    QChart *createPieChart(QString tag) const;
    //    QChart *createLineChart(QString tagX, QString tagY) const;
    //    QChart *createSplineChart(QString tagX, QString tagY) const;
    //    QChart *createScatterChart(QString tagX, QString tagY) const;

public slots:
    // 添加json数据
    void addJsonData(QJsonObject *addData);
    // 添加tag数据
    void addTagData(QJsonObject *addData);

    // 清空vn数据
    void clearVnData();

    // 过滤器条目下拉菜单更改时，对应更改后续内容
    void updateFilterComboBoxes();
    // 添加过滤器条目
    void addFilter();
    // 应用
    void ApplyFilter();


    void updateChartComboBox();
    void updateChart();

public:
    // 根据表格内容，输出filter数组
    QVector<QVector<QString>> getFilter();

private:
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    QList<QChartView *> m_charts;
    DataTable m_dataTable;
    QVector<galgame *> m_galgames;

    QChartView *chartView;

    QBarSeries *barSeries;
    QPieSeries *pieSeries;
    QLineSeries *lineSeries;
    QSplineSeries *splineSeries;
    QScatterSeries *scatterSeries;

    QChart *barChart;
    QChart *pieChart;
    QChart *lineChart;
    QChart *splineChart;
    QChart *scatterChart;

    QMap<QString, QVector<QString> > chartMap;

    QMap<QString, QVector<QString> > chartMap;

public:
    Ui_ThemeWidgetForm *m_ui;

private:
    // 模板
    QJsonObject schema;
    // 保存的tag数据
    QVector<tag *> tagData;
};

#endif /* THEMEWIDGET_H */
