#pragma once
#include "Cube.h"

BEGIN(Engine)

class CCubeBf;
class CTransform;
class CTexture;

END

class CCubeBlock : public Engine::CGameObject
{
private:
	explicit CCubeBlock(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeBlock(const CCubeBlock& rhs);
	virtual ~CCubeBlock();

public:

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);

private:
	CCubeBf*	m_pBuffer = nullptr;
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;

public:
	static CCubeBlock* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};