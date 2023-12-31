#pragma once

#include "Scene.h"
#include "DynamicCamera.h"
#include "SkyBoxBoss.h"
#include "Player.h"
#include "FootRay.h"
#include "CubeBlock.h"

#include "UIplayerhp.h"
#include "UIplayerstat.h"
#include "UIaimpoint.h"
#include "UIbutton.h"
#include "UIemptyslot.h"
#include "UIbasicslot.h"
#include "UIequipmentslot.h"
#include "UIbigmap.h"
#include "UIEscBackground.h"
#include "UIResumeButton.h"
#include "UIOptionsButton.h"
#include "UIQuitButton.h"
#include "UISpeech_OldMan.h"//Speech Bubble Test

#include "SkeletonKing.h"
#include "DimensionGate.h"

class CBossStage : public Engine::CScene
{
private:
	explicit CBossStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossStage();

public:
	virtual HRESULT Ready_Scene()							override;
	virtual _int	Update_Scene(const _float& fTimeDelta)	override;
	virtual void	LateUpdate_Scene()						override;
	virtual void	Render_Scene()							override;

private:
	HRESULT			Ready_Prototype();
	HRESULT			Ready_Layer_Environment(LAYERTAG _eLayerTag);
	HRESULT			Ready_Layer_GameLogic(LAYERTAG _eLayerTag);
	HRESULT			Ready_Layer_UI(LAYERTAG _eLayerTag);
	virtual HRESULT	Load_Data() override;

public:
	static CBossStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CSkeletonKing*	m_pBoss			= nullptr;
	_bool			m_bExecuteBoss	= false;
	_bool			m_bHekiReki = false;
	_bool			m_bGate = false;
	_bool			m_bOrbSPawn = false;
	_float			m_fGateTime = 0.f;
private:
	virtual void Free() override;

};

