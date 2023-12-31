#pragma once

#include "Layer.h"

class CPlayer;
class CDungeonSpider;
class CWorm;

BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene();

public:
	SCENETAG				Get_SceneTag() { return m_eSceneTag; }
	CLayer*					Get_Layer(LAYERTAG eLayerTag);
	vector<CGameObject*>&	Get_ObjectList(LAYERTAG eLayerTag, OBJECTTAG eObjTag) 
							{ return m_mapLayer[eLayerTag]->Get_ObjectList(eObjTag);}

public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene()PURE;

	virtual	HRESULT		Load_Data() { return S_OK; }

public:
	CPlayer*			Get_MainPlayer() { return m_pPlayer; }
	void                Set_MainPlayer(CPlayer* _pPlayer) { m_pPlayer = _pPlayer; }

protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev;
	map<LAYERTAG, CLayer*>			m_mapLayer;
	SCENETAG						m_eSceneTag;

	CPlayer*						m_pPlayer;
	CDungeonSpider*					m_pSpider;
	CWorm*							m_pWorm;

public:
	virtual void	Free();
};

END