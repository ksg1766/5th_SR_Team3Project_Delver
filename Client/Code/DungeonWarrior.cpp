#include "stdafx.h"
#include "SoundManager.h"
#include "..\Header\DungeonWarrior.h"
#include "Export_Function.h"
#include "Warrior_Attack.h"
#include "Monster_Move.h"
#include "Monster_Hit.h"
#include "Monster_Dead.h"
#include "Longitudinal.h"
#include "HorizontalMove.h"
#include "Player.h"
#include "EffectBlood.h"

#include "PoolManager.h"

CDungeonWarrior::CDungeonWarrior(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CMonster(pGraphicDev)
{
}

CDungeonWarrior::CDungeonWarrior(const CDungeonWarrior& rhs)
	: Engine::CMonster(rhs)
{

}

CDungeonWarrior::~CDungeonWarrior()
{

}

HRESULT CDungeonWarrior::Ready_Object()
{
	Set_ObjectTag(OBJECTTAG::MONSTER);
	Set_MonsterState(MONSTERTAG::WARRIOR);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

	//m_pTransform->Translate(_vec3(1.f, 3.f, 3.f));

	CState* pState = CMonster_Move::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::ROMIMG, pState);
	pState = CWarror_Attack::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::ATTACK, pState);
	pState = CMonster_Hit::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::HIT, pState);
	pState = CMonster_Dead::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::DEAD, pState);
	//
	//
	CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ROMIMG], STATE::ROMIMG, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ROMIMG, pAnimation);
	pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ATTACK], STATE::ATTACK, 6.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ATTACK, pAnimation);
	pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::HIT], STATE::HIT, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::HIT, pAnimation);
	pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::DEAD], STATE::DEAD, 3.f, TRUE);
	m_pAnimator->Add_Animation(STATE::DEAD, pAnimation);

	m_pStateMachine->Set_Animator(m_pAnimator);
	m_pStateMachine->Set_State(STATE::ROMIMG);

	Set_CenterPos(_vec3(25, 12, -40));
	Set_MoveRange(2.f);
	Set_RandomMoveRange(5.f);

#pragma region WarriorStat

	Init_Stat();

#pragma endregion

	return S_OK;
}

_int CDungeonWarrior::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	if (m_pStateMachine->Get_State() == STATE::DEAD)
		m_pRigidBody->UseGravity(false);


	if (IsKnockBack())
	{
		m_pStateMachine->Set_State(STATE::HIT);
		Set_KnockBack(false);
		
	}

	CPlayer& rPlayer = *SceneManager()->Get_Scene()->Get_MainPlayer();

	_float fDistance = D3DXVec3Length(&(rPlayer.m_pTransform->m_vInfo[INFO_POS] - m_pTransform->m_vInfo[INFO_POS]));


	if (fDistance < 15.f)
	{
		if (!m_bSearch)
		{
			m_bSearch = true;
			CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_WARRIOR);
			CSoundManager::GetInstance()->PlaySound(L"en_melee_2_alert_02.mp3", CHANNELID::SOUND_WARRIOR, 1.f);
		}
	}

	if (m_pBasicStat->Get_Stat()->fHP <= 0)
	{
		m_pStateMachine->Set_State(STATE::DEAD);

		if (m_pAnimator->Get_Animation()->Get_Frame() >= 1) // ? ) 확인 부탁드립니다
			m_pAnimator->Get_Animation()->Set_Loop(FALSE);
		{
		
		//////////////////////////////////////////////////////////////////////////////// 이펙트 
			if (m_bDieEffect)
			{
				if (!m_bDeadCheck)
				{
					CGameObject* pGameObject = CEffectBlood::Create(m_pGraphicDev);
					pGameObject->m_pTransform->Translate(_vec3(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y - .95f, m_pTransform->m_vInfo[INFO_POS].z));
					static_cast<CTempEffect*>(pGameObject)->Set_EffectColor(ECOLOR_RED);
					Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);

					m_bDeadCheck = true;
				}
				//m_bDieEffect = false;
			}
		//////////////////////////////////////////////////////////////////////////////// 이펙트 
			
			m_fDeadCoolTime += fTimeDelta;

			if(m_fDeadCoolTime > 3.f)
			CPoolManager::GetInstance()->Delete_Object(this);
		}
	}


	if (m_pStateMachine->Get_State() == STATE::DEAD)
		m_pRigidBody->UseGravity(false);

	m_pStateMachine->Update_StateMachine(fTimeDelta);
	//ForceHeight(m_pTransform->m_vInfo[INFO_POS]);

	return iExit;
}

void CDungeonWarrior::LateUpdate_Object()
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();

	m_pStateMachine->LateUpdate_StateMachine();

	//__super::Compute_ViewZ(&m_pTransform->m_vInfo[INFO_POS]);
}

void CDungeonWarrior::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	m_pStateMachine->Render_StateMachine();
	m_pBuffer->Render_Buffer();
}

void CDungeonWarrior::Init_Stat()
{
	m_pBasicStat->Get_Stat()->fSpeed = 4.f;
	m_pBasicStat->Get_Stat()->fAgility = 4.f;
	m_pBasicStat->Get_Stat()->fDeffense = 4.f;
	m_pBasicStat->Get_Stat()->fMagic = 4.f;
	m_pBasicStat->Get_Stat()->fAttack = 4.f;
	m_pBasicStat->Get_Stat()->fHealth = 4.f;
	m_pBasicStat->Get_Stat()->iExp = 6.f;
}

void CDungeonWarrior::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }



	if (_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYER &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::ITEM &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYERBULLET)
		__super::OnCollisionEnter(_pOther);

	// 충돌 밀어내기 후 이벤트 : 구현하시면 됩니다.

	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER
		&& this->Get_StateMachine()->Get_State() == STATE::ATTACK)
		if (!this->Get_AttackTick())		
		{
			CPlayerStat& PlayerStat = *static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat();
			this->Set_AttackTick(true);
			IsAttack(&PlayerStat);

			//cout << "워리어 공격" << endl;
		}
}

void CDungeonWarrior::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	// 충돌 밀어내기 후 이벤트 : 구현하시면 됩니다.
	if (_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYER &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::ITEM&&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYERBULLET)
		__super::OnCollisionStay(_pOther);


	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::ITEM)
	{
		ITEMTYPEID _eID = static_cast<CItem*>(_pOther->Get_Host())->Get_ItemTag();

		CPlayer& rPlayer = *SceneManager()->Get_Scene()->Get_MainPlayer();

		if (_eID.eItemID == ITEMID::WEAPON_SWORD && rPlayer.Get_Attack())
		{
			CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_WEAPON);
			CSoundManager::GetInstance()->PlaySound(L"attack_04.mp3", CHANNELID::SOUND_WEAPON, 1.f);
		}
	}
}

void CDungeonWarrior::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}


HRESULT CDungeonWarrior::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pRigidBody = static_cast<CRigidBody*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RigidBody"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::RIGIDBODY, pComponent);

	pComponent = m_pTexture[(_uint)STATE::ROMIMG] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Warrior"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::ATTACK] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_WarriorAttack"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::HIT] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_WarriorHit"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::DEAD] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_WarriorDead"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pAnimator = static_cast<CAnimator*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);

	pComponent = m_pStateMachine = static_cast<CStateMachine*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_State"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::STATEMACHINE, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CDungeonWarrior* CDungeonWarrior::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDungeonWarrior* pInstance = new CDungeonWarrior(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release<CDungeonWarrior*>(pInstance);

		MSG_BOX("Warrior Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDungeonWarrior::Free()
{
	__super::Free();
}
