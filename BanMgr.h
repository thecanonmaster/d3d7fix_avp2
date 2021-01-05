struct BanItem
{
	BanItem(char* szId)
	{
		strcpy(m_szId, szId);
	}

	char m_szId[64];
};

typedef std::list<BanItem*> BanList;

#define BAN_MGR_SECTION			"Ban_Manager"
#define BAN_MGR_NAME_OFFSET		"NameOffset"
#define BAN_MGR_ID_OFFSET		"IdOffset"
#define BAN_MGR_BAN				"Ban%d"

extern DWORD g_dwNameOffset;
extern DWORD g_dwIdOffset;
extern BanList g_BanList;

class CUDPConn
{
public:
	
	BYTE* GetRemoteAddr() { return m_nRawAddr; }
	
private:
	BYTE m_nData[0xBE]; // 0xBE
	BYTE m_nRawAddr[6];
};

class CClient
{
	
public:
	
	CUDPConn* GetConnection() { return m_pConnection; }
	
private:
	
	BYTE m_nData[0x3C7]; // 0x3C8
	CUDPConn* m_pConnection;
};

void BanList_Add(char* szId);
void BanList_Free();
BanItem* BanList_Find(char* szId);
BOOL BanList_IsBanned(DWORD hClient, void* pClientData, char** szName, char* szIP, char** szID);