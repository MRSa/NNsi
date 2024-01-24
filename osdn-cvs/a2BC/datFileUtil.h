#ifndef DATFILE_UTILITY_H
#define DATFILE_UTILITY_H

#include <QWidget>
#include <QString>
#include <QTextStream>

/*!
 *  datFileUtil
 * 
 */
class datFileUtil
{
public:
    datFileUtil();
    virtual ~datFileUtil();

public:
    QString pickupDatFileTitle(const QString &arPath, const QString &arFileName);
    int          checkResCount(const QString &arPath, const QString &arFileName);
    bool   moveDatFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst);
    bool   copyDatFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst);

public:

private:
    bool copyFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst, bool isRemoveSrcFile = false);

private:

};

#endif // #ifndef DATFILE_UTILITY_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
