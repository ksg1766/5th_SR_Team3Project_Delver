#include "MiniMeteor.h"
#include "Export_Function.h"
#include "SkeletonKing.h"
#include "Player.h"
#include "BossExplosion.h"
#include "MiniMeteor_Idle.h"
#include "FlyingCamera.h"
#include "CameraManager.h"
#include "PoolManager.h"
CMiniMeteor::CMiniMeteor(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev)
{
}

CMiniMeteor::CMiniMeteor(const CMiniMeteor& rhs)
	: CMonster(rhs)
{
}

CMiniMeteor::~CMiniMeteor()
{
	Free();
}

HRESULT CMiniMeteor::Ready_Object()
{
	m_eObjectTag = OBJECTTAG::MONSTER;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_fScale = 0.3f;
	m_fDuration = 0.f;
	m_bHit = false;
	m_bShake = false;
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());
	m_pBasicStat->Get_Stat()->fAttack = 1.f;
	CState* m_pState = CMiniMeteor_Idle::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::IDLE, m_pState);
	return S_OK;
}

_int CMiniMeteor::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_PRIORITY, this);
	if (SceneManager()->Get_GameStop()) { return 0; }
	_uint iExit = __super::Update_Object(fTimeDelta);
	CFlyingCamera* pCamera = static_cast<CFlyingCamera*>(CCameraManager::GetInstance()->Get_CurrentCam());
	pCamera->Set_ShakeForce(0.f, 0.02f, 0.15f, 2.f);
	pCamera->Shake_Camera();

	if ((3.f < m_fDuration) && (!m_bHit))
	{
		m_fDuration = 0.f;
		Engine::CGameObject* pGameObject = nullptr;
		pGameObject = CBossExplosion::Create(m_pGraphicDev);
		static_cast<CBossExplosion*>(pGameObject)->Set_Scale(2.f);
		static_cast<CBossExplosion*>(pGameObject)->m_pTransform->m_vInfo[INFO_POS] = m_pTransform->m_vInfo[INFO_POS];
		Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		CPoolManager::GetInstance()->Delete_Object(this);
	}
	m_fDuration += fTimeDelta;
	if(1.f > m_fScale)
		m_fScale += fTimeDelta/4;
	m_pTransform->Translate(m_vDir);
	m_pTransform->Rotate(ROT_X, -3.f);
	return iExit;
}

void CMiniMeteor::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(m_fScale, m_fScale, m_fScale));
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());
}

void CMiniMeteor::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	m_pTexture->Render_Texture(40);
	m_pCubeBf->Render_Buffer();//ť�����
}

void CMiniMeteor::Init_Stat()
{
}

void CMiniMeteor::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::OnCollisionEnter(_pOther);
	if (m_bHit) { return; }
	if (OBJECTTAG::PLAYER == _pOther->Get_ObjectTag())
	{
		m_bShake = true;
		CPlayerStat& PlayerState = *(static_cast<CPlayer*>(_pOther->Get_Host())->Get_Stat());
		PlayerState.Take_Damage(this->Get_BasicStat()->Get_Stat()->fAttack);
		this->Set_AttackTick(true);

		Engine::CGameObject* pGameObject = nullptr;
		pGameObject = CBossExplosion::Create(m_pGraphicDev);
		static_cast<CBossExplosion*>(pGameObject)->Set_Scale(2.f);
		_vec3 m_vDis = (static_cast<CPlayer*>(_pOther->Get_Host())->m_pTransform->m_vInfo[INFO_LOOK] * 0.2f);
		static_cast<CBossExplosion*>(pGameObject)->m_pTransform->m_vInfo[INFO_POS] = _vec3(m_pTransform->m_vInfo[INFO_POS].x + m_vDis.x, m_pTransform->m_vInfo[INFO_POS].y, m_pTransform->m_vInfo[INFO_POS].z+ m_vDis.z);
		Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		m_bHit = true;
		//m_IsDead = true;
		CPoolManager::GetInstance()->Delete_Object(this);
	}
}

void CMiniMeteor::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::OnCollisionStay(_pOther);
}

void CMiniMeteor::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	__super::OnCollisionExit(_pOther);
}

void CMiniMeteor::Set_Dir(_vec3 _Dir)
{
	m_vDir = _Dir;
}

HRESULT CMiniMeteor::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pCubeBf = static_cast<CCubeBf*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_CubeBf"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = static_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pCollider = static_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pBasicStat = static_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	pComponent = m_pTexture = static_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Cube"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);

	pComponent = m_pStateMachine = static_cast<CStateMachine*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_State"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::STATEMACHINE, pComponent);

	pComponent = m_pRigidBody = static_cast<CRigidBody*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RigidBody"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::RIGIDBODY, pComponent);
	
	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CMiniMeteor* CMiniMeteor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMiniMeteor* pInstance = new CMiniMeteor(pGraphicDev);
	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);

		MSG_BOX("MiniMeteor Cube Failed");
		return nullptr;
	}
	return pInstance;
}

void CMiniMeteor::Free()
{
	__super::Free();
}
