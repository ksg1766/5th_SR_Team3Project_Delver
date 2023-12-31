#include "stdafx.h"
#include "..\Header\EffectBubble.h"
#include "PoolManager.h"

CEffectBubble::CEffectBubble(LPDIRECT3DDEVICE9 pGraphicDev)
	: CTempEffect(pGraphicDev)
{
}

CEffectBubble::~CEffectBubble()
{
	Free();
}

HRESULT CEffectBubble::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CTempEffect::Ready_Object(), E_FAIL); // 초기화 및 초기 설정
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_EffectTag = EFFECTTAG::EFFECT_BUBBLE;

	m_bAnimation = true;

	m_fFrame = 0.f;
	m_fFrist = 0.f;
	m_fFinal = 5.f;
	m_fFrameSpeed = CTempEffect::Get_RandomFloat(1.f, 2.f);

	m_fLife       = CTempEffect::Get_RandomFloat(5.f, 10.f);

	m_fUpSpeed    = CTempEffect::Get_RandomFloat(1.f, 3.f);

	m_fEffectScale = CTempEffect::Get_RandomFloat(1.f, 1.5f);

	return S_OK;
}

Engine::_int CEffectBubble::Update_Object(const _float& fTimeDelta)
{
	if (m_RandomSet)
	{
		m_RandomSet = false;
		// 랜덤으로 50%의 확률로 자신 객체 하나 더 생성
		if (CTempEffect::Get_RandomFloat(0.0f, 1.f) > 0.3f)
		{
			CGameObject* pGameObject = CEffectBubble::Create(m_pGraphicDev);
			pGameObject->m_pTransform->Translate(_vec3(m_pTransform->m_vInfo[INFO_POS].x + CTempEffect::Get_RandomFloat(.5f, 1.f), m_pTransform->m_vInfo[INFO_POS].y + CTempEffect::Get_RandomFloat(.5f, 1.f), m_pTransform->m_vInfo[INFO_POS].z + CTempEffect::Get_RandomFloat(.5f, 1.f)));
			static_cast<CTempEffect*>(pGameObject)->Set_RandomSet(false);
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
			//CPoolManager::GetInstance()->Create_Effect(EFFECTTAG::, TargetPos);
		}
	}

	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (m_fTime > m_fLife || m_fFrame == m_fFinal && m_bAnimation && !m_bLoop)
	{
		CPoolManager::GetInstance()->Delete_Object(this);
	}

	_int iExit = CTempEffect::Update_Object(fTimeDelta);

	m_pTransform->m_vInfo[INFO_POS].y += m_fUpSpeed * fTimeDelta;

	return iExit;
}

void CEffectBubble::LateUpdate_Object(void)
{
	CTempEffect::LateUpdate_Object();

	m_pBillBoardCom->LateUpdate_Component();
	m_pTransform->Scale(_vec3(m_fEffectScale, m_fEffectScale, m_fEffectScale));
}

void CEffectBubble::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, & m_pTransform->WorldMatrix());

	m_pTextureCom->Render_Texture((_uint)m_fFrame);
	m_pBufferCom->Render_Buffer();
}

HRESULT CEffectBubble::Add_Component(void)
{
	CComponent*			pComponent = nullptr;

	pComponent = m_pBufferCom = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform_Logo"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTextureCom = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_EffectWhiteBubble"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pBillBoardCom = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	for (int i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

void CEffectBubble::Free()
{
	CTempEffect::Free();
}

CEffectBubble* CEffectBubble::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffectBubble*	pInstance = new CEffectBubble(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("CEffectBubble Create Failed");
		return nullptr;
	}

	return pInstance;
}
