#include "StdAfx.h"

DWORD g_dwNameOffset = 0;
DWORD g_dwIdOffset = 0;

BanList g_BanList;
char* g_szIdSearch;

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

bool BanList_HandleEqualFn(const BanItem* pItem)
{
	return !strcmp(pItem->m_szId, g_szIdSearch);
}

void BanList_Add(char* szId)
{
	BanItem* pItem = new BanItem(szId);	
	g_BanList.push_back(pItem);
}

void BanList_Free()
{	
	BanList::iterator iter = g_BanList.begin();

	while (true)
	{			
		if (iter == g_BanList.end()) 
			break;

		BanItem* pItem = *iter;
		delete pItem;

		iter++;
	}
	
	g_BanList.clear();
}

BanItem* BanList_Find(char* szId)
{
	g_szIdSearch = szId;
	BanList::iterator iter = std::find_if(g_BanList.begin(), g_BanList.end(), BanList_HandleEqualFn);
	
	if (iter != g_BanList.end())
		return (*iter);
	
	return NULL;
}

BOOL BanList_IsBanned(DWORD hClient, void* pClientData, char** szName, char* szIP, char** szID)
{
	char* szData = (char*)pClientData;	
	*szName = szData + g_dwNameOffset;
	
	BYTE aAddr[4];
	WORD wPort;
	GetClientAddr(hClient, aAddr, &wPort);
	sprintf(szIP, "%d.%d.%d.%d", aAddr[0], aAddr[1], aAddr[2], aAddr[3]);

	BanItem* pItem = BanList_Find(szIP);

	if (g_dwIdOffset && !pItem)
	{
		*szID = szData + g_dwIdOffset;
		pItem = BanList_Find(*szID);
	}

	return (pItem != NULL);
}
 