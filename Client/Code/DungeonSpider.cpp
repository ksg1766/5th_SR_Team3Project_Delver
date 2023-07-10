#include "..\Header\DungeonSpider.h"
#include "Export_Function.h"
#include "Terrain.h"
#include "Monster_Move.h"
#include "Monster_Jump.h"
#include "Player.h"

CDungeonSpider::CDungeonSpider(LPDIRECT3DDEVICE9 pGrapicDev)
	: Engine::CMonster(pGrapicDev), m_fFrame(0.f)
{
}

CDungeonSpider::CDungeonSpider(const CDungeonSpider& rhs)
	: Engine::CMonster(rhs), m_fFrame(rhs.m_fFrame)
{
}

CDungeonSpider::~CDungeonSpider()
{

}

HRESULT CDungeonSpider::Ready_Object()
{
	Set_ObjectTag(OBJECTTAG::MONSTER);
	Set_MonsterState(MONSTERTAG::SPIDER);

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	CState* pState = CMonster_Move::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::ROMIMG, pState);
	pState = CMonster_Jump::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::ATTACK, pState);



	CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ROMIMG], STATE::ROMIMG, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ROMIMG, pAnimation);

	pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ATTACK], STATE::ATTACK, 0.12f, TRUE);
	m_pAnimator->Add_Animation(STATE::ATTACK, pAnimation);

	m_pStateMachine->Set_Animator(m_pAnimator);

	m_pStateMachine->Set_State(STATE::ROMIMG);



	m_pBasicStat->Get_Stat()->fHealth = 4.f;
	m_pBasicStat->Get_Stat()->fAttack = 2.f;


	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

	m_pTransform->Translate(_vec3(2.f, 1.f, 5.f));

	return S_OK;
}

_int CDungeonSpider::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this); // ! 렌더를 맨 위에 올려 렌더는 되도록 하고

	if (SceneManager()->Get_GameStop()) { return 0; } // ! Esc 및 M키 누를 시 업데이트 멈추게 하는 용도 입니다.

	_int iExit = __super::Update_Object(fTimeDelta);

	_float fFrameFix = 0.f;

	CTransform* pPlayerTransform = SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front()->m_pTransform;
	NULL_CHECK_RETURN(pPlayerTransform, -1);

	if (m_pBasicStat->Get_Stat()->fHealth <= 0.f)
		EventManager()->DeleteObject(this);

	m_pStateMachine->Update_StateMachine(fTimeDelta);

	//m_pStateMachine->Render_StateMachine();//현재 도스창에 상태를 나타내기 위한 함수

	if (m_pStateMachine->Get_State() != STATE::ATTACK)
		ForceHeight(pPlayerTransform->m_vInfo[INFO_POS]);

	return iExit;
}

void CDungeonSpider::LateUpdate_Object()
{
	if (SceneManager()->Get_GameStop()) { return; } // ! Esc 및 M키 누를 시 업데이트 멈추게 하는 용도 입니다.

	__super::LateUpdate_Object();
}

void CDungeonSpider::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pStateMachine->Render_StateMachine();


	m_pBuffer->Render_Buffer();

#if _DEBUG
	m_pCollider->Render_Collider();
#endif

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDungeonSpider::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	// 충돌 밀어내기 후 이벤트 : 구현하시면 됩니다.
	if (!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::ITEM))
		__super::OnCollisionEnter(_pOther);

	if (_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::PLAYER
		&& this->Get_State() == STATE::ATTACK)
	{
		CPlayerStat& PlayerState = *dynamic_cast<CPlayer*>(_pOther->GetHost())->Get_Stat();

		if (!dynamic_cast<CPlayer*>(_pOther->Get_Host())->Get_AttackTick())
		{
			PlayerState.Take_Damage(1.f);
			dynamic_cast<CPlayer*>(_pOther->Get_Host())->Set_AttackTick(true);
		}

	}
}

void CDungeonSpider::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::OnCollisionStay(_pOther);
	// 충돌 밀어내기 후 이벤트 : 구현하시면 됩니다.
}

void CDungeonSpider::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

HRESULT CDungeonSpider::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);


	// Monster Animation Texture 
	pComponent = m_pTexture[(_uint)STATE::ROMIMG] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Spider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::ATTACK] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_SpiderAttack"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::HIT] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Hit"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::DEAD] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Dead"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);


	pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pCollider = dynamic_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = dynamic_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pStateMachine = dynamic_cast<CStateMachine*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_State"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::STATEMACHINE, pComponent);
	
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



void CDungeonSpider::ForceHeight(_vec3 _vPos)
{
	_float x = (VTXCNTX * VTXITV / 2.f) + _vPos.x;
	_float z = (VTXCNTZ * VTXITV / 2.f) + _vPos.z;

	x /= (_float)VTXITV;
	z /= (_float)VTXITV;

	_int col = ::floorf(x);
	_int row = ::floorf(z);

	_vec3 A = m_pTerrain->LoadTerrainVertex()[row * VTXCNTX + col];
	_vec3 B = m_pTerrain->LoadTerrainVertex()[row * VTXCNTX + col + 1];
	_vec3 C = m_pTerrain->LoadTerrainVertex()[(row + 1) * VTXCNTX + col];
	_vec3 D = m_pTerrain->LoadTerrainVertex()[(row + 1) * VTXCNTX + col + 1];

	_float dx = x - col;
	_float dz = z - row;

	_float height;
	//c-d b-d cdb 
	if (dz < 1.0f - dx)
	{
		/*
		Lerp(_float _a, _float _b, _float _c)
		{
			return a - (a * t) + (b * t);
		}
		*/

		_vec3 uy = B - A;
		_vec3 vy = C - A;

		height = A.y + (uy.y * dx) + (vy.y * dz) + 1.f;
		m_pTransform->m_vInfo[INFO_POS].y = height;
	}// c-a b-a cba
	else
	{
		_vec3 uy = C - D;
		_vec3 vy = B - D;

		height = D.y + (uy.y * (1.f - dx)) + (vy.y * (1.f - dz)) + 1.f;
		m_pTransform->m_vInfo[INFO_POS].y = height;
	}
}

CDungeonSpider* CDungeonSpider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDungeonSpider* pInstance = new CDungeonSpider(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release<CDungeonSpider*>(pInstance);

		MSG_BOX("Spiedt Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CDungeonSpider::Free()
{
	__super::Free();
}
