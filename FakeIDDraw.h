#pragma once

class FakeIDDraw : public IDirectDraw
{
public:
    FakeIDDraw(LPDIRECTDRAW pOriginal);
    virtual ~FakeIDDraw(void);

	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
    
    HRESULT  __stdcall Compact(void);
    HRESULT  __stdcall CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d);
	HRESULT  __stdcall CreateSurface(LPDDSURFACEDESC a, LPDIRECTDRAWSURFACE FAR* b, IUnknown FAR* c);
	HRESULT  __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE FAR* b);
	HRESULT  __stdcall EnumDisplayModes(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d);
	HRESULT  __stdcall EnumSurfaces(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d);
	HRESULT  __stdcall FlipToGDISurface(void);
	HRESULT  __stdcall GetCaps(LPDDCAPS a, LPDDCAPS b);
    HRESULT  __stdcall GetDisplayMode(LPDDSURFACEDESC a);
    HRESULT  __stdcall GetFourCCCodes(LPDWORD a, LPDWORD b);
    HRESULT  __stdcall GetGDISurface(LPDIRECTDRAWSURFACE FAR * a);
    HRESULT  __stdcall GetMonitorFrequency(LPDWORD a);
    HRESULT  __stdcall GetScanLine(LPDWORD a);
    HRESULT  __stdcall GetVerticalBlankStatus(LPBOOL a);
    HRESULT  __stdcall Initialize(GUID FAR* a);
    HRESULT  __stdcall RestoreDisplayMode(void);
	HRESULT  __stdcall SetCooperativeLevel(HWND a, DWORD b);
	HRESULT  __stdcall SetDisplayMode(DWORD a, DWORD b, DWORD c);
	HRESULT  __stdcall WaitForVerticalBlank(DWORD a, HANDLE b);

   	LPDIRECTDRAW FAR m_pIDDraw;
};

class FakeIDDraw7 : public IDirectDraw7
{
public:
    FakeIDDraw7(LPDIRECTDRAW7 pOriginal);
    virtual ~FakeIDDraw7(void);
	
	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
    
    HRESULT  __stdcall Compact(void);
    HRESULT  __stdcall CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER FAR* b, IUnknown FAR * c);
    HRESULT  __stdcall CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR * d);
    HRESULT  __stdcall CreateSurface(LPDDSURFACEDESC2 a, LPDIRECTDRAWSURFACE7 FAR * b, IUnknown FAR * c);
    HRESULT  __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE7 a, LPDIRECTDRAWSURFACE7 FAR * b);
    HRESULT  __stdcall EnumDisplayModes(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMMODESCALLBACK2 d);
    HRESULT  __stdcall EnumSurfaces(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMSURFACESCALLBACK7 d);
    HRESULT  __stdcall FlipToGDISurface(void);
    HRESULT  __stdcall GetCaps(LPDDCAPS a, LPDDCAPS b);
    HRESULT  __stdcall GetDisplayMode(LPDDSURFACEDESC2 a);
    HRESULT  __stdcall GetFourCCCodes(LPDWORD a, LPDWORD b);
    HRESULT  __stdcall GetGDISurface(LPDIRECTDRAWSURFACE7 FAR * a);
    HRESULT  __stdcall GetMonitorFrequency(LPDWORD a);
    HRESULT  __stdcall GetScanLine(LPDWORD a);
    HRESULT  __stdcall GetVerticalBlankStatus(LPBOOL a);
    HRESULT  __stdcall Initialize(GUID FAR * a);
    HRESULT  __stdcall RestoreDisplayMode(void);
    HRESULT  __stdcall SetCooperativeLevel(HWND a, DWORD b);
    HRESULT  __stdcall SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e);
    HRESULT  __stdcall WaitForVerticalBlank(DWORD a, HANDLE b);
    /*** Added in the v2 interface ***/
    HRESULT  __stdcall GetAvailableVidMem(LPDDSCAPS2 a, LPDWORD b, LPDWORD c);
    /*** Added in the V4 Interface ***/
    HRESULT  __stdcall GetSurfaceFromDC(HDC a, LPDIRECTDRAWSURFACE7 * b);
    HRESULT  __stdcall RestoreAllSurfaces(void);
    HRESULT  __stdcall TestCooperativeLevel(void);
    HRESULT  __stdcall GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 a, DWORD b);
    HRESULT  __stdcall StartModeTest(LPSIZE a, DWORD b, DWORD c);
    HRESULT  __stdcall EvaluateMode(DWORD a, DWORD * b);	

   	LPDIRECTDRAW7 FAR m_pIDDraw;
};

class FakeIDDrawSurface7Prime : public IDirectDrawSurface7
{
public:
	FakeIDDrawSurface7Prime(LPDIRECTDRAWSURFACE7 pOriginal, LPDIRECTDRAWSURFACE7 pCompat);
    virtual ~FakeIDDrawSurface7Prime(void);
	
	// The original DDraw function definitions BEGIN
    HRESULT __stdcall QueryInterface (REFIID a, LPVOID FAR * b);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);
	
	/*** IDirectDrawSurface methods ***/
    HRESULT  __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE7 a);
    HRESULT  __stdcall AddOverlayDirtyRect(LPRECT a);
    HRESULT  __stdcall Blt(LPRECT a,LPDIRECTDRAWSURFACE7 b, LPRECT c,DWORD d, LPDDBLTFX e);
    HRESULT  __stdcall BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c);
    HRESULT  __stdcall BltFast(DWORD a,DWORD b,LPDIRECTDRAWSURFACE7 c, LPRECT d,DWORD e);
    HRESULT  __stdcall DeleteAttachedSurface(DWORD a,LPDIRECTDRAWSURFACE7 b);
    HRESULT  __stdcall EnumAttachedSurfaces(LPVOID a,LPDDENUMSURFACESCALLBACK7 b);
    HRESULT  __stdcall EnumOverlayZOrders(DWORD a,LPVOID b,LPDDENUMSURFACESCALLBACK7 c);
    HRESULT  __stdcall Flip(LPDIRECTDRAWSURFACE7 a, DWORD b);
    HRESULT  __stdcall GetAttachedSurface(LPDDSCAPS2 a, LPDIRECTDRAWSURFACE7 FAR * b);
    HRESULT  __stdcall GetBltStatus(DWORD a);
    HRESULT  __stdcall GetCaps(LPDDSCAPS2 a);
    HRESULT  __stdcall GetClipper(LPDIRECTDRAWCLIPPER FAR* a);
    HRESULT  __stdcall GetColorKey(DWORD a, LPDDCOLORKEY b);
    HRESULT  __stdcall GetDC(HDC FAR * a);
    HRESULT  __stdcall GetFlipStatus(DWORD a);
    HRESULT  __stdcall GetOverlayPosition(LPLONG a, LPLONG b);
    HRESULT  __stdcall GetPalette(LPDIRECTDRAWPALETTE FAR* a);
    HRESULT  __stdcall GetPixelFormat(LPDDPIXELFORMAT a);
    HRESULT  __stdcall GetSurfaceDesc(LPDDSURFACEDESC2 a);
    HRESULT  __stdcall Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC2 b);
    HRESULT  __stdcall IsLost();
    HRESULT  __stdcall Lock(LPRECT a,LPDDSURFACEDESC2 b,DWORD c ,HANDLE d);
    HRESULT  __stdcall ReleaseDC(HDC a);
    HRESULT  __stdcall Restore();
    HRESULT  __stdcall SetClipper(LPDIRECTDRAWCLIPPER a);
    HRESULT  __stdcall SetColorKey(DWORD a, LPDDCOLORKEY b);
    HRESULT  __stdcall SetOverlayPosition(LONG a, LONG b);
    HRESULT  __stdcall SetPalette(LPDIRECTDRAWPALETTE a);
    HRESULT  __stdcall Unlock(LPRECT a);
    HRESULT  __stdcall UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE7 b,LPRECT c ,DWORD d, LPDDOVERLAYFX e);
    HRESULT  __stdcall UpdateOverlayDisplay(DWORD a);
    HRESULT  __stdcall UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE7 b);

    HRESULT  __stdcall GetDDInterface(LPVOID FAR * a);
    HRESULT  __stdcall PageLock(DWORD a);
    HRESULT  __stdcall PageUnlock(DWORD a);
    
    HRESULT  __stdcall SetSurfaceDesc(LPDDSURFACEDESC2 a, DWORD b);
    
    HRESULT  __stdcall SetPrivateData(REFGUID a, LPVOID b, DWORD c, DWORD d);
    HRESULT  __stdcall GetPrivateData(REFGUID a, LPVOID b, LPDWORD c);
    HRESULT  __stdcall FreePrivateData(REFGUID a);
    HRESULT  __stdcall GetUniquenessValue(LPDWORD a);
    HRESULT  __stdcall ChangeUniquenessValue();
    
    HRESULT  __stdcall SetPriority(DWORD a);
    HRESULT  __stdcall GetPriority(LPDWORD a);
    HRESULT  __stdcall SetLOD(DWORD a);
    HRESULT  __stdcall GetLOD(LPDWORD a);

	LPDIRECTDRAWSURFACE7 GetCompat(){ return m_pIDDrawSurface; }	
	LPDIRECTDRAWSURFACE7 GetReal(){ return m_pIDDrawSurfaceReal; }

	void SetCompatBB(LPDIRECTDRAWSURFACE7 pSet) { m_pIDDrawSurfaceBB = pSet; }
	void SetCompatTB(LPDIRECTDRAWSURFACE7 pSet) { m_pIDDrawSurfaceTB = pSet; }
	
	void SetRealBB(LPDIRECTDRAWSURFACE7 pSet) { m_pIDDrawSurfaceRealBB = pSet; }
	void SetRealTB(LPDIRECTDRAWSURFACE7 pSet) { m_pIDDrawSurfaceRealTB = pSet; }

private:
	
   	LPDIRECTDRAWSURFACE7 FAR m_pIDDrawSurface;
	LPDIRECTDRAWSURFACE7 FAR m_pIDDrawSurfaceBB;
	LPDIRECTDRAWSURFACE7 FAR m_pIDDrawSurfaceTB;
	LPDIRECTDRAWSURFACE7 FAR m_pIDDrawSurfaceReal;
	LPDIRECTDRAWSURFACE7 FAR m_pIDDrawSurfaceRealBB;
	LPDIRECTDRAWSURFACE7 FAR m_pIDDrawSurfaceRealTB;
};

HRESULT __stdcall FakeIDDrawSurface7LM_Blt(LPDIRECTDRAWSURFACE7 f, LPRECT a,LPDIRECTDRAWSURFACE7 b, LPRECT c,DWORD d, LPDDBLTFX e);