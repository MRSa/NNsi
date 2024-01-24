#ifndef A2BCDATFILE_CHECKER_H
#define A2BCDATFILE_CHECKER_H

#include <QWidget>
#include <QString>
#include <QTextStream>

/*!
 *  a2BCsubjectParser
 * 
 */
class a2BCsubjectParser;
class a2BCdatFileChecker : public QWidget
{
    Q_OBJECT

public:
    a2BCdatFileChecker(QWidget *parent = 0);
    virtual ~a2BCdatFileChecker();

public:
    bool appendDataFile(const QString &arPath, const QString &arDatFileName);
    void resetData(void);
    void outputDataFile(QString &arPath, QTextStream *apStream);
    void appendSubjectTxtFile(QString &arPath, a2BCsubjectParser *apParser);

public slots:

signals:

private:
    QString pickupDatFileTitle(const QString &arPath, const QString &arFileName);
    int     checkResCount(const QString &arPath, const QString &arFileName);

public:

private:
    QVector<QString> mPath;
    QVector<QString> mDatFileName;
    QVector<QString> mDatFileTitle;

};

#endif // #ifndef #ifndef A2BCDATFILE_CHECKER_H

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
