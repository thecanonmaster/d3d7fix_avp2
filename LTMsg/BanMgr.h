struct ClientIdItem
{
	ClientIdItem(char* szId)
	{
		strcpy(m_szId, szId);
	}

	char m_szId[64];
};

typedef std::list<ClientIdItem*> BanList;
typedef std::list<DWORD> IPList;

enum eBanReason
{
	BAN_REASON_NONE = 0,
	BAN_REASON_BANNED = 1,
	BAN_REASON_LIMIT_IP,
	BAN_REASON_MAX,
};

extern char* g_aszBanReasons[BAN_REASON_MAX];

#define BAN_MGR_SECTION			"Ban_Manager"
#define BAN_MGR_PLAYERS_PER_ID	"PlayersPerId"
#define BAN_MGR_CLIENT_DATA_LEN	"ClientDataLen"
#define BAN_MGR_NAME_OFFSET		"NameOffset"
#define BAN_MGR_ID_OFFSET		"IdOffset"
#define BAN_MGR_BAN				"Ban%d"

#define BAN_MGR_CLIENT_REJECTED	"Client rejected"

extern int g_nPlayersPerId;
extern DWORD g_dwClientDataLen;
extern DWORD g_dwNameOffset;
extern DWORD g_dwIdOffset;

extern BanList g_BanList;
extern IPList g_InGameIPList;

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
void InGameIPList_Add(DWORD hClient);
void InGameIPList_Remove(DWORD hClient);

void BanList_Free();
void InGameIPList_Free();

eBanReason BanList_IsBanned(DWORD hClient, void* pClientData, char** szName, char* szIP, char** szID, int& nCount);