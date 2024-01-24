#ifndef A2BBROWSERPARSER_H
#define A2BBROWSERPARSER_H
#include <QString>
#include <QVector>
#include "a2BC_AbstractOtherBrowserParser.h"
#include "bbsTableParser.h"
#include "a2BCdatFileChecker.h"
#include "a2BCfavoriteParser.h"
#include "a2BCindexParser.h"

/*
 *  a2B�̃f�[�^��̓N���X...
 * 
 */
class a2BbrowserParser : public a2BC_AbstractOtherBrowserParser
{

public:
    a2BbrowserParser();
    virtual ~a2BbrowserParser();

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

    virtual QString getSubjectTxtFilePath(QString &arNick);     // subject.txt�̃t�@�C����(full path)���擾����
    virtual bool receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);  // dat�t�@�C������M����
    virtual bool copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate);  // dat�t�@�C������M����
    virtual bool finalize(void);                             // ��n������B�B�B

private:
    bool checkDirectory(QString &arPath);
    void checkUnmanagedFiles(QString &arPath);
    void parseIndexFile(QString &arFileName);
    bool insertData(QString &arNick, QString &arFile, int aReadPoint, qint64 aLastUpdate);

private:
    bbsTableParser      *mpBbsTableParser;
    a2BCdatFileChecker  mDatFileChecker;
    int                 mCount;
    int                 mUnmanagedCount;
    QString              mBaseDirectory;

    QVector<QString>     mUnmanagedDatFilePath;
    QVector<QString>     mUnmanagedDatFileName;

    a2BCfavoriteParser   mFavoriteParser;
    a2BCindexParser      mIndexParser;
};

#endif // #ifndef #ifndef A2BBROWSERPARSER_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
