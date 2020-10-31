#include "StdAfx.h"

#define WIN32_LEAN_AND_MEAN	

typedef HRESULT (WINAPI* DirectDrawCreate_Type)(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
typedef HRESULT (WINAPI* DirectDrawCreateEx_Type)( GUID FAR *, LPVOID *, REFIID,IUnknown FAR *);
HMODULE WINAPI MyLoadLibraryA(LPCSTR lpFileName);
void HookEngineStuff1();

HRESULT __stdcall FakeD3DDevice_Load(LPDIRECT3DDEVICE7 lpDevice, LPDIRECTDRAWSURFACE7 lpDestTex,LPPOINT lpDestPoint,LPDIRECTDRAWSURFACE7 lpSrcTex,LPRECT lprcSrcRect,DWORD dwFlags)
{
	return lpDestTex->Blt(NULL, lpSrcTex, NULL, DDBLT_WAIT, 0); // FakeIDDrawSurface7LM_Blt(lpDestTex, NULL, lpSrcTex, NULL, DDBLT_WAIT, 0);
}

void GetD3D7FixVersion(char* szBuffer, BOOL bFullInfo)
{
	if (bFullInfo)
		sprintf(szBuffer, APP_NAME, APP_VERSION);
	else
		sprintf(szBuffer, "%.2f", APP_VERSION);
}

void ReadConfig(char* szFilename, char* szProfile)
{
	DWORD dwTemp;
	
	dwTemp = GetPrivateProfileInt("Global", GetCurrProfileOption(PO_DGVOODOO_MODE), FALSE, szFilename);
	SetCurrProfileBool(PO_DGVOODOO_MODE, dwTemp);
	
	if (!szProfile)
		szProfile = "Clean";

	char szSection[2048];
	DWORD dwSectionSize = GetPrivateProfileSection(szProfile, szSection, 2048, szFilename);
	if (!dwSectionSize)
	{
		szProfile = "Clean";
		dwSectionSize = GetPrivateProfileSection(szProfile, szSection, 2048, szFilename);
	}
	strcpy(g_szProfile, szProfile);	

	SectionToCurrProfileString(szSection, PO_DESCRIPTION);
	SectionToCurrProfileBool(szSection, PO_CLEAN_MODE, FALSE);

	if (GetCurrProfileBool(PO_CLEAN_MODE)) return;

	SectionToCurrProfileBool(szSection, PO_DONT_SHUTDOWN_RENDERER, FALSE);
	SectionToCurrProfileBool(szSection, PO_SHOW_FPS, FALSE);
	SectionToCurrProfileDWord(szSection, PO_FRAME_LIMITER_SLEEP, 0);
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
	SectionToCurrProfileBool(szSection, PO_MISC_CC, FALSE);
	SectionToCurrProfileBool(szSection, PO_RAW_MOUSE_INPUT, FALSE);
	SetCurrProfileFlag(PO_RAW_MOUSE_INPUT, GetCurrProfileBool(PO_RAW_MOUSE_INPUT));

	SectionToCurrProfileFloat(szSection, PO_RMI_SCALE_GLOBAL, 0.001f);
	SectionToCurrProfileFloat(szSection, PO_RMI_SCALE_Y, 1.1f);

	if (GetCurrProfileBool(PO_DGVOODOO_MODE))
	{
		SetCurrProfileBool(PO_INTEL_HD, FALSE);
		SetCurrProfileBool(PO_RADEON_5700, FALSE);
		SetCurrProfileBool(PO_LIGHT_LOAD, FALSE);
		SetCurrProfileBool(PO_TWM_DETAIL_TEX, FALSE);
	}

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
				g_LogFile = fopen("ltmsg.log", "w");			
				timeBeginPeriod(1);		
				HookEngineStuff1();
			}
		}
		break;

		case DLL_PROCESS_DETACH:
			if (strstr(g_szParentExeFilename, "lithtech"))
			{
				timeEndPeriod(1);
				ClearMultiLinesHolder();
				FontList_Clear(FALSE);
				fclose( g_LogFile );
			}
		break;
	}
	return TRUE;
}

void ApplyIntelHD_RHW_Fix()
{
	DWORD dwDLLAddress = (DWORD)GetModuleHandle("d3d.ren");
	logf("Applying IntelHD RHW fix");
	
	float fIntelHDFix = 0.5f;
	TLVertex* pVert = (TLVertex*)(dwDLLAddress + 0x58500);
	pVert[0].rhw = fIntelHDFix;
	pVert[1].rhw = fIntelHDFix;
	pVert[2].rhw = fIntelHDFix;
	pVert[3].rhw = fIntelHDFix;
	
	pVert = (TLVertex*)(dwDLLAddress + 0x58668);
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

float increaseHorFOV(float fFOVX, float fAspectRatio)
{
	float tempVradian = 2.0f * atanf(tanf(fFOVX / 2.0f) * 0.75f);
	return (2.0f * atanf(tanf(tempVradian / 2.0f) * fAspectRatio));
}

void (*OldSetCameraFOV)(DWORD hObj, float fovX, float fovY);
void MySetCameraFOV(DWORD hObj, float fovX, float fovY)
{
	fovX = increaseHorFOV(fovX, (float)(g_dwWidth * GetCurrProfileFloat(PO_CAMERA_FOV_SCALER)) / (float)g_dwHeight);
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
				int nSize = g_MultiLines.size();
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
			g_MultiLines.push_back(pString);

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

void DrawIntroduction()
{
	if (!g_hIntroductionSurface[0])
		CreateIntroductionSurface();

	DWORD hScreen = g_pLTClient->GetScreenSurface();

	for (int i = 0; i < INTRODUCTION_LINES; i++)
		g_pLTClient->DrawSurfaceToSurfaceTransparent(hScreen, g_hIntroductionSurface[i], NULL, 5, 5 + (i * INTRODUCTION_FONT_HEIGHT + 2), 0);
}

void DrawFrameRate()
{
	/*DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");
	bool & g_bInOptimized2D = *(bool*)(dwDllAddress + 0x5DE44);
	bool & g_bIn3D = *(bool*)(dwDllAddress + 0x5DE40);*/	
	
	if (!g_hFrameRateFontSurface[0])
		CreateFrameRateFontSurfaces();

	DWORD hScreen = g_pLTClient->GetScreenSurface();

	char szBuffer[64];
	itoa(g_nLastFrameRate, szBuffer, 10);

	int nLength = strlen(szBuffer);
	LTRect rcDest;
	rcDest.top = g_dwHeight - (FRAME_RATE_FONT_HEIGHT * FRAME_RATE_FONT_SCALE) - 5;
	rcDest.bottom = rcDest.top + (FRAME_RATE_FONT_HEIGHT * FRAME_RATE_FONT_SCALE);

	DWORD hFontColor = 0x0000FF00;
	if (g_nLastFrameRate < FRAME_RATE_LEVEL_RED)
		hFontColor = 0x00FF0000;
	else if (g_nLastFrameRate < FRAME_RATE_LEVEL_YELLOW)
		hFontColor = 0x00FFFF00;

	g_pLTClient->SetOptimized2DColor(hFontColor);

	for (int k = 0; k < nLength ; k++)
	{
		//g_pLTClient->DrawSurfaceToSurface(hScreen, g_hFrameRateFontSurface[szBuffer[k] - 0x30], NULL, nStartX + (k * (FRAME_RATE_FONT_HEIGHT + 1)), nStartY);	
		
		rcDest.left = 5 + (k * (FRAME_RATE_FONT_WIDTH + 1) * FRAME_RATE_FONT_SCALE);
		rcDest.right = rcDest.left + (FRAME_RATE_FONT_WIDTH * FRAME_RATE_FONT_SCALE);
		g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, g_hFrameRateFontSurface[szBuffer[k] - 0x30], &rcDest, NULL, 0);
	}
	g_pLTClient->SetOptimized2DColor(0x00FFFFFF);
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

	if (fTime - g_fIntroductionStartTime < INTRODUCTION_TIME)
		DrawIntroduction();
	
	if (GetCurrProfileBool(PO_SHOW_FPS) && g_bDrawFPS)
		DrawFrameRate();
	
	return OldEndOptimized2D();
}

typedef BOOL (__stdcall *DIEnumDevicesCallback_type)(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
BOOL (__stdcall *DIEnumDevicesCallback)(LPCDIDEVICEINSTANCE lpddii, LPVOID pvRef);
BOOL __stdcall MyDIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	BYTE* anDevType = (BYTE*)&lpddi->dwDevType;
	
	if (anDevType[0] != DIDEVTYPE_MOUSE && anDevType[0] != DIDEVTYPE_KEYBOARD /*&& !(lpddi->dwDevType & DIDEVTYPE_HID)*/)
	{
		logf("DirectInput non-mouse/keyboard device \"%s\"", lpddi->tszInstanceName);
		return DIENUM_CONTINUE;
	}
	
	return DIEnumDevicesCallback(lpddi, pvRef);
}

DWORD (*OldCreateObject)(ObjectCreateStruct *pStruct);
DWORD MyCreateObject(ObjectCreateStruct *pStruct)
{
	if (pStruct->m_ObjectType == OT_LIGHT)
		pStruct->m_Flags |= FLAG_DONTLIGHTBACKFACING;
	
	return OldCreateObject(pStruct);
}

void HookEngineStuff1()
{
	logf("Hooking engine stuff #1");
	
	DWORD dwRead;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle("lithtech.exe");
	
	EngineHack_WriteFunction(hProcess, (LPVOID)(dwExeAddress + 0x0C6100), (DWORD)MyLoadLibraryA, dwRead);

	DWORD dwEnum = (DWORD)MyDIEnumDevicesCallback;
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + 0x03EEC6), (BYTE*)&dwEnum, (BYTE*)&dwRead, 4);
	DIEnumDevicesCallback = (DIEnumDevicesCallback_type)dwRead;
}

void HookEngineStuff2()
{
	logf("Hooking engine stuff #2");

	DWORD dwRead;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle("lithtech.exe");
	g_pClientMgr = (CClientMgrBase*)(dwExeAddress + 0xDEFAC);
	g_pServerMgr = (CServerMgrBase*)(dwExeAddress + 0xE5DC8);
	g_pLTClient = g_pClientMgr->m_pClientMgr->m_pLTClient;

	DWORD* pOrigTable = (DWORD*)*(DWORD*)g_pLTClient;

	ILTCSBase_CPrint = (ILTCSBase_CPrint_type)pOrigTable[39]; 	
	ILTCSBase_CreateString = (ILTCSBase_CreateString_type)pOrigTable[46];
	ILTCSBase_FreeString = (ILTCSBase_FreeString_type)pOrigTable[47];
	
	ILTCSBase_GetStringData = (ILTCSBase_GetStringData_type)pOrigTable[50];
	ILTCSBase_GetVarValueFloat = (ILTCSBase_GetVarValueFloat_type)pOrigTable[51];
	ILTCSBase_GetVarValueString = (ILTCSBase_GetVarValueString_type)pOrigTable[52];
	ILTCSBase_GetTime = (ILTCSBase_GetTime_type)pOrigTable[53];
	ILTCSBase_GetFrameTime = (ILTCSBase_GetFrameTime_type)pOrigTable[54];
	
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
		
		IClientShell_Update = (IClientShell_Update_type)pOrigTable[15];
		EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + 15), (DWORD)MyIClientShell_Update, dwRead);
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
	}
	
	BYTE nNew = 0x03;
	BYTE nOld;
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + 0x9B5B5), &nNew, &nOld, 1);
}

void ApplyLightLoad_Fix()
{
	logf("Applying light load fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");
	//DWORD dwRenderStruct = dwDllAddress + 0x58470;

	EngineHack_WriteCall(hProcess, (LPVOID)(DWORD(dwDllAddress) + 0x34CF2), (DWORD)FakeD3DDevice_Load, TRUE);
}

BOOL* g_pDetailTextureCapable;
typedef void (*sub_3F0A2A7_type)();
void (*sub_3F0A2A7)();
void My_sub_3F0A2A7()
{
	*g_pDetailTextureCapable = FALSE;
	sub_3F0A2A7();
	*g_pDetailTextureCapable = TRUE;
}

void ApplyTWMDetailTex_Fix()
{
	logf("Applying TWM detail textures fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");

	g_pDetailTextureCapable = (BOOL*)(dwDllAddress + 0x5DE2C);
	sub_3F0A2A7 = (sub_3F0A2A7_type)(dwDllAddress + 0xA2A7);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + 0x9C5A), (DWORD)My_sub_3F0A2A7, FALSE);
}

float g_fServerFrameTimeClamp = 0.0f;
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
	DWORD dwExeAddress = (DWORD)GetModuleHandle("lithtech.exe");

	dsi_ClientSleep = (dsi_ClientSleep_type)(dwExeAddress + 0x35070);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwExeAddress + 0x10E06), (DWORD)My_dsi_ClientSleep, FALSE);

	pd_Update = (pd_Update_type)(DWORD(dwExeAddress) + 0x6E250);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwExeAddress + 0x15374), (DWORD)My_pd_Update, FALSE);

	DWORD dwNew = (DWORD)(&g_fServerFrameTimeClamp);
	BYTE anOld[4];
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + 0x83688), (BYTE*)(&dwNew), anOld, 4);
	EngineHack_WriteData(hProcess, (LPVOID)(dwExeAddress + 0x8369D), (BYTE*)(&dwNew), anOld, 4);

	UpdateSounds = (UpdateSounds_type)(dwExeAddress + 0x83370);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwExeAddress + 0x836D7), (DWORD)MyUpdateSounds, FALSE);
}

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
	DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");

	DWORD* dwSurfaceCounts = (DWORD*)(dwDllAddress + 0x4BF50);
	dwSurfaceCounts[0] = 256;
	dwSurfaceCounts[1] = 256;
	dwSurfaceCounts[2] = 256;
	dwSurfaceCounts[3] = 256;
	dwSurfaceCounts[4] = 256;
}

typedef DWORD (*r_FindFreeSlot_type)(void *pContext, DWORD dwWidth, DWORD dwHeight, DWORD *nX, DWORD *nY, void **param_6);
DWORD (*r_FindFreeSlot)(void *pContext, DWORD dwWidth, DWORD dwHeight, DWORD *nX, DWORD *nY, void **param_6);

DWORD My_r_FindFreeSlot(void *pContext, DWORD dwWidth, DWORD dwHeight, DWORD *nX, DWORD *nY, void **param_6)
{
	return r_FindFreeSlot(pContext, 64, 64, nX, nY, param_6);
}

void ApplyStaticLightSurfaces_Fix()
{
	logf("Applying static light surfaces fix");
	
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");
	
	r_FindFreeSlot = (r_FindFreeSlot_type)(dwDllAddress + 0x34000);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + 0x342FC), (DWORD)My_r_FindFreeSlot, FALSE);	
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
	
	if (_stricmp(lpFileName, "d3d.ren") == 0)
	{
		HANDLE hProcess = GetCurrentProcess();
		HMODULE hModule = GetModuleHandle("d3d.ren");
		
		EngineHack_WriteFunction(hProcess, (LPVOID)(DWORD(hModule) + 0x46000), (DWORD)FakeDirectDrawCreateEx, g_dwOriginalD3D);
	}

	return hModule;
}