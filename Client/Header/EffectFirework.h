#pragma once

#include "Export_Function.h"
#include "TempEffect.h"

class CEffectFirework : public CTempEffect
{
private:
	explicit CEffectFirework(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffectFirework();

public:
	HRESULT Ready_Object() override;
	_int    Update_Object(const _float& fTimeDelta) override;
	void    LateUpdate_Object(void) override;
	void    Render_Object(void) override;

private:
	HRESULT	Add_Component(void);

private:
	_vec3  m_vOriginPos;
	_float m_fDistance = 0.f;
	_float m_fHeight = 0.f;
	_float m_fSpeed = 0.f;
	_float m_fResetTime = 0.f;

public:
	static CEffectFirework* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};