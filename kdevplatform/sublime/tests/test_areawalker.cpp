/***************************************************************************
 *   Copyright 2006-2007 Alexander Dymo  <adymo@kdevelop.org>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include "test_areawalker.h"

#include <QStringList>
#include <QTest>

#include <sublime/area.h>
#include <sublime/view.h>
#include <sublime/controller.h>
#include <sublime/urldocument.h>

#include "areaprinter.h"

using namespace Sublime;

struct AreaStopper {
    explicit AreaStopper(const QString& stopAt): m_stopAt(stopAt) {}
    Area::WalkerMode operator()(AreaIndex *index)
    {
        foreach (View *view, index->views())
        {
            list << view->objectName();
            if (view->objectName() == m_stopAt)
                return Area::StopWalker;
        }
        return Area::ContinueWalker;
    }
    Area::WalkerMode operator()(View *view, Sublime::Position)
    {
        list << view->objectName();
        if (view->objectName() == m_stopAt)
            return Area::StopWalker;
        return Area::ContinueWalker;
    }
    QStringList list;
    QString m_stopAt;
};

void TestAreaWalker::viewWalkerModes()
{
    Controller *controller = new Controller(this);
    Document *doc = new UrlDocument(controller, QUrl::fromLocalFile(QStringLiteral("~/foo.cpp")));
    Area *area = new Area(controller, QStringLiteral("Area"));
    View *view1 = doc->createView();
    view1->setObjectName(QStringLiteral("1"));
    area->addView(view1);
    View *view2 = doc->createView();
    view2->setObjectName(QStringLiteral("2"));
    area->addView(view2, view1, Qt::Vertical);
    View *view3 = doc->createView();
    view3->setObjectName(QStringLiteral("3"));
    area->addView(view3, view1, Qt::Vertical);
    View *view4 = doc->createView();
    view4->setObjectName(QStringLiteral("4"));
    area->addView(view4, view1, Qt::Vertical);

    AreaViewsPrinter p;
    area->walkViews(p, area->rootIndex());
    QCOMPARE(p.result, QStringLiteral("\n\
[ vertical splitter ]\n\
    [ vertical splitter ]\n\
        [ vertical splitter ]\n\
            [ 1 ]\n\
            [ 4 ]\n\
        [ 3 ]\n\
    [ 2 ]\n\
"));

    AreaStopper stopper(QStringLiteral("1"));
    area->walkViews(stopper, area->rootIndex());
    QCOMPARE(stopper.list.join(QLatin1Char(' ')), QStringLiteral("1"));

    AreaStopper stopper2(QStringLiteral("2"));
    area->walkViews(stopper2, area->rootIndex());
    QCOMPARE(stopper2.list.join(QLatin1Char(' ')), QStringLiteral("1 4 3 2"));

    AreaStopper stopper3(QStringLiteral("3"));
    area->walkViews(stopper3, area->rootIndex());
    QCOMPARE(stopper3.list.join(QLatin1Char(' ')), QStringLiteral("1 4 3"));

    AreaStopper noStopper(QStringLiteral("X"));
    area->walkViews(noStopper, area->rootIndex());
    QCOMPARE(noStopper.list.join(QLatin1Char(' ')), QStringLiteral("1 4 3 2"));

    delete area;
    delete doc;
    delete controller;
}

void TestAreaWalker::toolViewWalkerModes()
{
    Controller *controller = new Controller(this);
    Document *doc = new UrlDocument(controller, QUrl::fromLocalFile(QStringLiteral("~/foo.cpp")));
    Area *area = new Area(controller, QStringLiteral("Area"));
    View *view = doc->createView();
    view->setObjectName(QStringLiteral("1"));
    area->addToolView(view, Sublime::Left);
    view = doc->createView();
    view->setObjectName(QStringLiteral("2"));
    area->addToolView(view, Sublime::Left);
    view = doc->createView();
    view->setObjectName(QStringLiteral("3"));
    area->addToolView(view, Sublime::Bottom);

    AreaStopper stopper1(QStringLiteral("1"));
    area->walkToolViews(stopper1, Sublime::AllPositions);
    QCOMPARE(stopper1.list.join(QLatin1Char(' ')), QStringLiteral("1"));

    AreaStopper stopper2(QStringLiteral("2"));
    area->walkToolViews(stopper2, Sublime::AllPositions);
    QCOMPARE(stopper2.list.join(QLatin1Char(' ')), QStringLiteral("1 2"));

    AreaStopper stopper3(QStringLiteral("3"));
    area->walkToolViews(stopper3, Sublime::AllPositions);
    QCOMPARE(stopper3.list.join(QLatin1Char(' ')), QStringLiteral("1 2 3"));

    AreaStopper noStopper(QStringLiteral("X"));
    area->walkToolViews(noStopper, Sublime::AllPositions);
    QCOMPARE(noStopper.list.join(QLatin1Char(' ')), QStringLiteral("1 2 3"));

    delete area;
    delete doc;
    delete controller;
}

QTEST_MAIN(TestAreaWalker)
