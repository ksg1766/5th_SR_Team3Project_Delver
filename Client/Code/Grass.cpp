#include "stdafx.h"
#include "..\Header\Grass.h"

#include "Export_Function.h"

CGrass::CGrass(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEnvironment(pGraphicDev)
{
}

CGrass::CGrass(const CGrass& rhs)
	: CEnvironment(rhs)
{
}

CGrass::~CGrass()
{
}

HRESULT CGrass::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::IMMORTAL;
	m_eEnvTag = ENVIRONMENTTAG::GRASS;

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	if (SCENETAG::EDITOR == SceneManager()->Get_Scene()->Get_SceneTag())
		__super::Ready_Object();

	return S_OK;
}

_int CGrass::Update_Object(const _float & fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	_uint iExit = 0;
	if (SCENETAG::EDITOR == SceneManager()->Get_Scene()->Get_SceneTag())
		return iExit;

	iExit = __super::Update_Object(fTimeDelta);

	return iExit;
}

void CGrass::LateUpdate_Object(void)
{
	__super::LateUpdate_Object();

	//m_pBillBoardCom->LateUpdate_Component();
	//m_pTransform->Scale(_vec3(4.f, 4.f, 4.f));
}

void CGrass::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	m_pTextureCom->Render_Texture(m_iGrassNumber);
	m_pRcBf->Render_Buffer();

	//////////////////////////////////////////////////////////////////////
	m_pTransform->Rotate(ROT_Y, D3DXToRadian(90.f));
	_matrix matWorld = m_pTransform->WorldMatrix();
	m_pTransform->Rotate(ROT_Y, D3DXToRadian(-90.f));

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Render_Texture(m_iGrassNumber);
	m_pRcBf->Render_Buffer();
}

HRESULT CGrass::Add_Component(void)
{
	CComponent*			pComponent = nullptr;

	pComponent = m_pRcBf = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTextureCom = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_VillageGrass"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	for (int i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CGrass* CGrass::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGrass*		pInstance = new CGrass(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGrass Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CGrass::Free()
{
	__super::Free();
}
