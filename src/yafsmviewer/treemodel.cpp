
#include <QtGui>

#include "treeitem.h"
#include "treemodel.h"

//! [0]
TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(0)
{
}

void TreeModel::setData(const QString& data, int columnCnt)
{
  delete rootItem;
  rootItem = 0;
  QList<QVariant> rootData;
  for( int idx = 0; idx < columnCnt; idx++)
  {
    rootData << "";
  }
  rootItem = new TreeItem(rootData);
  setupModelData(data.split(QString("\n")), rootItem);
}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

QVariant TreeModel::data(const QModelIndex &index, int column, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(column);
}


//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].mid(position, 1) != " ")
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split(";", QString::KeepEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
            {
              //printf("adding column data %s\n",qPrintable(columnStrings[column]));
              columnData << columnStrings[column];
            }

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        number++;
    }
}

QModelIndexList TreeModel::find( const QModelIndex & start, int role, const QVariant & value, int hits, Qt::MatchFlags flags) const
{
  QModelIndexList indexList;
  int iHits = 0;
  //printf("match(%d,%d)\n",start.row(),start.column());
  TreeItem *item = static_cast<TreeItem*>(start.internalPointer());
  //printf("start value=%s\n",qPrintable(item->data(start.column()).toString()));


  for(QModelIndex oIndex = start; oIndex.isValid(); oIndex = index(oIndex.row()+1,0))
  {
    for(QModelIndex oIndexC = oIndex; oIndexC.isValid(); oIndexC = index(oIndex.row(),oIndexC.column()+1,start))
    {
      //printf("searching index(%d,%d)\n",oIndexC.row(),oIndexC.column());
      if (role == Qt::DisplayRole)
      {
        TreeItem *item = static_cast<TreeItem*>(oIndexC.internalPointer());
        //printf("value=%s\n",qPrintable(item->data(oIndexC.column()).toString()));
        if( value == item->data(oIndexC.column()))
        {
          //printf("matched at(%d,%d)\n",oIndexC.row(),oIndexC.column());
          indexList.append(oIndexC);
          iHits++;
          if(hits == iHits)
          {
            break;
          }
        }
      }
    }

    if(hasChildren(oIndex))
    {
      //printf("recurse into childs\n");
      QModelIndex childIdx = index(0,0,oIndex);
      if(childIdx.isValid() && (hits != iHits))
      {
        // recurse into structure
        TreeItem *item = static_cast<TreeItem*>(childIdx.internalPointer());
        //printf("childvalue=%s\n",qPrintable(item->data(childIdx.column()).toString()));
        // append found indexes to indexlist.
        indexList += find(childIdx,role,value,hits,flags);
        iHits = indexList.count();
        if( hits <= iHits )
        {
          break;
        }
      }
    }

  }

  return indexList;
}
