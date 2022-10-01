#include "StdAfx.h"

int g_nPlayersPerId = 0;
DWORD g_dwClientDataLen = 0;
DWORD g_dwNameOffset = 0;
DWORD g_dwIdOffset = 0;

BanList g_BanList;
IPList g_InGameIPList;

char* g_szIdSearch;

char* g_aszBanReasons[BAN_REASON_MAX] = { NULL, "Client banned", "Limit reached" };

void GetClientAddr(DWORD hClient, BYTE pAddr[4], WORD *pPort)
{
	CClient* pClient = (CClient*)hClient;
	CUDPConn* pConnection = pClient->GetConnection();
	
	if (pConnection)
	{
		BYTE* pRealAddr = pConnection->GetRemoteAddr();
		
		((BYTE*)pPort)[0] = pRealAddr[1];
		((BYTE*)pPort)[1] = pRealAddr[0];
		pAddr[0] = pRealAddr[2];
		pAddr[1] = pRealAddr[3];
		pAddr[2] = pRealAddr[4];
		pAddr[3] = pRealAddr[5];	
	}
}

bool BanList_HandleEqualFn(const ClientIdItem* pItem)
{
	return !strcmp(pItem->m_szId, g_szIdSearch);
}

void BanList_Add(char* szId)
{
	ClientIdItem* pItem = new ClientIdItem(szId);
	g_BanList.push_back(pItem);
}

void InGameIPList_Add(DWORD hClient)
{
	BYTE aAddr[4];
	WORD wPort;
	GetClientAddr(hClient, aAddr, &wPort);

	g_InGameIPList.push_back(*(DWORD*)aAddr);
}

void InGameIPList_Remove(DWORD hClient)
{
	BYTE aAddr[4];
	WORD wPort;
	GetClientAddr(hClient, aAddr, &wPort);

	IPList::iterator iter = std::find(g_InGameIPList.begin(), g_InGameIPList.end(), *(DWORD*)aAddr);

	if (iter != g_InGameIPList.end())
		g_InGameIPList.erase(iter);	
}

void BanList_Free()
{	
	BanList::iterator iter = g_BanList.begin();

	while (true)
	{			
		if (iter == g_BanList.end()) 
			break;

		ClientIdItem* pItem = *iter;
		delete pItem;

		iter++;
	}
	
	g_BanList.clear();
}

void InGameIPList_Free()
{
	g_InGameIPList.clear();
}

ClientIdItem* BanList_Find(char* szId)
{
	g_szIdSearch = szId;
	BanList::iterator iter = std::find_if(g_BanList.begin(), g_BanList.end(), BanList_HandleEqualFn);
	
	if (iter != g_BanList.end())
		return (*iter);
	
	return NULL;
}

int InGameList_IP_Count(DWORD dwAddr)
{
	int nResult = 0;
	IPList::iterator iter = g_InGameIPList.begin();

	while (true)
	{
		if (iter == g_InGameIPList.end())
			break;

		if ((*iter) == dwAddr)
			nResult++;

		iter++;
	}

	return nResult;
}

eBanReason BanList_IsBanned(DWORD hClient, void* pClientData, char** szName, char* szIP, char** szID, int& nCount)
{
	char* szData = (char*)pClientData;	
	*szName = szData + g_dwNameOffset;
	
	BYTE aAddr[4];
	WORD wPort;
	GetClientAddr(hClient, aAddr, &wPort);
	sprintf(szIP, "%d.%d.%d.%d", aAddr[0], aAddr[1], aAddr[2], aAddr[3]);

	ClientIdItem* pItem = BanList_Find(szIP);

	if (g_dwIdOffset)
	{
		*szID = szData + g_dwIdOffset;
		pItem = BanList_Find(*szID);
	}

	if (pItem != NULL)
		return BAN_REASON_BANNED;

	if (g_nPlayersPerId)
	{
		nCount = InGameList_IP_Count(*(DWORD*)aAddr);
		if (nCount >= g_nPlayersPerId)
			return BAN_REASON_LIMIT_IP;
	}

	return BAN_REASON_NONE;
}
 