///////////////////////////////////////////////////////
//// NNsiCHHMgr.H  : NNsi�p�̓����N���X(�w�b�_)
///////////////////////////////////////////////////////
#ifndef  __NNSICHHMGR_H__
#define  __NNSICHHMGR_H__
class NNsiCHHMgr : public CHHMgr
{
  public:
	// �R���X�g���N�^
    NNsiCHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, eSyncTypes syncType = eDoNothing);
    NNsiCHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, char  *pName, DWORD  dwCreator, DWORD  dwType, WORD   dbFlags, WORD   dbVersion, int  iCardNum, eSyncTypes syncType );

};

#endif  // #ifndef __NNSICHHMGR_H__
