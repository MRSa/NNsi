/**
 *   ��2�����u���E�U�̃f�[�^�N���X�Ƃ̓����p�C���^�t�F�[�X�N���X
 * 
 */
#ifndef A2BC_ABSTRACTOTHERBROWSERPARSER_H
#define A2BC_ABSTRACTOTHERBROWSERPARSER_H
#include <QtGlobal>

class QString;
class bbsTableParser;
class a2BC_AbstractOtherBrowserParser
{
public:
    virtual ~a2BC_AbstractOtherBrowserParser() { };

public:
    /**
     *   �u���E�U������������
     *   (�_�C�A���O�{�b�N�X����s���E�B���h�E�ɕ\�����镶����ł�)
     * 
     *   @return �u���E�U�� 
     */
    virtual QString getBrowserName(void) = 0;

    /**
     *   ���O���i�[���Ă���f�B���N�g��(�̃��[�g�f�B���N�g��)��ݒ肷��
     *   (�t�H�[���́u�Ǘ��f�B���N�g���v�Ɏw�肳��Ă��镶���񂪓n����܂�)
     *  @param arPath ���O�Ǘ��f�B���N�g��
     * 
     */
    virtual void setPath(QString &arPath) = 0;
    
    /**
     *   BBSTABLE.HTML�̉�͏���ݒ肷��
     *   
     *   @param apBbsTable �ꗗ(URL)�������Ă���N���X�ւ̃|�C���^
     *   @param arBbsTable bbstable.html�ւ̐�΃p�X
     */
    virtual void setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable) = 0;
    
    /**
     *   �N���X�g�p�̏������s��
     *   (�u���E�U���Ǘ����Ă���dat�t�@�C����F������)
     * 
     *   @return  true:��������, false:�������s
     */
    virtual bool prepare(void) = 0;

    /**
     *  �u���E�U���Ǘ����Ă��郍�O�f�[�^(dat�t�@�C��)������������
     * 
     *  @return ���O�f�[�^(dat�t�@�C��)��
     */
    virtual int  datFileCount(void) = 0;

    /**
     *   �u���E�U���Ǘ����Ă��Ȃ����A���݂��Ă��郍�O�f�[�^(dat�t�@�C��)�����o���A
     *  ���̃��O�f�[�^������������
     *  �ia2B�p�̂�...���u���E�U�� 0 ����������j
     * 
     *  @return �u���E�U�Ǘ��O�̃��O�f�[�^��
     */
    virtual int  checkUnmanagedDatFile(void) = 0;

    /**
     *   �u���E�U���Ǘ����Ă��Ȃ����O�f�[�^�̃t�@�C�������������� 
     *   �ia2B�p�̂�...���u���E�U�� ""(�󕶎���)����������j
     * 
     *  @param aIndex �C���f�b�N�X�ԍ�
     *  @return �C���f�b�N�X�ԍ��ɂ���A�Ǘ��O�̃��O�t�@�C����
     */
    virtual QString getUnmanagedDatFileName(int aIndex) = 0;

    /**
     *   �u���E�U���Ǘ����Ă��Ȃ����O�f�[�^�̔j�b�N�l�[�������擾����
     *   �ia2B�p�̂�...���u���E�U�� ""(�󕶎���)����������j
     * 
     *  @param aIndex �C���f�b�N�X�ԍ�
     *  @return �C���f�b�N�X�ԍ��ɂ���A�Ǘ��O�̃��O�̔j�b�N�l�[��
     * 
     */
    virtual QString getUnmanagedDatNickName(int aIndex) = 0;

    /**
     *   �u���E�U���Ǘ����郍�O�f�[�^(dat�t�@�C��)�̃t�@�C�����i�t���p�X�j�𐶐�����
     * �i���u���E�U�֑���ꍇ�̃t�@�C�����j
     * 
     *  �@@param arNick �j�b�N�l�[�� (mobile/ �Ƃ� chakumelo/ �Ƃ�)
     *    @param arFileName dat�t�@�C���� (12345678.dat �Ƃ��A�A�A)
     *    @return �t�@�C���� (�t���p�X)
     */
    virtual QString getFullPathDatFileName(QString &arNick, QString &arFileName) = 0;

    /**
     *   �u���E�U���Ǘ����郍�O�f�[�^(dat�t�@�C��)�̃t�@�C�����i�t���p�X�j�𐶐�����
     *  �i���u���E�U�����荞�ޏꍇ�̃t�@�C�����j
     * 
     *  �@@param arNick �j�b�N�l�[�� (mobile/ �Ƃ� chakumelo/ �Ƃ�)
     *    @param arFileName dat�t�@�C���� (12345678.dat �Ƃ�a2B���炭��Ƃ��ɂ́A�g���q������Ă����������)
     * 
     */
    virtual QString getReceiveDatFileName(QString &arNick, QString &arFileName) = 0;

    /**
     *   �C���f�b�N�X�ԍ��ɂ���Adat�t�@�C�������擾����
     * 
     *   @param aIndex �C���f�b�N�X�ԍ�
     *   @return dat�t�@�C����
     */
    virtual QString getDatFileName(int aIndex) = 0;

    /**
     *   �C���f�b�N�X�ԍ��ɂ���A�j�b�N�l�[�����擾����
     * 
     *   @param aIndex �C���f�b�N�X�ԍ�
     *   @return �j�b�N�l�[��
     */
    virtual QString getBoardNick(int aIndex) = 0;

    /**
     *   �u���E�U���Ǘ����Ă��郌�X��؂肪a2B�i2�����˂�T�[�o�j�Ɠ������ǂ����H
     *   (0x0a �Ȃ瓯���A0x0d, 0x0a�Ȃ�Ⴄ)
     * 
     *   @return true:����(0x0a)  false:�Ⴄ(0x0d, 0x0a)
     */
    virtual bool   isSameLogFormat(void) = 0;

    /**
     *  �C���f�b�N�X�ԍ��ɂ���A�ǂ�ł���ꏊ�i���X�ԍ��j���擾����
     * 
     *   @param aIndex �C���f�b�N�X�ԍ�
     *   @return  �ǂ�ł���ꏊ�i�u�����܂œǂ񂾁v���X�ԍ��j
     * 
     */
    virtual int    getReadPoint(int aIndex) = 0;

    /**
     *  ���O���Ō�ɍX�V�������Ԃ��擾����
     * 
     *   @param aIndex �C���f�b�N�X�ԍ�
     *   @return  �ŏI�X�V���� (�Ǘ����Ă��Ȃ��ꍇ�ɂ́A0����������)
     * 
     */
    virtual qint64  getLastUpdate(int aIndex) = 0;

    /**
     *  �u���E�U���Ǘ����Ă��� subject.txt �̃t�@�C���ɂ��āA�t���p�X���擾����
     * �i���u���E�U����subject.txt���R�s�[����Ƃ��Ɏg�p����j
     * 
     *  @param arNick �j�b�N�l�[��
     *  @return �j�b�N�l�[����subject.txt�i�t���p�X�j
     * 
     */
    virtual QString getSubjectTxtFilePath(QString &arNick) = 0;

    /**
     *  dat�t�@�C�����u���E�U�̊Ǘ����ֈړ�������B
     * �i���̃u���E�U�ɂ���dat�t�@�C�����ړ�������j
     * 
     *  @param arNick      �j�b�N�l�[��
     *  @param arFile      �t�@�C����
     *  @param arDst       �ړ���dat�t�@�C�� (full path)
     *  @param arSrc       �ړ���dat�t�@�C�� (full path)
     *  @param isSame      ���X��؂肪2�����˂�T�[�o(a2B)�Ɠ����`�����H
     *  @param aReadPoint  ���ǃ��X�ʒu
     *  @param aLastUpdate �ŏI�X�V����
     * 
     */
    virtual bool receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate) = 0;

    /**
     *  dat�t�@�C�����u���E�U�̊Ǘ����փR�s�[����B
     * �i���̃u���E�U�ɂ���dat�t�@�C�����R�s�[����j
     * 
     *  @param arNick      �j�b�N�l�[��
     *  @param arFile      �t�@�C����
     *  @param arDst       �R�s�[��dat�t�@�C�� (full path)
     *  @param arSrc       �R�s�[��dat�t�@�C�� (full path)
     *  @param isSame      ���X��؂肪2�����˂�T�[�o(a2B)�Ɠ����`�����H
     *  @param aReadPoint  ���ǃ��X�ʒu
     *  @param aLastUpdate �ŏI�X�V����
     * 
     */
    virtual bool copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate) = 0;

    /**
     *  ���O�����̂��Ƃ��܂B
     * 
     *  @return true:��n���I��  false:���s
     * 
     */
    virtual bool finalize(void) = 0;

};
#endif // #ifndef #ifndef A2BC_ABSTRACTOTHERBROWSERPARSER_H
/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
