enum eProfileOptionType
{
	POT_BYTE = 0,
	POT_FLOAT = 1,
	POT_STRING,
	POT_DWORD,
	POT_MAX,
};

enum eProfileOption
{
	PO_DGVOODOO_MODE = 0,
	PO_DESCRIPTION = 1,
	PO_CLEAN_MODE,
	PO_DONT_SHUTDOWN_RENDERER,
	PO_SHOW_FPS,
	PO_FRAME_LIMITER_SLEEP,
	PO_CAMERA_FOV_SCALER,
	PO_MAX_FPS,
	PO_INTEL_HD,
	PO_RADEON_5700,
	PO_CAMERA_FOV,
	PO_VIEW_MODEL_FOV,
	PO_SOLID_DRAWING,
	PO_LIGHT_LOAD,
	PO_MISC_CC,
	PO_RAW_MOUSE_INPUT,
	PO_TWM_DETAIL_TEX,
	PO_TIME_CALIBRATION,
	PO_FLIP_SCREEN,
	PO_DYNAMIC_LIGHT_SURFACES,
	PO_STATIC_LIGHT_SURFACES,
	PO_PRELOAD_STATIC_LIGHT,
	PO_FULLSCREEN_OPTIMIZE,
	PO_NOVSYNC,
	PO_RMI_SCALE_GLOBAL,
	PO_RMI_SCALE_Y,
	PO_POSTPROCESS_ENABLED,
	PO_POSTPROCESS_INTENSITY,
	PO_POSTPROCESS_INTENSITY_MENU,
	PO_MAX,
};

struct ProfileOption
{
	ProfileOption(eProfileOptionType eOptionType, char* szOptionName)
	{
		Init(eOptionType, szOptionName);		
	}

	inline void Init(eProfileOptionType eOptionType, char* szOptionName)
	{
		memset(this, 0, sizeof(ProfileOption));
		eType = eOptionType;
		strcpy(szName, szOptionName);
	}

	union 
	{
		DWORD	dwValue;
		BOOL	bValue;
		float	fValue;
		char	szValue[256];
	};

	
	DWORD GetDWord()	{ return dwValue; }
	BOOL GetBool()		{ return bValue; }
	float GetFloat()	{ return fValue; }
	char* GetString()	{ return szValue; }

	void SetDWord(DWORD dwSet)	{ dwValue = dwSet; }
	void SetBool(BOOL bSet)		{ bValue = bSet; }
	void SetFloat(float fSet)	{ fValue = fSet; }
	void SetString(char* szSet) { strcpy(szValue, szSet); }

	BOOL				bFlag;
	char				szName[128];
	eProfileOptionType	eType;
}; 

extern FILE* g_LogFile;
//extern ProfileOption g_ProfileOptions[PO_MAX];
extern char g_szProfile[64];
extern BOOL g_bDrawFPS;
extern DWORD g_dwWidth;
extern DWORD g_dwHeight;
extern BOOL g_bWindowedSet;
extern BOOL g_bWindowed;
extern LPDIRECTDRAWSURFACE7 g_ddsBackBuffer;
extern LPDIRECT3DDEVICE7 g_d3dDevice;
extern LPDIRECTDRAW7 g_ddMainDDraw;
extern HWND g_hWindowHandle;
extern BOOL g_bWindowHooked;
extern float g_fLastFontListUpdate;

extern LONG g_lRMILastX;
extern LONG g_lRMILastY;
//extern DWORD g_hMouseSensitivity;

extern int g_nLastFrameRate;

#ifdef _DEBUG
	#define APP_NAME		"D3D7FIX v%.2f for Aliens vs Predator 2 (ltmsg.dll, DEBUG)"
#else
	#define APP_NAME		"D3D7FIX v%.2f for Aliens vs Predator 2 (ltmsg.dll)"
#endif
#define APP_VERSION		0.30f
#define CVAR_PROFILE	"D3D7FixProfile"

#define FONT_LIST_UPDATE_TIME		5.0f
#define FONT_LIST_CLEANUP_TIME		180.0f
#define FONT_STRING_CLEANUP_TIME	30.0f
#define FONT_STRING_LENGTH			1024
#define FONT_STRING_LINE_RESERVE	4

#define INTRODUCTION_FONT_HEIGHT	18
#define INTRODUCTION_FONT_WIDTH		10
#define INTRODUCTION_TIME			30.0f
#define INTRODUCTION_LINES			5

#define FRAME_RATE_FONT_HEIGHT	5
#define FRAME_RATE_FONT_WIDTH	3
#define FRAME_RATE_FONT_SCALE	2
#define FRAME_RATE_UPDATE_TIME	0.2f
#define FRAME_RATE_LEVEL_RED	30
#define FRAME_RATE_LEVEL_YELLOW	55

#define FLIPSCREEN_CANDRAWCONSOLE	(1<<0)
#define FLIPSCREEN_COPY				(1<<1)
#define FLIPSCREEN_DIRTY			(1<<2)

#define DIDEVTYPE_MOUSE         2
#define DIDEVTYPE_KEYBOARD      3

#define OT_LIGHT 4
#define FLAG_DONTLIGHTBACKFACING	(1<<6)

#define FLIP_FLAGS	(/*DDFLIP_NOVSYNC | */DDFLIP_WAIT)

#define TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR  | D3DFVF_TEX1 )

struct ObjectCreateStruct
{
	WORD			m_ObjectType;
	WORD			m_ContainerCode;
	DWORD			m_CreateFlags;
	DWORD			m_Flags;
	DWORD			m_Flags2;
	// more there
};

struct TLVertex
{	
	float m_VecX;
	float m_VecY;
	float m_VecZ;
	float rhw;	
	DWORD color1;
	DWORD color2;
	float tu, tv;
};

/*struct SolidSurface
{
	SolidSurface(DWORD hColor, DWORD hSurface)
	{
		m_hColor = hColor;
		m_hSurface = hSurface;
	}

	DWORD	m_hColor;
	DWORD	m_hSurface;
};

typedef std::list<SolidSurface*> SolidSurfaceList;*/

struct LTRect
{
	int left;
	int top;
	int right;
	int bottom;
};

struct FontString;
typedef std::vector<FontString*> FSLines;

struct FontString
{
	FontString(DWORD hSurface, char* szString, float fTime)
	{
		m_hSurface = hSurface;
		strncpy(m_szString, szString, FONT_STRING_LENGTH);
		m_szString[FONT_STRING_LENGTH - 1] = 0;
		m_fTime = fTime;
		/*m_pLines = NULL;
		m_rcSrc.left = 0;
		m_rcSrc.top = 0;
		m_rcSrc.right = 0;
		m_rcSrc.bottom = 0;*/
	}

	/*~FontString()
	{
		if (m_pLines)
			delete m_pLines;
	}*/
	
	DWORD			m_hSurface;
	char			m_szString[FONT_STRING_LENGTH];	
	float			m_fTime;
	/*FSLines*		m_pLines;
	LTRect			m_rcSrc;*/
};

typedef std::list<FontString*> FontStringList;

struct Font
{
	Font(DWORD hFont, float fTime)
	{
		m_hFont = hFont;
		m_fTime = fTime;
	}

	DWORD			m_hFont;
	float			m_fTime;
	FontStringList	m_StringList;
};

typedef std::list<Font*> FontList;

extern FontList g_FontList;
extern DWORD g_hWhitePixelSurface;
extern float g_fIntroductionStartTime;
extern DWORD g_hIntroductionSurface[INTRODUCTION_LINES];
extern DWORD g_hFrameRateFontSurface[10];
//extern SolidSurfaceList g_SolidSurfaceList;

extern DWORD g_hFontSearch;
extern char* g_szStringSearch;
extern DWORD g_hSurfaceSearch;
extern DWORD g_hColorSearch;

Font* FontList_FindItem(DWORD hFont);
FontString* FontStringList_FindItem(DWORD hSurface);
FontString* FontStringList_FindItem(FontStringList* pList, char* szString);
FontString* FontStringList_FindItem(FontStringList* pList, DWORD hSurface);
bool FontStringList_HandleEqualFn_ByString(const FontString* pItem);
bool FontStringList_HandleEqualFn_BySurface(const FontString* pItem);
bool FontList_HandleEqualFn(const Font* pItem);
void FontList_Update();
void FontList_Clear(BOOL bDeleteSurfaces);

union GenericColor 
{
    DWORD  dwVal;
    WORD  wVal;
    BYTE   bVal;
};

struct SurfaceTile
{
	void*					m_SrcImageRect;
	DWORD					m_nTileWidth;
	DWORD					m_nTileHeight;
	
	LPDIRECTDRAWSURFACE7	m_pTexture;
};

struct SurfaceTiles
{
	DWORD					m_nTilesX, m_nTilesY;
	SurfaceTile				m_Tiles[1];	
};

struct RSurface
{
	LPDIRECTDRAWSURFACE7	m_pSurface;
	_DDSURFACEDESC2			m_Desc;
	GenericColor			m_LastTransparentColor;	
	SurfaceTiles*			m_pTiles;
	bool					m_bTilesTransparent;
};

class BlitRequest
{
public:
	
	BlitRequest()
	{
		m_hBuffer = NULL;
		m_BlitOptions = 0;
		m_pSrcRect = NULL;
		m_pDestRect = NULL;
		m_Alpha = 1.0f;
	}
	
	
public:
	
	DWORD		   m_hBuffer;         
	DWORD          m_BlitOptions;      
	GenericColor   m_TransparentColor; 
	void*         *m_pSrcRect;        
	void*         *m_pDestRect;       
	float          m_Alpha;           
	
	void*		   *m_pWarpPts;
	int            m_nWarpPts;
};

class CisSurface
{
public:
	BYTE		m_Data1[8];
	BYTE       *m_pBackupBuffer;
	DWORD		m_hBuffer;
	DWORD       m_Width, m_Height;
	long        m_Pitch;
	DWORD       m_Flags;
	DWORD       m_OptimizedTransparentColor;
	void        *m_pUserData;
	float       m_Alpha;
};

#define MAX_RESTREES            20

class ILTCSBase
{
public:

	virtual DWORD StartHMessageWrite()=0;
};

typedef void (__cdecl *ILTCSBase_CPrint_type)(ILTCSBase* pBase, char *pMsg, ...);
typedef DWORD (__stdcall *ILTCSBase_CreateString_type)(char *pString);
typedef void (__stdcall *ILTCSBase_FreeString_type)(DWORD hString);
typedef char* (__stdcall *ILTCSBase_GetStringData_type)(DWORD hString);
typedef float (__stdcall *ILTCSBase_GetVarValueFloat_type)(DWORD hVar);
typedef char* (__stdcall *ILTCSBase_GetVarValueString_type)(DWORD hVar);
typedef float (__fastcall *ILTCSBase_GetTime_type)(ILTCSBase* pBase);
typedef float (__fastcall *ILTCSBase_GetFrameTime_type)(ILTCSBase* pBase);

typedef void (__fastcall *IClientShell_Update_type)(void* pShell);

extern void (__cdecl *ILTCSBase_CPrint)(ILTCSBase* pBase, char *pMsg, ...);
extern DWORD (__stdcall *ILTCSBase_CreateString)(char *pString);
extern void (__stdcall *ILTCSBase_FreeString)(DWORD hString);
extern char* (__stdcall *ILTCSBase_GetStringData)(DWORD hString);
extern float (__stdcall *ILTCSBase_GetVarValueFloat)(DWORD hVar);
extern char* (__stdcall *ILTCSBase_GetVarValueString)(DWORD hVar);
extern float (__fastcall *ILTCSBase_GetTime)(ILTCSBase* pBase);
extern float (__fastcall *ILTCSBase_GetFrameTime)(ILTCSBase* pBase);

extern void (__fastcall *IClientShell_Update)(void* pShell);


class ILTClient: public ILTCSBase
{
public:

	BYTE			m_Data0[52];
	void			(*Shutdown)();
	void			(*ShutdownWithMessage)( char *pMsg, ... );
	DWORD			(*FlipScreen)(DWORD flags);	
	BYTE			m_Data1[14]; // 18 34
	DWORD			(*StartOptimized2D)();
	DWORD			(*EndOptimized2D)();
	DWORD			(*SetOptimized2DBlend)(DWORD blend);
	DWORD			(*GetOptimized2DBlend)(DWORD &blend);
	DWORD			(*SetOptimized2DColor)(DWORD hColor);
	DWORD			(*GetOptimized2DColor)(DWORD &hColor);
	DWORD			(*End3D)();
	void*			(*GetRenderModes)();
	void			(*RelinquishRenderModes)(void *pModes);
	DWORD			(*GetRenderMode)(void *pMode);
	DWORD			(*SetRenderMode)(void *pMode);
	DWORD			(*ShutdownRender)(DWORD flags);	
	void*			(*GetFileList)(char *pDirName);	
	void			(*FreeFileList)(void *pHead);	
	DWORD			(*GetWorldInfoString)(char *pFilename, char *pInfoString, DWORD maxLen, DWORD *pActualLen);
	DWORD			(*ReadConfigFile)(char *pFilename);
	DWORD			(*WriteConfigFile)(char *pFilename);
	void			(*GetAxisOffsets)(float *offsets);	
	BYTE			m_Data2[164]; // 200

	DWORD			(*CreateFont)(char *pFontName, int width, int height, BOOL bItalic, BOOL bUnderline, BOOL bBold);
	void			(*DeleteFont)(DWORD hFont);
	DWORD			(*SetFontExtraSpace)(DWORD hFont, int pixels);
	DWORD			(*GetFontExtraSpace)(DWORD hFont, int &pixels);
	DWORD			(*CreateColor)(float r, float g, float b, BOOL bTransparent);
	void			(*DeleteColor)(DWORD hColor);
	DWORD			(*SetupColor1)(float r, float g, float b, BOOL bTransparent);
	DWORD			(*SetupColor2)(float r, float g, float b, BOOL bTransparent);
	DWORD			(*GetBorderSize)(DWORD hSurface, DWORD hColor, LTRect *pRect);	
	
	DWORD			(*OptimizeSurface)(DWORD hSurface, DWORD hTransparentColor);
	DWORD			(*UnoptimizeSurface)(DWORD hSurface);	
	DWORD			(*GetScreenSurface)();
	DWORD			(*CreateSurfaceFromBitmap)(char *pBitmapName);
	DWORD			(*CreateSurfaceFromString)(DWORD hFont, DWORD hString, DWORD hForeColor, DWORD hBackColor, int extraPixelsX, int extraPixelsY);
	DWORD			(*CreateSurface)(DWORD width, DWORD height);
	DWORD			(*DeleteSurface)(DWORD hSurface);
	void*			(*GetSurfaceUserData)(DWORD hSurface);
	void			(*SetSurfaceUserData)(DWORD hSurface, void *pUserData);
	DWORD			(*GetPixel)(DWORD hSurface, DWORD x, DWORD y, DWORD *color);
	DWORD			(*SetPixel)(DWORD hSurface, DWORD x, DWORD y, DWORD color);
	void			(*GetStringDimensions)(DWORD hFont, DWORD hString, int *sizeX, int *sizeY);
	
	void			(*DrawStringToSurface)(DWORD hDest, DWORD hFont, DWORD hString, LTRect* pRect, DWORD hForeColor, DWORD hBackColor);	
	void			(*GetSurfaceDims)(DWORD hSurf, DWORD *pWidth, DWORD *pHeight);	
	DWORD			(*DrawBitmapToSurface)(DWORD hDest, char *pSourceBitmapName, LTRect *pSrcRect, int destX, int destY);	
	DWORD			(*DrawSurfaceMasked)(DWORD hDest, DWORD hSrc, DWORD hMask, LTRect *pSrcRect, int destX, int destY, DWORD hColor);
	DWORD			(*DrawSurfaceSolidColor)(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY, DWORD hTransColor, DWORD hFillColor);
	DWORD			(*DrawSurfaceToSurface)(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY);
	DWORD			(*DrawSurfaceToSurfaceTransparent)(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY, DWORD hColor);
	DWORD			(*ScaleSurfaceToSurface)(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect);
	DWORD			(*ScaleSurfaceToSurfaceTransparent)(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect, DWORD hColor);	
	DWORD			(*ScaleSurfaceToSurfaceSolidColor)(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect, DWORD hTransColor, DWORD hFillColor); // 444
	BYTE			m_Data4[20];
	DWORD			(*FillRect)(DWORD hDest, LTRect *pRect, DWORD hColor);
	DWORD			(*GetSurfaceAlpha)(DWORD hSurface, float &alpha);
	DWORD			(*SetSurfaceAlpha)(DWORD hSurface, float alpha);
	DWORD			(*RegisterConsoleProgram)(char *pName, void* fn);	
	DWORD			(*UnregisterConsoleProgram)(char *pName);	
	DWORD			(*GetConsoleVar)(char *pName);
	BYTE			m_Data5[88]; 
	void			(*RunConsoleString)(char *pString);
	BYTE			m_Data6[32];
	DWORD			(*CreateObject)(ObjectCreateStruct *pStruct);
	BYTE			m_Data7[64]; // 100
	void			(*SetCameraFOV)(DWORD hObj, float fovX, float fovY);
};

extern ILTClient* g_pLTClient;

/*class CClientMgr
{	
public:
	
	BYTE			m_Data0[1220];
	ILTClient*		m_pLTClient;

	BYTE			m_Data00[344];
	void*			m_pClientShell;

	BYTE			m_Data1[328]; // 676
	float			m_fGlobalLightScaleR;
	float			m_fGlobalLightScaleG;
	float			m_fGlobalLightScaleB;
	
	BYTE			m_Data2[3785];
	BYTE*			m_pClientFileMgr;
	char*			m_ResTrees[MAX_RESTREES];
    DWORD			m_nResTrees;
};*/

class CClientShell // 5660
{
public:
	
	BYTE m_Data1[20];
	float m_LastGameTime;
	float m_GameTime;
	float m_GameFrameTime;
};

class CClientMgr
{	
public:
	
	BYTE			m_Data1[1220];
	ILTClient*		m_pLTClient;

	BYTE			m_Data11[344];
	void*			m_pClientShell;
	
	BYTE			m_Data2[3392]; // 3740
	float			m_CurTimeCopy;
	float			m_FrameTime;
	float			m_CurTime;
	BYTE			m_Data3[684];
	CClientShell*	m_pCurShell;
	BYTE			m_Data4[36];
	void*			m_pClientFileMgr;
	char*			m_ResTrees[MAX_RESTREES];
    DWORD			m_nResTrees;
	void*			m_pDemoMgr;
};

class CClientMgrBase
{
public:
	
	CClientMgr*		m_pClientMgr;
	//BYTE			m_Data0[388];
	//void*			m_pClientShell;
};

class CServerMgr
{	
public:
	
	BYTE			m_Data1[2668];
	DWORD			m_dwCRC32CheckResult;
	BYTE			m_Data2[76]; // 84
	float			m_FrameTime;
	float			m_LastServerFPS;
	float			m_TimeOffset;
	BYTE			m_Data3[16]; // 24
	float			m_GameTime;
	DWORD			m_nTargetTimeSteps;
	float			m_TrueFrameTime;
	float			m_TrueLastTime;
	BYTE			m_Data4[852]; // 884 972 3644
	void*			m_pServerFileMgr;
};

class CServerMgrBase
{
public:
	
	CServerMgr* m_pServerMgr;
};

extern CServerMgrBase* g_pServerMgr;
extern CClientMgrBase* g_pClientMgr;

void CreateFrameRateFontSurfaces();
void CreateIntroductionSurface();
BOOL RegisterRawMouseDevice();
void ProcessRawMouseInput(LPARAM lParam, LONG& lLastX, LONG& lLastY);
void EngineHack_WriteData(HANDLE hProcess, LPVOID lpAddr, BYTE* pNew, BYTE* pOld, DWORD dwSize);
void EngineHack_WriteFunction(HANDLE hProcess, LPVOID lpAddr, DWORD dwNew, DWORD& dwOld);
void EngineHack_WriteCall(HANDLE hProcess, LPVOID lpAddr, DWORD dwNew, BOOL bStructCall);


BOOL GetSectionString(char* szSection, char* szKey, char* szValue);
int GetSectionInt(char* szSection, char* szKey, int nDefault);
float GetSectionFloat(char* szSection, char* szKey, float fDefault);
void logf(char *msg, ...);

void SetCurrProfileDWord(eProfileOption eOption, DWORD dwSet);
void SetCurrProfileBool(eProfileOption eOption, BOOL bSet);
void SetCurrProfileFlag(eProfileOption eOption, BOOL bSet);
void SetCurrProfileFloat(eProfileOption eOption, float fSet);
void SetCurrProfileString(eProfileOption eOption, char* szSet);
char* GetCurrProfileOption(eProfileOption eOption);
int GetCurrProfileDWord(eProfileOption eOption);
BOOL GetCurrProfileBool(eProfileOption eOption);
BOOL GetCurrProfileFlag(eProfileOption eOption);
float GetCurrProfileFloat(eProfileOption eOption);
char* GetCurrProfileString(eProfileOption eOption);
void ParseCVarProfile(char* szData);
void LogCurrProfile();

void SectionToCurrProfileBool(char* szSection, eProfileOption eOption, int nDefault);
void SectionToCurrProfileDWord(char* szSection, eProfileOption eOption, DWORD dwDefault);
void SectionToCurrProfileFloat(char* szSection, eProfileOption eOption, float fDefault);
void SectionToCurrProfileString(char* szSection, eProfileOption eOption);