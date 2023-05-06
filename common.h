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
	GEN_FIX_VERSION = 0,
	PO_DGVOODOO_MODE = 1,
	PO_DEFAULT_PROFILE,
	PO_INTRODUCTION_TIME,
	PO_RENDER_DLL,
	PO_RENDER_WRAPPER_DLL,
	PO_CONSOLE_BACKGROUND,
	PO_DESCRIPTION,
	PO_CLEAN_MODE,
	PO_DONT_SHUTDOWN_RENDERER,
	PO_SHOW_FPS,
	PO_FRAME_LIMITER_SLEEP,
	PO_CAMERA_FOV_SCALER,
	PO_SERVER_FPS,
	PO_ENABLE_CONSOLE,
	PO_NO_ENVMAP_CONSOLE_PRINT,
	PO_MAX_FPS,
	PO_INTEL_HD,
	PO_RADEON_5700,
	PO_CAMERA_FOV,
	PO_VIEW_MODEL_FOV,
	PO_SOLID_DRAWING,
	PO_SOLID_DRAWING_WHITELIST,
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
	PO_UPDATE_OBJECT_LTO,
	PO_UPD_PROG_DMG_OBJECT_LTO,
	PO_FAST_CRC_CHECK,
	PO_RMI_SCALE_GLOBAL,
	PO_RMI_SCALE_Y,
	PO_POSTPROCESS_ENABLED,
	PO_POSTPROCESS_INTENSITY,
	PO_POSTPROCESS_INTENSITY_MENU,
	PO_POSTPROCESS_INTENSITY_VM,
	EXT_BAN_MANAGER,
	EXT_MOTD_TIMER,
	EXT_MOTD_STRING0,
	EXT_MOTD_STRING1,
	EXT_MOTD_STRING2,
	EXT_MOTD_STRING3,
	EXT_MOTD_STRING4,
	EXT_CACHE_LIST,
	EXT_CMD_LIST,
	PO_MAX,
};

enum eFpsCounterPos
{
	FCP_LEFT_BOTTOM = 0,
	FCP_LEFT_TOP = 1,
	FCP_RIGHT_TOP,
	FCP_RIGHT_BOTTOM,
	FCP_DISABLED,
	FCP_MAX
};

struct ProfileOption
{
	ProfileOption()
	{
		memset(this, 0, sizeof(ProfileOption));
	}

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

#define MAX_WORLD_NAME_LEN		100
#define MAX_SDW_FILENAME_LEN	128

struct WorldListItem
{
	WorldListItem(char* szId)
	{
		strcpy(m_szWorldName, szId);
	}
	
	char m_szWorldName[MAX_WORLD_NAME_LEN];
};

struct FilenameItem
{
	FilenameItem(char* szFilename)
	{
		strcpy(m_szFilename, szFilename);
	}

	char m_szFilename[MAX_SDW_FILENAME_LEN];
};

typedef std::vector<WorldListItem*> WorldList;
typedef std::vector<FilenameItem*> SDWList;

extern FILE* g_LogFile;
//extern ProfileOption g_ProfileOptions[PO_MAX];
extern char g_szProfile[64];
extern eFpsCounterPos g_eDrawFPS;
extern DWORD g_dwWidth;
extern DWORD g_dwHeight;
extern BOOL g_bWindowedSet;
extern BOOL g_bWindowed;
extern DWORD g_dwPPCurrIntensity;
extern LPDIRECTDRAWSURFACE7 g_ddsBackBuffer;
extern LPDIRECT3DDEVICE7 g_d3dDevice;
extern LPDIRECTDRAW7 g_ddMainDDraw;
extern HWND g_hWindowHandle;
extern BOOL g_bWindowHooked;
extern float g_fLastFontListUpdate;
extern BOOL g_bInTWMDetailTex_WorldList;
extern BOOL g_bCVarProfileIgnored;

extern LONG g_lRMILastX;
extern LONG g_lRMILastY;
//extern DWORD g_hMouseSensitivity;

extern int g_nLastFrameRate;

#ifdef _DEBUG
#ifdef PRIMAL_HUNT_BUILD
	#define APP_NAME				"D3D7FIX v%g for AVP2 Primal Hunt (ltmsg.dll, DEBUG)"
#else
	#define APP_NAME				"D3D7FIX v%g for Aliens vs Predator 2 (ltmsg.dll, DEBUG)"
#endif
#else
#ifdef PRIMAL_HUNT_BUILD
	#define APP_NAME				"D3D7FIX v%g for AVP2 Primal Hunt (ltmsg.dll)"
#else
	#define APP_NAME				"D3D7FIX v%g for Aliens vs Predator 2 (ltmsg.dll)"
#endif
#endif

#define ACTIVE_PROFILE	"Active profile = %s"
#define APP_NAME_SHORT	"D3D7FIX v%g"
#define APP_VERSION		0.32f
#define CVAR_PROFILE	"D3D7FixProfile"
#define CVAR_PROFILE_EX "D3D7FixProfileEx"

#define INI_FILENAME		".\\ltmsg.ini"
#define INI_FILENAME_EXT	".\\ltmsg_ext.ini"

#define INI_SECTION_SIZE		2048
#define INI_SECTION_SIZE_DS		4096

#define CMD_FLAG_NO_DI_HOOKS	"+D3D7FixNoDIHooks 1"
#define CMD_FLAG_NO_RS_HOOK		"+D3D7FixNoRenderSceneHook 1"
#define DI_LIB_NAME_HOME		".\\dinput.dll"

#define LITHTECH_EXE		"lithtech.exe"
#define SERVER_DLL			"server.dll"
#define RENDER_DLL_DEFAULT	"d3d.ren"
#define CONSOLE_BG_DEFAULT	"0x8F00FF00"
#define LTMSG_LOG			"ltmsg.log"

#define OBJECT_LTO_LOWER	"object.lto"
#define OBJECT_LTO_UPPER	"OBJECT.LTO"
#define CRES_DLL_LOWER		"cres.dll"
#define CRES_DLL_UPPER		"CRES.DLL"

#define PROFILE_GLOBAL				"Global"
#define PROFILE_CLEAN				"Clean"
#define PROFILE_DEDICATED_SERVER	"Dedicated_Server"
#define TWM_DETAIL_FIX_WORLD_LIST	"TWMDetailTex_WorldList"
#define SOLID_DRAWING_WHITELIST		"SolidDrawingWhitelist"
#define TWM_DETAIL_FIX_WORLD		"World%d"
#define SDW_FILENAME				"Filename%d"

#define DS_INTRO_DELAY				10.0f

#define POSTPROCESSING_RES			256.0f

#define FONT_LIST_UPDATE_TIME		5.0f
#define FONT_LIST_CLEANUP_TIME		180.0f
#define FONT_STRING_CLEANUP_TIME	30.0f
#define FONT_STRING_LENGTH			1024
#define FONT_STRING_LINE_RESERVE	4

#define INTRODUCTION_FONT_HEIGHT	18
#define INTRODUCTION_FONT_WIDTH		10
#define INTRODUCTION_LINES			7

#define FRAME_RATE_FONT_SCALE	2
#define FRAME_RATE_UPDATE_TIME	0.2f
#define FRAME_RATE_LEVEL_RED	30
#define FRAME_RATE_LEVEL_YELLOW	55

#define FIX_FLG_R5700_FLICKERING_MODELS		(1<<0)
#define FIX_FLG_R5700_BLACK_SCREEEN			(1<<1)

#define FIX_FLG_TWM_DETAILTEX_CHROMAKEYED	(1<<0)
#define FIX_FLG_TWM_DETAILTEX_ALL_IN_LIST	(1<<1)

#define FLIPSCREEN_CANDRAWCONSOLE	(1<<0)
#define FLIPSCREEN_COPY				(1<<1)
#define FLIPSCREEN_DIRTY			(1<<2)

#define DLIGHT_SURFACES_COUNT_OVERRIDE	256
#define FIND_FREE_SLOT_SIZE_OVERRIDE	64

#define DIDEVTYPE_MOUSE         2
#define DIDEVTYPE_KEYBOARD      3

#define OT_LIGHT 4
#define FLAG_DONTLIGHTBACKFACING	(1<<6)

#define FLIP_FLAGS	(/*DDFLIP_NOVSYNC | */DDFLIP_WAIT)

#define TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

#define LT_DISCON_MISCCRC		7
#define SERVERSHELL_MESSAGE		3

#define BASE_LIGHTANIM_NAME		"LightAnim_BASE"

#define SS_PAUSED		(1<<0)
#define SS_DEMOPLAYBACK	(1<<1)
#define SS_CACHING		(1<<2)

#define FT_MODEL		0
#define FT_SPRITE		1
#define FT_TEXTURE		2
#define FT_SOUND		3
#define FT_ERROR		0xFF

class ILTStream
{
protected:

	virtual				~ILTStream() {}

public:

	virtual void	Release()=0;
	virtual DWORD	Read(void *pData, DWORD size)=0;
	virtual DWORD	ReadString(char *pStr, DWORD maxBytes)=0;

	virtual DWORD	ErrorStatus()=0;

	virtual DWORD	SeekTo(DWORD offset)=0;
	virtual DWORD	GetPos(DWORD *offset)=0;
	virtual DWORD	GetLen(DWORD *len)=0;

	// More stuff there
};

struct LTRect
{
	int left;
	int top;
	int right;
	int bottom;
};

struct LTVector
{
	float x;
	float y;
	float z;
};

struct LTRotation
{
	float x;
	float y;
	float z;
	float w;
};

union GenericColor 
{
    DWORD  dwVal;
    WORD   wVal;
    BYTE   bVal;
};

class ConVarFloat
{
public:
	
	DWORD			m_dwVal;
	float			m_DefaultVal;
	float			m_fVal;		
	DWORD			m_Unknown1;
	DWORD			m_Unknown2;
	const char*		m_pName;
	DWORD			m_hParam;
	ConVarFloat*	m_pNext;
};

#define DRAWMODE_NORMAL     1   // Render normally.
#define DRAWMODE_OBJECTLIST 2   // Only render the objects in m_pObjectList.

struct SceneDesc
{
    int             m_DrawMode; // +0
	DWORD			*m_pTicks_Render_Objects;
	DWORD			*m_pTicks_Render_Models;
	DWORD			*m_pTicks_Render_Sprites;
	DWORD			*m_pTicks_Render_WorldModels;
	DWORD			*m_pTicks_Render_ParticleSystems;
	DWORD			*m_pTicks_Render_Lights;
	
	// More stuff there
};

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

class LMPage
{
public:
	BYTE		m_Data0[12];
	BYTE*		m_UnkArray128;
	DWORD		m_dwUnkCount;
	BYTE		m_Data1[16];
	LMPage*		m_pNextLMPage;
};

class RenderContext
{
public:
	LMPage* m_pPage;
	// more there
};

class WorldPoly
{
public:

	BYTE	m_Data0[72];
	LMPage** m_pLMPage;
	BYTE	m_nLightmapWidth;
	BYTE	m_nLightmapHeight;
	BYTE	m_nLMPageX;
	BYTE	m_nLMPageY;
	// more there
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

struct FontString;
typedef std::list<FontString*> FSLines;

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
extern DWORD g_hFont15Surface;
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
	
	DWORD		   m_hBuffer;         
	DWORD          m_BlitOptions;      
	GenericColor   m_TransparentColor; 
	LTRect         *m_pSrcRect;        
	LTRect         *m_pDestRect;       
	float          m_Alpha;           
	
	// More stuff there
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
#define INVALID_LIGHT_ANIM	0xFFFFFFFF
#define LIGHTANIMFRAME_NONE	0xFFFFFFFF

class LAInfo
{
public:
	
	BOOL		m_bShadowMap;		
	DWORD		m_iFrames[2];
	float		m_fPercentBetween;
	float		m_fBlendPercent;
	LTVector	m_vLightPos;
	LTVector	m_vLightColor;
	float		m_fLightRadius;
};

class ILTLightAnim
{
public:
	virtual DWORD FindLightAnim(const char *pName, DWORD &hLightAnim)=0;
	virtual DWORD GetNumFrames(DWORD hLightAnim, DWORD &nFrames)=0;
	virtual DWORD GetLightAnimInfo(DWORD hLightAnim, LAInfo &info)=0;
	virtual DWORD SetLightAnimInfo(DWORD hLightAnim, LAInfo &info)=0;
};

class ILTMath
{
public:
	
	virtual DWORD GetRotationVectors(LTRotation &rot, LTVector &right, LTVector &up, LTVector &forward);

	// More stuff there
};

class ILTCSBase
{
public:

	virtual DWORD StartHMessageWrite()=0;
	// More stuff there

//protected:
	
	ILTMath			m_MathLT;
	void			*m_pCommonLT;
	void			*m_pPhysicsLT;
	void			*m_pModelLT;
	void			*m_pTransformLT;
	ILTLightAnim	*m_pLightAnimLT;
	void			*m_pSoundMgr;
};

typedef void (__cdecl *ILTCSBase_CPrint_type)(ILTCSBase* pBase, char *pMsg, ...);
typedef DWORD (__stdcall *ILTCSBase_CreateString_type)(char *pString);
typedef void (__stdcall *ILTCSBase_FreeString_type)(DWORD hString);
typedef char* (__stdcall *ILTCSBase_GetStringData_type)(DWORD hString);
typedef float (__stdcall *ILTCSBase_GetVarValueFloat_type)(DWORD hVar);
typedef char* (__stdcall *ILTCSBase_GetVarValueString_type)(DWORD hVar);
typedef float (__fastcall *ILTCSBase_GetTime_type)(ILTCSBase* pBase);
typedef float (__fastcall *ILTCSBase_GetFrameTime_type)(ILTCSBase* pBase);

typedef void (__fastcall *IClientShell_PreLoadWorld_type)(void* pShell, void* notUsed, char *pWorldName);
typedef void (__fastcall *IClientShell_Update_type)(void* pShell);
typedef void (__fastcall *IServerShell_Update_type)(void* pShell, float timeElapsed);
typedef void (__fastcall *IServerShell_VerifyClient_type)(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD &nVerifyCode);
typedef void* (__fastcall *IServerShell_OnClientEnterWorld_type)(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD clientDataLen);
typedef void* (__fastcall *IServerShell_OnClientExitWorld_type)(void* pShell, void* notUsed, DWORD hClient);
typedef DWORD (__fastcall *IServerShell_ServerAppMessageFn_type)(void* pShell, void* notUsed, char *pMsg, int nLen);
typedef void (__fastcall *IServerShell_PostStartWorld_type)(void* pShell);

extern void (__cdecl *ILTCSBase_CPrint)(ILTCSBase* pBase, char *pMsg, ...);
extern DWORD (__stdcall *ILTCSBase_CreateString)(char *pString);
extern void (__stdcall *ILTCSBase_FreeString)(DWORD hString);
extern char* (__stdcall *ILTCSBase_GetStringData)(DWORD hString);
extern float (__stdcall *ILTCSBase_GetVarValueFloat)(DWORD hVar);
extern char* (__stdcall *ILTCSBase_GetVarValueString)(DWORD hVar);
extern float (__fastcall *ILTCSBase_GetTime)(ILTCSBase* pBase);
extern float (__fastcall *ILTCSBase_GetFrameTime)(ILTCSBase* pBase);

extern void (__fastcall *IClientShell_PreLoadWorld)(void* pShell, void* notUsed, char *pWorldName);
extern void (__fastcall *IClientShell_Update)(void* pShell);
extern void (__fastcall *IServerShell_Update)(void* pShell, float timeElapsed);
extern void (__fastcall *IServerShell_VerifyClient)(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD &nVerifyCode);
extern void* (__fastcall *IServerShell_OnClientEnterWorld)(void* pShell, void* notUsed, DWORD hClient, void *pClientData, DWORD clientDataLen);
extern void* (__fastcall *IServerShell_OnClientExitWorld)(void* pShell, void* notUsed, DWORD hClient);
extern DWORD (__fastcall *IServerShell_ServerAppMessageFn)(void* pShell, void* notUsed, char *pMsg, int nLen);
extern void (__fastcall *IServerShell_PostStartWorld)(void* pShell);

class StartGameRequest
{
public:
	
	int				m_Type;
	char			m_WorldName[MAX_WORLD_NAME_LEN];
	// More there
};

class ILTClient: public ILTCSBase
{
public:

	DWORD			(*StartGame)(StartGameRequest *pRequest);
	BYTE			m_Data0[20]; // 24 52
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

class ILTServer: public ILTCSBase
{
public:

	DWORD			(*SetCRCString)(char *szCRCFiles);
	DWORD			(*CreateStringCRC)();
	DWORD			(*CreateWorldCRC)();
	DWORD			(*GetGameInfo)(void **ppData, DWORD *pLen);
	DWORD			(*GetClass)(char *pName);

	DWORD			(*GetStaticObject)(DWORD hClass, DWORD *obj);
	DWORD			(*GetObjectClass)(DWORD hObject);
	BOOL			(*IsKindOf)(DWORD hClass, DWORD hTest);
	void*			(*CreateObject)(DWORD hClass, void *pStruct);
	void*			(*CreateObjectProps)(DWORD hClass, void *pStruct, char *pszProps);
	DWORD			(*GetServerFlags)();
	DWORD			(*SetServerFlags)(DWORD flags);
	DWORD			(*CacheFile)(DWORD fileType, char *pFilename);

	BYTE			m_Data0[120]; // 152 196
	DWORD			(*GetNextClient)(DWORD hPrev);
	DWORD			(*GetNextClientRef)(DWORD hRef);
	DWORD			(*GetClientRefInfoFlags)(DWORD hClient);
	BOOL			(*GetClientRefName)(DWORD hClient, char *pName, int maxLen);
	DWORD			(*GetClientRefObject)(DWORD hClient);
	DWORD			(*GetClientID)(DWORD hClient);
	BOOL			(*GetClientName)(DWORD hClient, char *pName, int maxLen);
	void			(*SetClientInfoFlags)(DWORD hClient, DWORD dwClientFlags);
	DWORD			(*GetClientInfoFlags)(DWORD hClient);
	void			(*SetClientUserData)(DWORD hClient, void *pData);
	void*			(*GetClientUserData)(DWORD hClient);	
	void			(*KickClient)(DWORD hClient);

	DWORD			(*SetClientViewPos)(DWORD hClient, LTVector *pPos);

	void			(*RunGameConString)(char *pString);	
	void			(*SetGameConVar)(char *pName, char *pVal);
	DWORD			(*GetGameConVar)(char *pName);
};

extern ILTServer* g_pLTServer;

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

class CBindModule
{
public:

	HMODULE m_hInstance;
	DWORD m_dwType;

	// more?
};

class CObjectModule
{
public:

	DWORD m_dwData0;
	DWORD m_dwData1;
	CBindModule* m_pModule;

	// more?
};

class CShellModule
{
public:

	CBindModule* m_pModule;
	void* m_pFunc1;
	void* m_pFunc2;

	// more?
};

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

	BYTE			m_Data11[332]; // 344

	CBindModule*	m_pClientResourceModule;
	CBindModule*	m_pLocalizedClientResourceModule;
    CShellModule*	m_pShellModule;

	void*			m_pClientShell; // 1568
	
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
	void*			m_pDemoMgr; // 5788
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
	BYTE			m_Data4[328]; // 592 852 884 972 3644
	void*			m_pServerShell; // 3120
	BYTE			m_Data5[260];
	ILTServer*		m_pLTServer; // 3384
	BYTE			m_Data6[256];
	void*			m_pServerFileMgr;
};

class CServerMgrBase
{
public:
	
	CServerMgr* m_pServerMgr;
};

class CClassMgr
{
public:

	CObjectModule*		m_pObjectModule;
	CBindModule*		m_pServerResourceModule;
	CShellModule*		m_pShellModule;
	
};

class CClassMgrBase
{
public:

	CClassMgr* m_pClassMgr;	
};

extern CServerMgrBase* g_pServerMgr;
extern CClientMgrBase* g_pClientMgr;
extern CClassMgrBase* g_pClassMgr;

void CreateFont15Surface();
void ReoptimizeFont15Surface();
void DrawFont15String(char* szString, int nX, int nY, int nSpacing, int nScale, DWORD dwColor, DWORD hDestSurfOverride = NULL);
void CreateIntroductionSurface();

BOOL RegisterRawMouseDevice();
void ProcessRawMouseInput(LPARAM lParam, LONG& lLastX, LONG& lLastY);

void EngineHack_WriteData(LPVOID lpAddr, BYTE* pNew, BYTE* pOld, DWORD dwSize);
void EngineHack_WriteFunction(LPVOID lpAddr, DWORD dwNew, DWORD& dwOld);
void EngineHack_WriteCall(LPVOID lpAddr, DWORD dwNew, BOOL bStructCall);
void EngineHack_AllowWrite(LPVOID lpAddr, DWORD dwSize);
void EngineHack_WriteJump(LPVOID lpAddr, DWORD dwNew);

BOOL SectionItemExists(char* szSection, char* szKey);
BOOL GetSectionString(char* szSection, char* szKey, char* szValue);
int GetSectionInt(char* szSection, char* szKey, int nDefault);
float GetSectionFloat(char* szSection, char* szKey, float fDefault);
void logf(char *msg, ...);
BOOL FileExists(char* szName);

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
BOOL ParseCVarProfile(char* szData);
void LogCurrProfile();
char* ParseCacheString(char* szString, DWORD& dwType);

void TWMDetailTex_WorldList_Reserve(int nSize);
void TWMDetailTex_WorldList_Add(char* szName);
WorldListItem* TWMDetailTex_WorldList_Find(char* szName);
void TWMDetailTex_WorldList_Free();

void SolidDrawingWhitelist_Reserve(int nSize);
void SolidDrawingWhitelist_Add(char* szFilename);
FilenameItem* SolidDrawingWhitelist_Find(char* szFilename);
void SolidDrawingWhitelist_Free();

void SectionToCurrProfileBool(char* szSection, eProfileOption eOption, int nDefault);
void SectionToCurrProfileDWord(char* szSection, eProfileOption eOption, DWORD dwDefault);
void SectionToCurrProfileFloat(char* szSection, eProfileOption eOption, float fDefault);
void SectionToCurrProfileString(char* szSection, eProfileOption eOption);
void SectionToCurrProfileString(char* szSection, eProfileOption eOption, char* szDefault);