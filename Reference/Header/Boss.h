#pragma once
#include "Monster.h"

BEGIN(Engine)

class ENGINE_DLL CBoss :
    public CMonster
{
  /*  enum class BOSS_STATE 
    {   WAKE_UP, IDLE, WALK, ATTACK, SKILL, GROGGY, BERSERK, DYING ,DEAD, STATE_END };*/
protected:
    explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBoss(const CBoss& rhs);
    virtual ~CBoss();

public:
    HRESULT     Ready_Object();
    _int        Update_Object(const _float& fTimeDelta);

private:
    

protected:
    // BOSS_ID m_eBossID;    // 필요하다면 추가
    // MonsterAI Component 추가해서 사용
    // 
// CGameObject로부터 상속
    //map<COMPONENTTAG, CComponent*>	m_mapComponent[ID_END];
    LPDIRECT3DDEVICE9					m_pGraphicDev;
    _bool								m_IsDead;

    OBJECTTAG							m_eObjectTag;
    CTransform*                       m_pTransform;	

protected:
    virtual void	Free(void);
};

END