#pragma once

#define POST_PROCESS_MAX_MAPS_PBT 4

class PostProcState
{
public:

	PostProcState();
	~PostProcState();
	void Init(LPDIRECTDRAWSURFACE7 dds);
	void Restore();
	void ClearSurfaces();

	int m_nMaps;
	int m_nStages;
	LPDIRECTDRAWSURFACE7 m_ddsBloom[POST_PROCESS_MAX_MAPS_PBT];
	DWORD m_dwBloomWidth[POST_PROCESS_MAX_MAPS_PBT];
	DWORD m_dwBloomHeight[POST_PROCESS_MAX_MAPS_PBT];
	LPDIRECTDRAWSURFACE7 m_ddsBase;
};

struct PPParams 
{
	DWORD Enable;
	DWORD Base;
	DWORD BaseSaturation;
	DWORD Bloom;
	DWORD BloomPersistence;
	DWORD BloomThreshold;
	DWORD BloomSpread;
};

void PostProcess(LPDIRECT3DDEVICE7 device, LPDIRECTDRAWSURFACE7 surface);
void PostProcessRestore();
void PostProcessClear();
