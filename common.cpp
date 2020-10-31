#include "StdAfx.h"
#include "numfont.h"

FILE* g_LogFile = NULL;
char g_szProfile[64];
BOOL g_bDrawFPS = TRUE;
DWORD g_dwWidth = 1024;
DWORD g_dwHeight = 768;
BOOL g_bWindowedSet = FALSE;
DWORD g_bWindowed = 0;
LPDIRECTDRAWSURFACE7 g_ddsBackBuffer = NULL;
LPDIRECT3DDEVICE7 g_d3dDevice = NULL;
LPDIRECTDRAW7 g_ddMainDDraw = NULL;
HWND g_hWindowHandle = NULL;
BOOL g_bWindowHooked = FALSE;
CClientMgrBase* g_pClientMgr = NULL;
CServerMgrBase* g_pServerMgr = NULL;
ILTClient* g_pLTClient = NULL;
float g_fLastFontListUpdate = 0.0f;

LONG g_lRMILastX = 0;
LONG g_lRMILastY = 0;
//DWORD g_hMouseSensitivity = 0;

ProfileOption g_ProfileOptions[PO_MAX] = 
{
	ProfileOption(POT_BYTE, "DgVoodooMode"),
	ProfileOption(POT_STRING, "Misc_Description"),
	ProfileOption(POT_BYTE, "Misc_CleanMode"),
	ProfileOption(POT_BYTE, "Misc_DontShutdownRenderer"),
	ProfileOption(POT_BYTE, "Misc_ShowFPS"),
	ProfileOption(POT_DWORD, "Misc_FrameLimiterSleep"),
	ProfileOption(POT_FLOAT, "Misc_CameraFOVXScaler"),
	ProfileOption(POT_FLOAT, "Fix_MaxFPS"),
	ProfileOption(POT_BYTE, "Fix_IntelHD"),
	ProfileOption(POT_BYTE, "Fix_Radeon5700"),
	ProfileOption(POT_BYTE, "Fix_CameraFOV"),
	ProfileOption(POT_BYTE, "Fix_ViewModelFOV"),
	ProfileOption(POT_BYTE, "Fix_SolidDrawing"),
	ProfileOption(POT_BYTE, "Fix_LightLoad"),
	ProfileOption(POT_BYTE, "Fix_MiscCC"),
	ProfileOption(POT_BYTE, "Fix_RawMouseInput"),
	ProfileOption(POT_BYTE, "Fix_TWMDetailTex"),
	ProfileOption(POT_BYTE, "Fix_TimeCalibration"),
	ProfileOption(POT_BYTE, "Fix_FlipScreen"),
	ProfileOption(POT_BYTE, "Fix_DynamicLightSurfaces"),
	ProfileOption(POT_BYTE, "Fix_StaticLightSurfaces"),
	ProfileOption(POT_BYTE, "Fix_PreloadStaticLight"),
	ProfileOption(POT_FLOAT, "RMI_ScaleGlobal"),
	ProfileOption(POT_FLOAT, "RMI_ScaleY"),
};

FontList g_FontList;
DWORD g_hWhitePixelSurface = NULL;
float g_fIntroductionStartTime = 0.0f;
DWORD g_hIntroductionSurface[INTRODUCTION_LINES] = { NULL, NULL, NULL, NULL, NULL };
DWORD g_hFrameRateFontSurface[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
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

void (__fastcall *IClientShell_Update)(void* pShell);

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
		
		for (i = 0; i < 10 ; i++)
		{
			if (g_hFrameRateFontSurface[i])
			{
				g_pLTClient->DeleteSurface(g_hFrameRateFontSurface[i]);
				g_hFrameRateFontSurface[i] = NULL;
			}			
		}
	}
	
	//int a = g_FontList.size();
	logf("Font list size on destruction = %d", g_FontList.size());
	FontList::iterator iter = g_FontList.begin();
	
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		Font* pItem = *iter;

		logf("Font's string list (%08X) size on destruction = %d", &pItem->m_StringList, pItem->m_StringList.size());
		if (pItem->m_StringList.size())
		{
			FontStringList::iterator string_iter = pItem->m_StringList.begin();
			while (true)
			{			
				if (string_iter == pItem->m_StringList.end()) 
					break;
				
				FontString* pString = *string_iter;
				//logf(0, LTRACE, "[%08X] %s (%d)", pString->m_hSurface, pString->m_szString, pString->m_pLines ? pString->m_pLines->size() : -1);
				logf("[%08X] %s", pString->m_hSurface, pString->m_szString);

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

void EngineHack_WriteData(HANDLE hProcess, LPVOID lpAddr, BYTE* pNew, BYTE* pOld, DWORD dwSize)
{
	DWORD dwOldProtect, dwTemp;
	void* pContent = (DWORD*)lpAddr;
	
	VirtualProtectEx(hProcess, lpAddr, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
	memcpy(pOld, pContent, dwSize);
	memcpy(pContent, pNew, dwSize);
	
	VirtualProtectEx(hProcess, lpAddr, dwSize, dwOldProtect, &dwTemp);
}

void EngineHack_WriteFunction(HANDLE hProcess, LPVOID lpAddr, DWORD dwNew, DWORD& dwOld)
{
	DWORD dwOldProtect, dwTemp;
	DWORD* dwContent = (DWORD*)lpAddr;
	
	VirtualProtectEx(hProcess, lpAddr, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);	
	dwOld = dwContent[0];
	dwContent[0] = dwNew;
	VirtualProtectEx(hProcess, lpAddr, 4, dwOldProtect, &dwTemp);
}

void EngineHack_WriteCall(HANDLE hProcess, LPVOID lpAddr, DWORD dwNew, BOOL bStructCall)
{
	DWORD dwOldProtect, dwTemp;
	BYTE* pContent = (BYTE*)lpAddr;
	DWORD* pCodeContent = (DWORD*)(pContent + 1);
	DWORD dwCallCode = dwNew - (DWORD)lpAddr - 5;
	
	if (bStructCall)
	{
		VirtualProtectEx(hProcess, lpAddr, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		
		pContent[0] = 0xE8; 
		pCodeContent[0] = dwCallCode;
		pContent[5] = 0x90;
		
		VirtualProtectEx(hProcess, lpAddr, 6, dwOldProtect, &dwTemp);
	}
	else
	{
		VirtualProtectEx(hProcess, lpAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		
		pContent[0] = 0xE8; 
		pCodeContent[0] = dwCallCode;
		
		VirtualProtectEx(hProcess, lpAddr, 5, dwOldProtect, &dwTemp);
	}
}

void CreateFrameRateFontSurfaces()
{
	for (int i = 0; i < 10 ; i++)
	{
		g_hFrameRateFontSurface[i] = g_pLTClient->CreateSurface(FRAME_RATE_FONT_WIDTH, FRAME_RATE_FONT_HEIGHT);
		for (int j = 0; j < (FRAME_RATE_FONT_WIDTH * FRAME_RATE_FONT_HEIGHT) ; j++)
		{
			if (g_nNums[i][j])
			{			
				DWORD y = j / FRAME_RATE_FONT_WIDTH;
				DWORD x = j % FRAME_RATE_FONT_WIDTH;
				g_pLTClient->SetPixel(g_hFrameRateFontSurface[i], x, y, 0x00FFFFFF);
			}
		}
	}	
}

void CreateIntroductionSurface()
{
	char szTitle[64];
	char szProfile[64];
	char szDescription[256];

	char* szIntro[INTRODUCTION_LINES];
	sprintf(szTitle, APP_NAME, APP_VERSION);
	szIntro[0] = szTitle;
	sprintf(szProfile, "Active profile = %s", g_szProfile);
	szIntro[1] = szProfile;
	sprintf(szDescription, "Profile description = %s", g_ProfileOptions[PO_DESCRIPTION].szValue);
	szIntro[2] = szDescription;
	szIntro[3] = "Page Up - borderless window toggle";
	szIntro[4] = "Page Down - draw FPS counter toggle";
	DWORD dwColorMap[INTRODUCTION_LINES] = { 0x0000FF00, 0x00FFFF00, 0x00FFFF00, 0x00FFFFFF, 0x00FFFFFF };

	if (GetCurrProfileBool(PO_CLEAN_MODE)) 
		dwColorMap[1] = 0x00FF0000;

	DWORD hFont = g_pLTClient->CreateFont("Terminal", INTRODUCTION_FONT_WIDTH, INTRODUCTION_FONT_HEIGHT, FALSE, FALSE, FALSE);
	
	for (int i = 0; i < INTRODUCTION_LINES; i++)
	{
		DWORD hString = ILTCSBase_CreateString(szIntro[i]);
		g_hIntroductionSurface[i] = g_pLTClient->CreateSurfaceFromString(hFont, hString, dwColorMap[i], 0, 0, 0);
		ILTCSBase_FreeString(hString);
	}

	g_pLTClient->DeleteFont(hFont);
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

void ParseCVarProfile(char* szData)
{
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

	// if (n > 0xffffff) n |= 0xff000000;
}

void LogCurrProfile()
{
	logf("Profile = %s", g_szProfile);
	
	for (int i = 0; i < PO_MAX ; i++)
	{
		switch (g_ProfileOptions[i].eType)
		{
			case POT_BYTE: logf("%s = %d", g_ProfileOptions[i].szName, g_ProfileOptions[i].bValue); break;
			case POT_FLOAT: logf("%s = %f", g_ProfileOptions[i].szName, g_ProfileOptions[i].fValue); break;
			case POT_STRING: logf("%s = %s", g_ProfileOptions[i].szName, g_ProfileOptions[i].szValue); break;
			case POT_DWORD: logf("%s = %d", g_ProfileOptions[i].szName, g_ProfileOptions[i].dwValue); break;
		}
	}
}

void logf(char *msg, ...)
{	
	va_list argp;
	
	va_start(argp, msg);
	vfprintf(g_LogFile, msg, argp);
	va_end(argp);
	
	fprintf(g_LogFile,"\n");
	
	fflush(g_LogFile);
}