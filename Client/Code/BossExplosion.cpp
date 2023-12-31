#include "stdafx.h"
#include "..\Header\BossExplosion.h"
#include "Export_Function.h"
#include "Player.h"
#include "SoundManager.h"
#include "FlyingCamera.h"
#include "CameraManager.h"
#include "GameManager.h"

CBossExplosion::CBossExplosion(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CMonster(pGraphicDev)
{
}

CBossExplosion::CBossExplosion(const CBossExplosion& rhs)
	:	Engine::CMonster(rhs)
{
}

CBossExplosion::~CBossExplosion()
{
	Free();
}

HRESULT CBossExplosion::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::MONSTERBULLET;

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_fFrame = 0.f;
	m_iCount = 0.f;
	m_pBasicStat->Get_Stat()->fAttack = 2.0;
	m_fScale = 1.f;
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
	m_bHit = false;
	CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_WIZARD);
	CSoundManager::GetInstance()->PlaySound(L"explode.mp3", CHANNELID::SOUND_WIZARD, 1.f);
	return S_OK;
}

_int CBossExplosion::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	m_fFrame += 16.f * fTimeDelta;

	if (8.f < m_fFrame)
	{
		Engine::EventManager()->DeleteObject(this);
		m_fFrame = 0.f;
	}

	return iExit;
}

void CBossExplosion::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
}

void CBossExplosion::Render_Object(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	m_pTexture->Render_Texture((_uint)m_fFrame);
	m_pBuffer->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void CBossExplosion::Init_Stat()
{
}

void CBossExplosion::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CBossExplosion::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	if (m_bHit) { return; }
	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER)
	{
		CFlyingCamera* pCamera = static_cast<CFlyingCamera*>(CCameraManager::GetInstance()->Get_CurrentCam());
		pCamera->Set_ShakeForce(0.f, 0.15f, 0.2f, 3.f);
		pCamera->Shake_Camera();

		CPlayerStat& PlayerState = *(static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat());
		PlayerState.Take_Damage(this->Get_BasicStat()->Get_Stat()->fAttack);
		this->Set_AttackTick(true);

		(static_cast<CPlayer*>(_pOther->Get_Host())->Get_RigidBody()->Add_Force(_vec3(0.f, 1.1f * 12.f, 0.f)));
		(static_cast<CPlayer*>(_pOther->Get_Host())->Get_RigidBody()->UseGravity(true));
		(static_cast<CPlayer*>(_pOther->Get_Host())->Set_JumpState(true));
		m_bHit = true;

		if(CGameManager::GetInstance()->Get_PlayOnce())
			CGameManager::GetInstance()->PlayMode(PD::MeteorExplosion);
	}
}

void CBossExplosion::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CBossExplosion::Set_StartPos(_vec3 _vec)
{
	m_pTransform->m_vInfo[INFO_POS] = _vec;
}

void CBossExplosion::Set_StartPosY(float _fY)
{
	m_pTransform->m_vInfo[INFO_POS].y += _fY;
}

void CBossExplosion::Set_Scale(_float _fSclae)
{
	m_fScale = _fSclae;
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());
}

HRESULT CBossExplosion::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_BossExplosion"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = static_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	//pComponent = m_pRigidBody = static_cast<CRigidBody*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RigidBody"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::RIGIDBODY, pComponent);

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CBossExplosion* CBossExplosion::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossExplosion* pInstance = new CBossExplosion(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BossProjectitle Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CBossExplosion::Free()
{
	__super::Free();
}
