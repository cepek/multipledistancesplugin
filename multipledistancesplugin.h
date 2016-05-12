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

#ifndef MULTIPLEDISTANCESPLUGIN_H
#define MULTIPLEDISTANCESPLUGIN_H

#include <QObject>
#include <QString>
#include <QTableWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QScrollArea>
#include <QGroupBox>
#include <QMessageBox>
#include <QCheckBox>
#include <QList>
#include <QStatusBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSpinBox>

#include "gamaq2interfaces.h"
#include "data.h"
#include "datalist.h"
#include "mytablewidgetitem.h"
#include "help.h"

<<<<<<< HEAD
#include "networkadjustmentpanel.h"
#include <gnu_gama/local/network.h>

=======
>>>>>>> 4f35a90f2ea91dcdf83a39c9c57f2e51af5d6b43
namespace GNU_gama {
namespace local {
    class LocalNetwork;
}
}

using LocalNetwork = GNU_gama::local::LocalNetwork;

class QFrame;

class MultipleDistancesPlugin : public QObject, AdjustmentInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gnu.gama.gama-q2.AdjustmentInterface/1.0")
    Q_INTERFACES(AdjustmentInterface)

public:
    MultipleDistancesPlugin();

    QString getName();
<<<<<<< HEAD
    QWidget* create(NetworkAdjustmentPanel* adjp);
=======
    QWidget* create(QString cname, LocalNetwork* ln);
>>>>>>> 4f35a90f2ea91dcdf83a39c9c57f2e51af5d6b43

private:

    QString title = "Multiple Distances plugin";

    enum columns{Id,Sort,Type,PointA,PointB,Count,CountAB,CountBA,Average,Testing};

    QAction *actionClose,*actionType, *actionCount, *actionTest, *actionSettings, *actionReset,
            *actionDistance, *actionSlopeDistance, *actionUncontrolledDistance,
            *actionAbout, *actionHelp;

    QColor color1 = QColor(255,255,255);
    QColor color2 = QColor(240,240,240);

    int statusBarTime = 4000;
    int precission = 4;

<<<<<<< HEAD
    NetworkAdjustmentPanel* nap {nullptr};
=======
>>>>>>> 4f35a90f2ea91dcdf83a39c9c57f2e51af5d6b43
    QString confName;
    LocalNetwork* lnet {nullptr};

    DataList *dataList;
    QMainWindow* mainWindow;
    QTableWidget* tableWidget;

    QMainWindow*  getMainWindow();
    QDialog* getSettings();

    void createMenuBar();
    QTableWidget* getTable();
    QTableWidget* getTest(const int &row, QList<Data*> dataList);
    double calculatePercentage(const int &row, QList<Data*> dataList);
    void filter();
    void filter(const int &row);    
    void reset();
    void setTableData();

    static double round(const double &value, const int &decimals);

private slots:
    void about();

};

#endif // MULTIPLEDISTANCESPLUGIN_H
