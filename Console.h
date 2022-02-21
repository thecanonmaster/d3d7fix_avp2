#define MAX_CONSOLE_TEXTLEN 256
#define CONSOLE_FONT_SCALE 2
#define CONSOLE_LINE_SPACE 2

typedef void (*ErrorLogFn)(char *pMsg);

class CGLLNode
{
public:
	
	CGLLNode	*m_pGPrev, *m_pGNext;
	
};

typedef CGLLNode* GPOS;

class CConTextLine : public CGLLNode
{
public:
	
	DWORD		m_Color;
	char		m_Text[MAX_CONSOLE_TEXTLEN];
};

class CConTextList
{
public:

	DWORD*			m_pVTable;
	DWORD			m_nElements;
	CGLLNode		*m_pHead;

	inline CConTextLine* GetHead() const
	{
		return (CConTextLine*)m_pHead;
	}

	inline CConTextLine* GetTail() const
	{
		return (CConTextLine*)m_pHead->m_pGPrev;
	}

	inline CConTextLine* GetNext( GPOS &pos ) const
	{
		CConTextLine* ret = (CConTextLine*)pos;
		
		if( pos->m_pGNext == m_pHead )
			pos = NULL;
		else
			pos = pos->m_pGNext;
		
		return ret;
	}
		
	inline CConTextLine* GetPrev( GPOS &pos ) const
	{
		CConTextLine* ret = (CConTextLine*)pos;
		
		if( pos->m_pGPrev == m_pHead )
			pos = NULL;
		else
			pos = pos->m_pGPrev;
		
		return ret;
	}
};

class CConsole
{
public:
	
	BYTE			m_Data1[944];
	
	ErrorLogFn		m_ErrorLogFn;
	
	HWND			m_hWnd;
	
	CConTextList	m_TextLines;
	DWORD			m_nTextLines;
};

extern BOOL g_bDrawConsole;
extern CConsole* g_pConsole;
extern DWORD* g_pnConTextColor;

void Console_Init();
void Console_Term();
void Console_Draw();