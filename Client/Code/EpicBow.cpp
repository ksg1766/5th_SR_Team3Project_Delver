#include "stdafx.h"
#include "..\Header\Player.h"

#include "..\Header\EpicBow.h"
#include "Export_Function.h"
#include "EffectDamage.h"

static _int iCount = 0;

CEpicBow::CEpicBow(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CItem(pGraphicDev)
{
}

CEpicBow::CEpicBow(const CEpicBow& rhs)
	: Engine::CItem(rhs)
{
}

CEpicBow::~CEpicBow()
{
	Free();
}

HRESULT CEpicBow::Ready_Object(_bool _Item)
{
	m_eObjectTag = OBJECTTAG::ITEM;
	m_bWorldItem = _Item;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	/*m_pCollider->
		InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());*/

	if (!Get_WorldItem())
	{
		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));

		CGameObject* pPlayer = SceneManager()->Get_Scene()->Get_MainPlayer();

		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));

		CTransform* pPlayerTransform = pPlayer->m_pTransform;

		_vec3 vOffSet = 0.6f * pPlayerTransform->m_vInfo[INFO_RIGHT] + 1.6f * pPlayerTransform->m_vInfo[INFO_LOOK] - 0.4f * pPlayerTransform->m_vInfo[INFO_UP];
		m_pTransform->m_vInfo[INFO_POS] = (pPlayerTransform->m_vInfo[INFO_POS] + vOffSet);

		m_iAttackTick = 10;
		m_fChase = 0.f;
		m_fChase2 = 0.f;
		m_fAngle = 0.f;

		CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
			m_pTexture[_uint(STATE::IDLE)], STATE::IDLE, 0.5f, TRUE);
		m_pAnimator->Add_Animation(STATE::IDLE, pAnimation);
		pAnimation = CAnimation::Create(m_pGraphicDev,
			m_pTexture[_uint(STATE::ROMIMG)], STATE::ROMIMG, 2.f, TRUE);
		m_pAnimator->Add_Animation(STATE::ROMIMG, pAnimation);
		pAnimation = CAnimation::Create(m_pGraphicDev,
			m_pTexture[_uint(STATE::ATTACK)], STATE::ATTACK, 5.f, TRUE);
		m_pAnimator->Add_Animation(STATE::ATTACK, pAnimation);

		m_pAnimator->Set_Animation(STATE::IDLE);
	}
	else
	{
		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));

		m_pCollider->
			InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

		m_pTransform->Translate(_vec3(0.0f, 2.f, 0.0f));
	}

	// 타입 및 아이디 지정
	m_ItemID.eItemType = ITEMTYPE::ITEMTYPE_WEAPONITEM;
	m_ItemID.eItemID = WEAPON_EPICBOW;
	m_ItemID.iCount = 1;

#pragma region EpicBow
	m_pBasicStat->Get_Stat()->iDamageMax = 15.f;
	m_pBasicStat->Get_Stat()->iDamageMin = 10.f;
#pragma endregion

	return S_OK;
}

_int CEpicBow::Update_Object(const _float& fTimeDelta)
{
	if (m_bWorldItem) {
		Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);
	}
	else if (!m_bWorldItem) {
		Engine::Renderer()->Add_RenderGroup(RENDER_ITEM, this);
	}

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	CPlayer* pPlayer = static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer());
	CItem* ItemType = static_cast<CItem*>(pPlayer->Get_CurrentEquipRight());
	ITEMTYPEID ItemID = {};

	if (Get_WorldItem())
	{

		if (m_bDropAnItem)
		{
			DropanItem(pPlayer);

			m_pRigidBody->UseGravity(true);
			m_pRigidBody->Update_RigidBody(fTimeDelta);
		}
		else
			m_pRigidBody->UseGravity(false);
	}

	if (ItemType != nullptr)
		ItemID = ItemType->Get_ItemTag();

	if (ItemID.eItemID != ITEMID::WEAPON_EPICBOW || !pPlayer->Get_ItemEquipRight())
		return iExit;

	if (!Get_WorldItem())
	{
		pPlayer = static_cast<CPlayer*>(m_pTransform->m_pParent->Get_Host());
		_vec3 vUp = *D3DXVec3Cross(&vUp, &m_pTransform->m_pParent->m_vInfo[INFO_LOOK],
			&m_pTransform->m_pParent->m_vInfo[INFO_RIGHT]);
		_matrix matRot;
		_vec3 vLook = *D3DXVec3Cross(&vLook, &m_pTransform->m_pParent->m_vInfo[INFO_UP],
			&m_pTransform->m_pParent->m_vInfo[INFO_RIGHT]);

		m_pRigidBody->UseGravity(false);

		if (pPlayer->Get_Attack())
		{
			m_pAnimator->Get_Animation()->Set_Loop(true);

			if (m_iCount < 24.f)
			{
				++m_iCount;

				m_pTransform->RotateAround(m_pTransform->m_pParent->m_vInfo[INFO_POS], vUp, -0.01f);
				m_pTransform->RotateAround(m_pTransform->m_vInfo[INFO_POS], m_pTransform->m_vInfo[INFO_UP], 0.06f);

				CGameObject* pGameObject = CEffectDamage::Create(m_pGraphicDev);
				pGameObject->m_pTransform->Translate(_vec3(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y, m_pTransform->m_vInfo[INFO_POS].z));
				Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);

				m_pAnimator->Set_Animation(STATE::ROMIMG);
			}

			if (m_iCount >= 24)
				m_pAnimator->Set_Animation(STATE::ATTACK);

			pPlayer->Set_Attack(false);
		}
		else
		{
			if (m_iCount > 0.f)
			{
				--m_iCount;
				m_pTransform->RotateAround(m_pTransform->m_pParent->m_vInfo[INFO_POS], vUp, 0.01f);
				m_pTransform->RotateAround(m_pTransform->m_vInfo[INFO_POS], m_pTransform->m_vInfo[INFO_UP], -0.06f);
			}
			else
			{
				CTransform* pPlayerTransform = pPlayer->m_pTransform;

				_vec3 vOffSet = 0.6f * pPlayerTransform->m_vInfo[INFO_RIGHT] + 1.6f * pPlayerTransform->m_vInfo[INFO_LOOK] - 0.4f * pPlayerTransform->m_vInfo[INFO_UP];
				m_pTransform->m_vInfo[INFO_POS] = (pPlayerTransform->m_vInfo[INFO_POS] + vOffSet);

				m_pAnimator->Set_Animation(STATE::IDLE);
			}
		}

		m_pAnimator->Update_Animator(fTimeDelta);

		return iExit;
	}
	else
	{
		m_pAnimator->Set_Animation(STATE::IDLE);
		return iExit;
	}

}

void CEpicBow::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));

	if (m_bDropAnItem)
	m_pCollider->SetCenterPos(m_pTransform->m_vInfo[INFO_POS] - _vec3(0.f, 0.3f, 0.f));
}

void CEpicBow::Render_Object(void)
{
	if (!m_bWorldItem) {
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	CPlayer* pPlayer = static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer());

	if (!Get_WorldItem() && pPlayer != nullptr)
	{
		CItem* ItemType = static_cast<CItem*>(pPlayer->Get_CurrentEquipRight());

		ITEMTYPEID ItemID = {};

		if (ItemType != nullptr)
			ItemID = ItemType->Get_ItemTag();

		if (ItemID.eItemID == ITEMID::WEAPON_EPICBOW && pPlayer->Get_ItemEquipRight())
		{
			m_pAnimator->Render_Animator();
			//m_pTexture->Render_Texture();
			m_pBuffer->Render_Buffer();

		}
	}
	else
	{
		m_pTexture[(_uint)STATE::IDLE]->Render_Texture();
		m_pBuffer->Render_Buffer();
	}

	if (!m_bWorldItem)
	{
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}
}

HRESULT CEpicBow::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture[(_uint)STATE::IDLE] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_EpicBowIdle"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::ROMIMG] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_EpicBowRoming"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::ATTACK] = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_EpicBowAttack"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	pComponent = m_pAnimator = static_cast<CAnimator*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);
	

	pComponent = m_pRigidBody = static_cast<CRigidBody*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RigidBody"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::RIGIDBODY, pComponent);

	//pComponent = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	if (!Get_WorldItem())
	{
		CTransform* pTransform = static_cast<CTransform*>(SceneManager()->Get_Scene()->Get_MainPlayer()->m_pTransform);

		m_pTransform->Set_Parent(pTransform);
		m_pTransform->Copy_RUL(m_pTransform->m_vInfo);

		for (int i = 0; i < ID_END; ++i)
			for (auto& iter : m_mapComponent[i])
				iter.second->Init_Property(this);
	}
	else
	{
		pComponent = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

		for (int i = 0; i < ID_END; ++i)
			for (auto& iter : m_mapComponent[i])
				iter.second->Init_Property(this);
	}

	return S_OK;
}

void CEpicBow::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (!(_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::MONSTER) &&
		!(_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER))
		__super::OnCollisionEnter(_pOther);
	// 몬스터거나 플레이어면 밀어내지않는다.


	if (Get_WorldItem())
	{
		__super::OnCollisionEnter(_pOther);
		m_bDropAnItem = false;
	}
}

void CEpicBow::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (Get_WorldItem())
	{
		__super::OnCollisionStay(_pOther);
		m_bDropAnItem = false;
	}
}

void CEpicBow::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

CEpicBow* CEpicBow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _Item)
{
	CEpicBow* pInstance = new CEpicBow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(_Item)))
	{
		Safe_Release(pInstance);

		MSG_BOX("TempItem Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CEpicBow::Free()
{
	__super::Free();
}
