#include "stdafx.h"
#include "RandomPotion.h"
#include "Export_Function.h"
#include "Player.h"
#include "EffectExplosion.h"
#include "SoundManager.h"

CRandomPotion::CRandomPotion(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CRandomPotion::CRandomPotion(const CRandomPotion& rhs)
	: CItem(rhs)
{
}

CRandomPotion::~CRandomPotion()
{
	Free();
}

HRESULT CRandomPotion::Ready_Object(_bool _Item)
{
	m_eObjectTag = OBJECTTAG::ITEM;
	m_bWorldItem = _Item;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	if (!Get_WorldItem())
	{
		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));
		m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT],
			m_pTransform->LocalScale());

		CGameObject* pPlayer = SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front();

		if (pPlayer != nullptr)
		{
#pragma region RandomPotion
			m_pBasicStat->Get_Stat()->iDamageMax = 4.f;
			m_pBasicStat->Get_Stat()->iDamageMin = 1.f;
			m_pBasicStat->Get_Stat()->fHP = 3.f;
			m_pBasicStat->Get_Stat()->fMaxHP = 5.f;
#pragma endregion
		}
	}
	else
	{
		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));
	}


	m_ItemID.eItemType = ITEMTYPE::ITEMTYPE_POTIONITEM;
	m_ItemID.eItemID = ITEMID::EAT_POTION5;
	m_ItemID.iCount = 1;

	m_fAddictionTick = 0.f;
	m_bRandomUsage = false;
	m_bUse = false;

	return S_OK;
}

_int CRandomPotion::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	if (m_bRandomUsage)
	{
		Random_Usage(fTimeDelta);
	}

	return iExit;
}

void CRandomPotion::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));
	//	__super::Compute_ViewZ(&m_pTransform->m_vInfo[INFO_POS]);
}

void CRandomPotion::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	m_pTexture->Render_Texture();
	m_pBuffer->Render_Buffer();
}

HRESULT CRandomPotion::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_RandomPotion"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	if (Get_WorldItem())
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

void CRandomPotion::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (!(_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::MONSTER) &&
		!(_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER))
		__super::OnCollisionEnter(_pOther);
}

void CRandomPotion::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CRandomPotion::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CRandomPotion::Random_Usage(const _float& fTimeDelta)
{
	_int iRandomVal = 1 + rand() % 3;
	const _int iAddiction = 1, iHeal = 2, iExplosion = 3;

	CPlayer& rPlayer =
		*static_cast<CPlayer*>(SceneManager()->
			Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());

	switch (iRandomVal)
	{
	case iAddiction:
		rPlayer.Set_Poisoning(true);
		m_bRandomUsage = false;
		break;
	case iHeal:
		rPlayer.Get_Stat()->Get_Stat()->fHP += m_pBasicStat->Get_Stat()->fHP;
		if (rPlayer.Get_Stat()->Get_Stat()->fHP >= rPlayer.Get_Stat()->Get_Stat()->fMaxHP)
			rPlayer.Get_Stat()->Get_Stat()->fHP = rPlayer.Get_Stat()->Get_Stat()->fMaxHP;

		m_bRandomUsage = false;
		break;
	case iExplosion:
		rPlayer.Get_Stat()->Get_Stat()->fHP -= m_pBasicStat->Get_Stat()->iDamageMax;

		CGameObject* pGameObject = nullptr;

		pGameObject = CEffectExplosion::Create(m_pGraphicDev);
		pGameObject->m_pTransform->Translate(rPlayer.m_pTransform->m_vInfo[INFO_POS] + _vec3(0.f,0.f,1.f));
		Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);

		CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_CONSUMABLE);
		CSoundManager::GetInstance()->PlaySound(L"sfx_explode.mp3", CHANNELID::SOUND_CONSUMABLE, 1.f);

		m_bRandomUsage = false;
		break;
	}
	
	CGameObject* pItem = rPlayer.Get_Inventory()->Get_IDItem(m_ItemID.eItemID);

	if (pItem != nullptr && m_bUse)
	{
		ITEMTYPEID	ItemType = static_cast<CItem*>(pItem)->Get_ItemTag();

		if (ItemType.iCount == 1)
		{
			rPlayer.Get_Inventory()->delete_FindItem(ItemType);
			Engine::UIManager()->Delete_FindItemUI(ItemType);
		}
		else if (ItemType.iCount > 1)
		{
			ItemType.iCount = 1;

			rPlayer.Get_Inventory()->delete_FindItem(ItemType);

			//아이템 UI 내부에서도 해당 아이템을 찾아 삭제.
			Engine::UIManager()->Delete_FindItemUI(ItemType);
		}

		m_bUse = false;
	}

}


CRandomPotion* CRandomPotion::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _Item)
{
	CRandomPotion* pInstance = new CRandomPotion(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(_Item)))
	{
		Safe_Release<CRandomPotion*>(pInstance);

		MSG_BOX("Bear Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CRandomPotion::Free()
{
	__super::Free();
}
