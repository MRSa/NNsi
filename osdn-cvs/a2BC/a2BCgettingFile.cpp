#include "a2BCgettingFile.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCgettingFile::a2BCgettingFile(QWidget *parent)
 : QWidget(parent), mFileName(0), mUrl(0), mIsPartGet(false)
{

}

/*!
 *  デストラクタ
 * 
 */
a2BCgettingFile::~a2BCgettingFile()
{

}

void a2BCgettingFile::setFileName(QString &arFileName)
{
    mFileName = arFileName;
}

void a2BCgettingFile::setUrl(QString &arUrl)
{
    mUrl = arUrl;
}

void a2BCgettingFile::setPartGet(bool aIsPartGet)
{
    mIsPartGet = aIsPartGet;
}

void a2BCgettingFile::setFileSize(long aFileSize)
{
    mFileSize = aFileSize;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
