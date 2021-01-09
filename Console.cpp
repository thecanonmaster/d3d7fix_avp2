#include "StdAfx.h"

BOOL g_bDrawConsole = FALSE;
CConsole* g_pConsole;
DWORD g_hBackground = NULL;
DWORD g_hVersion = NULL;
DWORD g_dwLastWidth;
DWORD g_dwLastHeight;
DWORD* g_pnConTextColor = NULL;

void Console_Init()
{
	if (g_dwWidth != g_dwLastWidth || g_dwHeight != g_dwLastHeight)
		Console_Term();

	if (!g_hBackground)
	{
		g_dwLastWidth = g_dwWidth;
		g_dwLastHeight = g_dwHeight;
		
		g_hBackground = g_pLTClient->CreateSurface(1, 1);
		g_pLTClient->SetPixel(g_hBackground, 0, 0, 0x00005500);
		g_pLTClient->SetSurfaceAlpha(g_hBackground, CONSOLE_BACKGROUND_ALPHA);
		g_pLTClient->OptimizeSurface(g_hBackground, 0);
	
		char szVersionStr[64];
		sprintf(szVersionStr, APP_NAME_SHORT, APP_VERSION);
		int nVersionStrLength = strlen(szVersionStr);
		g_hVersion = g_pLTClient->CreateSurface((FONT15_WIDTH + 1) * nVersionStrLength, FONT15_HEIGHT);
		DrawFont15String(szVersionStr, 0, 0, 1, 1, 0x00FFFFFF, g_hVersion);

		g_pLTClient->OptimizeSurface(g_hVersion, 0);
	}
}

void Console_Term()
{
	if (g_hBackground)
	{
		g_pLTClient->DeleteSurface(g_hBackground);
		g_hBackground = NULL;
	}

	if (g_hVersion)
	{
		g_pLTClient->DeleteSurface(g_hVersion);
		g_hVersion = NULL;
	}
}

void Console_Draw()
{	
	Console_Init();

	DWORD hScreen = g_pLTClient->GetScreenSurface();
	DWORD nHeight = (g_dwLastHeight >> 1);

	LTRect rcDest;
	rcDest.left = 0;
	rcDest.right = g_dwLastWidth;
	rcDest.top = 0;
	rcDest.bottom = nHeight;
	
	int nFontScaledHeight = FONT15_HEIGHT * CONSOLE_FONT_SCALE;

	g_pLTClient->ScaleSurfaceToSurface(hScreen, g_hBackground, &rcDest, NULL);

	DWORD nVersionSurfWidth = 0, nVersionSurfHeight = 0;
	g_pLTClient->GetSurfaceDims(g_hVersion, &nVersionSurfWidth, &nVersionSurfHeight);

	LTRect rcVersionDest;
	nVersionSurfWidth *= CONSOLE_FONT_SCALE;
	nVersionSurfHeight *= CONSOLE_FONT_SCALE;

	rcVersionDest.left = g_dwLastWidth - nVersionSurfWidth;
	rcVersionDest.right = rcVersionDest.left + nVersionSurfWidth;
	rcVersionDest.top = nHeight - nVersionSurfHeight - CONSOLE_LINE_SPACE;
	rcVersionDest.bottom = rcVersionDest.top + nVersionSurfHeight;
	g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, g_hVersion, &rcVersionDest, NULL, 0);

	if (g_pConsole)
	{
		int nMessagesPerScreen = nHeight / (nFontScaledHeight + CONSOLE_LINE_SPACE);
		
		int nLineDrawn = 0;
		int y = nHeight - nFontScaledHeight - CONSOLE_LINE_SPACE;

		GPOS pos = g_pConsole->m_TextLines.GetTail();

		while (pos && nLineDrawn < nMessagesPerScreen)
		{
			CConTextLine* pLine = g_pConsole->m_TextLines.GetPrev(pos);

			DrawFont15String(pLine->m_Text, CONSOLE_LINE_SPACE, y, 1, CONSOLE_FONT_SCALE, pLine->m_Color);

			y -= CONSOLE_LINE_SPACE + nFontScaledHeight;						
			pLine = g_pConsole->m_TextLines.GetTail();
			nLineDrawn++;
		}
	}
}