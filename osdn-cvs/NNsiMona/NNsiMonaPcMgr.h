/*****************************************************************************
 *
 * CNNsiMonaPCMgr Generic Conduit PC Database Mgr Header File
 *
 ****************************************************************************/
#ifndef _NNsiMona_PC_DB_MGR_H_
#define _NNsiMona_PC_DB_MGR_H_
#include <CPString.h>
#include <PcMgr.h>

#include "NNsiMonaRecord.h"

class CNNsiMonaPCMgr : public CPcMgr
{
public: 
    CNNsiMonaPCMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName = NULL, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CNNsiMonaPCMgr();
    virtual long RetrieveDB(void);
    virtual long StoreDB(void);


protected:
    CPString	m_csTableString;
    CPString        m_csDbFile;
    NNsiMonaSchema m_Schema;

public:

    virtual long CompactCategories() {return 0;}
    virtual long ExtractCategories(void) {return 0;}
    virtual BOOL IsAppInfo(void) { return TRUE;}
    virtual BOOL IsSortInfo(void){ return FALSE;}


                                     
protected:
    virtual BOOL    IsExtraAppInfo(void);
    virtual long    AllocateAppInfo(DWORD dwRawDataSize, BOOL bClearData);
    virtual long    SetExtraAppInfo(DWORD dwSize, BYTE *pData);
    virtual DWORD   GetExtraAppInfoSize(void);
    virtual long    GetExtraAppInfo(BYTE *pBuf, DWORD *pdwBufSize);
    virtual BYTE*   GetExtraAppInfo(void);

	virtual long ReadSerializedFieldValue(DWORD &dwValue, 
                                      WORD wIndex);
	virtual long ReadSerializedFieldValue(CPString &csValue, 
                                      DWORD &dwCaseSensitive, 
									  WORD wIndex);	
	virtual long ReadSerializedFieldValue(TCHAR *pStrBuf, 
                                      int *piSize, 
                                      DWORD &dwCaseSensitive, 
                                      WORD wIndex); 

    virtual long StoreCategories(void);
    virtual long LoadCategories(void);

    virtual long LoadSchema(void);
    virtual long StoreSchema(void);
	virtual long LoadRecords(void);
    virtual long StoreRecords(void);

	virtual long StoreRecord(CNNsiMonaRecord &Rec);
	virtual long AddCustomRec(CNNsiMonaRecord &rec);
	virtual long ConvertPCtoGeneric(CNNsiMonaRecord &rec, CPalmRecord &palmRec);
	virtual long ConvertGenericToPc(CPalmRecord &palmRec, CNNsiMonaRecord &rec, BOOL bClearAttributes=TRUE);
	virtual long StoreRecord(MEMORECORD &Rec);
	virtual long AddCustomRec(MEMORECORD &rec);
	virtual long ConvertPCtoGeneric(MEMORECORD &rec, CPalmRecord &palmRec);
	virtual long ConvertGenericToPc(CPalmRecord &palmRec, MEMORECORD &rec, BOOL bClearAttributes=TRUE);








};



#endif



