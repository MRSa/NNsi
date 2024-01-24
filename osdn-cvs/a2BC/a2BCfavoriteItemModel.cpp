/*!
 *   a2BCfavoriteItemModel
 * 
 *    [����]
 *      ���C�ɓ���ҏW�p�f�[�^�ێ��N���X
 * 
 */
#include "a2BCfavoriteItemModel.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCfavoriteItemModel::a2BCfavoriteItemModel(QObject *parent)
 : QStandardItemModel(parent)
{
    //
}

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCfavoriteItemModel::a2BCfavoriteItemModel(int rows, int columns, QObject *parent)
 : QStandardItemModel(rows, columns, parent)
{
    //
}

/*!
 *  �f�X�g���N�^
 * 
 */
a2BCfavoriteItemModel::~a2BCfavoriteItemModel()
{
    //
}

/*!
 * �w�b�_�f�[�^���擾����
 *
 */
QVariant a2BCfavoriteItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if ((section >= 0)&&(section < headerLabel.size()))
            {
                return (headerLabel[section]);   
               }
           }
           else
           {
               // Qt::Vertical
               return (QVariant(section + 1));    
          }
       }
       return (QVariant());
}

/*!
 * �w�b�_���x����ݒ肷��
 *
 */
void a2BCfavoriteItemModel::setHeaderLabel(const QStringList &headerList)
{
    headerLabel = headerList;
    reset();
}

/*!
 *  �J�����Ń\�[�g�H
 *
 */
void a2BCfavoriteItemModel::mysort(int column, Qt::SortOrder order)
{
    // qDebug("a2BCfavoriteItemModel::sort()");
    // Qt::AscendingOrder : Qt::DescendingOrder;

    int rowNum = rowCount();
    for (int loop = 0; loop < rowNum; loop++)
    {
        for (int loop2 = loop + 1; loop2 < rowNum; loop2++)
        {
            int result = compareData(column, loop, loop2);
            if ((result > 0)&&(order == Qt::AscendingOrder))
            {
                 myswapData(loop, loop2);
            }
            else if ((result < 0)&&(order == Qt::DescendingOrder))
            {
                 myswapData(loop, loop2);
            }
        }
    }
    return;
}

/*!
 *   �f�[�^�̓���ւ����s��
 *
 */
void a2BCfavoriteItemModel::myswapData(int data1, int data2)
{
    int columnNum = columnCount();
    for (int loop = 0; loop < columnNum; loop++)
    {
        QVariant value1, value2;
        QModelIndex index1 = index(data1, loop, QModelIndex());
        QModelIndex index2 = index(data2, loop, QModelIndex());
        value1 = data(index1);
        value2 = data(index2);
        setData(index1, value2);
        setData(index2, value1);        
       }
       return;
}

/*!
 *   �l�̔�r
 *
 */
int a2BCfavoriteItemModel::compareData(int column, int data1, int data2)
{
    QModelIndex index1 = index(data1, column, QModelIndex());
    QModelIndex index2 = index(data2, column, QModelIndex());
    QVariant value1 = data(index1);
    QVariant value2 = data(index2);
    if ((value1.type() == QVariant::String)&&(value2.type() == QVariant::String))
    {
        // �������r�����{
        QString v1 = value1.toString();
        QString v2 = value2.toString();
        return (QString::compare(v1, v2));
       }
    else if ((value1.canConvert(QVariant::ULongLong) == true)&&(value2.canConvert(QVariant::ULongLong) == true))
    {
        // ���l��r�����{
        quint64 v1 = value1.toULongLong();
        quint64 v2 = value2.toULongLong();
        return (v1 - v2);
       }
       return (0);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
