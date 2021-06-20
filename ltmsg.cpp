#include "StdAfx.h"

typedef HRESULT (WINAPI* DirectDrawCreate_Type)(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
typedef HRESULT (WINAPI* DirectDrawCreateEx_Type)( GUID FAR *, LPVOID *, REFIID,IUnknown FAR *);
HMODULE WINAPI MyLoadLibraryA(LPCSTR lpFileName);

void HookEngineStuff1();

#ifndef PRIMAL_HUNT_BUILD

void HookDSStuff1();
void ApplyTimeCalibrationDS_Fix();
void ApplyExtraCacheListDS_Fix();

#endif

HRESULT __stdcall FakeD3DDevice_Load(LPDIRECT3DDEVICE7 lpDevice, LPDIRECTDRAWSURFACE7 lpDestTex,LPPOINT lpDestPoint,LPDIRECTDRAWSURFACE7 lpSrcTex,LPRECT lprcSrcRect,DWORD dwFlags)
{
	return lpDestTex->Blt(NULL, lpSrcTex, NULL, DDBLT_WAIT, 0); // FakeIDDrawSurface7LM_Blt(lpDestTex, NULL, lpSrcTex, NULL, DDBLT_WAIT, 0);
}

void GetD3D7FixVersion(char* szBuffer, BOOL bFullInfo)
{
	if (bFullInfo)
		sprintf(szBuffer, APP_NAME, APP_VERSION);
	else
		sprintf(szBuffer, "%g", APP_VERSION);
}

#ifndef PRIMAL_HUNT_BUILD

void ReadConfigDS(char* szFilename)
{
	char szSection[2048];
	strcpy(g_szProfile, PROFILE_DEDICATED_SERVER);
	DWORD dwSectionSize = GetPrivateProfileSection(PROFILE_DEDICATED_SERVER, szSection, 2048, szFilename);

	SectionToCurrProfileBool(szSection, PO_TIME_CALIBRATION, FALSE);
	SectionToCurrProfileFloat(szSection, PO_SERVER_FPS, 0.0f);
	SectionToCurrProfileBool(szSection, EXT_BAN_MANAGER, FALSE);
	SectionToCurrProfileFloat(szSection, EXT_MOTD_TIMER, 0.0f);
	SectionToCurrProfileString(szSection, EXT_MOTD_STRING);

	if (GetCurrProfileBool(EXT_BAN_MANAGER))
	{
		dwSectionSize = GetPrivateProfileSection(BAN_MGR_SECTION, szSection, 2048, szFilename);
		
		g_dwClientDataLen = GetSectionInt(szSection, BAN_MGR_CLIENT_DATA_LEN, 0);
		g_dwNameOffset = GetSectionInt(szSection, BAN_MGR_NAME_OFFSET, 0);
		g_dwIdOffset = GetSectionInt(szSection, BAN_MGR_ID_OFFSET, 0);
		
		char szBanKey[64];
		char szBanValue[64];
		int i = 0;
		
		while (true)
		{
			sprintf(szBanKey, BAN_MGR_BAN, i);

			if (!GetSectionString(szSection, szBanKey, szBanValue)) 
				break;

			BanList_Add(szBanValue);
			i++;
		}
	}
	
	LogCurrProfile();
}

#endif

void ReadConfig(char* szFilename, char* szProfile)
{
	char szSection[2048];
	DWORD dwSectionSize = GetPrivateProfileSection(PROFILE_GLOBAL, szSection, 2048, szFilename);
	SectionToCurrProfileBool(szSection, PO_DGVOODOO_MODE, FALSE);
	SectionToCurrProfileFloat(szSection, PO_INTRODUCTION_TIME, 30.0f);
	SectionToCurrProfileString(szSection, PO_DEFAULT_PROFILE);

	if (!szProfile)
		szProfile = GetCurrProfileString(PO_DEFAULT_PROFILE);
	
	dwSectionSize = GetPrivateProfileSection(szProfile, szSection, 2048, szFilename);
	if (!dwSectionSize)
	{
		szProfile = PROFILE_CLEAN;
		dwSectionSize = GetPrivateProfileSection(szProfile, szSection, 2048, szFilename);
	}
	strcpy(g_szProfile, szProfile);	

	SectionToCurrProfileString(szSection, PO_DESCRIPTION);
	SectionToCurrProfileBool(szSection, PO_CLEAN_MODE, FALSE);

	if (GetCurrProfileBool(PO_CLEAN_MODE)) return;

	SectionToCurrProfileBool(szSection, PO_DONT_SHUTDOWN_RENDERER, FALSE);
	SectionToCurrProfileBool(szSection, PO_SHOW_FPS, FALSE);
	SectionToCurrProfileDWord(szSection, PO_FRAME_LIMITER_SLEEP, 0);
	SectionToCurrProfileBool(szSection, PO_ENABLE_CONSOLE, FALSE);
	SectionToCurrProfileBool(szSection, PO_NO_ENVMAP_CONSOLE_PRINT, FALSE);
	//SectionToCurrProfileFloat(szSection, PO_CAMERA_FOV_SCALER, 1.0f);
	
	float fFOVXScaler = GetSectionFloat(szSection, GetCurrProfileOption(PO_CAMERA_FOV_SCALER), 1.0f);

	if (fFOVXScaler > 1.2f) fFOVXScaler = 1.2f;
	if (fFOVXScaler < 0.8f)	fFOVXScaler = 0.8f;

	SetCurrProfileFloat(PO_CAMERA_FOV_SCALER, fFOVXScaler);

	SectionToCurrProfileFloat(szSection, PO_MAX_FPS, 0);	
	SectionToCurrProfileBool(szSection, PO_INTEL_HD, FALSE);
	SectionToCurrProfileBool(szSection, PO_RADEON_5700, FALSE);
	SectionToCurrProfileBool(szSection, PO_CAMERA_FOV, FALSE);
	SetCurrProfileFlag(PO_CAMERA_FOV, GetCurrProfileBool(PO_CAMERA_FOV));

	SectionToCurrProfileBool(szSection, PO_VIEW_MODEL_FOV, FALSE);
	SectionToCurrProfileBool(szSection, PO_SOLID_DRAWING, FALSE);
	SectionToCurrProfileBool(szSection, PO_LIGHT_LOAD, FALSE);
	SectionToCurrProfileBool(szSection, PO_TWM_DETAIL_TEX, FALSE);
	SectionToCurrProfileBool(szSection, PO_TIME_CALIBRATION, FALSE);
	SectionToCurrProfileBool(szSection, PO_FLIP_SCREEN, FALSE);
	SectionToCurrProfileBool(szSection, PO_PRELOAD_STATIC_LIGHT, FALSE);
	SectionToCurrProfileBool(szSection, PO_STATIC_LIGHT_SURFACES, FALSE);
	SectionToCurrProfileBool(szSection, PO_DYNAMIC_LIGHT_SURFACES, FALSE);
	SectionToCurrProfileBool(szSection, PO_FULLSCREEN_OPTIMIZE, FALSE);
	SectionToCurrProfileBool(szSection, PO_NOVSYNC, FALSE);	
	SectionToCurrProfileDWord(szSection, PO_UPDATE_OBJECT_LTO, 0);	
	SectionToCurrProfileBool(szSection, PO_MISC_CC, FALSE);
	SectionToCurrProfileBool(szSection, PO_RAW_MOUSE_INPUT, FALSE);
	SectionToCurrProfileBool(szSection, PO_FAST_CRC_CHECK, FALSE);
	SetCurrProfileFlag(PO_RAW_MOUSE_INPUT, GetCurrProfileBool(PO_RAW_MOUSE_INPUT));

	SectionToCurrProfileFloat(szSection, PO_RMI_SCALE_GLOBAL, 0.001f);
	SectionToCurrProfileFloat(szSection, PO_RMI_SCALE_Y, 1.1f);

	SectionToCurrProfileBool(szSection, PO_POSTPROCESS_ENABLED, FALSE);
	SectionToCurrProfileDWord(szSection, PO_POSTPROCESS_INTENSITY, 32);
	SectionToCurrProfileDWord(szSection, PO_POSTPROCESS_INTENSITY_MENU, 16);
	SectionToCurrProfileDWord(szSection, PO_POSTPROCESS_INTENSITY_VM, 8);

	if (GetCurrProfileBool(PO_DGVOODOO_MODE))
	{
		SetCurrProfileBool(PO_INTEL_HD, FALSE);
		SetCurrProfileBool(PO_RADEON_5700, FALSE);
		SetCurrProfileBool(PO_LIGHT_LOAD, FALSE);
		SetCurrProfileBool(PO_TWM_DETAIL_TEX, FALSE);
		SetCurrProfileBool(PO_FULLSCREEN_OPTIMIZE, FALSE);
		SetCurrProfileBool(PO_STATIC_LIGHT_SURFACES, FALSE);
	}

	char* szProfileEx = NULL;
	DWORD hProfileEx = g_pLTClient->GetConsoleVar(CVAR_PROFILE_EX);
	if (hProfileEx)
		szProfileEx = ILTCSBase_GetVarValueString(hProfileEx);
	
	if (szProfileEx)
		ParseCVarProfile(szProfileEx);

	LogCurrProfile();
}

void ClearMultiLinesHolder();

char g_szParentExeFilename[MAX_PATH + 1];
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			GetModuleFileName(NULL, g_szParentExeFilename, MAX_PATH);

			if (strstr(g_szParentExeFilename, "lithtech"))
			{
				g_LogFile = fopen(LTMSG_LOG, "w");
				timeBeginPeriod(1);
				HookEngineStuff1();
			}
#ifndef PRIMAL_HUNT_BUILD
			else if (strstr(g_szParentExeFilename, "Server"))
			{
				g_LogFile = fopen(LTMSG_LOG, "w");
				timeBeginPeriod(1);
				HookDSStuff1();
			}
#endif
		}
		break;

		case DLL_PROCESS_DETACH:
		{
			if (strstr(g_szParentExeFilename, "lithtech"))
			{
				timeEndPeriod(1);
				ClearMultiLinesHolder();
				FontList_Clear(FALSE);
				fclose( g_LogFile );
			}
#ifndef PRIMAL_HUNT_BUILD
			else if (strstr(g_szParentExeFilename, "Server"))
			{
				BanList_Free();
				timeEndPeriod(1);
				fclose( g_LogFile );
			}
#endif
		}
		break;
	}
	return TRUE;
}

void ApplyIntelHD_RHW_Fix()
{
	DWORD dwDLLAddress = (DWORD)GetModuleHandle(D3D_REN);
	logf("Applying IntelHD RHW fix");
	
	float fIntelHDFix = 0.5f;
	TLVertex* pVert = (TLVertex*)(dwDLLAddress + ADDR_D3D_LIGHTADD_POLY_RHW); // 0x58500
	pVert[0].rhw = fIntelHDFix;
	pVert[1].rhw = fIntelHDFix;
	pVert[2].rhw = fIntelHDFix;
	pVert[3].rhw = fIntelHDFix;
	
	pVert = (TLVertex*)(dwDLLAddress + ADDR_D3D_LIGHTSCALE_POLY_RHW); // 0x58668
	pVert[0].rhw = fIntelHDFix;
	pVert[1].rhw = fIntelHDFix;
	pVert[2].rhw = fIntelHDFix;
	pVert[3].rhw = fIntelHDFix;
}

float g_fTotalSensFull = 0.5f;
void (*OldGetAxisOffsets)(float *offsets);
void MyGetAxisOffsets(float *offsets)
{
	if (GetCurrProfileFlag(PO_RAW_MOUSE_INPUT))
	{
		/*float fSens = 1.0f;
		
		if (g_hMouseSensitivity)
			fSens = ILTCSBase_GetVarValueFloat(g_hMouseSensitivity);

		float fTotalSensFull = (0.166f + (fSens / 2.75f));*/
		
		float fGlobalScale = GetCurrProfileFloat(PO_RMI_SCALE_GLOBAL);

		offsets[0] = (float)g_lRMILastX * fGlobalScale * g_fTotalSensFull;
		offsets[1] = (float)g_lRMILastY * fGlobalScale * (g_fTotalSensFull * GetCurrProfileFloat(PO_RMI_SCALE_Y));
	}
	else
	{
		OldGetAxisOffsets(offsets);
	}
}

void (*OldRunConsoleString)(char *pString);
void MyRunConsoleString(char *pString)
{
	if (GetCurrProfileFlag(PO_RAW_MOUSE_INPUT) && (strstr(pString, "scale") == pString) && strstr(pString, "X"))
	{
		int nLength = strlen(pString);
		char* szArrow = pString + nLength - 1;

		while (*szArrow != ' ')
			szArrow--;
		
		szArrow++;

		g_fTotalSensFull = atof(szArrow) / 0.001f;

		//logf(0, LTRACE, "MyRunConsoleString - %s [%f]", pString, g_fTotalSensFull);
	}

	OldRunConsoleString(pString);
}

void ApplyRawMouseInput_Fix()
{
	logf("Registering raw mouse input = %d", RegisterRawMouseDevice());

	OldGetAxisOffsets = g_pLTClient->GetAxisOffsets;
	g_pLTClient->GetAxisOffsets = MyGetAxisOffsets;
	OldRunConsoleString = g_pLTClient->RunConsoleString;
	g_pLTClient->RunConsoleString = MyRunConsoleString;
}

float IncreaseHorFOV(float fFOVX, float fAspectRatio)
{
	float tempVradian = 2.0f * atanf(tanf(fFOVX / 2.0f) * 0.75f);
	return (2.0f * atanf(tanf(tempVradian / 2.0f) * fAspectRatio));
}

void (*OldSetCameraFOV)(DWORD hObj, float fovX, float fovY);
void MySetCameraFOV(DWORD hObj, float fovX, float fovY)
{
	fovX = IncreaseHorFOV(fovX, (float)(g_dwWidth * GetCurrProfileFloat(PO_CAMERA_FOV_SCALER)) / (float)g_dwHeight);
	OldSetCameraFOV(hObj, fovX, fovY);	
}

void ApplyCameraFOV_Fix()
{	
	logf("Applying camera FOV fix");

	OldSetCameraFOV = g_pLTClient->SetCameraFOV;
	g_pLTClient->SetCameraFOV = MySetCameraFOV;
}

DWORD (*OldDeleteSurface)(DWORD hSurface);
DWORD (*OldScaleSurfaceToSurfaceSolidColor)(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect, DWORD hTransColor, DWORD hFillColor);
DWORD (*OldDrawSurfaceSolidColor)(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY, DWORD hTransColor, DWORD hFillColor);
DWORD (*OldFillRect)(DWORD hDest, LTRect *pRect, DWORD hColor);
void (*OldDrawStringToSurface)(DWORD hDest, DWORD hFont, DWORD hString, LTRect* pRect, DWORD hForeColor, DWORD hBackColor);

DWORD MyDeleteSurface(DWORD hSurface)
{
	//if (g_pLTClient->GetSurfaceUserData(hSurface))
	//	return 0;

	return OldDeleteSurface(hSurface);
}

FontString* g_pPrevFontString = NULL;
FSLines g_MultiLines;
FSLines::iterator g_MultiLinesIter = NULL;

void ClearMultiLinesHolder()
{
	logf("Multi-lines holder size on destruction = %d (iter = %08X)", g_MultiLines.size(), g_MultiLinesIter);
	g_MultiLines.clear();
}

DWORD MyDrawSurfaceSolidColor(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY, DWORD hTransColor, DWORD hFillColor)
{	
	/*CisSurface* pCisSurface = NULL;
	RSurface* pSurface = NULL;
	if (hSrc)
	{
		pCisSurface = (CisSurface*)hSrc;
		pSurface = (RSurface*)pCisSurface->m_hBuffer;
	}*/

	if (hSrc)
	{
		FontString* pString = (FontString*)g_pLTClient->GetSurfaceUserData(hSrc);
		//g_pLTClient->SetSurfaceUserData(hSrc, NULL);
		if (pString)
		{
			hSrc = pString->m_hSurface;
			
			g_pLTClient->SetOptimized2DColor(hFillColor); // OldDrawSurfaceSolidColor(hDest, hSrc, pSrcRect, destX, destY, hTransColor, hFillColor);
			//DWORD dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, hSrc, pSrcRect, destX, destY, hTransColor);	

			DWORD dwRet;
			if (g_MultiLines.size())
			{
				//int nSize = g_MultiLines.size();
				if (!g_MultiLinesIter)
				{
					dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, hSrc, pSrcRect, destX, destY, hTransColor);	
					g_MultiLinesIter = g_MultiLines.begin();
				}
				else
				{
					FontString* pMultiString = *g_MultiLinesIter;
					dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, pMultiString->m_hSurface, pSrcRect, destX, destY, hTransColor);
					g_MultiLinesIter++;
				}
				
				if (g_MultiLinesIter == g_MultiLines.end())
				{
					g_MultiLinesIter = NULL;
					g_MultiLines.clear();
				}
			}
			else
			{
				dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, hSrc, pSrcRect, destX, destY, hTransColor);	
			}

			/*if (pString->m_pLines)
			{
				for (int i = 0; i < pString->m_pLines->size() ; i++)
				{
					FontString* pMultiString = pString->m_pLines->at(i);
					dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, pMultiString->m_hSurface, pSrcRect, destX, destY, hTransColor); // &pMultiString->m_rcSrc	
				}

				pString->m_pLines->clear();
			}*/

			g_pLTClient->SetOptimized2DColor(0xFFFFFFFF);
			
			return dwRet;
		}
	}

	return 0;
}

DWORD MyScaleSurfaceToSurfaceSolidColor(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect, DWORD hTransColor, DWORD hFillColor)
{
	if (!g_hWhitePixelSurface)
	{
		g_hWhitePixelSurface = g_pLTClient->CreateSurface(1, 1);
		//OldFillRect(g_hWhitePixelSurface, NULL, 0xFFFFFFFF);
		g_pLTClient->FillRect(g_hWhitePixelSurface, NULL, 0xFFFFFFFF);
	}
	
	g_pLTClient->SetOptimized2DColor(hFillColor);
	//DWORD dwRet = g_pLTClient->ScaleSurfaceToSurfaceTransparent(hDest, hSrc, pDestRect, pSrcRect, hTransColor);
	DWORD dwRet = g_pLTClient->ScaleSurfaceToSurfaceTransparent(hDest, g_hWhitePixelSurface, pDestRect, NULL, hTransColor);
	g_pLTClient->SetOptimized2DColor(0xFFFFFFFF);

	return dwRet;
}

/*DWORD MyFillRect(DWORD hDest, LTRect *pRect, DWORD hColor)
{
	if (hColor == 0x00FF00FF)
		return 0;

	return OldFillRect(hDest, pRect, hColor);
}*/

void MyDrawStringToSurface(DWORD hDest, DWORD hFont, DWORD hString, LTRect* pRect, DWORD hForeColor, DWORD hBackColor)
{	
	DWORD hOldDest = hDest;
	float fTime = ILTCSBase_GetTime(g_pLTClient);
	
	/*FontString* pPrevString = (FontString*)g_pLTClient->GetSurfaceUserData(hDest);
	if (pPrevString && (pRect->left || pRect->top))
	{
		pPrevString->m_fTime = fTime;
		OldDrawStringToSurface(pPrevString->m_hSurface, hFont, hString, pRect, hForeColor, hBackColor);

		return;
	}*/
	
	char* szString = ILTCSBase_GetStringData(hString);
	Font* pFont = FontList_FindItem(hFont);

	if (!pFont)
	{
		pFont = new Font(hFont, fTime);
		g_FontList.push_back(pFont);
	}

	FontString* pString = FontStringList_FindItem(&pFont->m_StringList, szString);	
	if (!pString)
	{		
		//pString = FontStringList_FindItem(&pFont->m_StringList, hDest);
		//if (pString)
		{
			DWORD dwHeight, dwWidth;
			g_pLTClient->GetSurfaceDims(hDest, &dwWidth, &dwHeight);
			hDest = g_pLTClient->CreateSurface(dwWidth, dwHeight);
			//OldFillRect(hDest, NULL, hBackColor);
			g_pLTClient->FillRect(hDest, NULL, hBackColor);
		}

		OldDrawStringToSurface(hDest, hFont, hString, pRect, hForeColor, hBackColor);
		pString = new FontString(hDest, szString, fTime);
		pFont->m_StringList.push_back(pString);
	}
	else
	{
		pString->m_fTime = fTime;
	}

	/*FontString* pPrevString = (FontString*)g_pLTClient->GetSurfaceUserData(hOldDest);
	if (pPrevString && (pRect->left || pRect->top))
	{
		if (!pPrevString->m_pLines)
		{
			pPrevString->m_pLines = new FSLines();
			pPrevString->m_pLines->reserve(FONT_STRING_LINE_RESERVE);
		}
		
		int nSize = pPrevString->m_pLines->size();

		pString->m_rcSrc = *pRect;
		pPrevString->m_pLines->push_back(pString);

		return;
	}*/

	if (pRect->left || pRect->top)
	{
		//OldDrawStringToSurface(g_pPrevFontString->m_hSurface, hFont, hString, pRect, hForeColor, hBackColor);	
		if (std::find(g_MultiLines.begin(), g_MultiLines.end(), pString) == g_MultiLines.end())
		{
			g_MultiLinesIter = NULL;
			g_MultiLines.push_back(pString);
		}

		g_pLTClient->SetSurfaceUserData(hOldDest, g_pPrevFontString);

		return;
	}

	g_pPrevFontString = pString;
	g_pLTClient->SetSurfaceUserData(hOldDest, pString);
}

void ApplySolidDrawing_Fix()
{
	logf("Applying solid drawing fix");

	OldDrawSurfaceSolidColor = g_pLTClient->DrawSurfaceSolidColor;
	g_pLTClient->DrawSurfaceSolidColor = MyDrawSurfaceSolidColor;

	OldScaleSurfaceToSurfaceSolidColor = g_pLTClient->ScaleSurfaceToSurfaceSolidColor;
	g_pLTClient->ScaleSurfaceToSurfaceSolidColor = MyScaleSurfaceToSurfaceSolidColor;
	
	//OldFillRect = g_pLTClient->FillRect;
	//g_pLTClient->FillRect = MyFillRect;
	//OldDeleteSurface = g_pLTClient->DeleteSurface;
	//g_pLTClient->DeleteSurface = MyDeleteSurface;
	OldDrawStringToSurface = g_pLTClient->DrawStringToSurface;
	g_pLTClient->DrawStringToSurface = MyDrawStringToSurface;
}

void (*OldShutdown)();
void (*OldShutdownWithMessage)( char *pMsg, ... );
void MyShutdown()
{
	FontList_Clear(TRUE);
	OldShutdown();
}

void MyShutdownWithMessage( char *pMsg, ... )
{
	FontList_Clear(TRUE);

	va_list argp;
	
	va_start(argp, pMsg);
	OldShutdownWithMessage(pMsg, argp);
	va_end(argp);
}

/*DWORD g_nNumberCoords[10][2];
void CreateFrameRateFontSurface()
{
	DWORD hFont = g_pLTClient->CreateFont("Terminal", FRAME_RATE_FONT_WIDTH, FRAME_RATE_FONT_HEIGHT, FALSE, FALSE, FALSE);
	DWORD hString = ILTCSBase_CreateString("0123456789");
	g_hFrameRateFontSurface = g_pLTClient->CreateSurfaceFromString(hFont, hString, 0x0000FF00, 0, 0, 0);
	ILTCSBase_FreeString(hString);
	g_pLTClient->DeleteFont(hFont);
	
	
	BOOL bSpace = TRUE;
	DWORD dwWidth, dwHeight;
	g_pLTClient->GetSurfaceDims(g_hFrameRateFontSurface, &dwWidth, &dwHeight);
	
	int nIndex = -1;		
	for (DWORD i = 0; i < dwWidth ; i++)
	{
		for (DWORD j = 0; j < dwHeight ; j++)
		{
			DWORD dwColor;
			g_pLTClient->GetPixel(g_hFrameRateFontSurface, i, j, &dwColor);
			
			if (dwColor)
			{
				if (bSpace) nIndex++;
				break;
			}
		}
		
		if (j == dwHeight)
		{
			if (!bSpace)
			{
				bSpace = TRUE;
				g_nNumberCoords[nIndex][1] = i;
			}
		}
		else
		{
			if (bSpace)
			{
				bSpace = FALSE;
				g_nNumberCoords[nIndex][0] = i;
			}
		}
	}
}*/

BOOL bConsoleIntroDrawn = FALSE;
void DrawIntroduction()
{
	if (!g_hIntroductionSurface[0])
		CreateIntroductionSurface();

	DWORD hScreen = g_pLTClient->GetScreenSurface();

	for (int i = 0; i < INTRODUCTION_LINES; i++)
		g_pLTClient->DrawSurfaceToSurfaceTransparent(hScreen, g_hIntroductionSurface[i], NULL, 5, 5 + (i * INTRODUCTION_FONT_HEIGHT + 2), 0);

	if (!bConsoleIntroDrawn)
	{
		bConsoleIntroDrawn = TRUE;

		DWORD dwOldColor = *g_pnConTextColor;
		*g_pnConTextColor = 0x00FF00FF;
		ILTCSBase_CPrint(g_pLTClient, APP_NAME, APP_VERSION);
		*g_pnConTextColor = dwOldColor;
	}
}

void DrawIntroductionF15()
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
	sprintf(szDescription, "Profile description = %s", GetCurrProfileString(PO_DESCRIPTION));
	szIntro[2] = szDescription;
	sprintf(szPostprocess, "Postprocessing enabled = %s", GetCurrProfileBool(PO_POSTPROCESS_ENABLED) ? "TRUE" : "FALSE");
	szIntro[3] = szPostprocess;
	szIntro[4] = "Page Up - borderless window toggle";
	szIntro[5] = "Page Down - draw FPS counter toggle";
	
#ifdef _DEBUG
	DWORD dwColorMap[INTRODUCTION_LINES] = { 0x006666FF, 0x00FFFF00, 0x00FFFF00, 0x00FF8800, 0x00FFFFFF, 0x00FFFFFF };
#else	
	DWORD dwColorMap[INTRODUCTION_LINES] = { 0x0000FF00, 0x00FFFF00, 0x00FFFF00, 0x00FF8800, 0x00FFFFFF, 0x00FFFFFF };
#endif
	
	if (GetCurrProfileBool(PO_CLEAN_MODE)) 
		dwColorMap[1] = 0x00FF0000;
	
	for (int i = 0; i < INTRODUCTION_LINES; i++)
		DrawFont15String(szIntro[i], 5, 5 + (i * (FONT15_HEIGHT * 3 + 2)), 1, 3, dwColorMap[i]);
}

void DrawFrameRate()
{
	char szBuffer[64];
	itoa(g_nLastFrameRate, szBuffer, 10);

	DWORD hFontColor = 0x0000FF00;
	if (g_nLastFrameRate < FRAME_RATE_LEVEL_RED)
		hFontColor = 0x00FF0000;
	else if (g_nLastFrameRate < FRAME_RATE_LEVEL_YELLOW)
		hFontColor = 0x00FFFF00;

	DrawFont15String(szBuffer, 5, g_dwHeight - (FONT15_HEIGHT * FRAME_RATE_FONT_SCALE) - 5, 1, FRAME_RATE_FONT_SCALE, hFontColor);
}

void DrawFrameRateOld()
{
	/*DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);
	bool & g_bInOptimized2D = *(bool*)(dwDllAddress + 0x5DE44);
	bool & g_bIn3D = *(bool*)(dwDllAddress + 0x5DE40);*/	
	
	if (!g_hFont15Surface)
		CreateFont15Surface();

	DWORD hScreen = g_pLTClient->GetScreenSurface();

	char szBuffer[64];
	itoa(g_nLastFrameRate, szBuffer, 10);

	int nLength = strlen(szBuffer);
	LTRect rcDest;
	rcDest.top = g_dwHeight - (FONT15_HEIGHT * FRAME_RATE_FONT_SCALE) - 5;
	rcDest.bottom = rcDest.top + (FONT15_HEIGHT * FRAME_RATE_FONT_SCALE);

	DWORD hFontColor = 0x0000FF00;
	if (g_nLastFrameRate < FRAME_RATE_LEVEL_RED)
		hFontColor = 0x00FF0000;
	else if (g_nLastFrameRate < FRAME_RATE_LEVEL_YELLOW)
		hFontColor = 0x00FFFF00;

	g_pLTClient->SetOptimized2DColor(hFontColor);

	for (int k = 0; k < nLength ; k++)
	{
		//g_pLTClient->DrawSurfaceToSurface(hScreen, g_hFrameRateFontSurface[szBuffer[k] - 0x30], NULL, nStartX + (k * (FRAME_RATE_FONT_HEIGHT + 1)), nStartY);	
		
		rcDest.left = 5 + (k * (FONT15_WIDTH + 1) * FRAME_RATE_FONT_SCALE);
		rcDest.right = rcDest.left + (FONT15_WIDTH * FRAME_RATE_FONT_SCALE);
		//g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, g_hFont15Surface[szBuffer[k] - FONT15_BEGIN], &rcDest, NULL, 0);
	}
	g_pLTClient->SetOptimized2DColor(0x00FFFFFF);

	//DrawFont15String(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¦", 0, 250, 2, 4, 0x00FFFFFF);
}

DWORD g_hViewModelBaseFOVX = 0;
DWORD g_hViewModelBaseFOVY = 0;
DWORD g_hExtraFOVXOffset = 0;
DWORD g_hExtraFOVYOffset = 0;
float g_fLastExtraFOVXOffset = 0.0f;

#define MATH_PI 3.141592f
#define VIEW_MODE_BASE_ASPECT 1.333333f

void __fastcall MyIClientShell_Update(void* pShell)
{	
	IClientShell_Update(pShell);

	if (GetCurrProfileBool(PO_VIEW_MODEL_FOV))
	{
		if (!g_hViewModelBaseFOVX)
		{
			g_hViewModelBaseFOVX = g_pLTClient->GetConsoleVar("ViewModelBaseFOVX");
			if (g_hViewModelBaseFOVX)
			{
				float fOldBaseX = ILTCSBase_GetVarValueFloat(g_hViewModelBaseFOVX);
				float fNewBaseX = atanf(tanf(fOldBaseX*MATH_PI/360.0f) * (float)g_dwHeight/(float)(g_dwWidth * GetCurrProfileFloat(PO_CAMERA_FOV_SCALER)) * VIEW_MODE_BASE_ASPECT)*360.0f/MATH_PI;
				
				char szTemp[64];
				sprintf(szTemp, "ViewModelBaseFOVX %f", fNewBaseX);
				g_pLTClient->RunConsoleString(szTemp);
			}
			
		}
		
		if (!g_hExtraFOVXOffset)
			g_hExtraFOVXOffset = g_pLTClient->GetConsoleVar("ExtraFOVXOffset");
		
		if (g_hViewModelBaseFOVX)
		{
			float fCurrXOffset = ILTCSBase_GetVarValueFloat(g_hExtraFOVXOffset);
			if (fCurrXOffset != g_fLastExtraFOVXOffset)
			{
				float fBase = ILTCSBase_GetVarValueFloat(g_hViewModelBaseFOVX);			
				float fCamera = fBase - fCurrXOffset;
				
				fCamera = atanf(tanf(fCamera * MATH_PI/360.0f) * 0.75f * (float)g_dwWidth/(float)(g_dwHeight /** g_fCameraFOVXScaler*/)) * 360.0f/MATH_PI;
				g_fLastExtraFOVXOffset = fBase - fCamera;
				
				char szTemp[64];
				sprintf(szTemp, "ExtraFOVXOffset %f", g_fLastExtraFOVXOffset);
				g_pLTClient->RunConsoleString(szTemp);
			}
		}
	}
	
	if (g_FontList.size())
	{
		float fTime = ILTCSBase_GetTime(g_pLTClient);
		
		if (fTime - g_fLastFontListUpdate > FONT_LIST_UPDATE_TIME)
		{
			g_fLastFontListUpdate = fTime;
			FontList_Update();
		}	
	}
	
	if (GetCurrProfileFlag(PO_RAW_MOUSE_INPUT))
	{
		//if (!g_hMouseSensitivity)
		//	g_hMouseSensitivity = g_pLTClient->GetConsoleVar("MouseSensitivity");	
		
		g_lRMILastX = 0;
		g_lRMILastY = 0;
	}

	if (GetCurrProfileBool(PO_MISC_CC))
		g_pLTClient->RunConsoleString("UpdateRate 60");

	//DWORD hTestVar = g_pLTClient->GetConsoleVar("UpdateRate");
	//float fTest = ILTCSBase_GetVarValueFloat(hTestVar);
}

DWORD (*OldEndOptimized2D)();
DWORD MyEndOptimized2D()
{
	float fTime = ILTCSBase_GetTime(g_pLTClient);
	float fIntroTime = GetCurrProfileFloat(PO_INTRODUCTION_TIME);

	if (fIntroTime && fTime - g_fIntroductionStartTime < fIntroTime)
		DrawIntroduction();
	
	if (GetCurrProfileBool(PO_SHOW_FPS) && g_bDrawFPS)
		DrawFrameRate();
	
	if (g_bDrawConsole)
		Console_Draw();
	
	return OldEndOptimized2D();
}

typedef BOOL (__stdcall *DIEnumDevicesCallback_type)(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL (__stdcall *DIEnumDevicesCallback)(LPCDIDEVICEINSTANCE lpddii, LPVOID pvRef);
BOOL __stdcall MyDIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	BYTE* anDevType = (BYTE*)&lpddi->dwDevType;
	
	/*if (anDevType[0] != DIDEVTYPE_MOUSE && anDevType[0] != DIDEVTYPE_KEYBOARD)
	{
		logf("DirectInput non-mouse/non-keyboard device \"%s\"", lpddi->tszInstanceName);
		return DIENUM_CONTINUE;
	}*/
	
	logf("DirectInput mouse/keyboard device \"%s\" [%d]", lpddi->tszInstanceName, anDevType[0]);
	return DIEnumDevicesCallback(lpddi, pvRef);
}

DWORD (*OldCreateObject)(ObjectCreateStruct *pStruct);
DWORD MyCreateObject(ObjectCreateStruct *pStruct)
{
	if (pStruct->m_ObjectType == OT_LIGHT)
		pStruct->m_Flags |= FLAG_DONTLIGHTBACKFACING;
	
	return OldCreateObject(pStruct);
}

BOOL g_bVisionModeEnabled = FALSE;
typedef int (*d3d_RenderScene_type)(SceneDesc *pDesc);
int (*d3d_RenderScene)(SceneDesc *pDesc);
int My_d3d_RenderScene(SceneDesc* pDesc)
{
	if (pDesc->m_DrawMode == DRAWMODE_OBJECTLIST || g_bDrawConsole)
		g_dwPPCurrIntensity = GetCurrProfileDWord(PO_POSTPROCESS_INTENSITY_MENU);
	else if (g_bVisionModeEnabled)
		g_dwPPCurrIntensity = GetCurrProfileDWord(PO_POSTPROCESS_INTENSITY_VM);
	else
		g_dwPPCurrIntensity = GetCurrProfileDWord(PO_POSTPROCESS_INTENSITY);
	
	return d3d_RenderScene(pDesc);
}

/*DWORD (*OldScaleSurfaceToSurface)(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect);
DWORD MyScaleSurfaceToSurface(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect)
{
	if (pDestRect && pDestRect->left == 0 && pDestRect->top == 0 && 
		pDestRect->right == g_dwWidth && pDestRect->bottom == g_dwHeight)
	{
		g_dwPPCurrIntensity = GetCurrProfileDWord(PO_POSTPROCESS_INTENSITY_MENU);
	}

	return OldScaleSurfaceToSurface(hDest, hSrc, pDestRect, pSrcRect);
}*/

typedef void (*d3d_BlitToScreen_type)(BlitRequest *pRequest);
void (*d3d_BlitToScreen)(BlitRequest *pRequest);
void My_d3d_BlitToScreen(BlitRequest *pRequest)
{
	if (pRequest->m_pDestRect && pRequest->m_pDestRect->left == 0 && pRequest->m_pDestRect->top == 0 && 
		pRequest->m_pDestRect->right == g_dwWidth && pRequest->m_pDestRect->bottom == g_dwHeight)
	{
		g_dwPPCurrIntensity = GetCurrProfileDWord(PO_POSTPROCESS_INTENSITY_MENU);
	}	
	
	d3d_BlitToScreen(pRequest);
}

#ifndef PRIMAL_HUNT_BUILD

void SendMessageFromServerApp(char* szText)
{
	char szBuffer[256] = { SERVERSHELL_MESSAGE };
	strcpy(szBuffer + 1, szText);
	
	DWORD dwNotUsed = 0;
	IServerShell_ServerAppMessageFn(g_pServerMgr->m_pServerMgr->m_pServerShell, &dwNotUsed, szBuffer, 256);
}

BOOL g_bVerifyClientBypassed = FALSE;
float g_fLastMOTDTime = 0.0f;
BOOL g_bExtraCacheListApplied = FALSE;

typedef DWORD (__stdcall *ILTServer_GetClientData_type)(DWORD hClient, void *&pData, DWORD &nLength);
DWORD (__stdcall *ILTServer_GetClientData)(DWORD hClient, void *&pData, DWORD &nLength);

void __fastcall MyIServerShell_Update(void* pShell, float timeElapsed)
{
	IServerShell_Update(pShell, timeElapsed);

	float fMOTDTimer = GetCurrProfileFloat(EXT_MOTD_TIMER);
	if (fMOTDTimer)
	{
		float fTime = ILTCSBase_GetTime(g_pLTServer);
		if (fTime - g_fLastMOTDTime > fMOTDTimer)
		{
			g_fLastMOTDTime = fTime;
			char* szMOTD = GetCurrProfileString(EXT_MOTD_STRING);
			
			ILTCSBase_CPrint(g_pLTServer, "[EXT] MOTD: %s", szMOTD);
			SendMessageFromServerApp(szMOTD);
		}
	}
	
	if (g_bVerifyClientBypassed)
	{
		char szBuffer[1024];
		
		g_bVerifyClientBypassed = FALSE;

		DWORD hCurrClient = g_pLTServer->GetNextClient(NULL);
		while (hCurrClient) 
		{
			void* pClientData;
			DWORD dwDataLen;
			ILTServer_GetClientData(hCurrClient, pClientData, dwDataLen);

			if (g_dwClientDataLen && g_dwClientDataLen != dwDataLen)
			{
				sprintf(szBuffer, "[EXT] BanMgr: Client has invalid client data size (%d != %d) [IServerShell_Update]", dwDataLen, g_dwClientDataLen);
				logf(szBuffer + 6);
				ILTCSBase_CPrint(g_pLTServer, szBuffer);

				g_pLTServer->KickClient(hCurrClient);
				hCurrClient = g_pLTServer->GetNextClient(NULL);
				continue;
			}

			char* szName = NULL;
			char szIP[64];
			char* szID = NULL;
			
			if (BanList_IsBanned(hCurrClient, pClientData, &szName, szIP, &szID))
			{		
				sprintf(szBuffer, "[EXT] BanMgr: %s (%s - %s) is banned! [IServerShell_Update]", szName, szIP, szID);
				logf(szBuffer + 6);
				ILTCSBase_CPrint(g_pLTServer, szBuffer);

				g_pLTServer->KickClient(hCurrClient);
				hCurrClient = g_pLTServer->GetNextClient(NULL);
				continue;
			}

			hCurrClient = g_pLTServer->GetNextClient(hCurrClient);
		}
	}
}

void __fastcall MyIServerShell_VerifyClient(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD &nVerifyCode)
{
	char szBuffer[1024];
	
	if (GetCurrProfileBool(EXT_BAN_MANAGER) && g_dwClientDataLen && hClient)
	{
		void *pData = NULL;
		DWORD dwDataLen = 0;

		ILTServer_GetClientData(hClient, pData, dwDataLen);

		if (g_dwClientDataLen != dwDataLen)
		{
			sprintf(szBuffer, "[EXT] BanMgr: Client has invalid client data size (%d != %d) [VerifyClient]", dwDataLen, g_dwClientDataLen);
			logf(szBuffer + 6);
			ILTCSBase_CPrint(g_pLTServer, szBuffer);
			SendMessageFromServerApp(BAN_MGR_CLIENT_REJECTED);

			nVerifyCode = LT_DISCON_MISCCRC;

			return;
		}
	}

	IServerShell_VerifyClient(pShell, notUsed, hClient, pClientData, nVerifyCode);

	if (GetCurrProfileBool(EXT_BAN_MANAGER) && hClient && pClientData)
	{
		char* szName = NULL;
		char szIP[64];
		char* szID = NULL;
		
		if (BanList_IsBanned(hClient, pClientData, &szName, szIP, &szID))
		{		
			sprintf(szBuffer, "[EXT] BanMgr: %s (%s - %s) is banned! [VerifyClient]", szName, szIP, szID);
			nVerifyCode = LT_DISCON_MISCCRC;
		}
		else
		{			
			sprintf(szBuffer, "[EXT] BanMgr: %s (%s - %s) is good to go!", szName, szIP, szID);
		}

		logf(szBuffer + 6);
		ILTCSBase_CPrint(g_pLTServer, szBuffer);
	}
}

void* __fastcall MyIServerShell_OnClientEnterWorld(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD clientDataLen)
{
	void* pResult = IServerShell_OnClientEnterWorld(pShell, notUsed, hClient, pClientData, clientDataLen);

	if (GetCurrProfileBool(EXT_BAN_MANAGER) && hClient && pClientData)
	{
		char* szName = NULL;
		char szIP[64];
		char* szID = NULL;

		if ((g_dwClientDataLen && g_dwClientDataLen != clientDataLen) || BanList_IsBanned(hClient, pClientData, &szName, szIP, &szID))
			g_bVerifyClientBypassed = TRUE;
	}
	
	return pResult;
}

void __fastcall MyIServerShell_PostStartWorld(void* pShell)
{
	g_fLastMOTDTime = 0.0f;
	g_bExtraCacheListApplied = FALSE;
	IServerShell_PostStartWorld(pShell);
}

/*DWORD __fastcall MyIServerShell_ServerAppMessageFn(void* pShell, void* notUsed, char *pMsg, int nLen)
{
	return IServerShell_ServerAppMessageFn(pShell, notUsed, pMsg, nLen);
}*/

typedef DWORD (*LoadServerBinaries_type)(CClassMgr *pClassMgr);
DWORD (*LoadServerBinaries)(CClassMgr *pClassMgr);
DWORD MyLoadServerBinaries(CClassMgr *pClassMgr)
{
	DWORD dwResult = LoadServerBinaries(pClassMgr);

	DWORD dwRead;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(SERVER_DLL);
	
	g_pServerMgr = (CServerMgrBase*)(dwDllAddress + ADDR_DS_SERVER_MGR); // 0x80E6C
	g_pLTServer = g_pServerMgr->m_pServerMgr->m_pLTServer;
	g_pClassMgr = (CClassMgrBase*)(dwDllAddress + ADDR_DS_CLASS_MGR); // 0x80E70

	DWORD* pOrigTable = (DWORD*)*(DWORD*)g_pLTServer;	
	ILTCSBase_CPrint = (ILTCSBase_CPrint_type)pOrigTable[V_CSBASE_CPRINT]; // 39
	ILTCSBase_GetTime = (ILTCSBase_GetTime_type)pOrigTable[V_CSBASE_GET_TIME]; // 53
	ILTCSBase_GetFrameTime = (ILTCSBase_GetFrameTime_type)pOrigTable[V_CSBASE_GET_FRAME_TIME]; // 54

	ILTServer_GetClientData = (ILTServer_GetClientData_type)pOrigTable[V_SERVER_GET_CLIENT_DATA]; // 124

	ReadConfigDS(".\\ltmsg.ini");
	
	if (GetCurrProfileBool(PO_TIME_CALIBRATION))
		ApplyTimeCalibrationDS_Fix();

	if (GetCurrProfileString(EXT_CACHE_LIST)[0])
		ApplyExtraCacheListDS_Fix();

	pOrigTable = (DWORD*)*(DWORD*)g_pServerMgr->m_pServerMgr->m_pServerShell;	
	IServerShell_Update = (IServerShell_Update_type)pOrigTable[V_SSHELL_UPDATE]; // 15
	EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + V_SSHELL_UPDATE), (DWORD)MyIServerShell_Update, dwRead); // 15
	IServerShell_VerifyClient = (IServerShell_VerifyClient_type)pOrigTable[V_SSHELL_VERIFY_CLIENT]; // 4
	EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + V_SSHELL_VERIFY_CLIENT), (DWORD)MyIServerShell_VerifyClient, dwRead); // 4
	IServerShell_OnClientEnterWorld = (IServerShell_OnClientEnterWorld_type)pOrigTable[V_SSHELL_ON_CLIENT_ENTER_WORLD]; // 5
	EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + V_SSHELL_ON_CLIENT_ENTER_WORLD), (DWORD)MyIServerShell_OnClientEnterWorld, dwRead); // 5	
	IServerShell_PostStartWorld = (IServerShell_PostStartWorld_type)pOrigTable[V_SSHELL_POST_START_WORLD]; // 8
	EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + V_SSHELL_POST_START_WORLD), (DWORD)MyIServerShell_PostStartWorld, dwRead); // 8

	if (GetCurrProfileFloat(EXT_MOTD_TIMER))
	{
		IServerShell_ServerAppMessageFn = (IServerShell_ServerAppMessageFn_type)pOrigTable[V_SSHELL_SERVER_APP_MESSAGE_FN]; // 1
		//EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + 1), (DWORD)MyIServerShell_ServerAppMessageFn, dwRead);
	}

	return dwResult;
}

void HookDSStuff1()
{
	logf("Hooking DS stuff #1");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(SERVER_DLL);

	LoadServerBinaries = (LoadServerBinaries_type)(dwDllAddress + ADDR_DS_LOAD_BINARIES); // 0x1928
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + ADDR_DS_LOAD_BINARIES_CALL), (DWORD)MyLoadServerBinaries, FALSE); // 0x3C40E
}

#endif

#ifdef PRIMAL_HUNT_BUILD
BYTE g_anLoadRenderLibCode1[6];
BYTE g_anLoadRenderLibCode2[6];
#endif

DWORD g_dwJumpBackDIEnumTwice = 0;
DWORD g_pDirectInput = 0;
__declspec(naked) void DIEnumDevicesTwice()
{
	__asm
	{
		mov eax, g_pDirectInput // 4E3B18h
		mov eax, [eax]
		push 1
		push 0
		push offset MyDIEnumDevicesCallback
		mov ecx, [eax]
		push 2
		push eax
		call dword ptr [ecx+10h]
		
		mov eax, g_pDirectInput // 4E3B18h
		mov eax, [eax]
		push 1
		push 0
		push offset MyDIEnumDevicesCallback
		mov ecx, [eax]
		push 3
		push eax
		call dword ptr [ecx+10h]

		jmp g_dwJumpBackDIEnumTwice
	}
}

void HookEngineStuff1()
{
	logf(APP_NAME, APP_VERSION);
	logf("Hooking engine stuff #1");
	
	DWORD dwRead;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle(LITHTECH_EXE);
	char* szCommandLine = GetCommandLine();
	
	EngineHack_WriteFunction(hProcess, (LPVOID)(dwExeAddress + ADDR_LOAD_LIBRARY), (DWORD)MyLoadLibraryA, dwRead); // 0x0C6100

	if (!strstr(szCommandLine, CMD_FLAG_NO_DI_HOOKS))
	{
		DWORD dwEnum = (DWORD)MyDIEnumDevicesCallback;
		EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_ENUM_DEVICES_CALLBACK), (BYTE*)&dwEnum, (BYTE*)&dwRead, 4); // 0x03EEC6
		DIEnumDevicesCallback = (DIEnumDevicesCallback_type)dwRead;
		
		BYTE anRead[22];
		BYTE anNops[22] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		g_pDirectInput = dwExeAddress + ADDR_LP_DIRECT_INPUT;
		EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_DI_ENUM_DEVICES_INJECT1), anNops, anRead, 22); // 0x3EEBC
		EngineHack_WriteJump(hProcess, (LPVOID)(dwExeAddress + ADDR_DI_ENUM_DEVICES_INJECT1), (DWORD)DIEnumDevicesTwice); // 0x3EEBC
		g_dwJumpBackDIEnumTwice = dwExeAddress + ADDR_DI_ENUM_DEVICES_INJECT2; // 0x3EEC1
	}

#ifdef PRIMAL_HUNT_BUILD
	memcpy(g_anLoadRenderLibCode1, (LPVOID)(dwExeAddress + ADDR_LOAD_RENDER_LIB_CODE1), 6); // 0x38219
	memcpy(g_anLoadRenderLibCode2, (LPVOID)(dwExeAddress + ADDR_LOAD_RENDER_LIB_CODE2), 6); // 0x75371
#endif
}

/*typedef DWORD (__fastcall *ILTLightAnim_FindLightAnim_type)(ILTLightAnim* pInterface, void* pEDX, const char *pName, DWORD &hLightAnim);
DWORD (__fastcall *ILTLightAnim_FindLightAnim)(ILTLightAnim* pInterface, void* pEDX, const char *pName, DWORD &hLightAnim);
DWORD __fastcall MyILTLightAnim_FindLightAnim(ILTLightAnim* pInterface, void* pEDX, const char *pName, DWORD &hLightAnim)
{
	return ILTLightAnim_FindLightAnim(pInterface, pEDX, pName, hLightAnim);;
}*/

typedef DWORD (__fastcall *ILTLightAnim_SetLightAnimInfo_type)(ILTLightAnim* pInterface, void* pEDX, DWORD hLightAnim, LAInfo &info);
DWORD (__fastcall *ILTLightAnim_SetLightAnimInfo)(ILTLightAnim* pInterface, void* pEDX, DWORD hLightAnim, LAInfo &info);
DWORD __fastcall MyILTLightAnim_SetLightAnimInfo(ILTLightAnim* pInterface, void* pEDX, DWORD hLightAnim, LAInfo &info)
{	
	if (hLightAnim != 0xFFFFFFFF && info.m_vLightPos.x == 0.0f && info.m_vLightPos.y == 0.0f && info.m_vLightPos.z == 0.0f)
	{
		g_bVisionModeEnabled = FALSE;
		if (!info.m_iFrames[0] && !info.m_iFrames[1])
		{
			/*float fMod = (1.0f - ((float)GetCurrProfileDWord(PO_POSTPROCESS_INTENSITY) / 255.0f)) * 1.5f;
			if (fMod < 0.1f)
				fMod = 0.1f;
			else if (fMod > 1.0f) 
				fMod = 1.0f;
			
			info.m_fBlendPercent *= fMod;*/
			g_bVisionModeEnabled = TRUE;
		}
		/*else
		{
			info.m_fBlendPercent = 1.0f;
		}*/
	}

	return ILTLightAnim_SetLightAnimInfo(pInterface, pEDX, hLightAnim, info);
}

void HookEngineStuff2()
{
	logf("Hooking engine stuff #2");

	DWORD dwRead;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle(LITHTECH_EXE);
	DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);
	g_pClientMgr = (CClientMgrBase*)(dwExeAddress + ADDR_CLIENT_MGR); // 0xDEFAC
	g_pServerMgr = (CServerMgrBase*)(dwExeAddress + ADDR_SERVER_MGR); // 0xE5DC8
	g_pLTClient = g_pClientMgr->m_pClientMgr->m_pLTClient;

	CConsole& console = *(CConsole*)(dwExeAddress + ADDR_CONSOLE); // 0xE2F88
	g_pConsole = &console;

	DWORD dwConTextColorAddr = dwExeAddress + ADDR_CONSOLE_TEXT_COLOR; // 0x0079FA
	g_pnConTextColor = (DWORD*)dwConTextColorAddr;
	EngineHack_AllowWrite(hProcess, (LPVOID)dwConTextColorAddr, 4);

	DWORD* pOrigTable = (DWORD*)*(DWORD*)g_pLTClient;

	ILTCSBase_CPrint = (ILTCSBase_CPrint_type)pOrigTable[V_CSBASE_CPRINT]; // 39	
	ILTCSBase_CreateString = (ILTCSBase_CreateString_type)pOrigTable[V_CSBASE_CREATE_STRING]; // 46
	ILTCSBase_FreeString = (ILTCSBase_FreeString_type)pOrigTable[V_CSBASE_FREE_STRING]; // 47
	
	ILTCSBase_GetStringData = (ILTCSBase_GetStringData_type)pOrigTable[V_CSBASE_GET_STRING_DATA]; // 50
	ILTCSBase_GetVarValueFloat = (ILTCSBase_GetVarValueFloat_type)pOrigTable[V_CSBASE_GET_VAR_FLOAT]; // 51
	ILTCSBase_GetVarValueString = (ILTCSBase_GetVarValueString_type)pOrigTable[V_CSBASE_GET_VAR_STRING]; // 52
	ILTCSBase_GetTime = (ILTCSBase_GetTime_type)pOrigTable[V_CSBASE_GET_TIME]; // 53
	ILTCSBase_GetFrameTime = (ILTCSBase_GetFrameTime_type)pOrigTable[V_CSBASE_GET_FRAME_TIME]; // 54
	
	char* szProfile = NULL;
	DWORD hProfile = g_pLTClient->GetConsoleVar(CVAR_PROFILE);
	if (hProfile)
		szProfile = ILTCSBase_GetVarValueString(hProfile);
	
	pOrigTable = (DWORD*)*(DWORD*)g_pClientMgr->m_pClientMgr->m_pClientShell;
	
	if (g_pLTClient->Shutdown != MyShutdown)
	{
		ReadConfig(".\\ltmsg.ini", szProfile);
		
		OldShutdown = g_pLTClient->Shutdown;
		OldShutdownWithMessage = g_pLTClient->ShutdownWithMessage;
		OldEndOptimized2D = g_pLTClient->EndOptimized2D;
		OldCreateObject = g_pLTClient->CreateObject;

		if (GetCurrProfileBool(PO_CLEAN_MODE))
			return;
		
		g_pLTClient->Shutdown = MyShutdown;
		g_pLTClient->ShutdownWithMessage = MyShutdownWithMessage;
		g_pLTClient->EndOptimized2D = MyEndOptimized2D;
		g_pLTClient->CreateObject = MyCreateObject;
		
		IClientShell_Update = (IClientShell_Update_type)pOrigTable[V_CSHELL_UPDATE]; // 15
		EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + V_CSHELL_UPDATE), (DWORD)MyIClientShell_Update, dwRead); // 15

		if (GetCurrProfileBool(PO_ENABLE_CONSOLE) && GetCurrProfileBool(PO_NO_ENVMAP_CONSOLE_PRINT))
		{
			BYTE anOldData[5];
			BYTE anFiveNops[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
			EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_CONSOLE_ENVMAP_PRINT), anFiveNops, anOldData, 5); // 0x22DFE
		}

		if (GetCurrProfileBool(PO_POSTPROCESS_ENABLED))
		{
			//OldScaleSurfaceToSurface = g_pLTClient->ScaleSurfaceToSurface;
			//g_pLTClient->ScaleSurfaceToSurface = MyScaleSurfaceToSurface;
			d3d_BlitToScreen = (d3d_BlitToScreen_type)(dwDllAddress + ADDR_D3D_BLIT_TO_SCREEN); // 0x1DE1C
			EngineHack_WriteFunction(hProcess, (LPVOID)(*(DWORD*)(dwDllAddress + ADDR_D3D_RENDER_STRUCT) + ADDR_D3D_BLIT_TO_SCREEN_TABLE), (DWORD)My_d3d_BlitToScreen, dwRead); // 0x58470 0xF4

			pOrigTable = (DWORD*)*(DWORD*)g_pLTClient->m_pLightAnimLT;	
			ILTLightAnim_SetLightAnimInfo = (ILTLightAnim_SetLightAnimInfo_type)pOrigTable[V_LA_SET_LIGHT_ANIM_INFO]; // 3
			EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + V_LA_SET_LIGHT_ANIM_INFO), (DWORD)MyILTLightAnim_SetLightAnimInfo, dwRead); // 3

			//ILTLightAnim_FindLightAnim = (ILTLightAnim_FindLightAnim_type)pOrigTable[0];
			//EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + 0), (DWORD)MyILTLightAnim_FindLightAnim, dwRead);
		}
	}
	
	//DWORD& g_CV_TraceConsole = *(DWORD*)(dwExeAddress + 0xE370C);
	//g_CV_TraceConsole = 1;
	
	if (GetCurrProfileBool(PO_MISC_CC))
	{
		//g_pLTClient->RunConsoleString("UpdateRateInitted 1");
		//g_pLTClient->RunConsoleString("UpdateRate 60");	
		g_pLTClient->RunConsoleString("LockOnFlip 0");
		//g_pLTClient->RunConsoleString("MaxModelShadows 0");
		g_pLTClient->RunConsoleString("MaxModelLights 10");
		g_pLTClient->RunConsoleString("NearZ 4");
		g_pLTClient->RunConsoleString("ReallyCloseNearZ 0.01");
		//g_pLTClient->RunConsoleString("FilterOptimized 1");
		//g_pLTClient->RunConsoleString("TripleBuffer 0");
	}
	
	BYTE nNew = 0x03;
	BYTE nOld;
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_CREATE_FONT_QUALITY), &nNew, &nOld, 1); // 0x9B5B5

	if (GetCurrProfileBool(PO_POSTPROCESS_ENABLED))
	{
		d3d_RenderScene = (d3d_RenderScene_type)(dwDllAddress + ADDR_D3D_RENDER_SCENE); // 0x17AA0
		EngineHack_WriteFunction(hProcess, (LPVOID)(*(DWORD*)(dwDllAddress + ADDR_D3D_RENDER_STRUCT) + ADDR_D3D_RENDER_SCENE_TABLE), (DWORD)My_d3d_RenderScene, dwRead); // 0x58470 0xB8
	}
}

void ApplyLightLoad_Fix()
{
	logf("Applying light load fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);
	//DWORD dwRenderStruct = dwDllAddress + 0x58470;

	EngineHack_WriteCall(hProcess, (LPVOID)(DWORD(dwDllAddress) + ADDR_D3DDEVICE_LOAD_CALL), (DWORD)FakeD3DDevice_Load, TRUE); // 0x34CF2
}

BOOL* g_pDetailTextureCapable;
ConVarFloat* g_pDetailTextures;

typedef void (*sub_3F0A2A7_type)();
void (*sub_3F0A2A7)();
void My_sub_3F0A2A7()
{
	g_pDetailTextures->m_dwVal = 0;
	sub_3F0A2A7();	
	g_pDetailTextures->m_dwVal = 1;
}

void ApplyTWMDetailTex_Fix()
{
	logf("Applying TWM detail textures fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);

	g_pDetailTextureCapable = (BOOL*)(dwDllAddress + ADDR_D3D_DETAIL_TEX_CAPABLE); // 0x5DE2C
	sub_3F0A2A7 = (sub_3F0A2A7_type)(dwDllAddress + ADDR_D3D_SUB_3F0A2A7); // 0xA2A7
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + ADDR_D3D_SUB_3F0A2A7_CALL), (DWORD)My_sub_3F0A2A7, FALSE); // 0x9C5A

	g_pDetailTextures = (ConVarFloat*)(dwDllAddress + ADDR_D3D_DETAIL_TEXTURES); // 0x51448
}

float g_fServerFrameTimeClamp = 0.001f;
typedef void (*dsi_ClientSleep_type)(DWORD dwMilliseconds);
void (__cdecl *dsi_ClientSleep)(DWORD dwMilliseconds);
typedef void (__fastcall *UpdateSounds_type)(CServerMgr* pServerMgr, float fDeltaTime);
void (__fastcall *UpdateSounds)(CServerMgr* pServerMgr, float fDeltaTime);
typedef void (*pd_Update_type)(CClientShell *pShell);
void (*pd_Update)(CClientShell *pShell);

__forceinline int Round(float v)
{
    int r;
    __asm
    {
        FLD     v
		FISTP   r
		FWAIT
    };
	
    return r;
}

void GameTimeCalibration_Client()
{
	g_pClientMgr->m_pClientMgr->m_pCurShell->m_GameFrameTime = (float)Round(g_pClientMgr->m_pClientMgr->m_pCurShell->m_GameFrameTime * 1000.0f) / 1000.0f;
}

void TimeCalibration_Client()
{
	g_pClientMgr->m_pClientMgr->m_FrameTime = (float)Round(g_pClientMgr->m_pClientMgr->m_FrameTime * 1000.0f) / 1000.0f;
}

void TimeCalibration_Server()
{
	g_pServerMgr->m_pServerMgr->m_TrueFrameTime  = (float)Round(g_pServerMgr->m_pServerMgr->m_TrueFrameTime * 1000.0f) / 1000.0f;
	g_pServerMgr->m_pServerMgr->m_FrameTime = (float)Round(g_pServerMgr->m_pServerMgr->m_FrameTime * 1000.0f) / 1000.0f; //g_pServerMgr->m_pServerMgr->m_TrueFrameTime;
}

void My_pd_Update(CClientShell *pShell)
{
	GameTimeCalibration_Client();
	pd_Update(pShell);
}

void __fastcall MyUpdateSounds(CServerMgr* pServerMgr, float fDeltaTime)
{
	TimeCalibration_Server();
	UpdateSounds(pServerMgr, fDeltaTime);
}

void My_dsi_ClientSleep(DWORD dwMilliseconds)
{
	TimeCalibration_Client();
	dsi_ClientSleep(dwMilliseconds);
}

void ApplyTimeCalibration_Fix()
{
	logf("Applying time calibration fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle(LITHTECH_EXE);

	dsi_ClientSleep = (dsi_ClientSleep_type)(dwExeAddress + ADDR_CLIENT_SLEEP); // 0x35070
	EngineHack_WriteCall(hProcess, (LPVOID)(dwExeAddress + ADDR_CLIENT_SLEEP_CALL), (DWORD)My_dsi_ClientSleep, FALSE); // 0x10E06

	pd_Update = (pd_Update_type)(DWORD(dwExeAddress) + ADDR_PD_UPDATE); // 0x6E250
	EngineHack_WriteCall(hProcess, (LPVOID)(dwExeAddress + ADDR_PD_UPDATE_CALL), (DWORD)My_pd_Update, FALSE); // 0x15374

	DWORD dwNew = (DWORD)(&g_fServerFrameTimeClamp);
	BYTE anOld[4];
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_SERVER_FRAMETIME_CLAMP1), (BYTE*)(&dwNew), anOld, 4); // 0x83688
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_SERVER_FRAMETIME_CLAMP2), (BYTE*)(&dwNew), anOld, 4); // 0x8369D

	UpdateSounds = (UpdateSounds_type)(dwExeAddress + ADDR_UPDATE_SOUNDS); // 0x83370
	EngineHack_WriteCall(hProcess, (LPVOID)(dwExeAddress + ADDR_UPDATE_SOUNDS_CALL), (DWORD)MyUpdateSounds, FALSE); // 0x836D7
}

#ifndef PRIMAL_HUNT_BUILD

void ApplyTimeCalibrationDS_Fix()
{
	logf("Applying DS time calibration fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(SERVER_DLL);

	UpdateSounds = (UpdateSounds_type)(dwDllAddress + ADDR_DS_UPDATE_SOUNDS); // 0x3CA4B
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + ADDR_DS_UPDATE_SOUNDS_CALL), (DWORD)MyUpdateSounds, FALSE); // 0x3CD72

	DWORD dwNew = (DWORD)(&g_fServerFrameTimeClamp);
	BYTE anOld[4];
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + ADDR_DS_SERVER_FRAMETIME_CLAMP1), (BYTE*)(&dwNew), anOld, 4); // 0x3CD23
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + ADDR_DS_SERVER_FRAMETIME_CLAMP2), (BYTE*)(&dwNew), anOld, 4); // 0x3CD38

	float& fServerFPS = *(float*)(dwDllAddress + ADDR_DS_SERVER_FPS); // 0x70EC0
	fServerFPS = GetCurrProfileFloat(PO_SERVER_FPS);
}

DWORD (*OldGetServerFlags)();
DWORD MyGetServerFlags()
{
	DWORD dwRet = OldGetServerFlags();
	if (!g_bExtraCacheListApplied && (dwRet & SS_CACHING))
	{
		g_bExtraCacheListApplied = TRUE;
		
		FILE* pFile = fopen(GetCurrProfileString(EXT_CACHE_LIST), "rb");
		fseek(pFile, 0, SEEK_END);

		int nSize = ftell(pFile);
		rewind(pFile);

		char* szBuffer = new char[nSize + 1];
		fread(szBuffer, 1, nSize, pFile);
		szBuffer[nSize] = '\r';

		int i = 0;
		int nStart = 0;
		while (i < nSize)
		{
			if (szBuffer[i] == '\r')
			{
				szBuffer[i] = 0;
				
				DWORD dwType;
				char* szFilename = ParseCacheString(szBuffer + nStart, dwType);
				if (dwType != FT_ERROR)
					g_pLTServer->CacheFile(dwType, szFilename);

				i += 2;
				nStart = i;
			}
			else
			{
				i++;
			}
		}
		
		delete[] szBuffer;
		fclose(pFile);
	}

	return dwRet;	
}

void ApplyExtraCacheListDS_Fix()
{
	logf("Applying DS extra cache list fix");
	
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(SERVER_DLL);
	
	OldGetServerFlags = g_pLTServer->GetServerFlags;
	g_pLTServer->GetServerFlags = MyGetServerFlags;
}

#endif

DWORD (*OldFlipScreen)(DWORD flags);
DWORD MyFlipScreen(DWORD flags)
{
	return OldFlipScreen(FLIPSCREEN_DIRTY);
}		

void ApplyFlipScreen_Fix()
{
	logf("Applying flip screen fix");
	
	if (g_pLTClient->FlipScreen != MyFlipScreen)
	{
		OldFlipScreen = g_pLTClient->FlipScreen;
		g_pLTClient->FlipScreen = MyFlipScreen;
	}
}

void ApplyDynamicLightSurfaces_Fix()
{
	logf("Applying dynamic light surfaces fix");
	
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);

	DWORD* dwSurfaceCounts = (DWORD*)(dwDllAddress + ADDR_D3D_DLIGHT_SURFACE_COUNTS); // 0x4BF50
	dwSurfaceCounts[0] = DLIGHT_SURFACES_COUNT_OVERRIDE;
	dwSurfaceCounts[1] = DLIGHT_SURFACES_COUNT_OVERRIDE;
	dwSurfaceCounts[2] = DLIGHT_SURFACES_COUNT_OVERRIDE;
	dwSurfaceCounts[3] = DLIGHT_SURFACES_COUNT_OVERRIDE;
	dwSurfaceCounts[4] = DLIGHT_SURFACES_COUNT_OVERRIDE;
}

float g_fUVMod1 = 0.03125f;
float g_fUVMod2 = 0.015625f;
BYTE* g_pbNextLMPageSize = NULL;

typedef DWORD (*r_FindFreeSlot_type)(RenderContext *pContext, DWORD dwWidth, DWORD dwHeight, DWORD *nX, DWORD *nY, LMPage **param_6);
DWORD (*r_FindFreeSlot)(RenderContext *pContext, DWORD dwWidth, DWORD dwHeight, DWORD *nX, DWORD *nY, LMPage **param_6);

DWORD My_r_FindFreeSlot(RenderContext *pContext, DWORD dwWidth, DWORD dwHeight, DWORD *nX, DWORD *nY, LMPage **pNewLMPage)
{
	DWORD dwSize = (dwWidth < dwHeight) ? dwHeight : dwWidth;

	*g_pbNextLMPageSize = dwSize;
	g_fUVMod1 = 1.0f / dwSize;
	g_fUVMod2 = 1.0f / (dwSize<<1);
	
	return r_FindFreeSlot(pContext, FIND_FREE_SLOT_SIZE_OVERRIDE, FIND_FREE_SLOT_SIZE_OVERRIDE, nX, nY, pNewLMPage);
}

/*typedef DWORD (*r_AddToLMPage_type)(void* pContext, WorldPoly* pPoly);
DWORD (*r_AddToLMPage)(void* pContext, WorldPoly* pPoly);
DWORD My_r_AddToLMPage(void* pContext, WorldPoly* pPoly)
{
	return r_AddToLMPage(pContext, pPoly);;
}*/

void ApplyStaticLightSurfaces_Fix()
{
	logf("Applying static light surfaces fix");
	
	BYTE anOld[32];
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);
	DWORD dwExeAddress = (DWORD)GetModuleHandle(LITHTECH_EXE);
	
	r_FindFreeSlot = (r_FindFreeSlot_type)(dwDllAddress + ADDR_D3D_FIND_FREE_SLOT); // 0x34000
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + ADDR_D3D_FIND_FREE_SLOT_CALL), (DWORD)My_r_FindFreeSlot, FALSE); // 0x342FC

	// 32x32 LMPage
	/*BYTE anPush20[2] = { 0x6A, 0x20 };
	BYTE anOld[4];
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + 0x341B2), anPush20, anOld, 2);*/
	EngineHack_AllowWrite(hProcess, (LPVOID)(dwDllAddress + ADDR_D3D_NEW_LM_PAGE_SIZE), 1); // 0x341B3
	g_pbNextLMPageSize = (BYTE*)(dwDllAddress + ADDR_D3D_NEW_LM_PAGE_SIZE); // 0x341B3
	*g_pbNextLMPageSize = 32; 

	// 32x32 LMPage size comparison (not needed?)
	/*BYTE anCmp20_1[3] = { 0x83, 0xFA, 0x20 };
	BYTE anCmp20_2[3] = { 0x83, 0xFF, 0x20 };
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + 0x34350), anCmp20_1, anOld, 3);
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + 0x34359), anCmp20_2, anOld, 3);*/

	/*r_AddToLMPage = (r_AddToLMPage_type)(dwDllAddress + 0x3429B);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + 0x344E1), (DWORD)My_r_AddToLMPage, FALSE);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + 0x346A3), (DWORD)My_r_AddToLMPage, FALSE);*/

	DWORD dwNew = (DWORD)(&g_fUVMod1);	
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + ADDR_D3D_NEW_LM_PAGE_UV_MOD1), (BYTE*)(&dwNew), anOld, 4); // 0x343F3
	dwNew = (DWORD)(&g_fUVMod2);
	EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + ADDR_D3D_NEW_LM_PAGE_UV_MOD2), (BYTE*)(&dwNew), anOld, 4); // 0x3445D

	// PORTAL TEST
	//WORD wIndex = 0;
	//EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + 0x35A1A), (BYTE*)(&wIndex), anOld, 2);
}

DWORD __stdcall SharedCommonLT_GetCRC_Cli(ILTStream* pStream, DWORD& dwCRC32)
{
	if (pStream)
	{
		dwCRC32 = CalcStreamCRC(pStream);
		return 0;
	}
	else
	{
		dwCRC32 = 0;
		return 1;
	}
}

DWORD __stdcall SharedCommonLT_GetCRC_Test1(ILTStream* pStream, DWORD& dwCRC32) // ILTStream
{
	if (pStream)
	{
		dwCRC32 = CalcStreamCRC(pStream);
		return 0;
	}
	else
	{
		dwCRC32 = 0;
		return 1;
	}
}

DWORD __stdcall SharedCommonLT_GetCRC_Srv(ILTStream* pStream, DWORD& dwCRC32)
{
	if (pStream)
	{
		dwCRC32 = CalcStreamCRC(pStream);
		return 0;
	}
	else
	{
		dwCRC32 = 0;
		return 1;
	}
}

void ApplyFastCRCCheck_Fix()
{
	logf("Applying fast CRC32 check fix");

	DWORD dwOld;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle(LITHTECH_EXE);

	EngineHack_WriteFunction(hProcess, (LPVOID)(dwExeAddress + ADDR_GET_CRC_CLI_TABLE), (DWORD)SharedCommonLT_GetCRC_Cli, dwOld); // 0x0C6884
	EngineHack_WriteFunction(hProcess, (LPVOID)(dwExeAddress + ADDR_GET_CRC_TEST1_TABLE), (DWORD)SharedCommonLT_GetCRC_Test1, dwOld); // 0x0C7514
	EngineHack_WriteFunction(hProcess, (LPVOID)(dwExeAddress + ADDR_GET_CRC_SRV_TABLE), (DWORD)SharedCommonLT_GetCRC_Srv, dwOld); // 0x0C83A4
}

DWORD g_dwOriginalD3D = 0;
HRESULT WINAPI FakeDirectDrawCreateEx(GUID FAR * lpGUID, LPVOID *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter)
{
	HookEngineStuff2();
	
	if (GetCurrProfileBool(PO_INTEL_HD))
		ApplyIntelHD_RHW_Fix();

	if (GetCurrProfileBool(PO_CAMERA_FOV))
	{
		SetCurrProfileBool(PO_CAMERA_FOV, FALSE);
		ApplyCameraFOV_Fix();
	}

	if (GetCurrProfileBool(PO_SOLID_DRAWING))
	{
		SetCurrProfileBool(PO_SOLID_DRAWING, FALSE);
		ApplySolidDrawing_Fix();
	}

	if (GetCurrProfileBool(PO_RAW_MOUSE_INPUT))
	{
		SetCurrProfileBool(PO_RAW_MOUSE_INPUT, FALSE);
		ApplyRawMouseInput_Fix();
	}

	if (GetCurrProfileBool(PO_LIGHT_LOAD))
		ApplyLightLoad_Fix();

	if (GetCurrProfileBool(PO_TWM_DETAIL_TEX))
		ApplyTWMDetailTex_Fix();

	if (GetCurrProfileBool(PO_TIME_CALIBRATION))
		ApplyTimeCalibration_Fix();

	if (GetCurrProfileBool(PO_FLIP_SCREEN))
		ApplyFlipScreen_Fix();

	if (GetCurrProfileBool(PO_DYNAMIC_LIGHT_SURFACES))
		ApplyDynamicLightSurfaces_Fix();

	if (GetCurrProfileBool(PO_STATIC_LIGHT_SURFACES))
		ApplyStaticLightSurfaces_Fix();

	if (GetCurrProfileBool(PO_FAST_CRC_CHECK))
		ApplyFastCRCCheck_Fix();

	DirectDrawCreateEx_Type DirectDrawCreateEx_fn = (DirectDrawCreateEx_Type)g_dwOriginalD3D;

	LPVOID FAR dummy;
	
	HRESULT hResult = DirectDrawCreateEx_fn(lpGUID, &dummy, iid, pUnkOuter);

	if (!GetCurrProfileBool(PO_CLEAN_MODE))
		*lplpDD = (LPDIRECTDRAW7) new FakeIDDraw7((LPDIRECTDRAW7)dummy);
	else
		*lplpDD = dummy;
	
	return hResult;
}

HMODULE WINAPI MyLoadLibraryA(LPCSTR lpFileName)
{
	HMODULE hModule = LoadLibraryA(lpFileName);
	
	if (_stricmp(lpFileName, D3D_REN) == 0)
	{ 
		HANDLE hProcess = GetCurrentProcess();
		DWORD dwDllAddress = (DWORD)GetModuleHandle(D3D_REN);
		
		EngineHack_WriteFunction(hProcess, (LPVOID)(dwDllAddress + ADDR_D3D_DDRAW_CREATE_EX), (DWORD)FakeDirectDrawCreateEx, g_dwOriginalD3D); // 0x46000
	}
	else if ((strstr(lpFileName, OBJECT_LTO_LOWER) || strstr(lpFileName, OBJECT_LTO_UPPER)) && GetCurrProfileDWord(PO_UPDATE_OBJECT_LTO))
	{
		HANDLE hProcess = GetCurrentProcess();
		DWORD dwDllAddress = (DWORD)GetModuleHandle(OBJECT_LTO_LOWER);

		BYTE anOldData[3];
		BYTE anXorEaxNop[3] = { 0x31, 0xc0, 0x90 };
		EngineHack_WriteData(hProcess, (LPVOID)(dwDllAddress + GetCurrProfileDWord(PO_UPDATE_OBJECT_LTO)), anXorEaxNop, anOldData, 3); // PH: 0xD56D5
	}
#ifdef PRIMAL_HUNT_BUILD
	else if (strstr(lpFileName, CRES_DLL_LOWER) || strstr(lpFileName, CRES_DLL_UPPER))
	{
		BYTE anOld[6];
		HANDLE hProcess = GetCurrentProcess();
		DWORD dwExeAddress = (DWORD)GetModuleHandle(LITHTECH_EXE);

		EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_LOAD_RENDER_LIB_CODE1), g_anLoadRenderLibCode1, anOld, 6);
		EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + ADDR_LOAD_RENDER_LIB_CODE2), g_anLoadRenderLibCode2, anOld, 6);
	}
#endif

	return hModule;
}
