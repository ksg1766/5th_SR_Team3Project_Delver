#include "stdafx.h"
#include "SoundManager.h"
#include "..\Header\Magic_Ball.h"
#include "Export_Function.h"
#include "Player.h"

CMagic_Ball::CMagic_Ball(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CMonster(pGraphicDev)
{

}

CMagic_Ball::CMagic_Ball(const CMagic_Ball& rhs)
	: Engine::CMonster(rhs)
{
}

CMagic_Ball::~CMagic_Ball()
{
	Free();
}

HRESULT CMagic_Ball::Ready_Object(CTransform* pOwner, _float _fSpeed, _vec3 _vOffset)
{
	FAILED_CHECK_RETURN(Add_Component(pOwner), E_FAIL);

	m_eObjectTag = OBJECTTAG::MONSTERBULLET;
	Set_State(STATE::ATTACK);

	m_bCheck = false;
	m_bIsAttack = false;
	m_vPrevPos = _vec3(0.f, 0.f, 0.f);

	m_pTransform->Scale(_vec3(0.8f, 0.8f, 0.8f));

	m_pCollider->InitOBB(
		m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT],
		m_pTransform->LocalScale() * 0.5);


	m_pTransform->Copy_RUL_AddPos(pOwner->Get_Transform()->m_vInfo);
	
	m_fFrame = 0.f;
	m_fSpeed = _fSpeed;
	m_fSound = 0.f;

	CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ATTACK], STATE::ATTACK, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ATTACK, pAnimation);
	pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::DEAD], STATE::DEAD, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::DEAD, pAnimation);

	m_pAnimator->Set_Animation(STATE::ATTACK);

	BASICSTAT* pOwnerStat = static_cast<CMonster*>(pOwner->Get_Host())->Get_BasicStat()->Get_Stat();

	if (pOwnerStat != nullptr)
	{
		m_pBasicStat->Get_Stat()->iDamageMin = pOwnerStat->iDamageMin;
		m_pBasicStat->Get_Stat()->iDamageMax = pOwnerStat->iDamageMax;
	}


	return S_OK;
}

_int CMagic_Ball::Update_Object(const _float& fTimeDelta)
{
	if (IsDead())
		return 0;

	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	CPlayer& pPlayer = *static_cast<CPlayer*>(SceneManager()->GetInstance()->
		Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());
	
	m_pAnimator->Update_Animator(fTimeDelta);

	if (!m_bCheck)
	{
		if (m_pAnimator->Get_Animation()->Get_Frame() >= 1)
			m_pAnimator->Get_Animation()->Set_Loop(FALSE);
	}

	if (!m_bIsAttack)
	{
		m_vInit = m_pTransform->m_vInfo[INFO_POS];
		m_vPrevPos = pPlayer.m_pTransform->m_vInfo[INFO_POS];
		m_bIsAttack = true;
	}
	
	_float fDistance = D3DXVec3Length(&(m_pTransform->m_vInfo[INFO_POS] - m_vInit));

	if (fDistance > m_fMaxDistance && Get_State() != STATE::DEAD)
	{
		Set_State(STATE::DEAD);
		EventManager()->DeleteObject(this);
	}
		

	_vec3 vDir = m_vPrevPos - m_vInit;
	D3DXVec3Normalize(&vDir, &vDir);
	
	m_pTransform->m_vInfo[INFO_POS] = m_pTransform->m_vInfo[INFO_POS] + vDir * m_fSpeed * fTimeDelta;

	return _int();
}

void CMagic_Ball::LateUpdate_Object()
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();

	CPlayer& rPlayer = *SceneManager()->Get_Scene()->Get_MainPlayer();

	_vec3 vDir = rPlayer.m_pTransform->m_vInfo[INFO_POS] - m_pTransform->m_vInfo[INFO_POS];
	_float fDistance = D3DXVec3Length(&vDir);

	_float fVolume = m_fMaxVolume - (fDistance / m_fMaxDistance) * (m_fMaxVolume - m_fMinVolume);

	if (fVolume <= 0.f)
		m_fSound = 0.f;
	else
		m_fSound = fVolume;

	//CSoundManager::GetInstance()->PlaySound(L"mg_fire_shoot_03.mp3", CHANNELID::SOUND_BULLET, m_fSound);


	//__super::Compute_ViewZ(&m_pTransform->m_vInfo[INFO_POS]);

}

void CMagic_Ball::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	m_pAnimator->Render_Animator();
	m_pBuffer->Render_Buffer();
}

void CMagic_Ball::Init_Stat()
{
}

void CMagic_Ball::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::ITEM
		&& _pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYER
		&& _pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::MONSTER)
		__super::OnCollisionEnter(_pOther);


	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER && this->Get_State() != STATE::DEAD)
	{
		CPlayerStat& PlayerStat = *static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat();
		this->Set_AttackTick(true);
		IsAttack(&PlayerStat);

		//cout << "마법구 충돌" << endl;

		Set_State(STATE::DEAD);
		m_pAnimator->Set_Animation(STATE::DEAD);
		m_bCheck = true;

	}

	//CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_BULLET);
	EventManager()->DeleteObject(this);

}

void CMagic_Ball::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::ITEM
		&& _pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYER
		&& _pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::MONSTER)
		__super::OnCollisionStay(_pOther);
}

void CMagic_Ball::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

HRESULT CMagic_Ball::Add_Component(CTransform* pOwner)
{
	CComponent* pComponent = nullptr;


	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	if (static_cast<CMonster*>(pOwner->Get_Host())->Get_MonsterTag() == MONSTERTAG::WIZARD)
	{
		pComponent = m_pTexture[(_uint)STATE::ATTACK] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_MonsterBullet_Wizard"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
		pComponent = m_pTexture[(_uint)STATE::DEAD] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_MonsterBulletDelete_Wizard"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	}
	else if(static_cast<CMonster*>(pOwner->Get_Host())->Get_MonsterTag() == MONSTERTAG::ALIEN)
	{
		pComponent = m_pTexture[(_uint)STATE::ATTACK] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_MonsterBullet_Alien"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
		pComponent = m_pTexture[(_uint)STATE::DEAD] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_MonsterBulletDelete_Alien"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	}

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	pComponent = m_pAnimator = static_cast<CAnimator*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CMagic_Ball* CMagic_Ball::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pOwner, _float _fSpeed, _vec3 _vOffset)
{
	CMagic_Ball* pInstance = new CMagic_Ball(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pOwner, _fSpeed, _vOffset)))
	{
		Safe_Release<CMagic_Ball*>(pInstance);

		MSG_BOX("Create MagicBall Failed");
		return nullptr;
	}

	return pInstance;
}

void CMagic_Ball::Free()
{
	__super::Free();
}
