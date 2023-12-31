#pragma once
#include "State.h"

class CBoss_Sturn :
    public CState
{
private:
    explicit CBoss_Sturn();
    explicit CBoss_Sturn(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBoss_Sturn();


public:
    virtual HRESULT 	Ready_State(CStateMachine* pOwner);
    virtual STATE		Update_State(const _float& fTimeDelta);
    virtual	void		LateUpdate_State();
    virtual void		Render_State();

public:
    virtual STATE	Key_Input(const _float& fTimeDelta) { return STATE(); }

private:
    //클래스 내에서만 쓸 함수
    //STATE 
private:
    //변수
    _float m_fCount;
    _bool   m_bStar;
    _bool   m_bSound;
public:
    static CBoss_Sturn* Create(LPDIRECT3DDEVICE9 pGraphicDev, CStateMachine* pOwner);

private:
    virtual void Free();
};

