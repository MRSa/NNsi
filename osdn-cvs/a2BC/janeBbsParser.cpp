#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "janeBbsParser.h"

/*!
 *  �R���X�g���N�^
 * 
 */
janeBbsParser::janeBbsParser()
{
    // �������Ȃ�...
}

/*!
 *  �f�X�g���N�^
 * 
 */
janeBbsParser::~janeBbsParser()
{
    // �������Ȃ�...
}

/**
 * 
 *  ����������������
 * 
 */
void janeBbsParser::prepare(const QString &arBasePath, const QString &arFileName)
{
    QString fileName = arBasePath + "/" + arFileName;
    mBasePath = arBasePath;
    mPathMap.clear();
    mCategory = "";

    parseFile(fileName);
    
    return;
}

/**
 *  �p�X�����擾����
 * 
 */
QString janeBbsParser::getPath(const QString &arNick)
{
    QString path = mBasePath + "/Logs/2ch/";

    QString nickPath = mPathMap[arNick];
    if (nickPath.isEmpty() != true)
    {
        path = path + nickPath;
    }
//  QMessageBox::information(0, "Info(janeBbsParser::getPath())",  path, QMessageBox::Ok);
    
    return (path);
}


/**
 * 
 *  ����������������
 * 
 */
bool janeBbsParser::parseFile(const QString &arFileName)
{
    QString fileName = arFileName;
//    QMessageBox::information(0, "Info(parseFile)",  fileName, QMessageBox::Ok);

    if (QFile::exists(fileName) != true)
    {
        // �t�@�C�������݂��Ȃ��ꍇ�A�������Ȃ�
        return (false);
    }

    QFile indexFile(fileName);
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }

    QTextStream in(&indexFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        process_line(line);
    }
    indexFile.close();
    
    return (true);
}

void janeBbsParser::process_line(QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        return;
    }
 
    QString categoryStr = arLineData.section("\t", 0, 0);
    QString urlStr      = arLineData.section("\t", 1, 1);
    QString nickStr     = arLineData.section("\t", 2, 2);
    QString bbsStr      = arLineData.section("\t", 3, 3);
    QString optionStr   = arLineData.section("\t", 4);

    if (categoryStr.isEmpty() != true)
    {
        mCategory = categoryStr;
        return;
    }
    if ((nickStr.isEmpty() == false)&&(bbsStr.isEmpty() == false))
    {
        QString nick = nickStr + "/";
        mPathMap[nick] = mCategory + "/" + bbsStr + "/";
    }
    return;    
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
