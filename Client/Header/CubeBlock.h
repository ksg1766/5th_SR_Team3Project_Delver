#pragma once
#include "Cube.h"

BEGIN(Engine)

class CCubeBf;
class CTransform;
class CTexture;
class CCollider;

END

class CCubeBlock : public Engine::CGameObject
{
protected:
	explicit CCubeBlock(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeBlock(const CCubeBlock& rhs);
	virtual ~CCubeBlock();

public:

public:
	virtual HRESULT		Ready_Object(void) override;
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void) override;
	virtual void		Render_Object(void) override;

	virtual void		OnCollisionEnter(CCollider* _pOther);
	virtual void		OnCollisionStay(CCollider* _pOther);
	virtual void		OnCollisionExit(CCollider* _pOther);

public:
	vector<_vec3>&		LoadCubeVertex()					{ return m_vecCubeVertex; }
	vector<INDEX32>&	LoadCubeIndex()						{ return m_vecCubeIndex; }

	_ubyte				Get_TextureNumber()							{ return m_byTextureNumber; }
	void				Set_TextureNumber(_uint _iTextureNumber)	{ m_byTextureNumber = _iTextureNumber; }

	BLOCKTAG			Get_BlockTag()						{ return m_eBlockTag; }
	void				Set_BlockTag(BLOCKTAG _eBlockTag)	{ m_eBlockTag = _eBlockTag; }
private:
	HRESULT				Add_Component(void);

protected:
	CTexture*	m_pTexture = nullptr;
	_ubyte		m_byTextureNumber;

	vector<_vec3>	m_vecCubeVertex;
	vector<INDEX32>	m_vecCubeIndex;

	BLOCKTAG		m_eBlockTag;

private:
	CCubeBf*	m_pBuffer = nullptr;

public:
	static CCubeBlock* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
