/*
    SPDX-FileCopyrightText: 2007 Andreas Pakulat <apaku@gmx.de>
    SPDX-FileCopyrightText: 2007 Dukju Ahn <dukjuahn@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDEVPLATFORM_PLUGIN_OUTPUTWIDGET_H
#define KDEVPLATFORM_PLUGIN_OUTPUTWIDGET_H

#include <QHash>
#include <QWidget>

#include <interfaces/itoolviewactionlistener.h>
#include <outputview/ioutputviewmodel.h>
#include <outputview/ioutputview.h>

class KExpandableLineEdit;
class KToggleAction;
class StandardOutputViewTest;
class QAction;
class QAbstractItemView;
class QLineEdit;
class QModelIndex;
class QSortFilterProxyModel;
class QStackedWidget;
class QString;
class QTabWidget;
class QToolButton;
class QTreeView;
class QWidgetAction;
class ToolViewData;

class OutputWidget : public QWidget, public KDevelop::IToolViewActionListener
{
    Q_OBJECT
    Q_INTERFACES(KDevelop::IToolViewActionListener)

    friend class StandardOutputViewTest;

public:
    OutputWidget(QWidget* parent, const ToolViewData* data);
    ~OutputWidget() override;

    void removeOutput( int id );
    void raiseOutput( int id );
public Q_SLOTS:
    void addOutput( int id );
    void changeModel( int id );
    void changeDelegate( int id );
    void closeActiveView();
    void closeOtherViews();
    void selectFirstItem();
    void selectNextItem() override;
    void selectPreviousItem() override;
    void selectLastItem();
    void activate(const QModelIndex&);
    void scrollToIndex( const QModelIndex& );
    void setTitle(int outputId, const QString& title);

Q_SIGNALS:
    void outputRemoved( int, int );

private Q_SLOTS:
    void nextOutput();
    void previousOutput();
    void copySelection();
    void selectAll();
    void outputFilter(const QString& filter);
    void updateFilter(int index);
    void clearModel();

private:
    enum SelectionMode {
        Last,
        Next,
        Previous,
        First
    };
    void selectItem(SelectionMode selectionMode);

    QTreeView* createListView(int id);
    void setCurrentWidget( QTreeView* view );
    QWidget* currentWidget() const;
    void enableActions();
    KDevelop::IOutputViewModel* outputViewModel() const;
    QAbstractItemView* outputView() const;
    void activateIndex(const QModelIndex& index, QAbstractItemView* view, KDevelop::IOutputViewModel* iface);
    void eventuallyDoFocus();
    int currentOutputIndex();

    struct FilteredView {
        QTreeView* view = nullptr;
        QSortFilterProxyModel* proxyModel = nullptr;
        QString filter;
    };
    QHash<int, FilteredView>::iterator findFilteredView(QAbstractItemView *view);

    QHash<int, FilteredView> m_views;
    QTabWidget* m_tabwidget;
    QStackedWidget* m_stackwidget;
    const ToolViewData* data;
    QToolButton* m_closeButton;
    QAction* m_closeOthersAction;
    QAction* m_nextAction;
    QAction* m_previousAction;
    KToggleAction* m_activateOnSelect;
    KToggleAction* m_focusOnSelect;
    KExpandableLineEdit* m_filterInput;
    QWidgetAction* m_filterAction;
};

#endif

