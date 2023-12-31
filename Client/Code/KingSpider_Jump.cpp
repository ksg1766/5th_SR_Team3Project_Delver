#include "stdafx.h"
#include "SoundManager.h"
#include "KingSpider_Jump.h"
#include "Export_Function.h"
#include "Player.h"
#include "KingSpider.h"
#include "FlyingCamera.h"
#include "CameraManager.h"
#include "KingSpiderFog.h"
#include "EffectSquare.h"
CKingSpider_Jump::CKingSpider_Jump()
{
}

CKingSpider_Jump::CKingSpider_Jump(LPDIRECT3DDEVICE9 pGraphicDev)
	: CState(pGraphicDev)
{
}

CKingSpider_Jump::~CKingSpider_Jump()
{
}

HRESULT CKingSpider_Jump::Ready_State(CStateMachine* pOwner)
{
	m_pOwner = pOwner;
	m_bJumpStart = false;
	m_bJumpAttack = false;
	m_fDelay = 0.f;

	m_vFogPos[0] = _vec3(4.5f, 0.f, 0.f);
	m_vFogPos[1] = _vec3(-4.5f, 0.f, 0.f);
	m_vFogPos[2] = _vec3(0., 0.f, 4.5f);
	m_vFogPos[3] = _vec3(0., 0.f, -4.5f);
	m_vFogPos[4] = _vec3(4.5f, 0.f, -4.5f);
	m_vFogPos[5] = _vec3(-4.5f, 0.f, 4.5f);
	m_vFogPos[6] = _vec3(-4.5f, 0.f, -4.5f);
	m_vFogPos[7] = _vec3(4.5f, 0.f, 4.5f);
	return S_OK;
}

STATE CKingSpider_Jump::Update_State(const _float& fTimeDelta)
{
	if (!m_bJumpStart)
	{
		m_fDelay+=fTimeDelta;
		static_cast<CKingSpider*>(m_pOwner->Get_Host())->Get_RigidBody()->UseGravity(false);
		if (1.f < m_fDelay)
		{
			m_fDelay = 0.f;
			m_bJumpStart = true;
		}
	}
	if (m_bJumpStart)
	{
		if (!m_bJumpAttack)
		{
			static_cast<CKingSpider*>(m_pOwner->Get_Host())->Get_RigidBody()->Add_Force(_vec3(0.f, 20.f, 0.f));
			static_cast<CKingSpider*>(m_pOwner->Get_Host())->Get_RigidBody()->UseGravity(true);
			m_bJumpAttack = true;
		}
		if (m_bJumpAttack) 
		{
			if (14.99f >= m_pOwner->Get_Transform()->m_vInfo[INFO_POS].y)
			{
				m_pOwner->Get_Transform()->m_vInfo[INFO_POS].y = 15.f;
				CFlyingCamera* pCamera = static_cast<CFlyingCamera*>(CCameraManager::GetInstance()->Get_CurrentCam());
				pCamera->Set_ShakeForce(0.f, 0.2f, 1.5f, 2.f);
				pCamera->Shake_Camera();

				static_cast<CKingSpider*>(m_pOwner->Get_Host())->Get_RigidBody()->UseGravity(false);

				CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_BOSS);
				CSoundManager::GetInstance()->PlaySound(L"KingSpider_Landing.wav", CHANNELID::SOUND_BOSS, 1.f);

				if (13.f >= SceneManager()->Get_Scene()->Get_MainPlayer()->m_pTransform->m_vInfo[INFO_POS].y)
				{
					CPlayerStat& PlayerState = *(static_cast<CPlayer*>(SceneManager()->Get_Scene()->Get_MainPlayer())->Get_Stat());
					PlayerState.Take_Damage(3.f);
				}

				for (int i = 0; i < 8; ++i)
				{
					Engine::CGameObject* pGameObject = nullptr;
					pGameObject = CKingSpiderFog::Create(m_pGraphicDev);
					static_cast<CKingSpiderFog*>(pGameObject)->m_pTransform->m_vInfo[INFO_POS] = m_pOwner->Get_Transform()->m_vInfo[INFO_POS] + m_vFogPos[i];
					static_cast<CKingSpiderFog*>(pGameObject)->m_pTransform->m_vInfo[INFO_POS].y = 13.f;
					Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
				}
				pCamera->Reset_ShakeForce();
				m_bJumpStart = false;
				m_bJumpAttack = false;
				return STATE::BOSS_IDLE;
			}
		}

	}
}

void CKingSpider_Jump::LateUpdate_State()
{
}

void CKingSpider_Jump::Render_State()
{
}

CKingSpider_Jump* CKingSpider_Jump::Create(LPDIRECT3DDEVICE9 pGraphicDev, CStateMachine* pOwner)
{
	CKingSpider_Jump* pState = new CKingSpider_Jump(pGraphicDev);

	if (FAILED(pState->Ready_State(pOwner)))
	{
		Safe_Release<CKingSpider_Jump*>(pState);

		MSG_BOX("KingSpider_Jump Failed");
		return nullptr;
	}

	return pState;
}

void CKingSpider_Jump::Free()
{
	__super::Free();
}
