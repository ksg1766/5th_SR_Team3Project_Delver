#pragma once
#include "Fragile.h"

BEGIN(Engine)

class CCubeBf;
class CTexture;
class CCollider;
class CRigidBody;

END
//부시면 장비아이템 나오게 할 예정
class CBoxCube
	: public Engine::CFragile
{
private:
	explicit CBoxCube(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoxCube(const CBoxCube& rhs);
	virtual ~CBoxCube();

public:

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;

	virtual void	OnCollisionEnter(CCollider* _pOther);
	virtual void	OnCollisionStay(CCollider* _pOther);
	virtual void	OnCollisionExit(CCollider* _pOther);

public:

private:
	HRESULT			Add_Component(void);
	void			Shake_Box(const _float& fTimeDelta);
	void			Drop_RandomItem();
	void			Create_Consum();

private:

public:
	static CBoxCube* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};
