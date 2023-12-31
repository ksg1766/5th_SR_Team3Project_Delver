#include "stdafx.h"
#include "Boss_Sturn.h"
#include "Export_Function.h"
#include "SkeletonKing.h"
#include "Boss_SturnEffect.h"
#include "Boss_MeteorReady.h"
#include "SoundManager.h"
#include "GameManager.h"

CBoss_Sturn::CBoss_Sturn()
{
}

CBoss_Sturn::CBoss_Sturn(LPDIRECT3DDEVICE9 pGraphicDev)
    :CState(pGraphicDev)
{
}

CBoss_Sturn::~CBoss_Sturn()
{
}

HRESULT CBoss_Sturn::Ready_State(CStateMachine* pOwner)
{
    m_pOwner = pOwner;
    m_fCount = 0.f;
    m_bStar = false;
    m_bSound = false;
    return S_OK;
}

STATE CBoss_Sturn::Update_State(const _float& fTimeDelta)
{
    CSkeletonKing* pBoss = static_cast<CSkeletonKing*>(m_pOwner->Get_Host());

    m_fCount += fTimeDelta;

   /* if (!m_bSound)
    {
        CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_BOSS);
        CSoundManager::GetInstance()->PlaySound(L"Boss_Death1.wav", CHANNELID::SOUND_BOSS, 1.f);
        m_bSound = true;
    }*/
    if (!m_bStar)
    {
        CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_BOSS);
        CSoundManager::GetInstance()->PlaySound(L"Boss_Death1.wav", CHANNELID::SOUND_BOSS, 1.f);

        Engine::CGameObject* pGameObject = nullptr;
        pGameObject = CBoss_SturnEffect::Create(m_pGraphicDev);
        static_cast<CBoss_SturnEffect*>(pGameObject)->m_pTransform->Translate(_vec3(3.f,0.f,0.f));
        Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
        m_bStar = true;
    }
    if (6.f < m_fCount)
    {
        CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_BOSS);
        CSoundManager::GetInstance()->PlaySound(L"Boss_Attack1.wav", CHANNELID::SOUND_BOSS, 1.f);

        pBoss->ReSet_Sturn();
        pBoss->Set_Sturn(false);
        m_bStar = false;
        m_bSound = false;
        m_fCount = 0.f;

        //if ((45 > pBoss->Get_BasicStat()->Get_Stat()->fHP)
        //    && (BOSSPHASE::PHASE2 == pBoss->Get_Phase()))
        //{
        //    m_bSound = false;
        //   
        //    pBoss->Set_Phase(BOSSPHASE::PHASE3);
        //    pBoss->Get_BasicStat()->Get_Stat()->fHP = pBoss->Get_BasicStat()->Get_Stat()->fMaxHP;
        //    CGameManager::GetInstance()->PlayMode(PD::ShowBossP3);

        //    return STATE::BOSS_TELEPORT;
        //}

        //if ((30 > pBoss->Get_BasicStat()->Get_Stat()->fHP)
        //    && (BOSSPHASE::PHASE3 == pBoss->Get_Phase()))
        //{
        //    m_bSound = false;

        //    pBoss->Set_Phase(BOSSPHASE::LASTPHASE);
        //   // CGameManager::GetInstance()->PlayMode(PD::ShowBoss);

        //    return STATE::BOSS_TELEPORT;
        //}

        return STATE::BOSS_IDLE;
    }

    pBoss->Set_Sturn(true);
}

void CBoss_Sturn::LateUpdate_State()
{
}

void CBoss_Sturn::Render_State()
{
}

CBoss_Sturn* CBoss_Sturn::Create(LPDIRECT3DDEVICE9 pGraphicDev, CStateMachine* pOwner)
{
    CBoss_Sturn* pState = new CBoss_Sturn(pGraphicDev);
    if (FAILED(pState->Ready_State(pOwner)))
    {
        Safe_Release<CBoss_Sturn*>(pState);

        MSG_BOX("Boss Sturn State Failed");
    }
    return pState;
}

void CBoss_Sturn::Free()
{
    __super::Free();
}
