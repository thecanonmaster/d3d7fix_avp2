#include "StdAfx.h"

typedef ULONG (__stdcall *Release_Type) (LPDIRECT3DDEVICE7);
typedef HRESULT (__stdcall *DrawPrimitiveVB_Type) (LPDIRECT3DDEVICE7,D3DPRIMITIVETYPE,LPDIRECT3DVERTEXBUFFER7,DWORD,DWORD,DWORD);

Release_Type g_dwOriginalRelease = NULL;
DrawPrimitiveVB_Type g_dwOriginalDrawPrimitiveVB = NULL;
DWORD* g_dwOrigDeviceVTable = NULL;
DWORD g_dwNewDeviceVTable[87];
WORD g_wVertexBufferIndices[126];

ULONG __stdcall FakeID3DDevice7_Release(LPDIRECT3DDEVICE7 device)
{
	((DWORD*)device)[0] = (DWORD)g_dwOrigDeviceVTable;
	g_dwOriginalRelease = NULL;
	
	ULONG dwCount = device->Release();
	if (dwCount == 0) 
		logf("FakeID3DDevice::Release - release success");
	else
		logf("FakeID3DDevice::Release - release failed, reference count = %d", dwCount);
	
	return dwCount;
}

HRESULT __stdcall FakeID3DDevice7_DrawPrimitiveVB(LPDIRECT3DDEVICE7 device, D3DPRIMITIVETYPE d3dptPrimitiveType,LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,DWORD dwStartVertex,DWORD dwNumVertices,DWORD dwFlags)
{	
	HRESULT hResult = device->DrawIndexedPrimitiveVB(d3dptPrimitiveType,lpd3dVertexBuffer,dwStartVertex,dwNumVertices,g_wVertexBufferIndices,dwNumVertices,dwFlags); 		
	return hResult;
}

FakeID3D7::FakeID3D7(LPDIRECT3D7 pOriginal)
{
	m_pID3D = pOriginal;
}


FakeID3D7::~FakeID3D7(void)
{

}

HRESULT FakeID3D7::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	return(m_pID3D->QueryInterface(riid, ppvObj));
}

ULONG FakeID3D7::AddRef(void)
{
	ULONG dwResult = m_pID3D->AddRef();
	return dwResult;
}

ULONG FakeID3D7::Release(void)
{
	ULONG dwCount = m_pID3D->Release();
	
	if (dwCount == 0) 
	{
		logf("FakeID3D7::Release - release success");
		m_pID3D = NULL;		
		delete(this); 
	}
	else
	{
		logf("FakeID3D7::Release - release failed, reference count = %d", dwCount);
		m_pID3D = NULL;		
		delete(this); 
	}
	
	return dwCount;
}

HRESULT FakeID3D7::EnumDevices(LPD3DENUMDEVICESCALLBACK7 a,LPVOID b)
{
	return(m_pID3D->EnumDevices(a, b));
}

HRESULT FakeID3D7::CreateDevice(REFCLSID rclsid,LPDIRECTDRAWSURFACE7 lpDDS,LPDIRECT3DDEVICE7* lplpD3DDevice)
{
	HRESULT hResult = m_pID3D->CreateDevice(rclsid, lpDDS, lplpD3DDevice);

	if (g_bWindowed)
	{
		DDSURFACEDESC2 desc;
		memset(&desc, 0, sizeof(DDSURFACEDESC2));
		desc.dwSize = sizeof(DDSURFACEDESC2);
		lpDDS->GetSurfaceDesc(&desc);
		g_dwWidth = desc.dwWidth;
		g_dwHeight = desc.dwHeight;
	}

	g_d3dDevice = *lplpD3DDevice;
	g_ddsBackBuffer = lpDDS;

	if (hResult == DD_OK && GetCurrProfileBool(PO_RADEON_5700))
	{
		logf("Radeon 5700 vertex buffer compatibility mode is enabled");
		
		DWORD* pOrigTable = (DWORD*)*(DWORD*)*lplpD3DDevice; // VX: 87 full table	
		
		if (!g_dwOriginalRelease)
		{
			memcpy(g_dwNewDeviceVTable, pOrigTable, 87 * sizeof(DWORD));
			
			g_dwOrigDeviceVTable = pOrigTable;
			g_dwOriginalRelease = (Release_Type)pOrigTable[2];
			g_dwOriginalDrawPrimitiveVB = (DrawPrimitiveVB_Type)pOrigTable[31];	
			
			g_dwNewDeviceVTable[31] = (DWORD)FakeID3DDevice7_DrawPrimitiveVB;
			g_dwNewDeviceVTable[2] = (DWORD)FakeID3DDevice7_Release;
		}
		
		((DWORD*)*lplpD3DDevice)[0] = (DWORD)g_dwNewDeviceVTable;
		
		// VX: workaround indices
		for (int i = 0; i < 126 ; i++)
		{
			g_wVertexBufferIndices[i] = (WORD)i;
		}
	}
	
	return hResult;
}

HRESULT FakeID3D7::CreateVertexBuffer(LPD3DVERTEXBUFFERDESC lpVBDesc,LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer,DWORD dwFlags)
{
	HRESULT hResult = m_pID3D->CreateVertexBuffer(lpVBDesc, lplpD3DVertexBuffer, dwFlags);

	return hResult;
}

HRESULT FakeID3D7::EnumZBufferFormats(REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, LPVOID lpContext)
{
	HRESULT hResult = m_pID3D->EnumZBufferFormats(riidDevice, lpEnumCallback, lpContext);
	return hResult;		
}

HRESULT FakeID3D7::EvictManagedTextures()
{
	return(m_pID3D->EvictManagedTextures());
}
