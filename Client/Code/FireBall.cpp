#include "..\Header\FireBall.h"
#include "Export_Function.h"
#include "Player.h"
#include "EffectProjectileTrace.h"

CFireBall::CFireBall(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CFireBall::CFireBall(const CFireBall& rhs)
	: Engine::CGameObject(rhs)
{
}

CFireBall::~CFireBall()
{
	Free();
}

HRESULT CFireBall::Ready_Object(CTransform* pWeapon, CTransform* pOwner, _float _fSpeed)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eObjectTag = OBJECTTAG::PLAYERBULLET;
	m_eState = STATE::ATTACK;
	//m_pTransform->Set_Parent(pOwner);
	m_pTransform->Scale(_vec3(0.5f, 0.5f, 0.5f));

	m_pCollider->InitOBB(
		m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT],
		m_pTransform->LocalScale());

	if (pWeapon != nullptr && pOwner != nullptr)
	{
		m_pTransform->m_vInfo[INFO_POS] = pWeapon->m_vInfo[INFO_POS];
		m_pTransform->Copy_RUL(pOwner->Get_Transform()->m_vInfo);

		m_vDir = pOwner->Get_Transform()->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&m_vDir, &m_vDir);
	}
	//m_pTransform->Copy_RUL_AddPos(pOwner->Get_Transform()->m_vInfo);

	CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ATTACK], STATE::ATTACK, 10.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ATTACK, pAnimation);

	m_pAnimator->Set_Animation(STATE::ATTACK);
	m_pBasicStat->Get_Stat()->fAttack = 1.f;

	// 투사체 흔적 이펙트 추가
	CGameObject* pGameObjectEffect = m_pEffect = CEffectProjectileTrace::Create(m_pGraphicDev);
	pGameObjectEffect->m_pTransform->Translate(m_pTransform->m_vInfo[INFO_POS]);
	Engine::EventManager()->CreateObject(pGameObjectEffect, LAYERTAG::GAMELOGIC);

	return S_OK;
}

_int CFireBall::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	CPlayer& pPlayer = *dynamic_cast<CPlayer*>(SceneManager()->GetInstance()->
		Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());


	m_pTransform->m_vInfo[INFO_POS] = m_pTransform->m_vInfo[INFO_POS] + m_vDir * 25.f * fTimeDelta;

	m_pAnimator->Update_Animator(fTimeDelta);
	
	return iExit;
}

void CFireBall::LateUpdate_Object()
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();
	__super::Compute_ViewZ(&m_pTransform->m_vInfo[INFO_POS]);
}

void CFireBall::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pStateMachine->Render_StateMachine();
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);


	m_pAnimator->Render_Animator();
	m_pBuffer->Render_Buffer();

#if _DEBUG
	m_pCollider->Render_Collider();
#endif

	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CFireBall::ForceHeight(_vec3 _vPos)
{
}

void CFireBall::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::BLOCK)
	{
		cout << "벽돌 충돌" << endl;

		if (m_pEffect != nullptr)
			Engine::EventManager()->DeleteObject(m_pEffect);

		EventManager()->GetInstance()->DeleteObject(this);
	}

	if (_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::MONSTER && m_eState != STATE::DEAD)
	{
		cout << "파이어볼 몬스터 충돌" << endl;
		
		if (m_pEffect != nullptr)
			Engine::EventManager()->DeleteObject(m_pEffect);

		Set_State(STATE::DEAD);
		EventManager()->GetInstance()->DeleteObject(this);
	}
}

void CFireBall::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CFireBall::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

HRESULT CFireBall::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTexture[(_uint)STATE::ATTACK] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_FireBullet"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pCollider = dynamic_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = dynamic_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pBasicStat = dynamic_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	pComponent = m_pAnimator = dynamic_cast<CAnimator*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CFireBall* CFireBall::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pWeapon, 
	CTransform* pOwner, _float _fSpeed)
{
	CFireBall* pInstance = new CFireBall(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pWeapon, pOwner, _fSpeed)))
	{
		Safe_Release<CFireBall*>(pInstance);

		MSG_BOX("Create FireBall Failed");
		return nullptr;
	}

	return pInstance;
}

void CFireBall::Free()
{
	__super::Free();
}
