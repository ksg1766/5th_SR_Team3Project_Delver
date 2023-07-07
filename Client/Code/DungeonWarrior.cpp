#include "..\Header\DungeonWarrior.h"
#include "Export_Function.h"
#include "Terrain.h"

CDungeonWarrior::CDungeonWarrior(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CMonster(pGraphicDev), m_fFrame(0.f)
{
}

CDungeonWarrior::CDungeonWarrior(const CDungeonWarrior& rhs)
	: Engine::CMonster(rhs), m_fFrame(rhs.m_fFrame)
{

}

CDungeonWarrior::~CDungeonWarrior()
{

}

HRESULT CDungeonWarrior::Ready_Object()
{
	
	Set_ObjectTag(OBJECTTAG::MONSTER);
	m_eMonster = MonsterState::Roaming;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransform->Translate(_vec3(1.f, 1.f, 3.f));
	m_pAI->Set_Transform(m_pTransform);


	return S_OK;
}

_int CDungeonWarrior::Update_Object(const _float& fTimeDelta)
{
	_int iExit = __super::Update_Object(fTimeDelta);


	CTransform* pPlayerTransform = SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front()->m_pTransform;
	NULL_CHECK_RETURN(pPlayerTransform, -1);

	_vec3	vPlayerPos;
	vPlayerPos = pPlayerTransform->m_vInfo[INFO_POS];

	_vec3	vDistance = vPlayerPos - m_pTransform->m_vInfo[INFO_POS];
	// �÷��̾�� ���� ��ġ�� ���� ����
	_float	fDistanceLength = D3DXVec3LengthSq(&vDistance);

	_float	fSightRangeSq = pow(10, 2);


	m_fFrame += 4.f * fTimeDelta;

	if (4.f < m_fFrame)
		m_fFrame = 0.f;


	ForceHeight(m_pTransform->m_vInfo[INFO_POS]);


	if (m_eMonster == MonsterState::Roaming)
	{
		if (fDistanceLength <= fSightRangeSq)
		{
			m_eMonster = MonsterState::Run;
			//m_pAI->Chase_Target(&vPlayerPos, fTimeDelta, 5.f);
		}
		else
		{
			if (m_pAI->Reached_TargetPosition())
				m_pAI->Set_NewTargetPosition();

			else
				m_pAI->Move_To_TargetPosition(fTimeDelta);
		}

	}
	else if (m_eMonster == MonsterState::Run)
	{
		if (fDistanceLength > fSightRangeSq)
		{
			m_eMonster = MonsterState::Roaming;
			m_pAI->Set_NewTargetPosition();
		}
		else
		{
			m_pAI->Chase_Target(&vPlayerPos, fTimeDelta, 5.f);
		}
	}


	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CDungeonWarrior::LateUpdate_Object()
{


	__super::LateUpdate_Object();
}

void CDungeonWarrior::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);



	m_pTexture->Render_Texture((_uint)m_fFrame);
	m_pBuffer->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
}

void CDungeonWarrior::ForceHeight(_vec3 _vPos)
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


HRESULT CDungeonWarrior::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Warrior"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = dynamic_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pAI = dynamic_cast<CWarriorAI*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Warrior_AI"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::MONSTERAI, pComponent);

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