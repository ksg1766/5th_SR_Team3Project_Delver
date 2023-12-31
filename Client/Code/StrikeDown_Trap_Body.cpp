#include "StrikeDown_Trap_Body.h"
#include "Export_Function.h"
#include "Player.h"
CStrikeDown_Trap::CStrikeDown_Trap(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CTrap(pGraphicDev)
{
}

CStrikeDown_Trap::CStrikeDown_Trap(const CStrikeDown_Trap& rhs)
	:Engine::CTrap(rhs)
{
}

CStrikeDown_Trap::~CStrikeDown_Trap()
{
}

HRESULT CStrikeDown_Trap::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::TRAP;
	m_eTrapTag = TRAPTAG::STRIKEDOWN;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransform->Scale(_vec3(2.f, 2.f, 2.f));
	m_fDelayTime = 0.f;
	m_bAttack = false;
	m_bMinHeight = false;
	m_bMaxHeight = false;
	m_bPlayerHit = false;
	m_bUp = false;
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale() * 1.0f);

	if (SCENETAG::EDITOR == SceneManager()->Get_Scene()->Get_SceneTag())
		__super::Ready_Object();

	return S_OK;
}

_int CStrikeDown_Trap::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_NONALPHA, this);
	_uint iExit = 0;
	if (SCENETAG::EDITOR == SceneManager()->Get_Scene()->Get_SceneTag())
		return iExit;

	if (SceneManager()->Get_GameStop()) { return 0; }
	iExit = __super::Update_Object(fTimeDelta);
	m_fDelayTime += fTimeDelta;
	if(!m_bUp)
		m_pTransform->Translate(_vec3(0.f, -0.3f, 0.f));
	if(m_bUp)
		m_pTransform->Translate(_vec3(0.f, 0.2f, 0.f));

	if ((!m_bUp)&&(m_fMinHeight >= m_pTransform->m_vInfo[INFO_POS].y))
	{
		m_pTransform->m_vInfo[INFO_POS].y = m_fMinHeight;
		m_bUp = true;
	}
	else if ((m_bUp) && (m_fInitialHeight <= m_pTransform->m_vInfo[INFO_POS].y))
	{
		m_pTransform->m_vInfo[INFO_POS].y = m_fInitialHeight;
	}
	if((m_bUp)&&(5.0f < m_fDelayTime))
	{
		m_bUp = false;
		m_fDelayTime = 0.f;
	}
	return iExit;
	}




void CStrikeDown_Trap::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::LateUpdate_Object();
}

void CStrikeDown_Trap::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	m_pTexture->Render_Texture();
	m_pCubeBf->Render_Buffer();
}

void CStrikeDown_Trap::Ground_Pounding(const _float& fTimeDelta)
{
	if (!m_bUp)
		m_pTransform->Translate(_vec3(0.f, -0.3f, 0.f));
	if (m_bUp)
		m_pTransform->Translate(_vec3(0.f, 0.2f, 0.f));

	if ((!m_bUp) && (m_fMinHeight >= m_pTransform->m_vInfo[INFO_POS].y))
	{
		m_pTransform->m_vInfo[INFO_POS].y = m_fMinHeight;
		m_fDelayTime = 0.f;
		m_bUp = true;
	}
	else if ((m_bUp) && (m_fInitialHeight <= m_pTransform->m_vInfo[INFO_POS].y))
	{
		m_pTransform->m_vInfo[INFO_POS].y = m_fInitialHeight;
	}
	if ((m_bUp) && (5.0f < m_fDelayTime))
	{
		m_bUp = false;
		m_fDelayTime = 0.f;
	}

}

void CStrikeDown_Trap::Set_InitailHeight(_float _Height)
{
	m_fInitialHeight = m_pTransform->m_vInfo[INFO_POS].y + _Height;
}

void CStrikeDown_Trap::Set_MinHeight(_float _Height)
{
	m_fMinHeight = m_pTransform->m_vInfo[INFO_POS].y - _Height;
}

void CStrikeDown_Trap::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	/*if (m_bMinHeight) { return; }
	m_pOtherObj = _pOther->Get_Host();
	if (OBJECTTAG::PLAYER == m_pOtherObj->Get_ObjectTag())
	{
		if (!m_bPlayerHit) {return;}
		CPlayerStat& PlayerState = *static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat();
		PlayerState.Take_Damage(2.f);
		m_bPlayerHit = false;

		_vec3	vDir = _pOther->Get_Host()->m_pTransform->m_vInfo[INFO_POS] - m_pTransform->m_vInfo[INFO_POS];
		(static_cast<CPlayer*>(_pOther->Get_Host())->Get_RigidBody()->Add_Force(_vec3(vDir.x, 1.1f * 5.f, vDir.z)));
		(static_cast<CPlayer*>(_pOther->Get_Host())->Get_RigidBody()->UseGravity(true));
		(static_cast<CPlayer*>(_pOther->Get_Host())->Set_JumpState(true));
	}*/

	if (OBJECTTAG::PLAYER == _pOther->Get_Host()->Get_ObjectTag())
	{
		if (!m_bUp)
		{
			CPlayerStat& PlayerState = *static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat();
			PlayerState.Take_Damage(2.f);
		}
	}
}

void CStrikeDown_Trap::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CStrikeDown_Trap::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

HRESULT CStrikeDown_Trap::Add_Component(void)
{
	CComponent* pComponent = nullptr;
	pComponent = m_pCubeBf = static_cast<CCubeBf*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_CubeBf"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_ProjectileTrapBody"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = static_cast<CRigidBody*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RigidBody"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::RIGIDBODY, pComponent);

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CStrikeDown_Trap* CStrikeDown_Trap::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStrikeDown_Trap* pInstance = new CStrikeDown_Trap(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Projectile_Trap_Body Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CStrikeDown_Trap::Free()
{
	__super::Free();
}
