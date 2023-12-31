#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLight :
    public CComponent
{
private:
	explicit CLight();
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CLight(const CLight& rhs);
	virtual ~CLight();

public:
	void			Light_Switch()			{ m_bOn = !m_bOn; }
	D3DLIGHT9*		Set_LightFatcor()		{ return &m_tLight; }

public:
	HRESULT			Ready_Light(LIGHT_TYPE _eLightType);
	virtual _int	Update_Component(const _float& fTimeDelta);

private:
	D3DLIGHT9		m_tLight;
	_bool			m_bOn;

public:
	static CLight* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	virtual void			Free();

};

END