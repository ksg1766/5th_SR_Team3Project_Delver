#include "stdafx.h"
#include "Shield.h"
#include "Export_Function.h"
#include "Player.h" 
#include "EffectDamage.h"
#include "SoundManager.h"
_int g_iCount = 0;

CShield::CShield(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CItem(pGraphicDev)
{
}

CShield::CShield(const CShield& rhs)
	: Engine::CItem(rhs)
{
}

CShield::~CShield()
{
	Free();
}

HRESULT CShield::Ready_Object(_bool _Item)
{
	m_eObjectTag = OBJECTTAG::ITEM;
	m_bWorldItem = _Item;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	if (!Get_WorldItem())
	{
		m_pTransform->Scale(_vec3(-0.3f, 0.3f, 0.3f));
		m_pCollider->
			InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

		CGameObject* pPlayer = SceneManager()->Get_Scene()->Get_MainPlayer();

		m_vDir = pPlayer->m_pTransform->m_vInfo[INFO_LOOK];
		D3DXVec3Normalize(&m_vDir, &m_vDir);
	}
	else
		m_pTransform->Scale(_vec3(-0.3f, 0.3f, 0.3f));

	m_ItemID.eItemType = ITEMTYPE::ITEMTYPE_GENERALITEM;
	m_ItemID.eItemID = ITEMID::GENERAL_SHIELD;
	m_ItemID.iCount = 1;

#pragma region Shield Stat
	m_pBasicStat->Get_Stat()->iArmorMax = 2.f;
	m_pBasicStat->Get_Stat()->iArmorMin = 1.f;
	m_pBasicStat->Get_Stat()->iDamageMax = 1.f;
	m_pBasicStat->Get_Stat()->iDamageMin = 1.f;
#pragma endregion


	m_iAttackTick = 10;
	m_iMoveTick = 10;

	m_bSound = false;
	return S_OK;
}

_int CShield::Update_Object(const _float& fTimeDelta)
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
	CItem* ItemType = static_cast<CItem*>(pPlayer->Get_CurrentEquipLeft());
	ITEMTYPEID ItemID = {};

	if (ItemType != nullptr)
		ItemID = ItemType->Get_ItemTag();

	if (ItemID.eItemID != ITEMID::GENERAL_SHIELD || !pPlayer->Get_ItemEquipLeft())
		return iExit;

	if (!Get_WorldItem())
	{
		if (pPlayer->IsThrowShield() && pPlayer != nullptr)
		{
			if (!m_bSound)
			{
				m_bSound = true;
				CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_WEAPON);
				CSoundManager::GetInstance()->PlaySound(L"ShieldSwing.wav", CHANNELID::SOUND_WEAPON, 1.f);
			}
#pragma region �θ޶�1
			m_iMoveTick = 0;
			if (m_iAttackTick > 0)
			{

				m_pTransform->Rotate(ROT_X, -10.f);
				m_pTransform->RotateAround(m_pTransform->m_vInfo[INFO_POS], m_pTransform->m_vInfo[INFO_UP],
					20.f);
				m_pTransform->Rotate(ROT_Z, 2.f);
				m_pTransform->Translate(m_pTransform->m_pParent->m_vInfo[INFO_LOOK] * 5.f * fTimeDelta);
			}
			else
			{
				m_pTransform->Rotate(ROT_Z, -2.f);
				m_pTransform->RotateAround(m_pTransform->m_vInfo[INFO_POS], m_pTransform->m_vInfo[INFO_UP],
					-20.f);
				m_pTransform->Rotate(ROT_X, 10.f);
				m_pTransform->Translate(m_pTransform->m_pParent->m_vInfo[INFO_LOOK] * -5.f * fTimeDelta);
				pPlayer->Set_Parrying(false);//Msh
			}
			--m_iAttackTick;

			if ((0.f < m_iAttackTick) && (7.f > m_iAttackTick))
				pPlayer->Set_Parrying(true);//Msh
			else
				pPlayer->Set_Parrying(false);//Msh

			if (-9 == m_iAttackTick)
			{
				m_iAttackTick = 10;
				m_bSound = false;
				pPlayer->Set_ThrowShield(false);
			}
#pragma endregion
		}
		else if (pPlayer->Get_StateMachine()->Get_State() == STATE::ROMIMG && !pPlayer->IsJump())
		{
			if (m_iMoveTick > 0)
				m_pTransform->Translate(m_pTransform->m_vInfo[INFO_UP] * 0.01f);
			else
				m_pTransform->Translate(m_pTransform->m_vInfo[INFO_UP] * -0.01f);

			--m_iMoveTick;

			if (-9 == m_iMoveTick)
			{
				m_bSound = false;
				m_iMoveTick = 10;
			}
		}
		else
		{
			CTransform* pPlayerTransform = pPlayer->m_pTransform;

			_vec3 vOffSet = -0.6f * pPlayerTransform->m_vInfo[INFO_RIGHT] + 1.6f * pPlayerTransform->m_vInfo[INFO_LOOK] - 0.5f * pPlayerTransform->m_vInfo[INFO_UP];
			m_pTransform->m_vInfo[INFO_POS] = (pPlayerTransform->m_vInfo[INFO_POS] + vOffSet);

			m_bSound = false;
			m_iMoveTick = 0;
		}
	}
	return iExit;
}

void CShield::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));
}


void CShield::Render_Object(void)
{
	if (!m_bWorldItem) {
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	CPlayer* pPlayer = static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer());

	if (!Get_WorldItem() && pPlayer != nullptr)
	{
		CItem* ItemType = static_cast<CItem*>(pPlayer->Get_CurrentEquipLeft());

		ITEMTYPEID ItemID = {};

		if (ItemType != nullptr)
			ItemID = ItemType->Get_ItemTag();

		if (ItemID.eItemID == ITEMID::GENERAL_SHIELD && pPlayer->Get_ItemEquipLeft())
		{
			m_pTexture->Render_Texture();
			m_pBuffer->Render_Buffer();
		}
	}
	else
	{
		m_pTexture->Render_Texture();
		m_pBuffer->Render_Buffer();
	}

	if (!m_bWorldItem)
	{
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}
}
	
HRESULT CShield::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Shield"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);


	if (!Get_WorldItem())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer());

		m_pTransform->Set_Parent(pPlayer->m_pTransform);
		m_pTransform->Copy_RUL(pPlayer->m_pTransform->m_vInfo);
	}
	else if (Get_WorldItem())
	{
		pComponent = m_pBillBoard = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);
	}

	for (int i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

void CShield::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	/*if (_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::MONSTER) &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYER))
		__super::OnCollisionEnter(_pOther);*/

	CPlayer& pPlayer = *static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer());

	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::MONSTER)
	{
		if (pPlayer.IsThrowShield() && static_cast<CMonster*>(_pOther->Get_Host())->Get_StateMachine()
			->Get_State() != STATE::DEAD)
		{
			pPlayer.IsAttack(static_cast<CMonster*>(_pOther->Get_Host())->Get_BasicStat());
			++g_iCount;

			if (static_cast<CMonster*>(_pOther->Get_Host())->Get_StateMachine()
				->Get_PrevState() != STATE::HIT && g_iCount > 4)
			{
				g_iCount = 0;
				static_cast<CMonster*>(_pOther->Get_Host())->Get_StateMachine()->Set_State(STATE::HIT);
			}

			CGameObject* pGameObject = CEffectDamage::Create(m_pGraphicDev);
			pGameObject->m_pTransform->Translate(_vec3(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y, m_pTransform->m_vInfo[INFO_POS].z));
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		}
	}
}

void CShield::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }


	/*if (_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::MONSTER &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::PLAYER)
		__super::OnCollisionStay(_pOther);*/
}

void CShield::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (!(_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::MONSTER) &&
		!(_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER))
	{
		if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::BLOCK)
		{
			CTransform* pPlayerTransform = SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front()->m_pTransform;

			_vec3 vOffSet = -0.6f * pPlayerTransform->m_vInfo[INFO_RIGHT] + 1.6f * pPlayerTransform->m_vInfo[INFO_LOOK] - 0.5f * pPlayerTransform->m_vInfo[INFO_UP];
			m_pTransform->m_vInfo[INFO_POS] = (pPlayerTransform->m_vInfo[INFO_POS] + vOffSet);
		}
	}
}

CShield* CShield::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _Item)
{
	CShield* pInstance = new CShield(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(_Item)))
	{
		Safe_Release(pInstance);

		MSG_BOX("TempItem Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShield::Free()
{
	__super::Free();
}