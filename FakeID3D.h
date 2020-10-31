#pragma once

class FakeID3D7 : public IDirect3D7
{
public:
    FakeID3D7(LPDIRECT3D7 pOriginal);
    virtual ~FakeID3D7(void);
	
	/*** IUnknown methods ***/
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);
	
	/*** IDirect3D7 methods ***/
	HRESULT __stdcall EnumDevices(LPD3DENUMDEVICESCALLBACK7 a,LPVOID b);
	HRESULT __stdcall CreateDevice(REFCLSID a,LPDIRECTDRAWSURFACE7 b,LPDIRECT3DDEVICE7* c);
	HRESULT __stdcall CreateVertexBuffer(LPD3DVERTEXBUFFERDESC a,LPDIRECT3DVERTEXBUFFER7* b,DWORD c);
	HRESULT __stdcall EnumZBufferFormats(REFCLSID a,LPD3DENUMPIXELFORMATSCALLBACK b,LPVOID c);
	HRESULT __stdcall EvictManagedTextures(void);
	
   	LPDIRECT3D7 FAR m_pID3D;
};

class FakeID3DVertexBuffer7 : public IDirect3DVertexBuffer7
{
public:
	
	FakeID3DVertexBuffer7(LPDIRECT3DVERTEXBUFFER7 pOriginal);
    virtual ~FakeID3DVertexBuffer7(void);

	/*** IUnknown methods ***/
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);

    /*** IDirect3DVertexBuffer7 methods ***/
    HRESULT __stdcall Lock(DWORD a,LPVOID* b,LPDWORD c);
    HRESULT __stdcall Unlock(void);
    HRESULT __stdcall ProcessVertices(DWORD a,DWORD b,DWORD c,LPDIRECT3DVERTEXBUFFER7 d,DWORD e,LPDIRECT3DDEVICE7 f,DWORD j);
    HRESULT __stdcall GetVertexBufferDesc(LPD3DVERTEXBUFFERDESC a);
    HRESULT __stdcall Optimize(LPDIRECT3DDEVICE7 a,DWORD b);
    HRESULT __stdcall ProcessVerticesStrided(DWORD a,DWORD b,DWORD c,LPD3DDRAWPRIMITIVESTRIDEDDATA d,DWORD e,LPDIRECT3DDEVICE7 f,DWORD j);

	virtual LPDIRECT3DVERTEXBUFFER7 getAsDirect3DVertexBuffer()
	{
		return m_pID3DVertexBuffer;
	}	

	LPDIRECT3DVERTEXBUFFER7 FAR m_pID3DVertexBuffer;
};

class FakeID3DDevice7: public IDirect3DDevice7
{
public:
	
	FakeID3DDevice7(LPDIRECT3DDEVICE7 pOriginal);
    virtual ~FakeID3DDevice7(void);

    HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);
	
    HRESULT __stdcall GetCaps(LPD3DDEVICEDESC7 a);
    HRESULT __stdcall EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK a,LPVOID b);
    HRESULT __stdcall BeginScene(THIS);
    HRESULT __stdcall EndScene(THIS);
    HRESULT __stdcall GetDirect3D(LPDIRECT3D7* a);
    HRESULT __stdcall SetRenderTarget(LPDIRECTDRAWSURFACE7 a,DWORD b);
    HRESULT __stdcall GetRenderTarget(LPDIRECTDRAWSURFACE7 *a);
    HRESULT __stdcall Clear(DWORD a,LPD3DRECT b,DWORD c,D3DCOLOR d,D3DVALUE e,DWORD f);
    HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE a,LPD3DMATRIX b);
    HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE a,LPD3DMATRIX b);
    HRESULT __stdcall SetViewport(LPD3DVIEWPORT7 a);
    HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE a,LPD3DMATRIX b);
    HRESULT __stdcall GetViewport(LPD3DVIEWPORT7 a);
    HRESULT __stdcall SetMaterial(LPD3DMATERIAL7 a);
    HRESULT __stdcall GetMaterial(LPD3DMATERIAL7 a);
    HRESULT __stdcall SetLight(DWORD a,LPD3DLIGHT7 b);
    HRESULT __stdcall GetLight(DWORD a,LPD3DLIGHT7 b);
    HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE a,DWORD b);
    HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE a,LPDWORD n);
    HRESULT __stdcall BeginStateBlock(THIS);
    HRESULT __stdcall EndStateBlock(LPDWORD a);
    HRESULT __stdcall PreLoad(LPDIRECTDRAWSURFACE7 a);
    HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE a,DWORD b,LPVOID c,DWORD d,DWORD e);
    HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE a,DWORD b,LPVOID c,DWORD d,LPWORD e,DWORD f,DWORD g);
    HRESULT __stdcall SetClipStatus(LPD3DCLIPSTATUS a);
    HRESULT __stdcall GetClipStatus(LPD3DCLIPSTATUS a);
    HRESULT __stdcall DrawPrimitiveStrided(D3DPRIMITIVETYPE a,DWORD b,LPD3DDRAWPRIMITIVESTRIDEDDATA c,DWORD d,DWORD e);
    HRESULT __stdcall DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE a,DWORD b,LPD3DDRAWPRIMITIVESTRIDEDDATA c,DWORD d,LPWORD e,DWORD f,DWORD g);
    HRESULT __stdcall DrawPrimitiveVB(D3DPRIMITIVETYPE a,LPDIRECT3DVERTEXBUFFER7 b,DWORD c,DWORD d,DWORD e);
    HRESULT __stdcall DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE a,LPDIRECT3DVERTEXBUFFER7 b,DWORD c,DWORD d,LPWORD e,DWORD f,DWORD g);
    HRESULT __stdcall ComputeSphereVisibility(LPD3DVECTOR a,LPD3DVALUE b,DWORD c,DWORD d,LPDWORD e);
    HRESULT __stdcall GetTexture(DWORD a,LPDIRECTDRAWSURFACE7 *b);
    HRESULT __stdcall SetTexture(DWORD a,LPDIRECTDRAWSURFACE7 b);
    HRESULT __stdcall GetTextureStageState(DWORD a,D3DTEXTURESTAGESTATETYPE b,LPDWORD c);
    HRESULT __stdcall SetTextureStageState(DWORD a,D3DTEXTURESTAGESTATETYPE b,DWORD c);
    HRESULT __stdcall ValidateDevice(LPDWORD a);
    HRESULT __stdcall ApplyStateBlock(DWORD a);
    HRESULT __stdcall CaptureStateBlock(DWORD a);
    HRESULT __stdcall DeleteStateBlock(DWORD a);
    HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE a,LPDWORD b);
    HRESULT __stdcall Load(LPDIRECTDRAWSURFACE7 a,LPPOINT b,LPDIRECTDRAWSURFACE7 c,LPRECT d,DWORD e);
    HRESULT __stdcall LightEnable(DWORD a,BOOL b);
    HRESULT __stdcall GetLightEnable(DWORD a,BOOL* b);
    HRESULT __stdcall SetClipPlane(DWORD a,D3DVALUE* b);
    HRESULT __stdcall GetClipPlane(DWORD a,D3DVALUE* b);
    HRESULT __stdcall GetInfo(DWORD a,LPVOID b,DWORD c);
	
	LPDIRECT3DDEVICE7 FAR m_pID3DDevice;
};
