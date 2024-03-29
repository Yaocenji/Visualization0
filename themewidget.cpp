#include "themewidget.h"

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

    chartView = new QChartView();
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_ui->horizontalLayout->addWidget(chartView);

    barChart = new QChart();
    pieChart = new QChart();
    lineChart = new QChart();
    splineChart = new QChart();
    scatterChart = new QChart();

    barSeries = new QBarSeries();
    pieSeries = new QPieSeries();
    lineSeries = new QLineSeries();
    splineSeries = new QSplineSeries();
    scatterSeries = new QScatterSeries();

    m_ui->chartType->addItems({"bar", "pie", "line", "spline", "scatter"});

    QVector<QString> v;
    v.append({"languages", "length_minutes", "length_votes", "platforms",
              "rating", "votecount", "released"});
    chartMap.insert("bar", v);
    v.clear();

    v.append({"languages", "platforms", "released"});
    chartMap.insert("pie", v);
    v.clear();

    v.append({"length_minutes", "length_votes", "rating", "votecount", "released"});
    chartMap.insert("line", v);
    v.clear();

    v.append({"length_minutes", "length_votes", "rating", "votecount", "released"});
    chartMap.insert("spline", v);
    v.clear();

    v.append({"length_minutes", "length_votes", "rating", "votecount", "released"});
    chartMap.insert("scatter", v);
    v.clear();

    connect(m_ui->chartType, SIGNAL(currentIndexChanged(int)), this,
            SLOT(updateChartComboBox()));
    updateChartComboBox();

    connect(m_ui->chartType, SIGNAL(currentIndexChanged(int)), this,
            SLOT(updateFetchButton()));
    connect(m_ui->tagX, SIGNAL(currentIndexChanged(int)), this,
            SLOT(updateFetchButton()));
    connect(m_ui->tagY, SIGNAL(currentIndexChanged(int)), this,
            SLOT(updateFetchButton()));

    // Set defaults
    m_ui->antialiasCheckBox->setChecked(true);

    // Set the colors from the light theme as default ones
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    updateUI();

    // 将api模板读取
    QFile tmpfile(":/Schema/enum.json");
    QString errMsg;
    QFileDevice::FileError err = QFileDevice::NoError;
    if (!tmpfile.open(QIODevice::ReadOnly)) {
        errMsg = tmpfile.errorString();
        err = tmpfile.error();
        qDebug() << errMsg << err;
    }
    QByteArray data = tmpfile.readAll();
    tmpfile.close();
    schema = QJsonDocument::fromJson(data).object();

    // 连接filter下拉菜单改变时与更新函数
    QObject::connect(m_ui->filterComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(updateFilterComboBoxes()));
    // 先更新一遍
    updateFilterComboBoxes();

    // filter表
    m_ui->filterTable->setColumnCount(3);
    m_ui->filterTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    // 连接添加filter行
    QObject::connect(m_ui->filterAdd, SIGNAL(clicked()), this,
                     SLOT(addFilter()));

    // 连接添加filter行
    QObject::connect(m_ui->filterApplyPushButton, SIGNAL(clicked()), this,
                     SLOT(ApplyFilter()));

    // 连接清空filter行
    QObject::connect(m_ui->filterDelete, SIGNAL(clicked()), this,
                     SLOT(clearFilter()));

    // 下拉框可搜索
    m_ui->filterItemCombobox->setEditable(true);
    QCompleter *pCompleter = new QCompleter(m_ui->filterItemCombobox->model());
    pCompleter->setFilterMode(Qt::MatchContains);
    m_ui->filterItemCombobox->setCompleter(pCompleter);
}

ThemeWidget::~ThemeWidget() {
    delete m_ui;
}

bool cmpByLength_Minutes(galgame *a, galgame *b) {
    return a->length_minutes > b->length_minutes;
}
bool cmpByLength_Votes(galgame *a, galgame *b) {
    return a->length_votes > b->length_votes;
}
bool cmpByRating(galgame *a, galgame *b) {
    return a->rating > b->rating;
}
bool cmpByVotecount(galgame *a, galgame *b) {
    return a->votecount > b->votecount;
}
bool cmpByReleased(galgame *a, galgame *b) {
    return a->released > b->released;
}

bool (*cmp(QString tag))(galgame *a, galgame *b) {
    if (tag == "length_minutes") return cmpByLength_Minutes;
    if (tag == "length_votes") return cmpByLength_Votes;
    if (tag == "rating") return cmpByRating;
    if (tag == "votecount") return cmpByVotecount;
    if (tag == "released") return cmpByReleased;
    qDebug() << "cmp error";
    return nullptr;
}

void ThemeWidget::updateFetchButton() {
    m_ui->filterApplyPushButton->setText(QString("[配置已更改]应用并抓取数据"));
}

void ThemeWidget::updateChartComboBox() {
    QString currentChart = m_ui->chartType->currentText();
    m_ui->tagX->clear();
    m_ui->tagY->clear();
    if (currentChart == "bar" || currentChart == "pie") {
        m_ui->tagY->hide();
        m_ui->lable_tagY->hide();
    } else {
        m_ui->tagY->show();
        m_ui->lable_tagY->show();
    }
    QVector<QString> dataVector = chartMap.value(currentChart);
    for (int i = 0; i < dataVector.count(); i++) {
        m_ui->tagX->addItem(dataVector[i]);
        if (dataVector[i] == "released") continue;
        m_ui->tagY->addItem(dataVector[i]);
        // qDebug() << currentChart;
    }
}

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
    //    m_ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    //    m_ui->themeComboBox->addItem("Blue Cerulean",
    //                                 QChart::ChartThemeBlueCerulean);
    //    m_ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    //    m_ui->themeComboBox->addItem("Brown Sand",
    //    QChart::ChartThemeBrownSand); m_ui->themeComboBox->addItem("Blue NCS",
    //    QChart::ChartThemeBlueNcs); m_ui->themeComboBox->addItem("High
    //    Contrast",
    //                                 QChart::ChartThemeHighContrast);
    //    m_ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    //    m_ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);
}

void ThemeWidget::populateAnimationBox() {
    //    m_ui->animatedComboBox->addItem("No Animations", QChart::NoAnimation);
    //    m_ui->animatedComboBox->addItem("GridAxis Animations",
    //                                    QChart::GridAxisAnimations);
    //    m_ui->animatedComboBox->addItem("Series Animations",
    //                                    QChart::SeriesAnimations);
    //    m_ui->animatedComboBox->addItem("All Animations",
    //    QChart::AllAnimations);
}

void ThemeWidget::populateLegendBox() {
    //    m_ui->legendComboBox->addItem("No Legend ", 0);
    //    m_ui->legendComboBox->addItem("Legend Top", Qt::AlignTop);
    //    m_ui->legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
    //    m_ui->legendComboBox->addItem("Legend Left", Qt::AlignLeft);
    //    m_ui->legendComboBox->addItem("Legend Right", Qt::AlignRight);
}

QChart *ThemeWidget::updateBarChart(QString tag) {
    barChart->setTitle("Bar chart");
    barSeries->clear();

    double max = -1;

    if (tag == "languages" || tag == "platforms") {
        QMap<QString, int> dataMap;
        int sum = m_galgames.count();
        for (int i = 0; i < sum; i++) {
            galgame *gal = m_galgames[i];
            QVector<QString> dataVector = gal->getVector(tag);
            for (int j = 0; j < dataVector.count(); j++) {
                QString curTag = dataVector[j];
                if (dataMap.contains(curTag))
                    dataMap.insert(curTag, dataMap[curTag] + 1);
                else
                    dataMap.insert(curTag, 1);
            }
        }

        QMap<QString, int>::const_iterator it;

        for (it = dataMap.constBegin(); it != dataMap.constEnd(); it++) {
            QString key = it.key();
            QBarSet *set = new QBarSet(key);
            if (it.value() > max) max = it.value();
            set->append(it.value());
            set->setLabel(key);
            barSeries->append(set);
            barSeries->setLabelsVisible(true);
        }
    } else if (tag == "released") {
        QMap<QString, int> dataMap;

        std::sort(m_galgames.begin(), m_galgames.end(), cmp(tag));
        int sum = m_galgames.count();
        for (int i = 0; i < sum; i++) {
            galgame *gal = m_galgames[i];
            QString dataString = gal->getString(tag);
            QStringList curTag = dataString.split('-');
            qDebug() << dataString;
            if (dataMap.contains(curTag[0]))
                dataMap.insert(curTag[0], dataMap[curTag[0]] + 1);
            else
                dataMap.insert(curTag[0], 1);
        }

        QMap<QString, int>::const_iterator it;

        for (it = dataMap.constBegin(); it != dataMap.constEnd(); it++) {
            QString key = it.key();
            QBarSet *set = new QBarSet(key);
            if (it.value() > max) max = it.value();
            set->append(it.value());
            set->setLabel(key);
            barSeries->append(set);
            barSeries->setLabelsVisible(true);
        }
    } else {
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            QBarSet *set = new QBarSet(gal->title);
            if (gal->getDouble(tag) > max) max = gal->getDouble(tag);
            set->append(gal->getDouble(tag));
            barSeries->append(set);
            barSeries->setLabelsVisible(true);
        }
    }

    barChart->addSeries(barSeries);

    barChart->createDefaultAxes();

    barChart->axes(Qt::Vertical).first()->setRange(0, max);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY =
        qobject_cast<QValueAxis *>(barChart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    barChart->update();
    return barChart;
}

QChart *ThemeWidget::updateLineChart(QString tagX, QString tagY) {
    lineChart->setTitle("Line Chart");
    lineSeries->clear();
    double maxX = -1;
    double maxY = -1;
    double minX = 10000000;
    QList<QPointF> replaceData;
    std::sort(m_galgames.begin(), m_galgames.end(), cmp(tagX));

    if (tagX == "released") {
        std::sort(m_galgames.begin(), m_galgames.end(), cmp(tagX));
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            QStringList data = gal->getString(tagX).split('-');
            QString dataString = QString::number(data[0].toDouble() + data[1].toDouble() / 12);
            if (dataString.toDouble() < minX) minX = dataString.toDouble();
            if (dataString.toDouble() > maxX) maxX = dataString.toDouble();
            if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
            replaceData.append(QPointF(dataString.toDouble(), gal->getDouble(tagY)));
        }
        scatterChart->axes(Qt::Horizontal).first()->setRange(minX, maxX);
    } else {
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            if (gal->getDouble(tagX) < minX) minX = gal->getDouble(tagX);
            if (gal->getDouble(tagX) > maxX) maxX = gal->getDouble(tagX);
            if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
            replaceData.append(QPointF(gal->getDouble(tagX), gal->getDouble(tagY)));
        }
    }
    lineChart->addSeries(lineSeries);
    lineChart->createDefaultAxes();
    lineChart->axes(Qt::Horizontal).first()->setRange(minX, maxX);
    lineChart->axes(Qt::Vertical).first()->setRange(0, maxY);
    lineSeries->replace(replaceData);
    lineChart->update();
    return lineChart;
}

QChart *ThemeWidget::updatePieChart(QString tag) {
    pieChart->setTitle("Pie Chart");
    pieSeries->clear();
    QMap<QString, int> replaceData;
    int sum = m_galgames.count();
    if (tag == "released") {
        for (int i = 0; i < sum; i++) {
            galgame *gal = m_galgames[i];
            QStringList data = gal->getString(tag).split('-');
            QString curTag = data[0];
            if (replaceData.contains(curTag))
                replaceData.insert(curTag, replaceData[curTag] + 1);
            else
                replaceData.insert(curTag, 1);
        }

        QMap<QString, int>::const_iterator it;

        for (it = replaceData.constBegin(); it != replaceData.constEnd(); it++) {
            QString key = it.key();
            QPieSlice *slice = pieSeries->append(key, it.value());
            if (it == replaceData.constBegin()) {
                slice->setLabelVisible();
                slice->setExploded();
                slice->setExplodeDistanceFactor(0.5);
            }
        }
    } else {

        for (int i = 0; i < sum; i++) {
            galgame *gal = m_galgames[i];
            QVector<QString> dataVector = gal->getVector(tag);
            for (int j = 0; j < dataVector.count(); j++) {
                QString curTag = dataVector[j];
                if (replaceData.contains(curTag))
                    replaceData.insert(curTag, replaceData[curTag] + 1);
                else
                    replaceData.insert(curTag, 1);
            }
        }

        QMap<QString, int>::const_iterator it;

        for (it = replaceData.constBegin(); it != replaceData.constEnd(); it++) {
            QString key = it.key();
            QPieSlice *slice = pieSeries->append(key, it.value());
            if (it == replaceData.constBegin()) {
                slice->setLabelVisible();
                slice->setExploded();
                slice->setExplodeDistanceFactor(0.5);
            }
        }
    }

    pieSeries->setLabelsVisible(true);
    pieChart->addSeries(pieSeries);

    pieSeries->setPieSize(0.4);

    pieChart->update();
    return pieChart;
}

QChart *ThemeWidget::updateSplineChart(QString tagX, QString tagY) {
    splineChart->setTitle("Spline Chart");
    splineSeries->clear();
    double maxX = -1;
    double maxY = -1;
    double minX = 10000000;
    QList<QPointF> replaceData;
    std::sort(m_galgames.begin(), m_galgames.end(), cmp(tagX));
    if (tagX == "released") {

        std::sort(m_galgames.begin(), m_galgames.end(), cmp(tagX));
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            QStringList data = gal->getString(tagX).split('-');
            QString dataString = QString::number(data[0].toDouble() + data[1].toDouble() / 12);
            if (dataString.toDouble() < minX) minX = dataString.toDouble();
            if (dataString.toDouble() > maxX) maxX = dataString.toDouble();
            if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
            replaceData.append(QPointF(dataString.toDouble(), gal->getDouble(tagY)));
        }
        scatterChart->axes(Qt::Horizontal).first()->setRange(minX, maxX);
    } else {
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            if (gal->getDouble(tagX) < minX) minX = gal->getDouble(tagX);
            if (gal->getDouble(tagX) > maxX) maxX = gal->getDouble(tagX);
            if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
            replaceData.append(QPointF(gal->getDouble(tagX), gal->getDouble(tagY)));
        }
    }
    splineChart->addSeries(splineSeries);
    splineChart->createDefaultAxes();
    splineChart->axes(Qt::Horizontal).first()->setRange(minX, maxX);
    splineChart->axes(Qt::Vertical).first()->setRange(0, maxY);
    splineSeries->replace(replaceData);
    splineChart->update();
    return splineChart;
}

QChart *ThemeWidget::updateScatterChart(QString tagX, QString tagY) {
    scatterChart->setTitle("Scatter Chart");
    scatterSeries->clear();
    double maxX = -1;
    double maxY = -1;
    double minX = 10000000;
    QList<QPointF> replaceData;

    if (tagX == "released") {

        std::sort(m_galgames.begin(), m_galgames.end(), cmp(tagX));
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            QStringList data = gal->getString(tagX).split('-');
            QString dataString = QString::number(data[0].toDouble() + data[1].toDouble() / 12);
            if (dataString.toDouble() < minX) minX = dataString.toDouble();
            if (dataString.toDouble() > maxX) maxX = dataString.toDouble();
            if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
            replaceData.append(QPointF(dataString.toDouble(), gal->getDouble(tagY)));
        }
        scatterChart->axes(Qt::Horizontal).first()->setRange(minX, maxX);
    } else {
        for (int i = 0; i < m_galgames.count(); i++) {
            galgame *gal = m_galgames[i];
            if (gal->getDouble(tagX) < minX) minX = gal->getDouble(tagX);
            if (gal->getDouble(tagX) > maxX) maxX = gal->getDouble(tagX);
            if (gal->getDouble(tagY) > maxY) maxY = gal->getDouble(tagY);
            replaceData.append(QPointF(gal->getDouble(tagX), gal->getDouble(tagY)));
        }
    }

    scatterChart->addSeries(scatterSeries);
    scatterChart->createDefaultAxes();
    scatterChart->axes(Qt::Horizontal).first()->setRange(minX, maxX);
    scatterChart->axes(Qt::Vertical).first()->setRange(0, maxY);
    // qDebug() << replaceData;
    scatterSeries->replace(replaceData);
    scatterChart->update();
    return scatterChart;
}

void ThemeWidget::updateChart() {
    QString chartType = m_ui->chartType->currentText();
    QString tagX = m_ui->tagX->currentText();
    QString tagY = m_ui->tagY->currentText();
    m_charts.clear();
    // m_ui->horizontalLayout->takeAt(1);
    if (chartType == "bar") {
        chartView->setChart(updateBarChart(tagX));
        m_charts << chartView;
    } else if (chartType == "line") {
        chartView->setChart(updateLineChart(tagX, tagY));
        m_charts << chartView;
    } else if (chartType == "pie") {
        chartView->setChart(updatePieChart(tagX));
        m_charts << chartView;
    } else if (chartType == "spline") {
        chartView->setChart(updateSplineChart(tagX, tagY));
        m_charts << chartView;
    } else if (chartType == "scatter") {
        chartView->setChart(updateScatterChart(tagX, tagY));
        m_charts << chartView;
    }
}
void ThemeWidget::updateUI() {
    QChart::ChartTheme theme = QChart::ChartThemeQt;

    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {
            chartView->chart()->setTheme(theme);
        }

        // Set palette colors based on selected theme

        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
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
    QChart::AnimationOptions options(QChart::AllAnimations);
    if (!m_charts.isEmpty() &&
        m_charts.at(0)->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }
    //![9]

    // Update legend alignment
    Qt::Alignment alignment(0);

    if (!alignment) {
        for (QChartView *chartView : charts)
            chartView->chart()->legend()->hide();
    } else {
        for (QChartView *chartView : charts) {
            chartView->chart()->legend()->setAlignment(alignment);
            chartView->chart()->legend()->show();
        }
    }
}

void ThemeWidget::addJsonData(QJsonObject *addData) {
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
        gal->released = galData["released"].toString();
        m_galgames.append(gal);
    }
    qDebug() << "added vn."
             << "there are " << m_galgames.size() << " vns now.";

    updateChart();
}

// 添加tag数据
void ThemeWidget::addTagData(QJsonObject *addData) {
    qDebug() << "added tag";
    QJsonArray results = (*addData)["results"].toArray();
    for (int i = 0; i < results.count(); i++) {
        QJsonObject tagJsonData(results.at(i).toObject());
        tag *newTag = new tag();
        newTag->id = tagJsonData["id"].toString();
        newTag->name = tagJsonData["name"].toString();
        newTag->vn_count = tagJsonData["vn_count"].toInt(0);
        tagData.append(newTag);

        //qDebug() << "added tag " << tagJsonData["name"].toString();
    }
}

void ThemeWidget::clearVnData() {
    for (int i = 0; i < m_galgames.size(); ++i) {
        delete m_galgames[i];
    }
    m_galgames.clear();

    updateScatterChart("length_votes", "rating");
    updateLineChart("length_votes", "rating");
    updateSplineChart("length_votes", "rating");
    updatePieChart("languages");
    updateBarChart("rating");
}

void ThemeWidget::updateFilterComboBoxes() {
    qDebug() << "updateFilterComboBoxes";
    QString itemText = m_ui->filterComboBox->currentText();
    if (itemText == "search") {
        m_ui->filterItemCombobox->hide();
        m_ui->filterOperatorComboBox->hide();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->show();
    } else if (itemText == "lang") {
        // 清空combobox
        m_ui->filterItemCombobox->clear();
        // 重新填入
        QJsonArray langEnum = schema["language"].toArray();
        for (const QJsonValue &i : langEnum) {
            m_ui->filterItemCombobox->addItem(i.toObject()["label"].toString());
        }

        m_ui->filterItemCombobox->show();
        m_ui->filterOperatorComboBox->hide();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->hide();
    } else if (itemText == "platform") {
        // 清空combobox
        m_ui->filterItemCombobox->clear();
        // 重新填入
        QJsonArray langEnum = schema["platform"].toArray();
        for (const QJsonValue &i : langEnum) {
            m_ui->filterItemCombobox->addItem(i.toObject()["label"].toString());
        }

        m_ui->filterItemCombobox->show();
        m_ui->filterOperatorComboBox->hide();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->hide();
    } else if (itemText == "length") {
        m_ui->filterItemCombobox->hide();
        m_ui->filterOperatorComboBox->show();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->show();
    } else if (itemText == "released") {
        m_ui->filterItemCombobox->hide();
        m_ui->filterOperatorComboBox->show();
        m_ui->filterDateEdit->show();
        m_ui->filterLineEdit->hide();
    } else if (itemText == "rating") {
        m_ui->filterItemCombobox->hide();
        m_ui->filterOperatorComboBox->show();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->show();
    } else if (itemText == "votecount") {
        m_ui->filterItemCombobox->hide();
        m_ui->filterOperatorComboBox->show();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->show();
    } else if (itemText == "has_anime") {
        // 清空combobox
        m_ui->filterItemCombobox->clear();
        // 重新填入
        m_ui->filterItemCombobox->addItem("true");
        m_ui->filterItemCombobox->addItem("false");

        m_ui->filterItemCombobox->show();
        m_ui->filterOperatorComboBox->hide();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->hide();
    } else if (itemText == "tag") {
        // 清空combobox
        m_ui->filterItemCombobox->clear();
        // 重新填入
        for (auto &i : tagData) {
            m_ui->filterItemCombobox->addItem(i->name);
        }

        m_ui->filterItemCombobox->show();
        m_ui->filterOperatorComboBox->hide();
        m_ui->filterDateEdit->hide();
        m_ui->filterLineEdit->hide();
    }
}

void ThemeWidget::addFilter() {
    qDebug() << "add Filter";
    QString itemText = m_ui->filterComboBox->currentText();
    int iRow = m_ui->filterTable->rowCount();
    m_ui->filterTable->setRowCount(iRow + 1);

    // 新行内容
    QString ItemStr, ItemOp, ItemContent;

    // 设置新行内容
    ItemStr = itemText;

    if (m_ui->filterOperatorComboBox->isHidden())
        ItemOp = "=";
    else
        ItemOp = m_ui->filterOperatorComboBox->currentText();

    if (!m_ui->filterItemCombobox->isHidden())
        ItemContent = m_ui->filterItemCombobox->currentText();
    else if (!m_ui->filterDateEdit->isHidden()) {
        int y, m, d;
        m_ui->filterDateEdit->date().getDate(&y, &m, &d);
        QString yStr = QString::number(y);
        QString mStr = QString::number(m);
        QString dStr = QString::number(d);
        if (m < 10) mStr = "0" + mStr;
        if (d < 10) dStr = "0" + dStr;
        ItemContent = yStr + "-" + mStr + "-" + dStr;
    } else if (!m_ui->filterLineEdit->isHidden()) {
        ItemContent = m_ui->filterLineEdit->text();
    }

    qDebug() << ItemStr << " " << ItemOp << " " << ItemContent;

    // 将内容设置到表格中
    QTableWidgetItem *itemStatus0 = new QTableWidgetItem(ItemStr);
    itemStatus0->setFlags(itemStatus0->flags() & (~Qt::ItemIsEditable));
    m_ui->filterTable->setItem(iRow, 0, itemStatus0);

    QTableWidgetItem *itemStatus1 = new QTableWidgetItem(ItemOp);
    itemStatus1->setFlags(itemStatus1->flags() & (~Qt::ItemIsEditable));
    m_ui->filterTable->setItem(iRow, 1, itemStatus1);

    QTableWidgetItem *itemStatus2 = new QTableWidgetItem(ItemContent);
    itemStatus2->setFlags(itemStatus2->flags() & (~Qt::ItemIsEditable));
    m_ui->filterTable->setItem(iRow, 2, itemStatus2);
}

void ThemeWidget::clearFilter() {
    m_ui->filterTable->clearContents();
    m_ui->filterTable->setRowCount(0);
}

void ThemeWidget::ApplyFilter() {
    qDebug() << "apply";
    auto test = getFilter();
    qDebug() << test;
}

QVector<QVector<QString>> ThemeWidget::getFilter() {
    QVector<QVector<QString>> ansFilter;
    ansFilter.resize(m_ui->filterTable->rowCount());
    for (int line = 0; line < m_ui->filterTable->rowCount(); ++line) {
        QString ItemStr = m_ui->filterTable->item(line, 0)->text();
        QString ItemOp = m_ui->filterTable->item(line, 1)->text();
        QString ItemContent = m_ui->filterTable->item(line, 2)->text();
        QString ItemContentID = ItemContent;
        if (ItemStr == "lang") {
            for (const auto &l : schema["language"].toArray()) {
                if ((l.toObject())["label"] == ItemContent)
                    ItemContentID = (l.toObject())["id"].toString();
            };
        }
        if (ItemStr == "platform") {
            for (const auto &l : schema["platform"].toArray()) {
                if ((l.toObject())["label"] == ItemContent)
                    ItemContentID = (l.toObject())["id"].toString();
            };
        }
        if (ItemStr == "has_anime") {
            ItemContentID = "1";
            if (ItemContent == "true")
                ItemOp = "=";
            else
                ItemOp = "!=";
        }
        if (ItemStr == "tag") {
            for (const auto &t : tagData) {
                if (t->name == ItemContent) ItemContentID = t->id;
            };
        }

        ansFilter[line].push_back(ItemStr);
        ansFilter[line].push_back(ItemOp);
        ansFilter[line].push_back(ItemContentID);
    }
    return ansFilter;
}
