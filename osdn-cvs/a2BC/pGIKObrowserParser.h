#ifndef PGIKOBROWSERPARSER_H
#define PGIKOBROWSERPARSER_H
#include <QString>
#include <QVector>
#include "a2BC_AbstractOtherBrowserParser.h"
#include "pGIKOacquiredParser.h"

/*
 * �ۂ��M�R�̃f�[�^��̓N���X...
 * 
 */
class QString;
class bbsTableParser;
class pGIKObrowserParser : public a2BC_AbstractOtherBrowserParser
{

public:
    pGIKObrowserParser();
    virtual ~pGIKObrowserParser();

public:
    virtual QString getBrowserName(void);          // �u���E�U������������
    virtual void setPath(QString &arPath);         // ���O�i�[�f�B���N�g��(�̃��[�g�f�B���N�g��)��ݒ�
    virtual void setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable); // BBSTABLE.HTML(�Ƃ������ꗗ�t�@�C��)�̊Ǘ��t�@�C�����w��
    virtual bool prepare(void);                   // ���O�f�[�^�̏����B�B�B
    virtual int  datFileCount(void);              // ���O�f�[�^������������

    virtual int     checkUnmanagedDatFile(void);                 // �Ǘ��O��dat�t�@�C�����`�F�b�N����
    virtual QString getUnmanagedDatFileName(int aIndex);       // �Ǘ��O��dat�t�@�C�������擾����
    virtual QString getUnmanagedDatNickName(int aIndex);       // �Ǘ��O��dat�j�b�N�l�[�����擾����
    virtual QString getFullPathDatFileName(QString &arNick, QString &arFileName);  // dat�t�@�C�����𐶐�����
    virtual QString getReceiveDatFileName(QString &arNick, QString &arFileName);   // ��M�pdat�t�@�C�����𐶐�����
    virtual QString getDatFileName(int aIndex);                // dat�t�@�C�������擾����
    virtual QString getBoardNick(int aIndex);                  // dat�t�@�C���̑��݂����nick���擾����
    virtual bool isSameLogFormat(void) {return (true); };   // ���O�t�H�[�}�b�g��a2B�Ɠ������H
    virtual int  getReadPoint(int aIndex);                    // �ǂ񂾏ꏊ����������
    virtual qint64 getLastUpdate(int aIndex);                  // �ŏI�X�V���Ԃ��擾����

    virtual QString getSubjectTxtFilePath(QString &arNick);         // subject.txt�̃t�@�C����(full path)���擾����
    virtual bool receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);  // dat�t�@�C������M����
    virtual bool copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);  // dat�t�@�C������M����
    virtual bool finalize(void);                             // ��n������B�B�B

private:
    bool checkDirectory(QString &arPath);
    void parseIndexFile(QString &arNickName, QString &arFileName);
    void clearAcquiredParser(void);
    bool updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);

private:
    bbsTableParser      *mpBbsTableParser;
    QString              mBbsTable;
    QString              mLogPath;

    QVector<QString>              mDatNickName;
    QVector<QString>              mDatFilePath;
    QVector<QString>              mDatFileName;

    QVector<QString>              mIdxFilePath;
    QVector<QByteArray>           mIdxTitle;
    QVector<int>                  mIdxReadNumber;

    QVector<QString>               mAcquiredNickName;
    QVector<QString>               mAcquiredFileName;
    QVector<pGIKOacquiredParser *> mAcquiredParser;
};

#endif // #ifndef #ifndef PGIKOBROWSERPARSER_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
