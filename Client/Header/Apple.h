#pragma once

#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CBasicStat;
class CBillBoard;

END

class CApple : public Engine::CItem
{
private:
	explicit CApple(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CApple(const CApple& rhs);
	virtual ~CApple();

public:
	virtual HRESULT Ready_Object(_bool _Item);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);

public:
	virtual void	OnCollisionEnter(CCollider* _pOther);
	virtual void	OnCollisionStay(CCollider* _pOther);
	virtual void	OnCollisionExit(CCollider* _pOther);

private:

private:
	CRcTex* m_pBuffer = nullptr;
	CTexture* m_pTexture = nullptr;
	CBillBoard* m_pBillBoard = nullptr;

public:
	static CApple* Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _Item);

private:
	virtual void Free() override;
};

