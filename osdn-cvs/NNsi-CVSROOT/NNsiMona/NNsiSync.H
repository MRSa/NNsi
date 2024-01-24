///////////////////////////////////////////////////////
//// NNsiSync.H  : NNsi�p�̓����N���X(�w�b�_)
///////////////////////////////////////////////////////
#ifndef  __NNSISYNC_H__
#define  __NNSISYNC_H__

// CSynchronizer�N���X���p�����č쐬����
class NNsiCSynchronizer : public CSynchronizer
{
  public:
    // �R���X�g���N�^(CSynchronizer�Ɠ���)
	NNsiCSynchronizer(HANDLE langInstance, CSyncProperties& rProps, DWORD dwDatabaseFlags = 0);

    // HotSync�̏����J�n
    virtual long Perform(void);

  protected:
	// �g�p����
	HANDLE m_hLangInstance;

	// �����[�gDB�̃��R�[�h�T�C�Y
	long m_remoteDBrecSize;

  private:
	// �f�o�b�O���O�o��
	void NNsiDebugLog(char *message, char *msg2, long value);

	// ���O���o��
	void NNsiOutputInfoLog(UINT strId, char *msg, long value);

	// ��������DB��������
	long PrepareRemoteDBInfo(char *dbName, long version, long recSize);


	// �����O����
	long PrologueAllSynchronize(void);
	long PrologueSynchronize(char *dbName);

	// �����㏈��
	long EpilogueAllSynchronize(void);
	long EpilogueSynchronize(char *dbName);

	// ��������
	long ExecuteSynchronize(char *dbName, long version, long recSize);

	// HH�}�l�[�W����NNsiMona�ŗL�̂��̂��`
	virtual long CreateHHManager(void);

	// �f�o�b�O�p�ɉ��ǉ�(AppInfo�͓��������Ȃ��悤�ɂ���B)
	long SynchronizeAppInfo(void);
	long CopyAppInfoHHtoPC(void);
	long CopyAppInfoPCtoHH(void);
};


#endif