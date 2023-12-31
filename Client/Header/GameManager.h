#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "Export_Function.h"

BEGIN(Engine)

END

class CPlayer;
class CFlyingCamera;
class CSkeletonKing;
class CGameManager :
    public CBase
{
	DECLARE_SINGLETON(CGameManager)

private:
	explicit CGameManager();
	virtual ~CGameManager();

public:
	_int	Update_Game(const _float& fTimeDelta);
	void	LateUpdate_Game();
	void	Render_Game(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void	PlayMode(PD _ePD_Mode) {  m_ePrev_PD = m_eCurr_PD; m_eCurr_PD = _ePD_Mode; }

public:
	_bool	Get_PlayOnce() { return m_bPlayOnce; }
	PD		Get_PlayMode() { return m_ePrev_PD; }

	void	SetTimer(_float& _fTimer) { m_fTimer = _fTimer; }

private:
	void	ShowVillage(const _float& fTimeDelta);
	void	ShowSewer(const _float& fTimeDelta);
	void	ShowTower(const _float& fTimeDelta);
	void	ShowBoss(const _float& fTimeDelta);
	void	ShowBossP2(const _float& fTimeDelta);
	void	ShowBossP3(const _float& fTimeDelta);
	void	ShowMiniBoss(const _float& fTimeDelta);
	void	MeteorExplosion(const _float& fTimeDelta);
	void	ShowBossClone(const _float& fTimeDelta);
	void	HekirekiIssen(const _float& fTimeDelta);
	void	ClearGame(const _float& fTimeDelta);

private:
	CPlayer*		m_pPlayer;
	CFlyingCamera*	m_pCamera;
	CSkeletonKing*	m_pBoss;

	PD m_eCurr_PD = PD::Normal;
	PD m_ePrev_PD = PD::Normal;

	_float	m_fTimer = 10.f;
	
	_uint	m_iVisitCount = 0;

	_bool	m_bPlayOnce = true;

	// HekiRekiIssen
	_bool	m_bReadyBreath = false;
	_bool	m_bEffectCreated[3] = {0};

public:
	virtual void Free() override;
};

