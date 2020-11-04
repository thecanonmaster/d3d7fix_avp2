#include "stdafx.h"


typedef HRESULT (__stdcall *Blt_Type) (LPDIRECTDRAWSURFACE7, LPRECT, LPDIRECTDRAWSURFACE7, LPRECT, DWORD, LPDDBLTFX);
typedef HRESULT (__stdcall *BltFast_Type) (LPDIRECTDRAWSURFACE7, DWORD, DWORD, LPDIRECTDRAWSURFACE7, LPRECT, DWORD);
typedef ULONG (__stdcall *Release_Type) (LPDIRECTDRAWSURFACE7);
Release_Type g_dwOriginalRelease = NULL;
Blt_Type g_dwOriginalBlt = NULL;
BltFast_Type g_dwOriginalBltFast = NULL;
DWORD* g_dwOrigSurfaceTable = NULL;
DWORD g_dwNewSurfaceVTable[49];

ULONG __stdcall FakeIDDrawSurface7LM_Release(LPDIRECTDRAWSURFACE7 f)
{
	((DWORD*)f)[0] = (DWORD)g_dwOrigSurfaceTable;
	g_dwOriginalRelease = NULL;
	return f->Release();
}

HRESULT __stdcall FakeIDDrawSurface7LM_Blt(LPDIRECTDRAWSURFACE7 f, LPRECT a,LPDIRECTDRAWSURFACE7 b, LPRECT c,DWORD d, LPDDBLTFX e)
{					
	if (!b) 
	{			
		DDSURFACEDESC2 descDest;
		descDest.dwSize = sizeof(DDSURFACEDESC2);
		
		//if (g_dwOriginalLock(f, NULL, &descDest, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL) == DD_OK)
		if (f->Lock(NULL, &descDest, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL) == DD_OK)
		{				
			memset(descDest.lpSurface, e->dwFillColor, (descDest.dwWidth * descDest.dwHeight) << 2);	
			//g_dwOriginalUnlock(f, NULL);
			f->Unlock(NULL);
		}
		
		return DD_OK; 
	}
	
	return g_dwOriginalBlt(f, a, b, c, d, e);	
}

HRESULT __stdcall FakeIDDrawSurface7LM_BltFast(LPDIRECTDRAWSURFACE7 f,DWORD a,DWORD b,LPDIRECTDRAWSURFACE7 c, LPRECT d,DWORD e)
{		
	DDSURFACEDESC2 descSrc, descDest;
	descSrc.dwSize = sizeof(DDSURFACEDESC2);
	descDest.dwSize = sizeof(DDSURFACEDESC2);
		
	//if (g_dwOriginalLock(c, d, &descSrc, DDLOCK_WAIT | DDLOCK_READONLY, NULL) == DD_OK)
	if (c->Lock(d, &descSrc, DDLOCK_WAIT | DDLOCK_READONLY, NULL) == DD_OK)
	{
		//if (g_dwOriginalLock(f, NULL, &descDest, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL) == DD_OK)
		if (f->Lock(NULL, &descDest, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL) == DD_OK)
		{
			DWORD srcWidth = d->right - d->left;
			DWORD srcHeight = d->bottom - d->top;
			BYTE* pSrcLine = (BYTE*)descSrc.lpSurface;
				
			BYTE* pDestLine = (BYTE*)descDest.lpSurface + (b * (descDest.dwWidth << 2) + (a << 2));
				
			int nCount = srcHeight;
				
			while (nCount) 
			{
				nCount--;
				memcpy(pDestLine, pSrcLine, srcWidth << 2);
					
				pSrcLine += descSrc.lPitch;
				pDestLine += descDest.lPitch; 
			}			
				
			//g_dwOriginalUnlock(c, NULL);
			//g_dwOriginalUnlock(f, NULL);
			c->Unlock(NULL);
			f->Unlock(NULL);
		}
	}
		
	return DD_OK;
}


typedef LRESULT CALLBACK WindowProc_type(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef WindowProc_type *WindowProc_type_ptr;
WindowProc_type_ptr OldWindowProc;

#ifndef WM_INPUT
#define WM_INPUT						0x00FF
#endif

LRESULT CALLBACK NewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_bWindowed && uMsg == WM_KEYDOWN && wParam == VK_PRIOR /*0x91*/)
	{
		LONG lResult = GetWindowLong(g_hWindowHandle, GWL_STYLE);
		if (lResult & WS_CAPTION)
		{
			lResult &= ~WS_BORDER;
			lResult &= ~WS_CAPTION;
		}
		else
		{
			lResult |= WS_BORDER;
			lResult |= WS_CAPTION;
		}

		int nHalfScreenResX = (GetSystemMetrics(SM_CXSCREEN) >> 1);
		int nHalfScreenResY = (GetSystemMetrics(SM_CYSCREEN) >> 1);
		int nHalfWinResX = (g_dwWidth >> 1);
		int nHalfWinResY = (g_dwHeight >> 1);

		SetWindowLong(g_hWindowHandle, GWL_STYLE, lResult);
		SetWindowPos(g_hWindowHandle, HWND_TOPMOST, nHalfScreenResX - nHalfWinResX, nHalfScreenResY - nHalfWinResY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		SetWindowPos(g_hWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	if (uMsg == WM_KEYDOWN && wParam == VK_NEXT)
		g_bDrawFPS = !g_bDrawFPS;

	if (GetCurrProfileFlag(PO_RAW_MOUSE_INPUT) && uMsg == WM_INPUT)
	{
		ProcessRawMouseInput(lParam, g_lRMILastX, g_lRMILastY);
	}
	
	if (uMsg == WM_ACTIVATEAPP && GetCurrProfileBool(PO_DONT_SHUTDOWN_RENDERER) && g_bWindowed)
	{
		return TRUE;
	}
	else
	{
		return OldWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

FakeIDDraw7::FakeIDDraw7(LPDIRECTDRAW7 pOriginal)
{
	m_pIDDraw = pOriginal;
	g_ddMainDDraw = pOriginal;
}


FakeIDDraw7::~FakeIDDraw7(void)
{

}

HRESULT __stdcall FakeIDDraw7::QueryInterface (REFIID a, LPVOID FAR * b)
{

	HRESULT hResult = m_pIDDraw->QueryInterface(a, b); 

	if (hResult == NOERROR) 
	{
		if (a == IID_IDirect3D7) 
			*b = new FakeID3D7((LPDIRECT3D7)*b);		
	}

	return hResult;
}


ULONG __stdcall FakeIDDraw7::AddRef(void)
{
	ULONG dwResult = m_pIDDraw->AddRef();
	return dwResult;
}


ULONG __stdcall FakeIDDraw7::Release(void)
{
	ULONG dwCount = m_pIDDraw->Release();

	if (dwCount == 0) 
	{
		logf("FakeIDDraw7::Release - release success");
		m_pIDDraw = NULL;		
		delete(this); 
	}
	else
	{
		logf("FakeIDDraw7::Release - release failed, reference count = %d", dwCount);
		m_pIDDraw = NULL;		
		delete(this); 
	}

	return dwCount;
}


HRESULT  __stdcall FakeIDDraw7::Compact(void)
{
	return(m_pIDDraw->Compact());
}


HRESULT  __stdcall FakeIDDraw7::CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR* lplpDDClipper, IUnknown FAR *pUnkOuter)
{
	HRESULT hResult = m_pIDDraw->CreateClipper(dwFlags, lplpDDClipper, pUnkOuter);
	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::CreatePalette(DWORD a, LPPALETTEENTRY b, LPDIRECTDRAWPALETTE FAR* c, IUnknown FAR* d)
{
	HRESULT r = m_pIDDraw->CreatePalette(a, b, c, d);
	return r;
}


HRESULT  __stdcall FakeIDDraw7::CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPDIRECTDRAWSURFACE7 FAR * lplpDDSurface, IUnknown FAR * pUnkOuter)
{		
	if (lpDDSurfaceDesc2->dwMipMapCount == 0 && lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount == 32 
		&& !(lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_ZBUFFER))
	{
		HRESULT hResult = m_pIDDraw->CreateSurface(lpDDSurfaceDesc2, lplpDDSurface, pUnkOuter);
		if (hResult == DD_OK)
		{			
			if (GetCurrProfileBool(PO_PRELOAD_STATIC_LIGHT) && (lpDDSurfaceDesc2->ddsCaps.dwCaps2 & DDSCAPS2_TEXTUREMANAGE) && 
				!(lpDDSurfaceDesc2->ddsCaps.dwCaps2 & DDSCAPS2_OPAQUE))
			{
				g_d3dDevice->PreLoad(*lplpDDSurface);
			}
			
			if (GetCurrProfileBool(PO_DGVOODOO_MODE))
			{
				DWORD* pOrigTable = (DWORD*)*(DWORD*)*lplpDDSurface;

				if (!g_dwOriginalRelease)
				{
					memcpy(g_dwNewSurfaceVTable, pOrigTable, 49 * sizeof(DWORD));
					
					g_dwOrigSurfaceTable = pOrigTable;
					g_dwOriginalRelease = (Release_Type)pOrigTable[2];
					g_dwOriginalBlt = (Blt_Type)pOrigTable[5];
					g_dwOriginalBltFast = (BltFast_Type)pOrigTable[7];
					
					g_dwNewSurfaceVTable[2] = (DWORD)FakeIDDrawSurface7LM_Release;
					g_dwNewSurfaceVTable[5] = (DWORD)FakeIDDrawSurface7LM_Blt;
					g_dwNewSurfaceVTable[7] = (DWORD)FakeIDDrawSurface7LM_BltFast;
				}
				
				((DWORD*)*lplpDDSurface)[0] = (DWORD)g_dwNewSurfaceVTable;	
			}
		}

		return hResult;
	}	
	
	HRESULT hResult = m_pIDDraw->CreateSurface(lpDDSurfaceDesc2, lplpDDSurface, pUnkOuter);

	if((lpDDSurfaceDesc2->dwFlags & DDSD_CAPS) && (lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE))  
	{
		LPDIRECTDRAWSURFACE7 pCompatSurface = NULL;

		if (GetCurrProfileBool(PO_FULLSCREEN_OPTIMIZE) && !g_bWindowed)
		{
			DDSURFACEDESC2 compatDesc, displayDesc;
			displayDesc.dwSize = sizeof(DDSURFACEDESC2);
			
			m_pIDDraw->GetDisplayMode(&displayDesc);
			
			compatDesc = *lpDDSurfaceDesc2;
			compatDesc.dwFlags |= DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
			compatDesc.dwWidth = g_dwWidth;
			compatDesc.dwHeight = g_dwHeight;
			compatDesc.ddsCaps.dwCaps &= ~DDSCAPS_PRIMARYSURFACE;
			compatDesc.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
			//compatDesc.ddsCaps.dwCaps2 |= DDCAPS2_FLIPNOVSYNC;
			compatDesc.ddpfPixelFormat = displayDesc.ddpfPixelFormat; 

			HRESULT hCompatResult = m_pIDDraw->CreateSurface(&compatDesc, &pCompatSurface, pUnkOuter);
			logf("Compatibility primary surface creation result = %08x", hCompatResult);
		}
		
		*lplpDDSurface = new FakeIDDrawSurface7Prime(*lplpDDSurface, pCompatSurface);
	}

	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::DuplicateSurface(LPDIRECTDRAWSURFACE7 a, LPDIRECTDRAWSURFACE7 FAR * b)
{
	return(m_pIDDraw->DuplicateSurface(a, b));
}


HRESULT  __stdcall FakeIDDraw7::EnumDisplayModes(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMMODESCALLBACK2 d)
{
	return(m_pIDDraw->EnumDisplayModes(a, b, c, d));
}


HRESULT  __stdcall FakeIDDraw7::EnumSurfaces(DWORD a, LPDDSURFACEDESC2 b, LPVOID c, LPDDENUMSURFACESCALLBACK7 d)
{
	return(m_pIDDraw->EnumSurfaces(a, b, c, d));
}


HRESULT  __stdcall FakeIDDraw7::FlipToGDISurface(void)
{
	return(m_pIDDraw->FlipToGDISurface());
}


HRESULT  __stdcall FakeIDDraw7::GetCaps(LPDDCAPS lpDDSurfaceCaps, LPDDCAPS lpDDHELCaps)
{
	HRESULT hResult = m_pIDDraw->GetCaps(lpDDSurfaceCaps, lpDDHELCaps);
	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::GetDisplayMode(LPDDSURFACEDESC2 a)
{
	return(m_pIDDraw->GetDisplayMode(a));
}


HRESULT  __stdcall FakeIDDraw7::GetFourCCCodes(LPDWORD a, LPDWORD b)
{
	return(m_pIDDraw->GetFourCCCodes(a, b));
}


HRESULT  __stdcall FakeIDDraw7::GetGDISurface(LPDIRECTDRAWSURFACE7 FAR * a)
{
	return(m_pIDDraw->GetGDISurface(a));
}


HRESULT  __stdcall FakeIDDraw7::GetMonitorFrequency(LPDWORD a)
{
	return(m_pIDDraw->GetMonitorFrequency(a));
}


HRESULT  __stdcall FakeIDDraw7::GetScanLine(LPDWORD a)
{
	return(m_pIDDraw->GetScanLine(a));
}


HRESULT  __stdcall FakeIDDraw7::GetVerticalBlankStatus(LPBOOL a)
{
	return(m_pIDDraw->GetVerticalBlankStatus(a));
}


HRESULT  __stdcall FakeIDDraw7::Initialize(GUID FAR* a)
{
	return(m_pIDDraw->Initialize(a));
}


HRESULT  __stdcall FakeIDDraw7::RestoreDisplayMode(void)
{
	HRESULT hResult = m_pIDDraw->RestoreDisplayMode();
	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	if (!g_bWindowedSet)
	{
		g_bWindowedSet = TRUE;
		if (dwFlags & DDSCL_EXCLUSIVE) 
			g_bWindowed = 0; 
		else
			g_bWindowed = 1;
	}

	if (!g_bWindowHooked /*&& g_dwWindowed*/)
	{
		g_hWindowHandle = hWnd;
		//OldWindowProc = (WindowProc_type_ptr)GetWindowLong(g_hWindowHandle, GWL_WNDPROC);
		LONG lResult = SetWindowLong(g_hWindowHandle, GWL_WNDPROC, (LONG)NewWindowProc);
		if (lResult)
		{
			OldWindowProc = (WindowProc_type_ptr)lResult;
			logf("WindowProc hook set! (%08X -> %08X)", lResult, (LONG)NewWindowProc);
		}
		else
		{
			logf("Could not set WindowProc hook!");
		}

		g_bWindowHooked = TRUE;
	}

	HRESULT hResult = m_pIDDraw->SetCooperativeLevel(hWnd, dwFlags);
	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::SetDisplayMode(DWORD a, DWORD b, DWORD c, DWORD d, DWORD e)
{
	g_dwWidth = a;
	g_dwHeight = b;
	logf("FakeIDDraw7::SetDisplayMode - %d x %d", g_dwWidth, g_dwHeight);

	HRESULT h = m_pIDDraw->SetDisplayMode(a, b, c, d, e);
	return(h); 
}


HRESULT  __stdcall FakeIDDraw7::WaitForVerticalBlank(DWORD a, HANDLE b)
{
	return(m_pIDDraw->WaitForVerticalBlank(a, b));
}


HRESULT  __stdcall FakeIDDraw7::GetAvailableVidMem(LPDDSCAPS2 lpDDSCaps2, LPDWORD lpdwTotal, LPDWORD lpdwFree)
{
	HRESULT hResult = m_pIDDraw->GetAvailableVidMem(lpDDSCaps2, lpdwTotal, lpdwFree);
	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::GetSurfaceFromDC(HDC a, LPDIRECTDRAWSURFACE7 * b)
{
	return(m_pIDDraw->GetSurfaceFromDC(a, b));
}


HRESULT  __stdcall FakeIDDraw7::RestoreAllSurfaces(void)
{
	return(m_pIDDraw->RestoreAllSurfaces());
}


HRESULT  __stdcall FakeIDDraw7::TestCooperativeLevel(void)
{
	return(m_pIDDraw->TestCooperativeLevel());
}


HRESULT  __stdcall FakeIDDraw7::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 lpdddi, DWORD dwFlags)
{
	HRESULT hResult = m_pIDDraw->GetDeviceIdentifier(lpdddi, dwFlags);
	return hResult;
}


HRESULT  __stdcall FakeIDDraw7::StartModeTest(LPSIZE a, DWORD b, DWORD c)
{
	return(m_pIDDraw->StartModeTest(a, b, c));
}


HRESULT  __stdcall FakeIDDraw7::EvaluateMode(DWORD a, DWORD * b)
{
	return(m_pIDDraw->EvaluateMode(a, b));
}