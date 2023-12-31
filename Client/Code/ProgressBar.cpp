#include "stdafx.h"
#include "..\Header\ProgressBar.h"

CProgressBar::CProgressBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: CTempUI(pGraphicDev)
{
}

CProgressBar::~CProgressBar()
{
}

HRESULT CProgressBar::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::UI;
	FAILED_CHECK_RETURN(CTempUI::Ready_Object(), E_FAIL); // �ʱ�ȭ

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransform->m_vInfo[INFO_POS].x = WINCX / 2.f;
	m_pTransform->m_vInfo[INFO_POS].y = 100.f;
	m_pTransform->m_vLocalScale.x = 450.f;
	m_pTransform->m_vLocalScale.y = 18.f;

	WorldMatrix(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y, m_pTransform->m_vLocalScale.x, m_pTransform->m_vLocalScale.y);
	
	m_fCurrentBar = 0.f;
	m_fMaxBar = 102.f;

	return S_OK;
}

Engine::_int CProgressBar::Update_Object(const _float& fTimeDelta)
{
	m_fCurrentBar += 1.f * fTimeDelta * 20.f;

	if (102.f < m_fCurrentBar) {
		m_fCurrentBar = 102.f;
		m_bFinish = true;
	}

	_int iExit = CTempUI::Update_Object(fTimeDelta);

	return iExit;
}

void CProgressBar::LateUpdate_Object(void)
{
	CTempUI::LateUpdate_Object();

	Update_BarUI();
}

void CProgressBar::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &WorldMatrix(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y, m_pTransform->m_vLocalScale.x, m_pTransform->m_vLocalScale.y));

	m_pTextureCom->Render_Texture(1);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &WorldMatrix(m_fPosition, 100.f, 450.f - m_flength, 18.f));

	m_pTextureCom->Render_Texture(2);
	m_pBufferCom->Render_Buffer();
}

HRESULT CProgressBar::Add_Component(void)
{
	CComponent*			pComponent = nullptr;

	pComponent = m_pBufferCom = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform_Logo"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTextureCom = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_StageBackground"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	for (int i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}


void CProgressBar::Key_Input(void)
{
}

void CProgressBar::Update_BarUI()
{
	m_flength = (450.f / m_fMaxBar) * (m_fMaxBar - m_fCurrentBar);
	m_fPosition = (WINCX / 2.f) - (m_flength / 2.f) - (m_fMaxBar - m_fCurrentBar) * 2.15f;
}

void CProgressBar::Free()
{
	CTempUI::Free();
}

CProgressBar* CProgressBar::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CProgressBar*	pInstance = new CProgressBar(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BackGround Create Failed");
		return nullptr;
	}

	return pInstance;
}
