#include "StdAfx.h"

FILE* g_LogFile = NULL;
char g_szProfile[64] = { 0 };
eFpsCounterPos g_eDrawFPS = FCP_LEFT_BOTTOM;
DWORD g_dwWidth = 1024;
DWORD g_dwHeight = 768;
BOOL g_bWindowedSet = FALSE;
BOOL g_bWindowed = FALSE;
DWORD g_dwPPCurrIntensity = 0;
LPDIRECTDRAWSURFACE7 g_ddsBackBuffer = NULL;
LPDIRECT3DDEVICE7 g_d3dDevice = NULL;
LPDIRECTDRAW7 g_ddMainDDraw = NULL;
HWND g_hWindowHandle = NULL;
BOOL g_bWindowHooked = FALSE;
CClientMgrBase* g_pClientMgr = NULL;
CServerMgrBase* g_pServerMgr = NULL;
CClassMgrBase* g_pClassMgr = NULL;
ILTClient* g_pLTClient = NULL;
ILTServer* g_pLTServer = NULL;
float g_fLastFontListUpdate = 0.0f;
BOOL g_bInTWMDetailTex_WorldList = FALSE;
WorldList g_TWMDetailTex_WorldList;
SDWList g_SolidDrawingWhitelist;
BOOL g_bCVarProfileIgnored = FALSE;

LONG g_lRMILastX = 0;
LONG g_lRMILastY = 0;
//DWORD g_hMouseSensitivity = 0;

ProfileOption g_ProfileOptions[PO_MAX] = 
{
	ProfileOption(POT_STRING, "FixVersion"),
	ProfileOption(POT_BYTE, "DgVoodooMode"),
	ProfileOption(POT_STRING, "DefaultProfile"),
	ProfileOption(POT_FLOAT, "IntroductionTime"),
	ProfileOption(POT_STRING, "RenderDll"),
	ProfileOption(POT_STRING, "RenderWrapperDll"),
	ProfileOption(POT_STRING, "ConsoleBackground"),
	ProfileOption(POT_STRING, "Misc_Description"),
	ProfileOption(POT_BYTE, "Misc_CleanMode"),
	ProfileOption(POT_BYTE, "Misc_DontShutdownRenderer"),
	ProfileOption(POT_BYTE, "Misc_ShowFPS"),
	ProfileOption(POT_DWORD, "Misc_FrameLimiterSleep"),
	ProfileOption(POT_FLOAT, "Misc_CameraFOVXScaler"),
	ProfileOption(POT_FLOAT, "Misc_ServerFPS"),
	ProfileOption(POT_BYTE, "Misc_EnableConsole"),
	ProfileOption(POT_BYTE, "Misc_NoEnvMapConsolePrint"),
	ProfileOption(POT_FLOAT, "Fix_MaxFPS"),
	ProfileOption(POT_BYTE, "Fix_IntelHD"),
	ProfileOption(POT_DWORD, "Fix_Radeon5700"),
	ProfileOption(POT_BYTE, "Fix_CameraFOV"),
	ProfileOption(POT_BYTE, "Fix_ViewModelFOV"),
	ProfileOption(POT_BYTE, "Fix_SolidDrawing"),
	ProfileOption(POT_BYTE, "Fix_SolidDrawingWhitelist"),
	ProfileOption(POT_BYTE, "Fix_LightLoad"),
	ProfileOption(POT_BYTE, "Fix_MiscCC"),
	ProfileOption(POT_BYTE, "Fix_RawMouseInput"),
	ProfileOption(POT_DWORD, "Fix_TWMDetailTex"),
	ProfileOption(POT_BYTE, "Fix_TimeCalibration"),
	ProfileOption(POT_BYTE, "Fix_FlipScreen"),
	ProfileOption(POT_BYTE, "Fix_DynamicLightSurfaces"),
	ProfileOption(POT_BYTE, "Fix_StaticLightSurfaces"),
	ProfileOption(POT_BYTE, "Fix_PreloadStaticLight"),
	ProfileOption(POT_BYTE, "Fix_FullscreenOptimize"),
	ProfileOption(POT_BYTE, "Fix_NoVSync"),
	ProfileOption(POT_DWORD, "Fix_UpdateObjectLTO"),
	ProfileOption(POT_DWORD, "Fix_UpdProgDmgObjectLTO"),
	ProfileOption(POT_BYTE, "Fix_FastCRCCheck"),
	ProfileOption(POT_FLOAT, "RMI_ScaleGlobal"),
	ProfileOption(POT_FLOAT, "RMI_ScaleY"),
	ProfileOption(POT_BYTE, "PP_Enabled"),
	ProfileOption(POT_DWORD, "PP_Intensity"),
	ProfileOption(POT_DWORD, "PP_IntensityMenu"),
	ProfileOption(POT_DWORD, "PP_IntensityVisionMode"),
	ProfileOption(POT_BYTE, "Ext_BanManager"),
	ProfileOption(POT_FLOAT, "Ext_MOTDTimer"),
	ProfileOption(POT_STRING, "Ext_MOTDString0"),
	ProfileOption(POT_STRING, "Ext_MOTDString1"),
	ProfileOption(POT_STRING, "Ext_MOTDString2"),
	ProfileOption(POT_STRING, "Ext_MOTDString3"),
	ProfileOption(POT_STRING, "Ext_MOTDString4"),
	ProfileOption(POT_STRING, "Ext_CacheList"),
	ProfileOption(POT_STRING, "Ext_CmdList"),
	ProfileOption(POT_DWORD, "Ext_OnInfoQuery"),
	ProfileOption(POT_DWORD, "Ext_SendResponseInfo"),
	ProfileOption(POT_STRING, "Ext_GSResponseKey0"),
	ProfileOption(POT_STRING, "Ext_GSResponseKey1"),
	ProfileOption(POT_STRING, "Ext_GSResponseKey2"),
	ProfileOption(POT_STRING, "Ext_GSResponseValue0"),
	ProfileOption(POT_STRING, "Ext_GSResponseValue1"),
	ProfileOption(POT_STRING, "Ext_GSResponseValue2"),
};

FontList g_FontList;
DWORD g_hWhitePixelSurface = NULL;
float g_fIntroductionStartTime = 0.0f;
DWORD g_hIntroductionSurface[INTRODUCTION_LINES] = { NULL, NULL, NULL, NULL, NULL, NULL };
DWORD g_hFont15Surface = NULL;
int g_nLastFrameRate = 0;
//SolidSurfaceList g_SolidSurfaceList;

DWORD g_hFontSearch;
char* g_szStringSearch;
DWORD g_hSurfaceSearch;
DWORD g_hColorSearch;

void (__cdecl *ILTCSBase_CPrint)(ILTCSBase* pBase, char *pMsg, ...);
DWORD (__stdcall *ILTCSBase_CreateString)(char *pString);
void (__stdcall *ILTCSBase_FreeString)(DWORD hString);
char* (__stdcall *ILTCSBase_GetStringData)(DWORD hString);
float (__stdcall *ILTCSBase_GetVarValueFloat)(DWORD hVar);
char* (__stdcall *ILTCSBase_GetVarValueString)(DWORD hVar);
float (__fastcall *ILTCSBase_GetTime)(ILTCSBase* pBase);
float (__fastcall *ILTCSBase_GetFrameTime)(ILTCSBase* pBase);

DWORD (__fastcall *ILTServer_StartMessage)(ILTCSBase* pServer, void* notUsed, DWORD hSendTo, BYTE nMsgID);
DWORD (__fastcall *ILTServer_EndMessage2)(ILTCSBase* pServer, void* notUsed, DWORD hMessage, DWORD dwFlags);

void (__fastcall *IClientShell_PreLoadWorld)(void* pShell, void* notUsed, char *pWorldName);
void (__fastcall *IClientShell_Update)(void* pShell);

void (__fastcall *IServerShell_Update)(void* pShell, float timeElapsed);
void (__fastcall *IServerShell_VerifyClient)(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD &nVerifyCode);
void* (__fastcall *IServerShell_OnClientEnterWorld)(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD clientDataLen);
void (__fastcall *IServerShell_OnClientExitWorld)(void* pShell, void* notUsed, DWORD hClient);
DWORD (__fastcall *IServerShell_ServerAppMessageFn)(void* pShell, void* notUsed, char *pMsg, int nLen);
void (__fastcall *IServerShell_PostStartWorld)(void* pShell);

/*bool SolidSurfaceList_HandleEqualFn(const SolidSurface* pItem)
{
	return (pItem->m_hColor == g_hColorSearch);
}

SolidSurface* SolidSurfaceList_FindItem(DWORD hColor)
{
	g_hColorSearch = hColor;
	SolidSurfaceList::iterator iter = std::find_if(g_SolidSurfaceList.begin(), g_SolidSurfaceList.end(), SolidSurfaceList_HandleEqualFn);
	
	if (iter != g_SolidSurfaceList.end())
		return (*iter);
	
	return NULL;
}*/

bool FontStringList_HandleEqualFn_ByString(const FontString* pItem)
{
	return (strcmp(pItem->m_szString, g_szStringSearch) == 0);
}

bool FontStringList_HandleEqualFn_BySurface(const FontString* pItem)
{
	return (pItem->m_hSurface == g_hSurfaceSearch);
}

bool FontList_HandleEqualFn(const Font* pItem)
{
	return pItem->m_hFont == g_hFontSearch;
}

Font* FontList_FindItem(DWORD hFont)
{
	g_hFontSearch = hFont;
	FontList::iterator iter = std::find_if(g_FontList.begin(), g_FontList.end(), FontList_HandleEqualFn);
	
	if (iter != g_FontList.end())
		return (*iter);

	return NULL;
}

FontString* FontStringList_FindItem(FontStringList* pList, char* szString)
{
	g_szStringSearch = szString;
	FontStringList::iterator iter = std::find_if(pList->begin(), pList->end(), FontStringList_HandleEqualFn_ByString);
	
	if (iter != pList->end())
		return (*iter);
	
	return NULL;
}

FontString* FontStringList_FindItem(FontStringList* pList, DWORD hSurface)
{
	g_hSurfaceSearch = hSurface;
	FontStringList::iterator iter = std::find_if(pList->begin(), pList->end(), FontStringList_HandleEqualFn_BySurface);
	
	if (iter != pList->end())
		return (*iter);
	
	return NULL;
}

FontString* FontStringList_FindItem(DWORD hSurface)
{
	FontList::iterator iter = g_FontList.begin();
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		Font* pItem = *iter;
		
		FontString* pString = FontStringList_FindItem(&pItem->m_StringList, hSurface);
		if (pString)
			return pString;
			
		iter++;
	}

	return NULL;
}

void FontList_Update()
{
	FontList::iterator iter = g_FontList.begin();
	float fTime = ILTCSBase_GetTime(g_pLTClient);
	
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		BOOL bDelete = FALSE;
		Font* pItem = *iter;
		
		if (pItem->m_StringList.size())
		{
			FontStringList::iterator string_iter = pItem->m_StringList.begin();
			while (true)
			{			
				if (string_iter == pItem->m_StringList.end()) 
					break;
				
				FontString* pString = *string_iter;

				if (fTime - pString->m_fTime > FONT_STRING_CLEANUP_TIME)
				{
					//g_pLTClient->SetSurfaceUserData(pString->m_hSurface, NULL);

					//if (pString->m_hSurface)
						g_pLTClient->DeleteSurface(pString->m_hSurface);

					delete pString;
					string_iter = pItem->m_StringList.erase(string_iter);
				}
				else
				{
					string_iter++;
				}			
			}
		}
		else
		{
			if (fTime - pItem->m_fTime > FONT_LIST_CLEANUP_TIME)
			{
				delete pItem;
				iter = g_FontList.erase(iter);
				bDelete = TRUE;
			}
		}

		if (!bDelete)
			iter++;
	}
}

void FontList_Clear(BOOL bDeleteSurfaces)
{
	if (bDeleteSurfaces)
	{
		if (g_hWhitePixelSurface)
		{
			g_pLTClient->DeleteSurface(g_hWhitePixelSurface);
			g_hWhitePixelSurface = NULL;
		}
		
		for (int i = 0; i < INTRODUCTION_LINES ; i++)
		{
			if (g_hIntroductionSurface[i])
			{
				g_pLTClient->DeleteSurface(g_hIntroductionSurface[i]);
				g_hIntroductionSurface[i] = NULL;
			}
		}
		
		if (g_hFont15Surface)
		{
			g_pLTClient->DeleteSurface(g_hFont15Surface);
			g_hFont15Surface = NULL;
		}	
		
		Console_Term();
	}
	
	//int a = g_FontList.size();
	LogPrintF("Font list size on destruction = %d", g_FontList.size());
	FontList::iterator iter = g_FontList.begin();
	
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		Font* pItem = *iter;

		LogPrintF("Font's string list (%08X) size on destruction = %d", &pItem->m_StringList, pItem->m_StringList.size());
		if (pItem->m_StringList.size())
		{
			FontStringList::iterator string_iter = pItem->m_StringList.begin();
			while (true)
			{			
				if (string_iter == pItem->m_StringList.end()) 
					break;
				
				FontString* pString = *string_iter;
				//logf(0, LTRACE, "[%08X] %s (%d)", pString->m_hSurface, pString->m_szString, pString->m_pLines ? pString->m_pLines->size() : -1);
				LogPrintF("[%08X] %s", pString->m_hSurface, pString->m_szString);

				if (bDeleteSurfaces /*&& pString->m_hSurface*/)
				{
					//g_pLTClient->SetSurfaceUserData(pString->m_hSurface, NULL);
					g_pLTClient->DeleteSurface(pString->m_hSurface);
				}

				delete pString;
				string_iter++;
			}
		}
		
		delete pItem;
		iter++;
	}
	
	g_FontList.clear();
}

BOOL RegisterRawMouseDevice()
{
	RAWINPUTDEVICE Rid;
	
	Rid.usUsagePage = 0x01; 
	Rid.usUsage = 0x02; 
	Rid.dwFlags = 0; //RIDEV_NOLEGACY; 
	Rid.hwndTarget = 0;
	
	return RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
}

void ProcessRawMouseInput(LPARAM lParam, LONG& lLastX, LONG& lLastY)
{
	UINT dwSize;
				
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb) 
	{
		UINT dwRetSize = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		if (dwRetSize == dwSize)
		{
			RAWINPUT* raw = (RAWINPUT*)lpb;	

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				lLastX += raw->data.mouse.lLastX;
				lLastY += raw->data.mouse.lLastY;
			}
		}
	} 
}

void EngineHack_WriteData(LPVOID lpAddr, BYTE* pNew, BYTE* pOld, DWORD dwSize)
{
	DWORD dwOldProtect, dwTemp;
	void* pContent = (DWORD*)lpAddr;
	
	if (!VirtualProtect(lpAddr, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		LogPrintF("VirtualProtect fails on address = %08X, error = %08X", lpAddr, GetLastError());
	
	memcpy(pOld, pContent, dwSize);
	memcpy(pContent, pNew, dwSize);
	
	VirtualProtect(lpAddr, dwSize, dwOldProtect, &dwTemp);
}

void EngineHack_WriteFunction(LPVOID lpAddr, DWORD dwNew, DWORD& dwOld)
{
	DWORD dwOldProtect, dwTemp;
	DWORD* dwContent = (DWORD*)lpAddr;
	
	if (!VirtualProtect(lpAddr, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		LogPrintF("VirtualProtect fails on address = %08X, error = %08X", lpAddr, GetLastError());

	dwOld = dwContent[0];
	dwContent[0] = dwNew;
	VirtualProtect(lpAddr, 4, dwOldProtect, &dwTemp);
}

void EngineHack_WriteCall(LPVOID lpAddr, DWORD dwNew, BOOL bStructCall)
{
	DWORD dwOldProtect, dwTemp;
	BYTE* pContent = (BYTE*)lpAddr;
	DWORD* pCodeContent = (DWORD*)(pContent + 1);
	DWORD dwCallCode = dwNew - (DWORD)lpAddr - 5;
	
	if (bStructCall)
	{
		if (!VirtualProtect(lpAddr, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			LogPrintF("VirtualProtect fails on address = %08X, error = %08X", lpAddr, GetLastError());
		
		pContent[0] = 0xE8; 
		pCodeContent[0] = dwCallCode;
		pContent[5] = 0x90;
		
		VirtualProtect(lpAddr, 6, dwOldProtect, &dwTemp);
	}
	else
	{
		if (!VirtualProtect(lpAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			LogPrintF("VirtualProtect fails on address = %08X, error = %08X", lpAddr, GetLastError());
		
		pContent[0] = 0xE8; 
		pCodeContent[0] = dwCallCode;
		
		VirtualProtect(lpAddr, 5, dwOldProtect, &dwTemp);
	}
}

void EngineHack_AllowWrite(LPVOID lpAddr, DWORD dwSize)
{
	DWORD dwOldProtect;	

	if (!VirtualProtect(lpAddr, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		LogPrintF("VirtualProtect fails on address = %08X, error = %08X", lpAddr, GetLastError());
}

void EngineHack_WriteJump(LPVOID lpAddr, DWORD dwNew)
{
	DWORD dwOldProtect, dwTemp;
	BYTE* pContent = (BYTE*)lpAddr;
	DWORD* pCodeContent = (DWORD*)(pContent + 1);
	DWORD dwCallCode = dwNew - (DWORD)lpAddr - 5;
	
	
	if (!VirtualProtect(lpAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		LogPrintF("VirtualProtect fails on address = %08X, error = %08X", lpAddr, GetLastError());
	
	pContent[0] = 0xE9; 
	pCodeContent[0] = dwCallCode;
	
	VirtualProtect(lpAddr, 5, dwOldProtect, &dwTemp);
}

void CreateFont15Surface()
{
	g_hFont15Surface = g_pLTClient->CreateSurface(FONT15_WIDTH * FONT15_LENGTH, FONT15_HEIGHT);

	for (int i = 0; i < FONT15_LENGTH ; i++)
	{
		for (int j = 0; j < (FONT15_WIDTH * FONT15_HEIGHT) ; j++)
		{
			if (g_anFont[i][j])
			{			
				DWORD y = j / FONT15_WIDTH;
				DWORD x = (i * FONT15_WIDTH) + (j % FONT15_WIDTH);
				g_pLTClient->SetPixel(g_hFont15Surface, x, y, 0x00FFFFFF);
			}
		}
	}

	g_pLTClient->OptimizeSurface(g_hFont15Surface, 0);
}

void ReoptimizeFont15Surface()
{
	g_pLTClient->OptimizeSurface(g_hFont15Surface, 0);
}

void DrawFont15String(char* szString, int nX, int nY, int nSpacing, int nScale, DWORD dwColor, DWORD hDestSurfOverride/* = NULL*/)
{	
	if (!g_hFont15Surface)
		CreateFont15Surface();

	DWORD hDestSurf = !hDestSurfOverride ? g_pLTClient->GetScreenSurface() : hDestSurfOverride;
	int nLength = strlen(szString);

	LTRect rcSrc;
	rcSrc.top = 0;
	rcSrc.bottom = FONT15_HEIGHT;

	g_pLTClient->SetOptimized2DColor(dwColor);

	if (nScale == 1)
	{
		for (int i = 0; i < nLength ; i++)
		{
			rcSrc.left = (Font15_GetCharIndex(szString[i]) * FONT15_WIDTH);
			rcSrc.right = rcSrc.left + FONT15_WIDTH;
			
			g_pLTClient->DrawSurfaceToSurfaceTransparent(hDestSurf, g_hFont15Surface, &rcSrc, nX + (i * (FONT15_WIDTH + nSpacing)), nY, 0);
		}
	}
	else
	{
		LTRect rcDest;
		rcDest.top = nY;
		rcDest.bottom = rcDest.top + (FONT15_HEIGHT * nScale);
		
		for (int i = 0; i < nLength ; i++)
		{	
			rcDest.left = nX + (i * (FONT15_WIDTH + nSpacing) * nScale);
			rcDest.right = rcDest.left + (FONT15_WIDTH * nScale);
			
			rcSrc.left = (Font15_GetCharIndex(szString[i]) * FONT15_WIDTH);
			rcSrc.right = rcSrc.left + FONT15_WIDTH;
			
			g_pLTClient->ScaleSurfaceToSurfaceTransparent(hDestSurf, g_hFont15Surface, &rcDest, &rcSrc, 0);
		}
	}

	g_pLTClient->SetOptimized2DColor(0x00FFFFFF);
}

void CreateIntroductionSurface()
{
	char szTitle[64];
	char szProfile[64];
	char szDescription[256];
	char szPostprocess[64];

	char* szIntro[INTRODUCTION_LINES];
	sprintf(szTitle, APP_NAME, APP_VERSION);
	szIntro[0] = szTitle;
	sprintf(szProfile, "Active profile = %s", g_szProfile);
	szIntro[1] = szProfile;
	sprintf(szDescription, "Profile description = %s", g_ProfileOptions[PO_DESCRIPTION].szValue);
	szIntro[2] = szDescription;
	sprintf(szPostprocess, "Postprocessing enabled = %s", g_ProfileOptions[PO_POSTPROCESS_ENABLED].bValue ? "TRUE" : "FALSE");
	szIntro[3] = szPostprocess;
	szIntro[4] = "Page Up - borderless window toggle";
	szIntro[5] = "Page Down - FPS counter mode";
	szIntro[6] = !g_bCVarProfileIgnored ? "WARNING! CVar profile is ignored (D3D7Fix version differs)" : NULL;

#ifdef _DEBUG
	DWORD dwColorMap[INTRODUCTION_LINES] = { 0x006666FF, 0x00FFFF00, 0x00FFFF00, 0x00FF8800, 0x00FFFFFF, 0x00FFFFFF };
#else	
	DWORD dwColorMap[INTRODUCTION_LINES] = { 0x0000FF00, 0x00FFFF00, 0x00FFFF00, 0x00FF8800, 0x00FFFFFF, 0x00FFFFFF };
#endif

	if (g_ProfileOptions[PO_CLEAN_MODE].bValue) 
		dwColorMap[1] = 0x00FF0000;

	DWORD hFont = g_pLTClient->CreateFont("Terminal", INTRODUCTION_FONT_WIDTH, INTRODUCTION_FONT_HEIGHT, FALSE, FALSE, FALSE);
	
	for (int i = 0; i < INTRODUCTION_LINES; i++)
	{
		if (szIntro[i])
		{
			DWORD hString = ILTCSBase_CreateString(szIntro[i]);
			g_hIntroductionSurface[i] = g_pLTClient->CreateSurfaceFromString(hFont, hString, dwColorMap[i], 0, 0, 0);
			ILTCSBase_FreeString(hString);
		}
	}

	g_pLTClient->DeleteFont(hFont);
}

BOOL SectionItemExists(char* szSection, char* szKey)
{
	int nKeyLen = strlen(szKey);
	int nCurrStart = 0;
	char* szCurr = szSection;
	int i = 0;

	while (true)
	{
		if (szSection[i] == 0)
		{
			szCurr = szSection + nCurrStart;
			if (!strncmp(szCurr, szKey, nKeyLen) && szCurr[nKeyLen] == '=')
				return TRUE;

			if (szSection[i + 1] == 0)
				return FALSE;

			nCurrStart = i + 1;
		}

		i++;
	}
}

BOOL GetSectionString(char* szSection, char* szKey, char* szValue)
{
	int nKeyLen = strlen(szKey);
	int nCurrStart = 0;
	char* szCurr = szSection;
	int i = 0;
	
	while (true)
	{
		if (szSection[i] == 0)
		{			
			szCurr = szSection + nCurrStart;
			if (!strncmp(szCurr, szKey, nKeyLen) && szCurr[nKeyLen] == '=')
			{
				strcpy(szValue, szCurr + nKeyLen + 1);
				return TRUE;
			}

			if (szSection[i + 1] == 0)
				return FALSE;

			nCurrStart = i + 1;
		}
		
		i++;
	}
}

int GetSectionInt(char* szSection, char* szKey, int nDefault)
{
	char szValue[128];
	BOOL bRet = GetSectionString(szSection, szKey, szValue);

	return bRet ? atoi(szValue) : nDefault;
}

float GetSectionFloat(char* szSection, char* szKey, float fDefault)
{
	char szValue[128];
	BOOL bRet = GetSectionString(szSection, szKey, szValue);
	
	return bRet ? atof(szValue) : fDefault;
}

void SetCurrProfileDWord(eProfileOption eOption, DWORD dwSet)
{
	g_ProfileOptions[eOption].SetDWord(dwSet);
}

void SetCurrProfileBool(eProfileOption eOption, BOOL bSet)
{
	g_ProfileOptions[eOption].SetBool(bSet);
}

void SetCurrProfileFlag(eProfileOption eOption, BOOL bSet)
{
	g_ProfileOptions[eOption].bFlag = bSet;
}

void SetCurrProfileFloat(eProfileOption eOption, float fSet)
{
	g_ProfileOptions[eOption].SetFloat(fSet);
}

void SetCurrProfileString(eProfileOption eOption, char* szSet)
{
	g_ProfileOptions[eOption].SetString(szSet);
}

int GetCurrProfileDWord(eProfileOption eOption)
{
	return g_ProfileOptions[eOption].GetDWord();
}

BOOL GetCurrProfileBool(eProfileOption eOption)
{
	return g_ProfileOptions[eOption].GetBool();
}

BOOL GetCurrProfileFlag(eProfileOption eOption)
{
	return g_ProfileOptions[eOption].bFlag;
}

float GetCurrProfileFloat(eProfileOption eOption)
{
	return g_ProfileOptions[eOption].GetFloat();
}

char* GetCurrProfileString(eProfileOption eOption)
{
	return g_ProfileOptions[eOption].GetString();
}

char* GetCurrProfileOption(eProfileOption eOption)
{
	return g_ProfileOptions[eOption].szName; 
}

void SectionToCurrProfileBool(char* szSection, eProfileOption eOption, int nDefault)
{
	int nTemp = GetSectionInt(szSection, g_ProfileOptions[eOption].szName, nDefault);
	SetCurrProfileBool(eOption, nTemp);
}

void SectionToCurrProfileDWord(char* szSection, eProfileOption eOption, DWORD dwDefault)
{
	DWORD dwTemp = GetSectionInt(szSection, g_ProfileOptions[eOption].szName, dwDefault);
	SetCurrProfileDWord(eOption, dwTemp);
}

void SectionToCurrProfileFloat(char* szSection, eProfileOption eOption, float fDefault)
{
	float fTemp = GetSectionFloat(szSection, g_ProfileOptions[eOption].szName, fDefault);
	SetCurrProfileFloat(eOption, fTemp);
}

void SectionToCurrProfileString(char* szSection, eProfileOption eOption)
{
	GetSectionString(szSection, g_ProfileOptions[eOption].szName, g_ProfileOptions[eOption].szValue);
}

void SectionToCurrProfileString(char* szSection, eProfileOption eOption, char* szDefault)
{
	GetSectionString(szSection, g_ProfileOptions[eOption].szName, g_ProfileOptions[eOption].szValue);

	if (!g_ProfileOptions[eOption].szValue[0])
		strcpy(g_ProfileOptions[eOption].szValue, szDefault);
}

BOOL ParseCVarProfile(char* szData)
{
	ProfileOption aTempProfileOptions[PO_MAX];
	memcpy(aTempProfileOptions, g_ProfileOptions, sizeof(ProfileOption) * PO_MAX);
	
	char szBuffer[16];
	int nLength = strlen(szData);

	int i = 0;
	while (i < nLength)
	{
		szBuffer[0] = szData[i + 0];
		szBuffer[1] = szData[i + 1];
		szBuffer[2] = 0;

		DWORD dwOption = strtol(szBuffer, NULL, 16);
		i += 2;

		switch (g_ProfileOptions[dwOption].eType)
		{
			case POT_BYTE:
			{
				szBuffer[0] = szData[i + 0];
				szBuffer[1] = szData[i + 1];
				szBuffer[2] = 0;

				SetCurrProfileBool((eProfileOption)dwOption, strtol(szBuffer, NULL, 16));
				i += 2;
			}
			break;

			case POT_FLOAT:
			case POT_STRING:
			{
				szBuffer[0] = szData[i + 0];
				szBuffer[1] = szData[i + 1];
				szBuffer[2] = 0;
				
				char szVal[256];
				int nValLen = strtol(szBuffer, NULL, 16);
				szVal[nValLen] = 0;

				for (int j = 0; j < nValLen ; j++)
				{
					i += 2;

					szBuffer[0] = szData[i + 0];
					szBuffer[1] = szData[i + 1];
					szBuffer[2] = 0;
					
					szVal[j] = strtol(szBuffer, NULL, 16);
				}

				if (g_ProfileOptions[dwOption].eType == POT_FLOAT)
					SetCurrProfileFloat((eProfileOption)dwOption, atof(szVal));
				else
					SetCurrProfileString((eProfileOption)dwOption, szVal);

				i += 2;
			}
			break;

			case POT_DWORD:
			{
				szBuffer[0] = szData[i + 0];
				szBuffer[1] = szData[i + 1];	
				szBuffer[2] = szData[i + 2];
				szBuffer[3] = szData[i + 3];
				szBuffer[4] = 0;

				SetCurrProfileDWord((eProfileOption)dwOption, strtol(szBuffer, NULL, 16));
				i += 8;
			}
			break;
		}
	}

	char* szVersion = GetCurrProfileString(GEN_FIX_VERSION);
	char* szExpectedVersion = aTempProfileOptions[GEN_FIX_VERSION].GetString();
	if (strcmp(szVersion, szExpectedVersion))
	{
		LogPrintF("Version <> ExpectedVersion, %s <> %s", szVersion, szExpectedVersion);
		memcpy(g_ProfileOptions, aTempProfileOptions, sizeof(ProfileOption) * PO_MAX);
		return FALSE;
	}

	return TRUE;
}

char* ParseCacheString(char* szString, DWORD& dwType)
{
	char* szArrow = szString;
	
	while(*szArrow != '|')
		szArrow++;
	
	int nTypeLen = szArrow - szString;
	
	if (!strncmp(szString, "FT_MODEL", nTypeLen)) dwType = FT_MODEL;
	else if (!strncmp(szString, "FT_SPRITE", nTypeLen)) dwType = FT_SPRITE;
	else if (!strncmp(szString, "FT_TEXTURE", nTypeLen)) dwType = FT_TEXTURE;
	else if (!strncmp(szString, "FT_SOUND", nTypeLen)) dwType = FT_SOUND;
	else dwType = FT_ERROR;

	return szArrow + 1;
}

void LogCurrProfile()
{
	LogPrintF("Profile = %s", g_szProfile);
	
	for (int i = 0; i < PO_MAX ; i++)
	{
		switch (g_ProfileOptions[i].eType)
		{
			case POT_BYTE: LogPrintF("%s = %d", g_ProfileOptions[i].szName, g_ProfileOptions[i].bValue); break;
			case POT_FLOAT: LogPrintF("%s = %f", g_ProfileOptions[i].szName, g_ProfileOptions[i].fValue); break;
			case POT_STRING: LogPrintF("%s = %s", g_ProfileOptions[i].szName, g_ProfileOptions[i].szValue); break;
			case POT_DWORD: LogPrintF("%s = %d", g_ProfileOptions[i].szName, g_ProfileOptions[i].dwValue); break;
		}
	}

	i = 0;
	WorldList::iterator iter1 = g_TWMDetailTex_WorldList.begin();
	while (true)
	{
		if (iter1 == g_TWMDetailTex_WorldList.end())
			break;

		WorldListItem* pItem = *iter1;
		LogPrintF("TWMDetailTex_WorldList[%d] = %s", i, pItem->m_szWorldName);

		iter1++;
		i++;
	}

	i = 0;
	SDWList::iterator iter2 = g_SolidDrawingWhitelist.begin();
	while (true)
	{
		if (iter2 == g_SolidDrawingWhitelist.end())
			break;
		
		FilenameItem* pItem = *iter2;
		LogPrintF("SolidDrawingWhitelist[%d] = %s", i, pItem->m_szFilename);
		
		iter2++;
		i++;
	}
}

void LogPrintF(char *msg, ...)
{	
	SYSTEMTIME now;
	GetLocalTime(&now);
	va_list argp;
	
	fprintf(g_LogFile,"\n[%02d:%02d:%02d.%03d] ", now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
	va_start(argp, msg);
	vfprintf(g_LogFile, msg, argp);	
	va_end(argp);
	
	fflush(g_LogFile);
}

void LogPrintF_Raw(char *msg, ...)
{	
	va_list argp;

	va_start(argp, msg);
	vfprintf(g_LogFile, msg, argp);	
	va_end(argp);
	
	fflush(g_LogFile);
}

BOOL FileExists(char* szName)
{
	struct stat aBuf;   
	return (stat(szName, &aBuf) == 0); 
}

void TWMDetailTex_WorldList_Reserve(int nSize)
{
	g_TWMDetailTex_WorldList.reserve(nSize);
}

void TWMDetailTex_WorldList_Add(char* szName)
{
	g_TWMDetailTex_WorldList.push_back(new WorldListItem(szName));
}

char* g_szWorldNameSearch;
bool WorldList_HandleEqualFn(const WorldListItem* pItem)
{
	return !strcmp(pItem->m_szWorldName, g_szWorldNameSearch);
}

WorldListItem* TWMDetailTex_WorldList_Find(char* szName)
{
	char szTemp[MAX_WORLD_NAME_LEN] = { 0 };
	strcpy(szTemp, szName);
	strupr(szTemp);
	
	g_szWorldNameSearch = szTemp;
	WorldList::iterator iter = std::find_if(g_TWMDetailTex_WorldList.begin(), g_TWMDetailTex_WorldList.end(), WorldList_HandleEqualFn);

	if (iter != g_TWMDetailTex_WorldList.end())
		return (*iter);

	return NULL;
}

void TWMDetailTex_WorldList_Free()
{
	WorldList::iterator iter = g_TWMDetailTex_WorldList.begin();

	while (true)
	{
		if (iter == g_TWMDetailTex_WorldList.end())
			break;

		WorldListItem* pItem = *iter;
		delete pItem;

		iter++;
	}

	g_TWMDetailTex_WorldList.clear();
}

void TWMDetailTex_WorldList_AppendFromFile(char* szFilename)
{
	FILE* file = fopen(szFilename, "r");
	if (file == NULL)
		return;

	char szLine[MAX_WORLD_NAME_LEN] = { 0 };
	int nLen = 0;

	while (fgets(szLine, MAX_WORLD_NAME_LEN, file))
	{
		szLine[strcspn(szLine, "\r\n")] = 0;
		TWMDetailTex_WorldList_Add(szLine);
	}

	fclose(file);
}

void SolidDrawingWhitelist_Reserve(int nSize)
{
	g_SolidDrawingWhitelist.reserve(nSize);
}

void SolidDrawingWhitelist_Add(char* szFilename)
{
	g_SolidDrawingWhitelist.push_back(new FilenameItem(szFilename));
}

char* g_szSDWFilenameSearch;
bool SDWList_HandleEqualFn(const FilenameItem* pItem)
{
	return !strcmp(pItem->m_szFilename, g_szSDWFilenameSearch);
}

FilenameItem* SolidDrawingWhitelist_Find(char* szFilename)
{
	char szTemp[MAX_SDW_FILENAME_LEN] = { 0 };
	strcpy(szTemp, szFilename);
	strupr(szTemp);

	g_szSDWFilenameSearch = szTemp;
	SDWList::iterator iter = std::find_if(g_SolidDrawingWhitelist.begin(), g_SolidDrawingWhitelist.end(), SDWList_HandleEqualFn);

	if (iter != g_SolidDrawingWhitelist.end())
		return (*iter);

	return NULL;
}

void SolidDrawingWhitelist_Free()
{
	SDWList::iterator iter = g_SolidDrawingWhitelist.begin();

	while (true)
	{
		if (iter == g_SolidDrawingWhitelist.end())
			break;

		FilenameItem* pItem = *iter;
		delete pItem;

		iter++;
	}

	g_SolidDrawingWhitelist.clear();
}