#include "StdAfx.h"

BOOL g_bDrawConsole = FALSE;
CConsole* g_pConsole;
DWORD g_hBackground = NULL;
DWORD g_hVersion = NULL;
DWORD g_dwLastWidth;
DWORD g_dwLastHeight;
DWORD* g_pnConTextColor = NULL;


std::string m_szCommand;
std::vector<std::string> m_szPrevCommands;
int m_nPrevCommandIndex = 0;
UINT32 m_nCursorPositionTimer = 0;

void Console_Init()
{
	if (g_dwWidth != g_dwLastWidth || g_dwHeight != g_dwLastHeight)
		Console_Term();

	if (!g_hBackground)
	{
		g_dwLastWidth = g_dwWidth;
		g_dwLastHeight = g_dwHeight;

		DWORD dwColor = strtoul(GetCurrProfileString(PO_CONSOLE_BACKGROUND), NULL, 0);
		dwColor = 0xAA191919;
		g_hBackground = g_pLTClient->CreateSurface(1, 1);

		g_pLTClient->SetPixel(g_hBackground, 0, 0, dwColor & 0x00FFFFFF);

		g_pLTClient->SetSurfaceAlpha(g_hBackground, (float)(dwColor >> 24) / 255.0f);
		g_pLTClient->OptimizeSurface(g_hBackground, 0);
	
		char szVersionStr[64];
		sprintf(szVersionStr, APP_NAME_SHORT, APP_VERSION);
		int nVersionStrLength = strlen(szVersionStr);
		g_hVersion = g_pLTClient->CreateSurface((FONT15_WIDTH + 1) * nVersionStrLength, FONT15_HEIGHT);
		DrawFont15String(szVersionStr, 0, 0, 1, 1, 0x00FFFFFF, g_hVersion);

		g_pLTClient->OptimizeSurface(g_hVersion, 0);
	}

	m_nCursorPositionTimer = GetTickCount();
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

static std::string wstrtostr(const std::wstring& wstr)
{
	// Convert a Unicode string to an ASCII string
	std::string strTo;
	char* szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
}

static std::wstring strtowstr(const std::string& str)
{
	// Convert an ASCII string to a Unicode String
	std::wstring wstrTo;
	wchar_t* wszTo = new wchar_t[str.length() + 1];
	wszTo[str.size()] = L'\0';
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
	wstrTo = wszTo;
	delete[] wszTo;
	return wstrTo;
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
	rcDest.bottom = nHeight + 15;
	
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

			DrawFont15String(pLine->m_Text, CONSOLE_LINE_SPACE, y, 2, CONSOLE_FONT_SCALE, pLine->m_Color);

			y -= CONSOLE_LINE_SPACE + nFontScaledHeight;						
			pLine = g_pConsole->m_TextLines.GetTail();
			nLineDrawn++;
		}
		
		//Draw text input area
		DrawFont15String(">", CONSOLE_LINE_SPACE, nHeight + (nFontScaledHeight - 8), 2, CONSOLE_FONT_SCALE, 0x00ffffff);
		DrawFont15String((char*)m_szCommand.c_str(), CONSOLE_LINE_SPACE + 10, nHeight + (nFontScaledHeight - 8), 2, CONSOLE_FONT_SCALE, 0x00ffff00);
		//Drawblinking cursor
		if (m_nCursorPositionTimer % 1000 < 500)
			DrawFont15String("_", CONSOLE_LINE_SPACE + 10 + (m_szCommand.length() * 10), nHeight + (nFontScaledHeight - 8), 2, CONSOLE_FONT_SCALE, 0x00ffff00);
	}
	
	m_nCursorPositionTimer = GetTickCount();
}

/// <summary>
/// Convert to lower case.
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
std::string str_tolower(std::string s) 
{
	std::transform(s.begin(), s.end(), s.begin(),[](unsigned char c) { return std::tolower(c); });
	return s;
}

/// <summary>
/// Prints the message to the console with color UINT32 arranged as ARGB
/// </summary>
/// <param name="szMessage">Message to print</param>
/// <param name="dwColor">ARGB</param>
void PrintMessageToConsole(char* szMessage, DWORD dwColor)
{
	DWORD dwOldColor = *g_pnConTextColor;
	*g_pnConTextColor = dwColor;
	ILTCSBase_CPrint(g_pLTClient, szMessage);
	*g_pnConTextColor = dwOldColor;
}

/// <summary>
/// Prints the message to the console with RGB values as color input. 0-255
/// </summary>
/// <param name="szMessage">Message to print</param>
/// <param name="r">Red</param>
/// <param name="g">Green</param>
/// <param name="b">Blue</param>
void PrintMessageToConsole(char* szMessage, UINT8 r, UINT8 g, UINT8 b)
{
	DWORD dwColor = (r << 16) | (g << 8) | b;
	PrintMessageToConsole(szMessage, dwColor);
}

void CConsole::ProcessCommand()
{
	//convert to lowercase
	std::string szCommand = str_tolower(m_szCommand);

	m_szPrevCommands.push_back(m_szCommand);
	m_nPrevCommandIndex = m_szPrevCommands.size() - 1;

	//remove commands more than 50
	if (m_szPrevCommands.size() > 4)
		m_szPrevCommands.erase(m_szPrevCommands.begin());
	

	if (szCommand.find("wireframe") != std::string::npos)
	{

		//check if string has a 1 or a 0 after
		if (szCommand.find("1") != std::string::npos)
		{
			PrintMessageToConsole("Wireframe Enabled", 0x0000ff00);
		}
		else if (szCommand.find("0") != std::string::npos)
		{
			PrintMessageToConsole("Wireframe Disabled", 0x00ba3729);
		}
	}

	if (szCommand.find("drawflat") != std::string::npos)
	{

		//check if string has a 1 or a 0 after
		if (szCommand.find("1") != std::string::npos)
		{
			PrintMessageToConsole("Drawing all polygons in flat shades", 0x0000ff00);
		}
		else if (szCommand.find("0") != std::string::npos)
		{
			PrintMessageToConsole("Drawing all polygons normally", 0x00ba3729);
		}
	}

	if (szCommand.find("clip") != std::string::npos)
	{
		g_pLTClient->RunConsoleString("cheat mpsixthsense");
	}
	
	if (szCommand.find("god") != std::string::npos)
	{
		g_pLTClient->RunConsoleString("cheat mpcanthurtme");
	}
	
	if (szCommand.find("weapons") != std::string::npos)
	{
		g_pLTClient->RunConsoleString("cheat mpschuckit");
	}
	
	g_pLTClient->RunConsoleString((char*)m_szCommand.c_str());
	
	m_szCommand.erase();
}