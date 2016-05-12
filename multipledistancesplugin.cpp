/*  Extension of Gama-q2 for excluding outlying distances
    based on Grubb's test.
    Copyright (C) 2016 Dominik Hladík

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <QtPlugin>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

#include "multipledistancesplugin.h"
#include <gnu_gama/local/network.h>

#include <QLabel>

#include <QDebug>

MultipleDistancesPlugin::MultipleDistancesPlugin()
{
}

QString MultipleDistancesPlugin::getName()
{
    return title;
}

<<<<<<< HEAD
QWidget* MultipleDistancesPlugin::create(NetworkAdjustmentPanel *adjp)
{    
    NetworkAdjustmentPanel* nap = adjp;
    confName = nap->configurationName();
    lnet = nap->adjustment()->get_local_network();
    dataList = new DataList(lnet);
=======
QWidget* MultipleDistancesPlugin::create(QString cname, LocalNetwork* ln)
{
    confName = cname;
    lnet = ln;
    dataList = new DataList(lnet);

>>>>>>> 4f35a90f2ea91dcdf83a39c9c57f2e51af5d6b43
    return getMainWindow();
}

QMainWindow *MultipleDistancesPlugin::getMainWindow()
{
    mainWindow = new QMainWindow;
    if (mainWindow) {

        mainWindow->setWindowTitle(title + " '" + confName + "'");

        dataList->addData(DataList::HorizontalDistance);
        dataList->addData(DataList::SlopeDistance);

        mainWindow->setCentralWidget(getTable());
        mainWindow->resize(800,400);

        mainWindow->setStatusBar(new QStatusBar);
        mainWindow->statusBar()->showMessage("Ready!",statusBarTime);

        createMenuBar();
    }

    return mainWindow;
}

QDialog *MultipleDistancesPlugin::getSettings()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->setWindowTitle("Settings");

    QSpinBox *spinBoxPrecission = new QSpinBox;
    spinBoxPrecission->setValue(precission);
    spinBoxPrecission->setMinimum(0);
    spinBoxPrecission->setMaximum(12);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel("Output Precission"),0,0);
    gridLayout->addWidget(spinBoxPrecission,0,1);

    QPushButton *pushButtonOK = new QPushButton("OK");
    QPushButton *pushButtonClose = new QPushButton("Close");

    QDialogButtonBox* box = new QDialogButtonBox;
    box->addButton(pushButtonOK,QDialogButtonBox::AcceptRole);
    box->addButton(pushButtonClose,QDialogButtonBox::RejectRole);

    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addLayout(gridLayout);
    verticalLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding));
    verticalLayout->addWidget(box,0,Qt::AlignCenter);

    dialog->setLayout(verticalLayout);

    connect(pushButtonOK, &QPushButton::clicked, [this,dialog,spinBoxPrecission](){
        dialog->close();
        precission = spinBoxPrecission->value();

        for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
        tableWidget->sortByColumn(Id,Qt::AscendingOrder);
        QList<Data*> data = dataList->data();

        for(int i = 0;i < tableWidget->rowCount();i++){
            if(data[i]->count()>1) tableWidget->item(i,Average)->setText(QString::number(round(data[i]->avg(),precission),'h',precission));
            else                   tableWidget->item(i,Average)->setText("");

            QString percentage = QString::number(round(100*calculatePercentage(i,data),precission),'h',0) + " %";
            QPushButton* pushButton = tableWidget->cellWidget(i,Testing)->findChildren<QPushButton *>().first();
            pushButton->setText(percentage);
            if(percentage=="100 %"){
                tableWidget->cellWidget(i,Testing)->setStyleSheet("color: green; background-color: rgb(" +
                                      QString::number(color2.red()) + ", " +
                                      QString::number(color2.green())+ "," +
                                      QString::number(color2.blue())+ ");");
            }else{
                tableWidget->cellWidget(i,Testing)->setStyleSheet("color: red; background-color: rgb(" +
                                      QString::number(color2.red()) + ", " +
                                      QString::number(color2.green())+ "," +
                                      QString::number(color2.blue())+ ");");
            }
        }        
        tableWidget->sortByColumn(Sort,Qt::AscendingOrder);

    });
    connect(pushButtonClose,SIGNAL(clicked(bool)),dialog,SLOT(close()));
    return dialog;
}

void MultipleDistancesPlugin::createMenuBar()
{
    QMenu* menuFile = new QMenu(tr("&File"),mainWindow);
    actionClose = menuFile->addAction(tr("&Close"));
    connect(actionClose, SIGNAL(triggered()),mainWindow,SLOT(close()));

    QMenu* menuSettings = new QMenu(tr("&Settings"), mainWindow);
    actionReset = menuSettings->addAction(tr("&Reset"));
    connect(actionReset, &QAction::triggered,[this](){
        reset();
        mainWindow->statusBar()->showMessage("Data were reloaded!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionDistance = menuSettings->addAction(tr("&Distances"));
    actionDistance->setCheckable(true);
    actionDistance->setChecked(true);
    connect(actionDistance, &QAction::toggled,[this](){
        filter();
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    actionSlopeDistance = menuSettings->addAction(tr("&Slope Distances"));
    actionSlopeDistance->setCheckable(true);
    actionSlopeDistance->setChecked(true);
    connect(actionSlopeDistance, &QAction::toggled,[this](){
        filter();
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    actionUncontrolledDistance = menuSettings->addAction(tr("&Uncontrolled Distances"));
    actionUncontrolledDistance->setCheckable(true);
    actionUncontrolledDistance->setChecked(true);
    connect(actionUncontrolledDistance, &QAction::toggled,[this](){
        filter();
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionType = menuSettings->addAction(tr("&Type of Measurements"));
    actionType->setCheckable(true);
    actionType->setChecked(true);
    connect(actionType, &QAction::toggled,[this](bool isChecked){
        if(isChecked) tableWidget->showColumn(Type);
        else          tableWidget->hideColumn(Type);
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    actionCount = menuSettings->addAction(tr("&Count"));
    actionCount->setCheckable(true);
    actionCount->setChecked(true);
    connect(actionCount, &QAction::toggled,[this](bool isChecked){
        if(isChecked){
            tableWidget->showColumn(Count);
            tableWidget->showColumn(CountAB);
            tableWidget->showColumn(CountBA);
        }else{
            tableWidget->hideColumn(Count);
            tableWidget->hideColumn(CountAB);
            tableWidget->hideColumn(CountBA);
        }
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionTest = menuSettings->addAction(tr("&Grubb's Test"));
    actionTest->setCheckable(true);
    actionTest->setChecked(true);
    connect(actionTest, &QAction::toggled,[this](bool isChecked){
        if(isChecked) tableWidget->showColumn(Testing);
        else          tableWidget->hideColumn(Testing);
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionSettings = menuSettings->addAction(tr("&Settings"));
    connect(actionSettings, &QAction::triggered,[this](){
        getSettings()->show();
        mainWindow->statusBar()->showMessage("Settings shown!",statusBarTime);
    });

    QMenu* menuHelp = new QMenu(tr("&Help"),mainWindow);
    actionHelp = menuHelp->addAction(title + " Help");
    connect(actionHelp, &QAction::triggered,[this](){
        Help::get()->show();
        mainWindow->statusBar()->showMessage("Help shown!",statusBarTime);
    });
    actionAbout = menuHelp->addAction("About " + title);
    connect(actionAbout, &QAction::triggered,[this](){
        about();
        mainWindow->statusBar()->showMessage("About shown!",statusBarTime);
    });

    mainWindow->menuBar()->addMenu(menuFile);
    mainWindow->menuBar()->addMenu(menuSettings);
    mainWindow->menuBar()->addMenu(menuHelp);
}

QTableWidget* MultipleDistancesPlugin::getTable()
{
    tableWidget = new QTableWidget;

    QStringList headerData{"Id","Sort","Type","A","B","Count","Count A→B","Count B→A","Mean [m]","Test"};

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setColumnCount(headerData.size());    
    tableWidget->setHorizontalHeaderLabels(headerData);

    tableWidget->hideColumn(Id);
    tableWidget->hideColumn(Sort);

    setTableData();

    tableWidget->setSortingEnabled(true);
    return tableWidget;
}

void MultipleDistancesPlugin::setTableData()
{
    tableWidget->setRowCount(dataList->count());

    QList<QColor> colors{color1,color1,color2,color1,color1,color2,color2,color2,color1,color2};

    QList<Data*> data = dataList->data();
    for(int i = 0;i < data.size();i++){

        tableWidget->setItem(i,Id, new MyTableWidgetItem);
        tableWidget->item(i,Id)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Id)->setData(Qt::BackgroundRole,QVariant(colors[Id]));
        tableWidget->item(i,Id)->setText(QString::number(i));

        tableWidget->setItem(i,Sort, new MyTableWidgetItem);
        tableWidget->item(i,Sort)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Sort)->setData(Qt::BackgroundRole,QVariant(colors[Sort]));
        tableWidget->item(i,Sort)->setText(QString::number(i));

        tableWidget->setItem(i,Type, new MyTableWidgetItem);
        tableWidget->item(i,Type)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Type)->setData(Qt::BackgroundRole,QVariant(colors[Type]));
        tableWidget->item(i,Type)->setText(data[i]->type());

        tableWidget->setItem(i,PointA, new MyTableWidgetItem);
        tableWidget->item(i,PointA)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,PointA)->setData(Qt::BackgroundRole,QVariant(colors[PointA]));
        tableWidget->item(i,PointA)->setText(data[i]->a());

        tableWidget->setItem(i,PointB, new MyTableWidgetItem);
        tableWidget->item(i,PointB)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,PointB)->setData(Qt::BackgroundRole,QVariant(colors[PointB]));
        tableWidget->item(i,PointB)->setText(data[i]->b());

        tableWidget->setItem(i,Count, new MyTableWidgetItem);
        tableWidget->item(i,Count)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Count)->setData(Qt::BackgroundRole,QVariant(colors[Count]));
        tableWidget->item(i,Count)->setText(QString::number(data[i]->count()) + " / " + QString::number(data[i]->size()));

        tableWidget->setItem(i,CountAB, new MyTableWidgetItem);
        tableWidget->item(i,CountAB)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,CountAB)->setData(Qt::BackgroundRole,QVariant(colors[CountAB]));
        tableWidget->item(i,CountAB)->setText(QString::number(data[i]->countAB()));

        tableWidget->setItem(i,CountBA, new MyTableWidgetItem);
        tableWidget->item(i,CountBA)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,CountBA)->setData(Qt::BackgroundRole,QVariant(colors[CountBA]));
        tableWidget->item(i,CountBA)->setText(QString::number(data[i]->countBA()));

        tableWidget->setItem(i,Average, new MyTableWidgetItem);
        tableWidget->item(i,Average)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Average)->setData(Qt::BackgroundRole,QVariant(colors[Average]));
        if(data[i]->count()>1) tableWidget->item(i,Average)->setText(QString::number(round(data[i]->avg(),precission),'h',precission));
        else                   tableWidget->item(i,Average)->setText("");

        QPushButton* pushButton = new QPushButton;
        if(data[i]->count()>2){
            QString percentage = QString::number(round(100*calculatePercentage(i,data),precission),'h',0) + " %";
            pushButton->setText(percentage);
        }else pushButton->setText("");
        pushButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        QWidget *widget = new QWidget;
        if(pushButton->text()=="100 %"){
            widget->setStyleSheet("color: green; background-color: rgb(" +
                                  QString::number(colors[Testing].red()) + ", " +
                                  QString::number(colors[Testing].green())+ "," +
                                  QString::number(colors[Testing].blue())+ ");");
        }else{
            widget->setStyleSheet("color: red; background-color: rgb(" +
                                  QString::number(colors[Testing].red()) + ", " +
                                  QString::number(colors[Testing].green())+ "," +
                                  QString::number(colors[Testing].blue())+ ");");
        }
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->addWidget(pushButton);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0,0,0,0);
        widget->setLayout(layout);
        tableWidget->setCellWidget(i,Testing,widget);

        connect(pushButton, &QPushButton::clicked, [this,data,i](){
            QMainWindow* dialog = new QMainWindow;
            dialog->setWindowModality(Qt::ApplicationModal);
            dialog->setWindowTitle("Test '" + data[i]->a() + "-" + data[i]->b() + "'");
            dialog->setCentralWidget(getTest(i,data));
            dialog->resize(600,300);
            dialog->show();
        });
    }
}

QTableWidget* MultipleDistancesPlugin::getTest(const int &row, QList<Data*> dataList)
{
    Data* data = dataList[row];

    QTableWidget* table = new QTableWidget;

    table->setSortingEnabled(true);

    enum columns{SubId,SubSort,SubFrom,SubTo,SubValue,SubStdDev,SubOutliers,SubActive};
    QStringList headerData{"Id","Sort","From","To","Value [m]","StdDev [m]","Outliers [m]", "Active"};
    QList<QColor> colors{color1,color1,color2,color2,color1,color1,color2,color2};

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setColumnCount(headerData.size());
    table->setRowCount(data->size());
    table->setHorizontalHeaderLabels(headerData);

    table->hideColumn(SubId);
    table->hideColumn(SubSort);

    QVector<double> v,k1;
    double kg = 0;
    data->grubbsTest(1-lnet->conf_pr(),v,k1,kg);

//    double diff = data->diff();
//    double normal = data->normalTest(1-lnet->conf_pr())/1000;

    for(int i = 0;i < data->size();i++){

        table->setItem(i,SubId, new MyTableWidgetItem);
        table->item(i,SubId)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubId)->setData(Qt::BackgroundRole,QVariant(colors[SubId]));
        table->item(i,SubId)->setText(QString::number(i));

        table->setItem(i,SubSort, new MyTableWidgetItem);
        table->item(i,SubSort)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubSort)->setData(Qt::BackgroundRole,QVariant(colors[SubSort]));
        table->item(i,SubSort)->setText(QString::number(i));

        table->setItem(i,SubFrom, new MyTableWidgetItem);
        table->item(i,SubFrom)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubFrom)->setData(Qt::BackgroundRole,QVariant(colors[SubFrom]));
        table->item(i,SubFrom)->setText(QString::fromStdString(data->observation(i)->from().str()));

        table->setItem(i,SubTo, new MyTableWidgetItem);
        table->item(i,SubTo)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubTo)->setData(Qt::BackgroundRole,QVariant(colors[SubTo]));
        table->item(i,SubTo)->setText(QString::fromStdString(data->observation(i)->to().str()));

        table->setItem(i,SubValue, new MyTableWidgetItem);
        table->item(i,SubValue)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubValue)->setData(Qt::BackgroundRole,QVariant(colors[SubValue]));
        table->item(i,SubValue)->setText(QString::number(round(data->observation(i)->value(),precission),'h',precission));

        table->setItem(i,SubStdDev, new MyTableWidgetItem);
        table->item(i,SubStdDev)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubStdDev)->setData(Qt::BackgroundRole,QVariant(colors[SubStdDev]));
        table->item(i,SubStdDev)->setText(QString::number(round(data->observation(i)->stdDev()/1000,precission),'h',precission));

        table->setItem(i,SubOutliers, new MyTableWidgetItem);
        table->item(i,SubOutliers)->setTextAlignment(Qt::AlignCenter);
        table->item(i,SubOutliers)->setData(Qt::BackgroundRole,QVariant(colors[SubOutliers]));

        if(data->count()>2) {
            table->item(i,SubOutliers)->setText(QString::number(round(data->avg()-data->observation(i)->value(),precission),'h',precission));
            if(k1[i] > kg && data->observation(i)->active()){
                table->item(i,SubOutliers)->setTextColor(Qt::red);
            }else if(k1[i] <= kg && data->observation(i)->active()){
                table->item(i,SubOutliers)->setTextColor(Qt::darkGreen);
            }else{ // unused
                table->item(i,SubOutliers)->setTextColor(Qt::black);
            }
        }/*else if(data->count()==2){
            table->item(i,SubOutliers)->setText(QString::number(round(data->avg()-data->observation(i)->value(),precission),'h',precission));
            if(diff > normal && data->observation(i)->active()){
                table->item(i,SubOutliers)->setTextColor(Qt::red);
            }else if(diff <= normal && data->observation(i)->active()){
                table->item(i,SubOutliers)->setTextColor(Qt::darkGreen);
            }else{ // unused
                table->item(i,SubOutliers)->setTextColor(Qt::black);
            }
        }*/else{
            table->item(i,SubOutliers)->setText("");
            table->item(i,SubOutliers)->setTextColor(Qt::black);
        }

        QCheckBox* checkBox = new QCheckBox;
        checkBox->setChecked(data->observation(i)->active());
        QWidget *widget = new QWidget;
        widget->setStyleSheet("background-color: rgb(" +
                              QString::number(colors[SubActive].red()) + ", " +
                              QString::number(colors[SubActive].green())+ "," +
                              QString::number(colors[SubActive].blue())+ ")");
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0,0,0,0);
        widget->setLayout(layout);
        table->setCellWidget(i,SubActive,widget);

        connect(checkBox,&QCheckBox::toggled,[this,data,i,table,row,dataList,colors](bool isChecked){

            for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
            for(int j = 0;j < table->rowCount();j++) table->item(j,SubSort)->setText(QString::number(j));

            tableWidget->sortByColumn(Id,Qt::AscendingOrder);
            table->sortByColumn(SubId,Qt::AscendingOrder);

            if(isChecked) data->observation(i)->set_active();
            else          data->observation(i)->set_passive();

            tableWidget->item(row,Count)  ->setText(QString::number(data->count()) + " / " + QString::number(data->size()));
            tableWidget->item(row,CountAB)->setText(QString::number(data->countAB()));
            tableWidget->item(row,CountBA)->setText(QString::number(data->countBA()));

            if(data->count()>2) {
                tableWidget->item(row,Average)->setText(QString::number(round(data->avg(),precission),'h',precission));

                QVector<double> v,k1;
                double kg = 0;
                data->grubbsTest(1-lnet->conf_pr(),v,k1,kg);

                for(int j = 0;j < table->rowCount();j++){
                    int i =  table->item(j,SubId)->text().toInt();
                    table->item(j,SubOutliers)->setText(QString::number(round(data->avg()-data->observation(i)->value(),precission),'h',precission));

                    if(k1[i] > kg && data->observation(i)->active()){
                        table->item(j,SubOutliers)->setTextColor(Qt::red);
                    }else if(k1[i] <= kg && data->observation(i)->active()){
                        table->item(j,SubOutliers)->setTextColor(Qt::darkGreen);
                    }else{ // unused
                        table->item(j,SubOutliers)->setTextColor(Qt::black);
                    }
                }
            }else if(data->count()==2){
                tableWidget->item(row,Average)->setText(QString::number(round(data->avg(),precission),'h',precission));

                //double diff = data->diff();
                //double normal = data->normalTest(1-lnet->conf_pr())/1000;

                for(int j = 0;j < table->rowCount();j++){
                    int i =  table->item(j,SubId)->text().toInt();
                    table->item(j,SubOutliers)->setText(QString::number(round(data->avg()-data->observation(i)->value(),precission),'h',precission));

                    /*
                    if(diff > normal && data->observation(i)->active()){
                        table->item(j,SubOutliers)->setTextColor(Qt::red);
                    }else if(diff <= normal && data->observation(i)->active()){
                        table->item(j,SubOutliers)->setTextColor(Qt::darkGreen);
                    }else{ // unused
                        table->item(j,SubOutliers)->setTextColor(Qt::black);
                    }
                    */

                    table->item(j,SubOutliers)->setTextColor(Qt::black);
                }
            }else{
                tableWidget->item(row,Average)->setText("");
                for(int j = 0;j < table->rowCount();j++){
                    table->item(j,SubOutliers)->setText("");
                    table->item(j,SubOutliers)->setTextColor(Qt::black);
                }
            }

            QPushButton* pushButton = tableWidget->cellWidget(row,Testing)->findChildren<QPushButton *>().first();
            if(data->count()>2){
                QString percentage = QString::number(round(100*calculatePercentage(row,dataList),precission),'h',0) + " %";
                pushButton->setText(percentage);
            }else pushButton->setText("");
            if(pushButton->text()=="100 %"){
                tableWidget->cellWidget(row,Testing)->setStyleSheet("color: green; background-color: rgb(" +
                                      QString::number(color2.red()) + ", " +
                                      QString::number(color2.green())+ "," +
                                      QString::number(color2.blue())+ ");");
            }else{
                tableWidget->cellWidget(row,Testing)->setStyleSheet("color: red; background-color: rgb(" +
                                      QString::number(color2.red()) + ", " +
                                      QString::number(color2.green())+ "," +
                                      QString::number(color2.blue())+ ");");
            }            
            filter(row);
            tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
            table->sortByColumn(SubSort,Qt::AscendingOrder);
        });
    }

    return table;
}

double MultipleDistancesPlugin::calculatePercentage(const int &row, QList<Data*> dataList)
{
    Data* data = dataList[row];

    if(data->count()>2){

        QVector<double> v,k1;
        double kg = 0;
        data->grubbsTest(1-lnet->conf_pr(),v,k1,kg);

        double countRight = 0;
        //double countWrong = 0;
        for(int i = 0;i < data->size();i++){
            if(k1[i] <= kg && data->observation(i)->active()) countRight++;
            //else if(k1[i] > kg && data->observation(i)->active()) countWrong++;
        }
        return countRight/data->count();

    }/*else if(data->count()==2){


        double diff = data->diff();
        double normal = data->normalTest(1-lnet->conf_pr())/1000;

        if(diff > normal) return 0;
        else return 1;

    }*/else{ // unused
        return 0;
    }
}

void MultipleDistancesPlugin::filter()
{
    for(int i = 0;i < tableWidget->rowCount();i++){
        filter(i);
    }
}

void MultipleDistancesPlugin::filter(const int &row)
{
    if(tableWidget->item(row,Average)->text().isEmpty() && !actionUncontrolledDistance->isChecked()){
        tableWidget->hideRow(row);
        return;
    }else tableWidget->showRow(row);

    if(tableWidget->item(row,Type)->text()=="horizontal"){
        if(actionDistance->isChecked()) tableWidget->showRow(row);
        else                            tableWidget->hideRow(row);
    }else if(tableWidget->item(row,Type)->text()=="slope"){
        if(actionSlopeDistance->isChecked()) tableWidget->showRow(row);
        else                                 tableWidget->hideRow(row);
    }
}

void MultipleDistancesPlugin::about()
{
    QString version = "Alpha";
    QString year = "2016";

    QMessageBox about(mainWindow);
    about.setWindowTitle("About " + title);
    about.setTextFormat(Qt::RichText);
    about.setText(tr("<p>Extension of Gama-q2 for excluding outlying distances "
                     "based on Grubb's two-side test.</p>") +

                  "<p><b>" + title + "&nbsp;" + version + "</b>"
                  " Copyright (C) " + year + " Dominik Hladík</p>"

                  "<p>This program is free software: you can redistribute it and/or modify "
                  "it under the terms of the GNU General Public License as published by "
                  "the Free Software Foundation, either version 3 of the License, or "
                  "(at your option) any later version.</p>"

                  "<p>This program is distributed in the hope that it will be useful, "
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                  "GNU General Public License for more details.</p>"

                  "<p>You should have received a copy of the GNU General Public License "
                  " along with this program.  If not, see <a "
                  "href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a>.</p>"
                  );
    about.exec();
}

double MultipleDistancesPlugin::round(const double &value, const int &decimals)
{
    return std::round(std::pow(10,decimals)*value)/std::pow(10,decimals);
}

void MultipleDistancesPlugin::reset()
{
    // 1) load data
    dataList->clearData();
    dataList->addData(DataList::HorizontalDistance);
    dataList->addData(DataList::SlopeDistance);

    // 2) delete data from table
    tableWidget->sortByColumn(Id,Qt::AscendingOrder);
    tableWidget->setSortingEnabled(false);
    while (tableWidget->rowCount()!=0)  tableWidget->removeRow(0);

    // 3) add data into table
    setTableData();
    tableWidget->setSortingEnabled(true);
}
