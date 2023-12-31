#pragma once

#include "Monster.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CAnimator;
class CStateMachine;

END

class CTerrain;

class CDungeonWarrior : public CMonster
{

private:
	explicit CDungeonWarrior(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDungeonWarrior(const CDungeonWarrior& rhs);
	virtual ~CDungeonWarrior();

public:
	virtual HRESULT	Ready_Object()							override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object()						override;
	virtual void	Render_Object()							override;

public:
public:
	virtual void	Init_Stat()								override;


public:
	virtual void		OnCollisionEnter(CCollider* _pOther);
	virtual void		OnCollisionStay(CCollider* _pOther);
	virtual void		OnCollisionExit(CCollider* _pOther);

public:
	

private:
	HRESULT	Add_Component();

private:
	CRcTex*			m_pBuffer = nullptr;
	CTexture*		m_pTexture[(_uint)STATE::STATE_END] = {};
	CAnimator*		m_pAnimator = nullptr;

	_bool			m_bSearch = false;

public:
	static CDungeonWarrior* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

