#include "Npc_OldMan.h"
#include "Export_Function.h"
#include "Terrain.h"
#include "OldMan_Idle.h"
CNpc_OldMan::CNpc_OldMan(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CNpc(pGraphicDev)
{
}

CNpc_OldMan::CNpc_OldMan(const CNpc_OldMan& rhs)
	: Engine::CNpc(rhs)
{
}

CNpc_OldMan::~CNpc_OldMan()
{
}

HRESULT CNpc_OldMan::Ready_Object()
{
	m_eObjectTag = OBJECTTAG::NPC;
	m_eNPCTag = NPCTAG::TRADER;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	CState* pState = CNpc_OldMan_Idle::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::IDLE, pState);

	CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::IDLE], STATE::IDLE, 8.f, TRUE);
	m_pAnimator->Add_Animation(STATE::IDLE, pAnimation);

	m_pStateMachine->Set_Animator(m_pAnimator);
	m_pStateMachine->Set_State(STATE::IDLE);


	D3DXFONT_DESC	lf;
	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));
	lf.Height = 50;//���� ������ ����
	lf.Width = 32;//�ʺ� ������ ����
	lf.Weight = 1000;//������ ����	0~1000
	lf.Italic = false;//����Ӳ�
	lf.MipLevels = 1;//��û�� �Ӽ���. ���� 1�� �� �ؽ�ó ������ ȭ�� ������ �����ϰ� ����
	lf.CharSet = DEFAULT_CHARSET;//��������
	lf.OutputPrecision = OUT_DEFAULT_PRECIS;//��� ���е�
	lf.Quality = DEFAULT_QUALITY;//ǰ��
	lf.PitchAndFamily = DEFAULT_PITCH;//��ġ �� �йи�
	lf.FaceName, "��������";//���ڿ��� ���̰� 32�� �ʰ��ϸ� �ȵ�. �۲ü���.

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &lf, &m_pFontconfig)))
	{
		MessageBox(0, TEXT("D3DXCreateFontIndirect() - FAILED"), 0, 0);
	}
	dynamic_cast<CFont*>(m_pFont)->Set_pFont(m_pFontconfig);
	dynamic_cast<CFont*>(m_pFont)->Set_FontColor(_uint(0xFFFF0000));
	dynamic_cast<CFont*>(m_pFont)->Set_Rect(RECT { 0, 500, WINCX, WINCY });
	dynamic_cast<CFont*>(m_pFont)->Set_Anchor(DT_CENTER| DT_NOCLIP);
	m_fFontTime = 0.f;
	return S_OK;
}

_int CNpc_OldMan::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }
	_uint iExit = __super::Update_Object(fTimeDelta);
	m_fFontTime += fTimeDelta;
	ForceHeight(m_pTransform->m_vInfo[INFO_POS]);
	m_pStateMachine->Update_StateMachine(fTimeDelta);
	return iExit;
}

void CNpc_OldMan::LateUpdate_Object()
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::LateUpdate_Object();
}

void CNpc_OldMan::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pStateMachine->Render_StateMachine();
	m_pBuffer->Render_Buffer();
	if (0.5 < m_fFontTime)
	{
		m_pFont->DrawText(L"W A R R I N G");
		if(1.f < m_fFontTime)
			m_fFontTime = 0.f;
	}
#if _DEBUG
	m_pCollider->Render_Collider();
#endif
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CNpc_OldMan::ForceHeight(_vec3 _vPos)
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

void CNpc_OldMan::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (OBJECTTAG::PLAYER == _pOther->Get_ObjectTag())
	{

	}
}

void CNpc_OldMan::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

void CNpc_OldMan::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
}

HRESULT CNpc_OldMan::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pCollider = dynamic_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

#pragma region �ؽ���
	pComponent = m_pTexture[(_uint)STATE::IDLE] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_NpcOldMan"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
#pragma endregion �ؽ���

	pComponent = dynamic_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pAnimator = dynamic_cast<CAnimator*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);

	pComponent = m_pStateMachine = dynamic_cast<CStateMachine*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_State"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::STATEMACHINE, pComponent);

	pComponent = m_pBasicStat = dynamic_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	pComponent = m_pFont = dynamic_cast<CFont*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Font"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::FONT, pComponent);

	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CNpc_OldMan* CNpc_OldMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNpc_OldMan* pInstance = new CNpc_OldMan(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release<CNpc_OldMan*>(pInstance);

		MSG_BOX("Trader Npc Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CNpc_OldMan::Free()
{
	__super::Free();
}