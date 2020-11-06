#include "stdafx.h"

float g_fLastFrameRateUpdate = 0.0f;
void FrameLimiter()
{
	double fps = GetCurrProfileFloat(PO_MAX_FPS);
	
	if (!fps) 
		return;
	
	static LARGE_INTEGER liFrequency = {0};
	static LARGE_INTEGER liStart = {0};
	LARGE_INTEGER liEnd = {0};
	
	if(!liFrequency.QuadPart)
		QueryPerformanceFrequency(&liFrequency);
	
	QueryPerformanceCounter(&liEnd);
	
	double fDifference = double(liEnd.QuadPart - liStart.QuadPart);
	fDifference /= double(liFrequency.QuadPart / 1000);
	
	//if(fps)
	{
		// slow down the client shell
		while(fDifference < 1000.0 / fps)
		{
			Sleep(GetCurrProfileDWord(PO_FRAME_LIMITER_SLEEP));
			QueryPerformanceCounter(&liEnd);
			fDifference = double(liEnd.QuadPart - liStart.QuadPart);
			fDifference /= double(liFrequency.QuadPart / 1000);
		}

		if (GetCurrProfileBool(PO_SHOW_FPS))
		{
			float fTime = ILTCSBase_GetTime(g_pLTClient);

			if (fTime - g_fLastFrameRateUpdate > FRAME_RATE_UPDATE_TIME)
			{
				g_fLastFrameRateUpdate = fTime;
				g_nLastFrameRate = (int)(1000.0f / fDifference + 0.1f);
			}	
		}
	}
	
	QueryPerformanceCounter(&liStart);
}

HRESULT __stdcall MyEnumCallbackCompat(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
{
	FakeIDDrawSurface7Prime* pPrime = (FakeIDDrawSurface7Prime*)lpContext;
	if (pPrime->GetCompat() != lpDDSurface)
	{
		if (lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
			pPrime->SetCompatBB(lpDDSurface);
		else
			pPrime->SetCompatTB(lpDDSurface);
		
		lpDDSurface->EnumAttachedSurfaces(lpContext, MyEnumCallbackCompat);
	}
	
	lpDDSurface->Release();
	
	return DDENUMRET_OK;
}

HRESULT __stdcall MyEnumCallbackReal(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
{
	FakeIDDrawSurface7Prime* pPrime = (FakeIDDrawSurface7Prime*)lpContext;
	if (pPrime->GetReal() != lpDDSurface)
	{
		if (lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
			pPrime->SetRealBB(lpDDSurface);
		else
			pPrime->SetRealTB(lpDDSurface);
		
		lpDDSurface->EnumAttachedSurfaces(lpContext, MyEnumCallbackReal);
	}
	
	lpDDSurface->Release();

	return DDENUMRET_OK;
}


FakeIDDrawSurface7Prime::FakeIDDrawSurface7Prime(LPDIRECTDRAWSURFACE7 pOriginal, LPDIRECTDRAWSURFACE7 pCompat)
{
	if (pCompat)
	{
		m_pIDDrawSurface = pCompat;
		m_pIDDrawSurface->EnumAttachedSurfaces(this, MyEnumCallbackCompat);
		m_pIDDrawSurfaceReal = pOriginal;
		m_pIDDrawSurfaceReal->EnumAttachedSurfaces(this, MyEnumCallbackReal);
	}
	else
	{
		m_pIDDrawSurface = pOriginal;
		m_pIDDrawSurfaceBB = NULL;
		m_pIDDrawSurfaceTB = NULL;
		m_pIDDrawSurfaceReal = NULL;
		m_pIDDrawSurfaceRealBB = NULL;
		m_pIDDrawSurfaceRealTB = NULL;
	}

	PostProcessClear();
}


FakeIDDrawSurface7Prime::~FakeIDDrawSurface7Prime(void)
{

}


HRESULT FakeIDDrawSurface7Prime::QueryInterface(REFIID a, LPVOID FAR * b)
{	
	*b = NULL;
		
	HRESULT hResult = m_pIDDrawSurface->QueryInterface(a, b); 
		
	return hResult;	
}

ULONG FakeIDDrawSurface7Prime::AddRef(void)
{
	ULONG dwResult = m_pIDDrawSurface->AddRef();
	return dwResult;	
}

ULONG FakeIDDrawSurface7Prime::Release(void)
{
	ULONG dwCount = m_pIDDrawSurface->Release();

	if (m_pIDDrawSurfaceReal)
		m_pIDDrawSurfaceReal->Release();
	
	if (dwCount == 0) 
	{		
		logf("FakeIDDrawSurface7::Release - release success");
		m_pIDDrawSurface = NULL;		
		delete(this); 
	}
	else
	{
		logf("FakeIDDrawSurface7::Release - release failed, reference count = %d", dwCount);
		m_pIDDrawSurface = NULL;		
		delete(this); 
	}

	g_ddsBackBuffer = NULL;
		
	return dwCount;	
}

HRESULT FakeIDDrawSurface7Prime::AddAttachedSurface(LPDIRECTDRAWSURFACE7 a)
{
	HRESULT hResult = m_pIDDrawSurface->AddAttachedSurface(a);
	return hResult;	
}

HRESULT FakeIDDrawSurface7Prime::AddOverlayDirtyRect(LPRECT a)
{
	return(m_pIDDrawSurface->AddOverlayDirtyRect(a));	
}

HRESULT FakeIDDrawSurface7Prime::Blt(LPRECT a,LPDIRECTDRAWSURFACE7 b, LPRECT c,DWORD d, LPDDBLTFX e)
{
	if (GetCurrProfileBool(PO_POSTPROCESS_ENABLED))
		PostProcess(g_d3dDevice, b);
	
	FrameLimiter();

	if (g_bWindowed || !m_pIDDrawSurfaceReal)
	{
		return m_pIDDrawSurface->Blt(a, b, c, d, e);
	}
	else
	{
		/*if (!m_pIDDrawSurfaceReal)
		{
			return m_pIDDrawSurface->Flip(NULL, FLIP_FLAGS);
		}
		else*/
		{
			if (GetCurrProfileBool(PO_NOVSYNC))
			{
				return m_pIDDrawSurfaceReal->Blt(a, b, c, d, e);
			}
			else
			{
				m_pIDDrawSurfaceRealBB->Blt(a, b, c, d, e);
				m_pIDDrawSurface->Flip(NULL, FLIP_FLAGS);
				return m_pIDDrawSurfaceReal->Flip(NULL, FLIP_FLAGS);
			}
		}
	}
}


HRESULT FakeIDDrawSurface7Prime::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
	return(m_pIDDrawSurface->BltBatch(a, b, c));	
}


HRESULT FakeIDDrawSurface7Prime::BltFast(DWORD a,DWORD b,LPDIRECTDRAWSURFACE7 c, LPRECT d,DWORD e)
{
	return(m_pIDDrawSurface->BltFast(a, b, c, d, e));	
}


HRESULT FakeIDDrawSurface7Prime::DeleteAttachedSurface(DWORD a,LPDIRECTDRAWSURFACE7 b)
{
	return(m_pIDDrawSurface->DeleteAttachedSurface(a,((FakeIDDrawSurface7Prime*)b)->m_pIDDrawSurface));	
}


HRESULT FakeIDDrawSurface7Prime::EnumAttachedSurfaces(LPVOID a,LPDDENUMSURFACESCALLBACK7 b)
{
	return(m_pIDDrawSurface->EnumAttachedSurfaces(a, b));	
}


HRESULT FakeIDDrawSurface7Prime::EnumOverlayZOrders(DWORD a,LPVOID b,LPDDENUMSURFACESCALLBACK7 c)
{
	return(m_pIDDrawSurface->EnumOverlayZOrders(a, b, c));	
}


HRESULT FakeIDDrawSurface7Prime::Flip(LPDIRECTDRAWSURFACE7 a, DWORD b)
{	
	FrameLimiter();
	
	HRESULT hResult = m_pIDDrawSurface->Flip(a, b);
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::GetAttachedSurface(LPDDSCAPS2 lpDDSCaps, LPDIRECTDRAWSURFACE7 FAR * lplpDDAttachedSurface)
{
	HRESULT hResult = m_pIDDrawSurface->GetAttachedSurface(lpDDSCaps, lplpDDAttachedSurface);
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::GetBltStatus(DWORD a)
{
	return(m_pIDDrawSurface->GetBltStatus(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetCaps(LPDDSCAPS2 a)
{
	return(m_pIDDrawSurface->GetCaps(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetClipper(LPDIRECTDRAWCLIPPER FAR* a)
{
	return(m_pIDDrawSurface->GetClipper(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
	return(m_pIDDrawSurface->GetColorKey(a, b));	
}


HRESULT FakeIDDrawSurface7Prime::GetDC(HDC FAR * a)
{
	return(m_pIDDrawSurface->GetDC(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetFlipStatus(DWORD a)
{
	return(m_pIDDrawSurface->GetFlipStatus(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetOverlayPosition(LPLONG a, LPLONG b)
{
	return(m_pIDDrawSurface->GetOverlayPosition(a, b));	
}


HRESULT FakeIDDrawSurface7Prime::GetPalette(LPDIRECTDRAWPALETTE FAR* a)
{
	return(m_pIDDrawSurface->GetPalette(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetPixelFormat(LPDDPIXELFORMAT a)
{
	HRESULT hResult = m_pIDDrawSurface->GetPixelFormat(a);
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::GetSurfaceDesc(LPDDSURFACEDESC2 a)
{
	HRESULT hResult = m_pIDDrawSurface->GetSurfaceDesc(a);
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC2 b)
{
	return(m_pIDDrawSurface->Initialize(a, b));	
}


HRESULT FakeIDDrawSurface7Prime::IsLost()
{
	return(m_pIDDrawSurface->IsLost());	
}


HRESULT FakeIDDrawSurface7Prime::Lock(LPRECT lpDestRect,LPDDSURFACEDESC2 lpDDSurfaceDesc,DWORD dwFlags ,HANDLE hEvent)
{
	HRESULT hResult = m_pIDDrawSurface->Lock(lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent);

	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::ReleaseDC(HDC a)
{
	return(m_pIDDrawSurface->ReleaseDC(a));	
}


HRESULT FakeIDDrawSurface7Prime::Restore()
{
	return(m_pIDDrawSurface->Restore());	
}


HRESULT FakeIDDrawSurface7Prime::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
	HRESULT hResult = m_pIDDrawSurface->SetClipper(lpDDClipper);
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
	return(m_pIDDrawSurface->SetColorKey(a, b));	
}


HRESULT FakeIDDrawSurface7Prime::SetOverlayPosition(LONG a, LONG b)
{
	return(m_pIDDrawSurface->SetOverlayPosition(a, b));	
}


HRESULT FakeIDDrawSurface7Prime::SetPalette(LPDIRECTDRAWPALETTE a)
{
	return(m_pIDDrawSurface->SetPalette(a));	
}


HRESULT FakeIDDrawSurface7Prime::Unlock(LPRECT lpRect)
{
	HRESULT hResult = m_pIDDrawSurface->Unlock(lpRect);	
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE7 b,LPRECT c ,DWORD d, LPDDOVERLAYFX e)
{
	return(m_pIDDrawSurface->UpdateOverlay(a, ((FakeIDDrawSurface7Prime*)b)->m_pIDDrawSurface, c, d, e));	
}


HRESULT FakeIDDrawSurface7Prime::UpdateOverlayDisplay(DWORD a)
{
	return(m_pIDDrawSurface->UpdateOverlayDisplay(a));	
}


HRESULT FakeIDDrawSurface7Prime::UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE7 b)
{
	return(m_pIDDrawSurface->UpdateOverlayZOrder(a, ((FakeIDDrawSurface7Prime*)b)->m_pIDDrawSurface));	
}



HRESULT FakeIDDrawSurface7Prime::GetDDInterface(LPVOID FAR * a)
{
	return(m_pIDDrawSurface->GetDDInterface(a));	
}


HRESULT FakeIDDrawSurface7Prime::PageLock(DWORD a)
{
	return(m_pIDDrawSurface->PageLock(a));	
}


HRESULT FakeIDDrawSurface7Prime::PageUnlock(DWORD a)
{
	return(m_pIDDrawSurface->PageUnlock(a));	
}



HRESULT FakeIDDrawSurface7Prime::SetSurfaceDesc(LPDDSURFACEDESC2 a, DWORD b)
{
	return(m_pIDDrawSurface->SetSurfaceDesc(a, b));	
}



HRESULT FakeIDDrawSurface7Prime::SetPrivateData(REFGUID a, LPVOID b, DWORD c, DWORD d)
{
	return(m_pIDDrawSurface->SetPrivateData(a, b, c, d));	
}


HRESULT FakeIDDrawSurface7Prime::GetPrivateData(REFGUID a, LPVOID b, LPDWORD c)
{
	return(m_pIDDrawSurface->GetPrivateData(a, b, c));	
}


HRESULT FakeIDDrawSurface7Prime::FreePrivateData(REFGUID a)
{
	return(m_pIDDrawSurface->FreePrivateData(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetUniquenessValue(LPDWORD a)
{
	return(m_pIDDrawSurface->GetUniquenessValue(a));	
}


HRESULT FakeIDDrawSurface7Prime::ChangeUniquenessValue()
{
	return(m_pIDDrawSurface->ChangeUniquenessValue());	
}



HRESULT FakeIDDrawSurface7Prime::SetPriority(DWORD a)
{
	HRESULT hResult = m_pIDDrawSurface->SetPriority(a);
	return hResult;	
}


HRESULT FakeIDDrawSurface7Prime::GetPriority(LPDWORD a)
{
	return(m_pIDDrawSurface->GetPriority(a));	
}


HRESULT FakeIDDrawSurface7Prime::SetLOD(DWORD a)
{
	return(m_pIDDrawSurface->SetLOD(a));	
}


HRESULT FakeIDDrawSurface7Prime::GetLOD(LPDWORD a)
{
	return(m_pIDDrawSurface->GetLOD(a));	
}

