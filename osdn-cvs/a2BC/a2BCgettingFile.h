#ifndef A2BCGETTINGFILE_H
#define A2BCGETTINGFILE_H

#include <QWidget>
#include <QString>

class a2BCgettingFile : public QWidget
{
    Q_OBJECT

public:
    a2BCgettingFile(QWidget *parent = 0);
    virtual ~a2BCgettingFile();

public:
    void setFileName(QString &arFileName);
    void setUrl(QString &arUrl);
    void setPartGet(bool aIsPartGet);
    void setFileSize(long aFileSize);

    QString getFileName() { return (mFileName); };
    QString getUrl() { return (mUrl); };
    bool getPartGet() { return (mIsPartGet); };
    long getFileSize() { return (mFileSize); };

public slots:

signals:

public:

private:
    QString mFileName;
    QString mUrl;
    bool   mIsPartGet;
    long   mFileSize;

};

#endif // #ifndef #ifndef A2BCGETTINGFILE_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
