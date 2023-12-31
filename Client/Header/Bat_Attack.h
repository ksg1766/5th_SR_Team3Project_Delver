#pragma once

#include "State.h"

BEGIN(Engine)


END


class CBat_Attack : public CState
{
private:
	explicit CBat_Attack();
	explicit CBat_Attack(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBat_Attack();

public:
	virtual HRESULT	Ready_State(CStateMachine* pOwner);
	virtual STATE	Update_State(const _float& fTimeDelta);
	virtual void	LateUpdate_State();
	virtual void	Render_State();

public:
	virtual STATE	Key_Input(const _float& fTimeDelta) { return STATE(); }

private:
	_vec3	m_vPrevPos;
	_bool	m_bIsAttack;
	_float	m_fSpeed;

public:
	static CBat_Attack* Create(LPDIRECT3DDEVICE9 pGraphicDev, CStateMachine* pOwner);

private:
	virtual void Free();
};