#include "stdafx.h"
#include "..\Header\EffectProjectileTrace.h"
#include "EffectTrace.h"
#include "PoolManager.h"
#include "Player.h"

CEffectProjectileTrace::CEffectProjectileTrace(LPDIRECT3DDEVICE9 pGraphicDev)
	: CTempEffect(pGraphicDev)
{
}

CEffectProjectileTrace::~CEffectProjectileTrace()
{
	Free();
}

HRESULT CEffectProjectileTrace::Ready_Object(void)
{
	FAILED_CHECK_RETURN(CTempEffect::Ready_Object(), E_FAIL); // 초기화 및 초기 설정
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_EffectTag = EFFECTTAG::EFFECT_PROJECTILETRACE;

	m_fLife  = 50.f;
	m_fSpeed = 24.f;

	CPlayer* pPlayer = static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer());
	if (pPlayer != nullptr)
	{
		m_vecDir = pPlayer->m_pTransform->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&m_vecDir, &m_vecDir);
	}

	return S_OK;
}

Engine::_int CEffectProjectileTrace::Update_Object(const _float& fTimeDelta)
{
	//Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	m_fCreatTime += 1.f * fTimeDelta;

	if (m_fCreatTime > .05f)
	{
		if (m_fCreatCount > 0.f)
		{
			m_fCreatTime = 0.f;

			CGameObject* pGameObject = CEffectTrace::Create(m_pGraphicDev);
			pGameObject->m_pTransform->Translate(_vec3(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y, m_pTransform->m_vInfo[INFO_POS].z));
			static_cast<CEffectTrace*>(pGameObject)->Set_TraceNumber(m_fCreatCount);
			static_cast<CTempEffect*>(pGameObject)->Set_EffectColor(m_EffectColor);
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
			//CPoolManager::GetInstance()->Create_Effect(EFFECTTAG::, TargetPos);

			m_fCreatCount -= 1.f;
		}
	}

	m_pTransform->m_vInfo[INFO_POS] += m_vecDir * fTimeDelta * m_fSpeed;

	if (m_fTime > m_fLife || m_fFrame == m_fFinal && m_bAnimation && !m_bLoop)
	{
		CPoolManager::GetInstance()->Delete_Object(this);
	}

	_int iExit = CTempEffect::Update_Object(fTimeDelta);

	return iExit;
}

void CEffectProjectileTrace::LateUpdate_Object(void)
{
	CTempEffect::LateUpdate_Object();
}

void CEffectProjectileTrace::Render_Object(void)
{
}

HRESULT CEffectProjectileTrace::Add_Component(void)
{
	CComponent*			pComponent = nullptr;

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform_Logo"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	for (int i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

void CEffectProjectileTrace::Free()
{
	CTempEffect::Free();
}

CEffectProjectileTrace* CEffectProjectileTrace::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffectProjectileTrace*	pInstance = new CEffectProjectileTrace(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("CEffectProjectileTrace Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CEffectProjectileTrace::Set_vDir(_vec3 _vInfo)
{
	m_vecDir = _vInfo;
	D3DXVec3Normalize(&m_vecDir, &m_vecDir);
}
