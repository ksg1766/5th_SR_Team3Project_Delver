#include "stdafx.h"
#include "..\Header\BossFireWave.h"
#include "Export_Function.h"
#include "SoundManager.h"
#include "Player.h"
#include "FlyingCamera.h"
#include "CameraManager.h"
CBossFireWave::CBossFireWave(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CMonster(pGraphicDev), m_fFrame(0.f)
{
}

CBossFireWave::CBossFireWave(const CBossFireWave& rhs)
	:	Engine::CMonster(rhs), m_fFrame(rhs.m_fFrame)
{
}

CBossFireWave::~CBossFireWave()
{
	Free();
}

HRESULT CBossFireWave::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::MONSTERBULLET;

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pBasicStat->Get_Stat()->fAttack = 0.5f;
	m_fDuration = 0.f;
	m_fSpeed = 20.f;
	m_fAngle = 0.f;
	m_fDuration = 0.f;
	m_fScale = 2.f;
	m_bSound = false;
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS]+_vec3(0.f, -1.f, 0.f), &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale()*0.4f);

	return S_OK;
}

_int CBossFireWave::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	m_fDuration += fTimeDelta;
	m_fFrame += 8.f * fTimeDelta * 2;
	if (8.f < m_fFrame)
		m_fFrame = 0.f;
	m_pTransform->Translate(m_vDir* fTimeDelta);
	if (3.f < m_fDuration)
		EventManager()->DeleteObject(this);
	return iExit;
}

void CBossFireWave::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale() * 0.4f);
}

void CBossFireWave::Render_Object(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	m_pTexture->Render_Texture((_uint)m_fFrame);
	m_pBuffer->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void CBossFireWave::Init_Stat()
{
}

void CBossFireWave::Set_Dir(_vec3 _vDir)
{
	m_vDir = _vDir;
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	m_vDir *= m_fSpeed;
}

void CBossFireWave::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	if (_pOther->Get_Host()->Get_ObjectTag() == OBJECTTAG::PLAYER)
	{
		CFlyingCamera* pCamera = static_cast<CFlyingCamera*>(CCameraManager::GetInstance()->Get_CurrentCam());
		pCamera->Set_ShakeForce(0.f, 0.05f, 0.2f, 2.f);
		pCamera->Shake_Camera();

		CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_SPIDER);
		CSoundManager::GetInstance()->PlaySound(L"Fire_Hit1.wav", CHANNELID::SOUND_SPIDER, 1.f);
		CPlayerStat& PlayerState = *(static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat());
		PlayerState.Take_Damage(this->Get_BasicStat()->Get_Stat()->fAttack);
		this->Set_AttackTick(true);
		//Engine::EventManager()->DeleteObject(this);
	}
}

void CBossFireWave::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	
}

void CBossFireWave::OnCollisionExit(CCollider* _pOther)
{

}

HRESULT CBossFireWave::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = static_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_BossFire"));
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

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CBossFireWave* CBossFireWave::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossFireWave* pInstance = new CBossFireWave(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BossProjectitle Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CBossFireWave::Free()
{
	__super::Free();
}
