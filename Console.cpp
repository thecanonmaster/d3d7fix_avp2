#include "StdAfx.h"

BOOL g_bConsoleEnabled = FALSE;
CConsole* g_pConsole;
DWORD g_hBackground = NULL;
DWORD g_dwLastWidth;
DWORD g_dwLastHeight;

void Console_Init()
{
	if (g_dwWidth != g_dwLastWidth || g_dwHeight != g_dwLastHeight)
		Console_Term();

	if (!g_hBackground)
	{
		g_dwLastWidth = g_dwWidth;
		g_dwLastHeight = g_dwHeight;
		
		g_hBackground = g_pLTClient->CreateSurface(1, 1);
		g_pLTClient->SetPixel(g_hBackground, 0, 0, 0x0000AA00);
		g_pLTClient->SetSurfaceAlpha(g_hBackground, CONSOLE_BACKGROUND_ALPHA);
		g_pLTClient->OptimizeSurface(g_hBackground, 0);
	}
}

void Console_Term()
{
	if (g_hBackground)
		g_pLTClient->DeleteSurface(g_hBackground);
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