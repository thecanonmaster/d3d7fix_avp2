#include "StdAfx.h"

PostProcState ppState;
PPParams pp;

static inline void mkQuad(LPD3DTLVERTEX v, int color = 0, int shift = 0) 
{	
	v[0]=D3DTLVERTEX(D3DVECTOR(-1, (float)(g_dwHeight>>shift), 0 ), 0.5, color, 0, 0, 1 );
	v[1]=D3DTLVERTEX(D3DVECTOR(-1, -1, 0 ), 0.5, color, 0, 0, 0 );
	v[2]=D3DTLVERTEX(D3DVECTOR((float)(g_dwWidth>>shift), (float)(g_dwHeight>>shift), 0 ), 0.5, color, 0, 1, 1 );
	v[3]=D3DTLVERTEX(D3DVECTOR((float)(g_dwWidth>>shift), -1, 0 ), 0.5, color, 0, 1, 0 );
}

static inline void drawQuad(LPDIRECT3DDEVICE7 d, DWORD color=0, int shift=0, LPDIRECTDRAWSURFACE7 surf=NULL, DWORD alphaenable=1, DWORD srcblend=D3DBLEND_SRCALPHA, DWORD destblend=D3DBLEND_INVSRCALPHA, 
							DWORD colorop=D3DTOP_MODULATE, DWORD alphaop=D3DTOP_SELECTARG2, DWORD magfilter=D3DTFG_POINT, DWORD minfilter=D3DTFN_POINT) 
{	
	d->SetTexture(0, surf);
	d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, alphaenable);
	d->SetRenderState(D3DRENDERSTATE_SRCBLEND, srcblend);
	d->SetRenderState(D3DRENDERSTATE_DESTBLEND, destblend);
	d->SetTextureStageState(0, D3DTSS_MAGFILTER, magfilter);
	d->SetTextureStageState(0, D3DTSS_MINFILTER, minfilter);
	d->SetTextureStageState(0, D3DTSS_COLOROP,   colorop);
	d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	d->SetTextureStageState(0, D3DTSS_ALPHAOP,   alphaop);
	d->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	d->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	D3DTLVERTEX v[4];
	mkQuad(v, color, shift);
	d->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &v, 4, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT);
}

void PostProcessRestore()
{
	ppState.Restore();
}

void PostProcessClear()
{
	ppState.ClearSurfaces();
}

void PostProcess(LPDIRECT3DDEVICE7 device, LPDIRECTDRAWSURFACE7 surface) 
{
	// Initialize post-processing surfaces.
	ppState.Init(surface);

	RECT full_r={0, 0, g_dwWidth, g_dwHeight};
	DWORD color;

	// set bloom level
	pp.Bloom = g_dwPPCurrIntensity;
	
	// filtered mode
	DWORD dwMagFilter = D3DTFG_LINEAR;
	DWORD dwMinFilter = D3DTFN_LINEAR;

	// Save some states we need to restore later.
	const int savestages=8;
	struct 
	{
		DWORD D3DRENDERSTATE_ALPHABLENDENABLE,
			  D3DRENDERSTATE_SRCBLEND,
			  D3DRENDERSTATE_DESTBLEND,
			  D3DRENDERSTATE_TEXTUREFACTOR,
			  D3DRENDERSTATE_FOGENABLE;
		struct 
		{
			LPDIRECTDRAWSURFACE7 tex;
			DWORD D3DTSS_MAGFILTER,	D3DTSS_MINFILTER,
				  D3DTSS_COLOROP,	D3DTSS_COLORARG1, D3DTSS_COLORARG2,
				  D3DTSS_ALPHAOP, D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG2, 
				  D3DTSS_TEXCOORDINDEX, D3DTSS_ADDRESS;
		} stage[savestages];
	} state={0};

	device->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &state.D3DRENDERSTATE_ALPHABLENDENABLE);
	device->GetRenderState(D3DRENDERSTATE_SRCBLEND, &state.D3DRENDERSTATE_SRCBLEND);
	device->GetRenderState(D3DRENDERSTATE_DESTBLEND, &state.D3DRENDERSTATE_DESTBLEND);
	device->GetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, &state.D3DRENDERSTATE_TEXTUREFACTOR);
	device->GetRenderState(D3DRENDERSTATE_FOGENABLE, &state.D3DRENDERSTATE_FOGENABLE);
	int c = 0;
	for(c=0; c<savestages; c++) 
	{
		device->GetTexture(c, &state.stage[c].tex);
		device->GetTextureStageState(c, D3DTSS_MAGFILTER, &state.stage[c].D3DTSS_MAGFILTER);
		device->GetTextureStageState(c, D3DTSS_MINFILTER, &state.stage[c].D3DTSS_MINFILTER);
		device->GetTextureStageState(c, D3DTSS_COLOROP, &state.stage[c].D3DTSS_COLOROP);
		device->GetTextureStageState(c, D3DTSS_COLORARG1, &state.stage[c].D3DTSS_COLORARG1);
		device->GetTextureStageState(c, D3DTSS_COLORARG2, &state.stage[c].D3DTSS_COLORARG2);
		device->GetTextureStageState(c, D3DTSS_ALPHAOP, &state.stage[c].D3DTSS_ALPHAOP);
		device->GetTextureStageState(c, D3DTSS_ALPHAARG1, &state.stage[c].D3DTSS_ALPHAARG1);
		device->GetTextureStageState(c, D3DTSS_ALPHAARG2, &state.stage[c].D3DTSS_ALPHAARG2);
		device->GetTextureStageState(c, D3DTSS_TEXCOORDINDEX, &state.stage[c].D3DTSS_TEXCOORDINDEX);
		device->GetTextureStageState(c, D3DTSS_ADDRESS, &state.stage[c].D3DTSS_ADDRESS);
	}

	// just begin
	device->BeginScene();

	// Disable fog
	device->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);

	// Texture addressing is clamped at the edges.
	device->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

	// Copy rgb (base) and luminance (grey) maps out of the current backbuffer.
	ppState.m_ddsBase->Blt(0, surface, &full_r, DDBLT_WAIT, 0);

	if(pp.Bloom) 
	{
		if(pp.BloomThreshold) 
		{
			// Threshold. Subtract a constant color from the backbuffer.
			drawQuad(device, 0xff000000|pp.BloomThreshold, 0, ppState.m_ddsBase, 0, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, D3DTOP_SUBTRACT);
		}

		// Adjust bloom intensity by blending in some black.
		drawQuad(device, (0xff-(pp.Bloom&0xff))<<24, 0, NULL, 1, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, D3DTOP_DISABLE, D3DTOP_DISABLE);

		// Copy the result out to bloom level zero.
		ppState.m_ddsBloom[0]->Blt(0, surface, &full_r, DDBLT_WAIT, 0);

		// Blur and scale down recursively.
		device->SetTextureStageState(0, D3DTSS_MAGFILTER, dwMagFilter); // D3DTFG_LINEAR
		device->SetTextureStageState(0, D3DTSS_MINFILTER, dwMinFilter); // D3DTFN_LINEAR

		float size=.5, sizemult=(float)(10000-pp.BloomSpread)*.0001f;

		for(int i = 1; i < ppState.m_nMaps; i++) 
		{		
			DWORD dwCurrentWidth = ppState.m_dwBloomWidth[i-1];
			DWORD dwCurrentHeight = ppState.m_dwBloomHeight[i-1];
			float fCurrentWidth = (float)dwCurrentWidth;
			float fCurrentHeight = (float)dwCurrentHeight;

			device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			color=0xff000000;
			size *= sizemult;

			struct VERTEX 
			{
				D3DVALUE sx, sy, sz, rhw;
				D3DCOLOR color;
				struct { 
					D3DVALUE u, v; 
				} t[4];
			} 
			
			v[4]={{-1.0, fCurrentHeight, 0, .5, color, {{.5f-size, .5f+size}, {.5f-size, .5f+size}, {.5f-size, .5f+size}, {.5f-size, .5f+size}}}, 
					{-1, -1, 0, .5, color, {{.5f-size, .5f-size}, {.5f-size, .5f-size}, {.5f-size, .5f-size}, {.5f-size, .5f-size}}}, 
					{fCurrentWidth, fCurrentHeight, 0, .5, color, {{.5f+size, .5f+size}, {.5f+size, .5f+size}, {.5f+size, .5f+size}, {.5f+size, .5f+size}}}, 
					{fCurrentWidth, -1, 0, .5, color, {{.5f+size, .5f-size}, {.5f+size, .5f-size}, {.5f+size, .5f-size}, {.5f+size, .5f-size}}}};

			const float offset[4][2]={{1.5, .5}, {-.5, 1.5}, {-1.5, -.5}, {.5, -1.5}};

			// Blend in the bloom map several times with some UV offsets.
			for(c=0; c<4; c++) 
			{
				device->SetTexture(c, ppState.m_ddsBloom[i-1]);
				device->SetTextureStageState(c, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
				device->SetTextureStageState(c, D3DTSS_MAGFILTER, dwMagFilter); // D3DTFG_LINEAR 
				device->SetTextureStageState(c, D3DTSS_MINFILTER, dwMinFilter); // D3DTFN_LINEAR
				device->SetTextureStageState(c, D3DTSS_COLOROP,   D3DTOP_ADD);
				device->SetTextureStageState(c, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				device->SetTextureStageState(c, D3DTSS_COLORARG2, D3DTA_CURRENT);
				device->SetTextureStageState(c, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
				device->SetTextureStageState(c, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
				device->SetTextureStageState(c, D3DTSS_TEXCOORDINDEX, c);
				for(int s = 0; s < 4; s++) 
				{
					v[s].t[c].u += offset[c][0]/fCurrentWidth;
					v[s].t[c].v += offset[c][1]/fCurrentHeight;
				}
			}

			device->SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, 0xff000000|pp.BloomPersistence);
			device->SetTextureStageState(4, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			device->SetTextureStageState(4, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			device->SetTextureStageState(4, D3DTSS_COLORARG2, D3DTA_CURRENT);
			device->SetTextureStageState(4, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
			device->SetTextureStageState(4, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX4, v, 4, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT);

			// VX: Copy the result out to the next level bloom map.
			RECT r={0, 0, dwCurrentWidth, dwCurrentHeight};
			ppState.m_ddsBloom[i]->Blt(0, surface, &r, DDBLT_WAIT, 0);
			
			for(c=0; c<8; c++) 
			{
				device->SetTexture(c, NULL);
				device->SetTextureStageState(c, D3DTSS_COLOROP, D3DTOP_DISABLE);
				device->SetTextureStageState(c, D3DTSS_MAGFILTER, state.stage[c].D3DTSS_MAGFILTER);
				device->SetTextureStageState(c, D3DTSS_MINFILTER, state.stage[c].D3DTSS_MINFILTER);
				device->SetTextureStageState(c, D3DTSS_TEXCOORDINDEX, 0);
			}
		}

		// Render bloom.
		device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
		device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
		
		// Blend all 8 bloom levels in one pass.
		for(int c = 0; c < 8 && c+1<ppState.m_nMaps; c++) 
		{
			device->SetTexture(c, ppState.m_ddsBloom[c+1]);
			device->SetTextureStageState(c, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
			device->SetTextureStageState(c, D3DTSS_MAGFILTER, dwMagFilter); // D3DTFG_LINEAR
			device->SetTextureStageState(c, D3DTSS_MINFILTER, dwMinFilter); // D3DTFN_LINEAR
			device->SetTextureStageState(c, D3DTSS_COLOROP,   D3DTOP_ADD);
			device->SetTextureStageState(c, D3DTSS_COLORARG1, D3DTA_CURRENT);
			device->SetTextureStageState(c, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			device->SetTextureStageState(c, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
			device->SetTextureStageState(c, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		}
		
		D3DTLVERTEX v[4];
		mkQuad(v, 0xff000000, 0);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &v, 4, D3DDP_DONOTCLIP|D3DDP_DONOTUPDATEEXTENTS|D3DDP_DONOTLIGHT);
		
		for(c = 0; c < 8; c++) 
		{
			device->SetTexture(c, NULL);
			device->SetTextureStageState(c, D3DTSS_COLOROP, D3DTOP_DISABLE);
			device->SetTextureStageState(c, D3DTSS_MAGFILTER, state.stage[c].D3DTSS_MAGFILTER);
			device->SetTextureStageState(c, D3DTSS_MINFILTER, state.stage[c].D3DTSS_MINFILTER);
		}
	}

	// Render base.
	drawQuad(device, (pp.BaseSaturation<<24)|pp.Base, 0, ppState.m_ddsBase, 1, D3DBLEND_SRCALPHA, pp.Bloom ? D3DBLEND_ONE : D3DBLEND_ZERO);
		
	device->EndScene();

	// Back to start scene?
	//device->BeginScene();

	// Restore some states.
	device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, state.D3DRENDERSTATE_ALPHABLENDENABLE);
	device->SetRenderState(D3DRENDERSTATE_SRCBLEND, state.D3DRENDERSTATE_SRCBLEND);
	device->SetRenderState(D3DRENDERSTATE_DESTBLEND, state.D3DRENDERSTATE_DESTBLEND);
	device->SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, state.D3DRENDERSTATE_TEXTUREFACTOR);
	device->SetRenderState(D3DRENDERSTATE_FOGENABLE, state.D3DRENDERSTATE_FOGENABLE);

	for(c = 0; c < savestages; c++) 
	{
		device->SetTexture(c, state.stage[c].tex);
		device->SetTextureStageState(c, D3DTSS_MAGFILTER, state.stage[c].D3DTSS_MAGFILTER);
		device->SetTextureStageState(c, D3DTSS_MINFILTER, state.stage[c].D3DTSS_MINFILTER);
		device->SetTextureStageState(c, D3DTSS_COLOROP, state.stage[c].D3DTSS_COLOROP);
		device->SetTextureStageState(c, D3DTSS_COLORARG1, state.stage[c].D3DTSS_COLORARG1);
		device->SetTextureStageState(c, D3DTSS_COLORARG2, state.stage[c].D3DTSS_COLORARG2);
		device->SetTextureStageState(c, D3DTSS_ALPHAOP, state.stage[c].D3DTSS_ALPHAOP);
		device->SetTextureStageState(c, D3DTSS_ALPHAARG1, state.stage[c].D3DTSS_ALPHAARG1);
		device->SetTextureStageState(c, D3DTSS_ALPHAARG2, state.stage[c].D3DTSS_ALPHAARG2);
		device->SetTextureStageState(c, D3DTSS_TEXCOORDINDEX, state.stage[c].D3DTSS_TEXCOORDINDEX);
		device->SetTextureStageState(c, D3DTSS_ADDRESS, state.stage[c].D3DTSS_ADDRESS);
	}
}

PostProcState::PostProcState() 
{
	m_nMaps = 0;
	memset(m_ddsBloom, 0, sizeof(m_ddsBloom));
	m_ddsBase = NULL;
}

PostProcState::~PostProcState() 
{
	ClearSurfaces();
}

void PostProcState::Restore()
{
	if(m_ddsBase && m_ddsBase->IsLost()) m_ddsBase->Restore();

	for(int i = 0; i<m_nMaps; i++)
	{
		if(m_ddsBloom[i] && m_ddsBloom[i]->IsLost()) m_ddsBloom[i]->Restore();
	}
}

void PostProcState::ClearSurfaces()
{
	if(m_ddsBase) 
	{
		m_ddsBase->Release();
		m_ddsBase = NULL;
	}
	
	for(int i = 0; i<m_nMaps; i++)
	{
		if(m_ddsBloom[i]) 
		{
			m_ddsBloom[i]->Release();
			m_ddsBloom[i] = NULL;
		}
	}

	m_nMaps = 0;
}

void PostProcState::Init(LPDIRECTDRAWSURFACE7 dds) 
{
	if(!m_nMaps) 
	{
		pp.Enable = 1;
		pp.Base = 0xffffff;
		pp.BaseSaturation = 255;
		pp.Bloom = 255;
		pp.BloomPersistence = 0x252525; 
		pp.BloomThreshold = 0x202020;
		pp.BloomSpread = 7;
		
		DDSURFACEDESC2 desc;
		memset(&desc, 0, sizeof(DDSURFACEDESC2));
		desc.dwSize = sizeof(DDSURFACEDESC2);
		dds->GetSurfaceDesc(&desc);
	
		if (GetCurrProfileBool(PO_DGVOODOO_MODE))
			desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_SYSTEMMEMORY;
		else
			desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_VIDEOMEMORY;
		
		g_ddMainDDraw->CreateSurface(&desc, &m_ddsBase, 0);

		DWORD dwBloomHeight = (DWORD)(POSTPROCESSING_RES * ((float)desc.dwHeight / 720.0f));
		float fRatio = (float)desc.dwWidth/(float)desc.dwHeight;
		desc.dwHeight = dwBloomHeight;
		desc.dwWidth = DWORD(dwBloomHeight * fRatio);
		int i = 0;
		for(i = 0; i < POST_PROCESS_MAX_MAPS_PBT; i++)
		{			
			g_ddMainDDraw->CreateSurface(&desc, &m_ddsBloom[i], 0);
			m_dwBloomWidth[i] = desc.dwWidth;
			m_dwBloomHeight[i] = desc.dwHeight;
		}
		m_nMaps = i;
		
		logf("Postprocess maps count = %d, size = %d x %d", m_nMaps, m_dwBloomWidth[0], m_dwBloomHeight[0]);
	} 
}
