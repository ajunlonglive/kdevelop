/*
    SPDX-FileCopyrightText: 2007 Alexander Dymo <adymo@kdevelop.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "aggregatemodel.h"

#include <QStandardItem>
#include <QStandardItemModel>


namespace Sublime {

class AggregateModelPrivate
{
public:
    /*Instance of this class is used as an internal pointer to the aggregator items
    in the model to differentiate between aggregators and non-aggregators.*/
    class AggregateInternalData {
    };

    AggregateModelPrivate()
    {
        internal = new AggregateInternalData();
    }
    ~AggregateModelPrivate()
    {
        delete internal;
    }

    QList<QStandardItemModel*> modelList;
    QMap<QStandardItemModel*, QString> modelNames;
    AggregateInternalData *internal;
};



AggregateModel::AggregateModel(QObject *parent)
    :QAbstractItemModel(parent)
    ,d_ptr(new AggregateModelPrivate())
{
}

AggregateModel::~AggregateModel() = default;

void AggregateModel::addModel(const QString &name, QStandardItemModel *model)
{
    Q_D(AggregateModel);

    beginResetModel();
    d->modelList << model;
    d->modelNames[model] = name;
    endResetModel();
}

void AggregateModel::removeModel(QStandardItemModel *model)
{
    Q_D(AggregateModel);

    beginResetModel();
    d->modelList.removeAll(model);
    d->modelNames.remove(model);
    endResetModel();
}



// reimplemented methods from QAbstractItemModel

Qt::ItemFlags AggregateModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AggregateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    //there's nothing to return here because aggregated models will have different headers
    //so we just use empty headers for aggregate model.
    return QString();
}

int AggregateModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    //only 1 column is supported atm
    return 1;
}

int AggregateModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const AggregateModel);

    if (!parent.isValid())
    {
        //toplevel items represent aggregated models
        return d->modelList.count();
    }
    else
    {
        //Qt model guideline - only 1st column has children
        if (parent.column() != 0)
            return 0;

        //find out if the parent is an aggregator
        if (parent.internalPointer() == d->internal)
        {
            //return the number of toplevel rows in the source model
            return d->modelList[parent.row()]->rowCount(QModelIndex());
        }
        else
        {
            //we have a standard item in the source model - just map it into our model
            auto *item = static_cast<QStandardItem*>(parent.internalPointer());
            return item->rowCount();
        }
    }
}

QVariant AggregateModel::data(const QModelIndex &index, int role) const
{
    Q_D(const AggregateModel);

    if (!index.isValid() || (role != Qt::DisplayRole))
        return QVariant();

    if (!index.parent().isValid())
    {
        //aggregator item
        return d->modelNames[d->modelList[index.row()]];
    }
    else
    {
        //we have a standard item in the source model - just map it into our model
        auto *item = static_cast<QStandardItem*>(index.internalPointer());
        return item->data(role);
    }
}

QModelIndex AggregateModel::parent(const QModelIndex &index) const
{
    Q_D(const AggregateModel);

    if (!index.isValid())
        return QModelIndex();

    if (index.internalPointer() == d->internal)
    {
        //this is aggregator item, it has no parents
        return QModelIndex();
    }

    //this is just an item from the model
    auto *item = static_cast<QStandardItem*>(index.internalPointer());
    QModelIndex parent;
    if (!item->parent())
    {
        //we need to find the aggregator item that owns this index
        //first find the model for this index
        QStandardItemModel *model = item->model();
        //next find the row number of the aggregator item
        int row = d->modelList.indexOf(model);
        parent = createIndex(row, 0, d->internal);
    }
    else
    {
        //we have a standard item in the source model - just map it into our model
        parent = createIndex(item->parent()->row(), 0, item->parent());
    }
    return parent;
}

QModelIndex AggregateModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const AggregateModel);

    if (row < 0 || column < 0)
        return QModelIndex();

    if (!parent.isValid())
    {
        if (column > 1 || row >= d->modelList.count())
            return QModelIndex();
        //this is an aggregator item
        return createIndex(row, column, d->internal);
    }
    else if (parent.internalPointer() == d->internal)
    {
        //the parent is an aggregator
        //find the model that holds the items
        QStandardItemModel *model = d->modelList[parent.row()];
        //this is the first level of items
        QStandardItem *item = model->item(row, column);
        if (item)
            return createIndex(row, column, item);
        else
            return QModelIndex();
    }
    else
    {
        //we have a standard item in the source model - just map it into our model
        auto *parentItem = static_cast<QStandardItem*>(parent.internalPointer());
        return createIndex(row, column, parentItem->child(row, column));
    }
}

}


