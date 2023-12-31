#include "stdafx.h"
#include "..\Header\SkyBoxBoss.h"

#include "Export_Function.h"

CSkyBoxBoss::CSkyBoxBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CSkyBoxBoss::CSkyBoxBoss(const CSkyBoxBoss& rhs)
	: CGameObject(rhs)
{
}

CSkyBoxBoss::~CSkyBoxBoss()
{
}

HRESULT CSkyBoxBoss::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::SKYBOX;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransform->Scale(_vec3(400.f, 400.f, 400.f));

	return S_OK;
}

_int CSkyBoxBoss::Update_Object(const _float & fTimeDelta)
{

	_int iExit = __super::Update_Object(fTimeDelta);

	Engine::Renderer()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CSkyBoxBoss::LateUpdate_Object(void)
{
	__super::LateUpdate_Object();

	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	::CopyMemory(m_pTransform->m_vInfo[INFO_POS], matView.m[3], sizeof(_vec3));
	//m_pTransformCom->m_vInfo[INFO_POS] = _vec3(matView._41, matView._42 + 3.f, matView._43);


}

void CSkyBoxBoss::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, FALSE);

	m_pTexture->Render_Texture(0);
	m_pCubeBf->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSkyBoxBoss::Add_Component(void)
{
	CComponent*			pComponent = nullptr;

	pComponent = m_pCubeBf = static_cast<CCubeBf*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_CubeBf"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_BossSkyBox"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	return S_OK;
}

CSkyBoxBoss* CSkyBoxBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBoxBoss*		pInstance = new CSkyBoxBoss(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CSkyBoxBoss Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSkyBoxBoss::Free()
{
	__super::Free();
}
