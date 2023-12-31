#pragma once

#include "GameObject.h"
BEGIN(Engine)

class CPlayerBf;
class CTransform;
class CRigidBody;
class CStateMachine;
class CPlayerStat;
class CAnimator;
class CInventory;

END

class CFlyingCamera;
class CPlayer : public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;

private:
	HRESULT			Add_Component(void);
	void			Key_Input(const _float& fTimeDelta);
	void            Use_SlotItem(INVENKEYSLOT _SlotNum);
	void			PoisonDamage(const _float& fTimeDelta);
public:

	const _vec3*	Get_Offset()			{ return &m_vOffset; }
	const _vec3*	Get_LeftOffset()		{ return &m_vLeftOffset; }
	CPlayerStat*	Get_Stat()				{ return m_pStat; }
	//CGameObject*	Get_Item(ITEMTAG _eItem){ return m_pItem[(_uint)_eItem]; }
	CGameObject*	Get_CurrentEquipRight()	{ return m_pCurrentEquipItemRight; }
	CGameObject*    Get_CurrentEquipLeft()  { return m_pCurrentEquipItemLeft; }
	CGameObject*	Get_PrevEquipRight()	{ return m_pPrevEquipItemRight; }
	CGameObject*	Get_PrevEquipLeft()		{ return m_pPrevEquipItemLeft; }
	CInventory*		Get_Inventory()			{ return m_pInventory; }
	_bool			Get_ItemEquipRight()    { return m_bItemEquipRight; }
	_bool			Get_ItemEquipLeft()     { return m_bItemEquipLeft; }
	_bool			Get_Attack()			{ return m_bIsAttack; }
	_bool			Get_AttackTick()		{ return m_bAttackTick; }
	_bool			Get_UseUI()				{ return m_bUseUI; }
	_bool			Get_Drunk()				{ return m_bDrunk; }
	_bool			Poisoned_State()		{ return m_bPoisoning; }
	_bool			InWater()				{ return m_bInWater; }
	_bool			DropWater()				{ return m_bDropWater; }

	_float			Get_Speed()				{ return m_fSpeed; }
	_bool			IsJump()				{ return m_IsJump; }
	CRigidBody*		Get_RigidBody()			{ return m_pRigidBody; }
	CStateMachine*	Get_StateMachine()		{ return m_pStateMachine; }

	_bool			IsThrowShield()			{ return m_bThrowShield; }
	_bool			IsTalk()				{ return m_bIsTalk; }
	_uint			Get_PuzzleResult()		{ return m_iPuzzleCount; }
	_bool			QuestClear()			{ return m_bQuestResult; }
	_bool			AltarOnOrb()			{ return m_bOrbOnAltar; }

public:
	void			Set_Offset(_vec3 _vOffset)						{ m_vOffset = _vOffset; }
	void			Set_ItemEquipRight(_bool _ItemOn)				{ m_bItemEquipRight = _ItemOn; }
	void			Set_CurrentEquipRight(CGameObject* _pCurItem)	{ m_pCurrentEquipItemRight = _pCurItem; }
	void			Set_ItemEquipLeft(_bool _ItemOn)				{ m_bItemEquipLeft = _ItemOn; }
	void			Set_CurrentEquipLeft(CGameObject* _pCurItem)	{ m_pCurrentEquipItemLeft = _pCurItem; }
	void			Set_Attack(_bool _bAttack)						{ m_bIsAttack = _bAttack; }
	void			Set_AttackTick(_bool _bTick)					{ m_bAttackTick = _bTick; }
	void			Set_State(STATE _eState)						{ m_eState = _eState; }
	void			Set_UseUI(_bool _use)							{ m_bUseUI = _use; }
	void			Set_PrevEquipRight(CGameObject* _pPrevItem)		{ m_pPrevEquipItemRight = _pPrevItem; }
	void			Set_PrevEquipLeft(CGameObject* _pPrevItem)		{ m_pPrevEquipItemLeft = _pPrevItem; }
	void			Set_LeftOffset(_vec3 _vOffset)					{ m_vLeftOffset = _vOffset; }
	void			Set_Drunk(_bool _Drunk)							{ m_bDrunk = _Drunk; }
	void			Set_ThrowShield(_bool _Throw)					{ m_bThrowShield = _Throw; }
	void			Set_InWater(_bool _Wather)						{ m_bInWater = _Wather; }
	void			Set_DropWather(_bool _Wather)					{ m_bDropWater = _Wather; }
	// ksg
	void			Set_JumpState(_bool _bJump)						{ m_IsJump = _bJump; }

	void			Set_Addiction(_bool _Addiction)					{ m_bIsAddiction = _Addiction; }
	//Msh
	void			Set_Poisoning(_bool	_bPoisoning)				{ m_bPoisoning = _bPoisoning; }
	void			Set_Parrying(_bool	_bParrying)					{ m_bParrying = _bParrying; }
	void			Set_Talk(_bool _talk)							{ m_bIsTalk = _talk; }
	void			Set_Quest(_bool _result)						{ m_bQuestResult = _result; }
	void			Set_Orb(_bool _On)								{ m_bOrbOnAltar = _On; }

	void			Set_Slow(_bool _Slow)							{ m_bSlow = _Slow; }

	_bool			Get_Parrying()									{ return m_bParrying; }
	_bool			Get_Slow()										{ return m_bSlow; }
	void			SlowDuration(const _float& fTimeDelta);

	void			IsDrunk();
	void			Add_Exp(CGameObject* pExp);
	void			Equip_Weapon(CGameObject* pWeapon);
	
public:
	virtual void	OnCollisionEnter(CCollider* _pOther);
	virtual void	OnCollisionStay(CCollider* _pOther);
	virtual void	OnCollisionExit(CCollider* _pOther);

public:
	//void			Add_Item(CGameObject* pItem, ITEMTAG _eItem) { m_pItem[(_uint)_eItem] = pItem; }
	void            IsAttack(CBasicStat* _MonsterStat);
	void			IsAddiction(const _float& fTimeDelta);
	void			Eating(CBasicStat* _foodStat);
	void			Create_Item(CCollider*	_pOther);

	void			Foot_Sound(const _float& fTimeDelta);
	void			Grab_Puzzle() { ++m_iPuzzleCount; }

private:
	CFlyingCamera*	m_pMainCamera = nullptr;

	CRigidBody*		m_pRigidBody = nullptr;
	CStateMachine*	m_pStateMachine = nullptr;
	CPlayerBf*		m_pBuffer = nullptr;
	CPlayerStat*	m_pStat = nullptr;
	CAnimator*		m_pAnimator = nullptr;
	CInventory*     m_pInventory = nullptr;
	_matrix			m_matPlayerWorld;

	//CGameObject*	m_pItem[(_uint)ITEMTAG::ITEM_END];      //-> 인벤토리로 이동
	_bool			m_bItemEquipRight;                      // 플레이어의 들고있는 아이템 장착 여부 : 오른손
	_bool			m_bItemEquipLeft;                       // 플레이어의 들고있는 아이템 장착 여부 : 왼손
	CGameObject*	m_pCurrentEquipItemRight = nullptr;     // 현재 플레이어가 들고 있는 아이템     : 오른손
	CGameObject*	m_pPrevEquipItemRight = nullptr;		// 이전 플레이어가 들고 있는 아잉템		: 오른손
	CGameObject*    m_pCurrentEquipItemLeft = nullptr;      // 현재 플레이어가 들고 있는 아이템     : 왼손
	CGameObject*	m_pPrevEquipItemLeft = nullptr;			// 이전 플레이어가 들고 있는 아이템		: 왼손
	//ITEMTAG		m_eItem = ITEMTAG::ITEM_END;

	// Player 
	STATE			m_eState = STATE::STATE_END;

	_bool			m_bInWater = false;
	_bool			m_bDropWater = false;
	_bool			m_bQuestResult = false;
	_bool			m_bOrbOnAltar = false;
	_uint			m_iPuzzleCount;

	_bool			m_bUseUI;
	_bool			m_bIsAttack;  // 공격형 아이템의 업데이트에 신호를 줄 불 변수
	_bool			m_bAttackTick;
	_bool			m_bDrunk;
	_bool			m_bEquipStat;
	_bool			m_bThrowShield;
	_bool			m_bIsAddiction;
	_bool			m_bIsTalk;
	_float			m_fDrunkTime;
	_float			m_fJumpVelocity = 15;

	_int			m_iDrunkCount;
	_int			m_iAddictionCount;
	_float			m_fSpeed = 10.f;
	_float			m_fAddictionTime;
	_bool			m_IsJump = false;
	_bool			m_bTestJump = false;

	_bool			m_OnGround = false;

	_int			m_iRootCount;

	// Debuff
	_vec3			m_vOffset;
	_vec3			m_vLeftOffset;

	// Default Stat
	_int			iDefalutDamageMax;
	_int			iDefalutDamageMin;
	_int			iArmorMax;
	_int			iArmorMin;

	//Msh
	_bool			m_bParrying = false;
	_bool			m_bPoisoning = false;
	_bool			m_bSlow = false;;
	_float			m_fPoisoningTime = 0.f;
	_uint			m_iPosingingCount = 0;
	_float			m_fSlowDuration = 0.f;

	_vec3			m_vOriginUp;
	_vec3			m_vOriginLook;
	_vec3			m_vOriginRight;

	_float          m_fWaterEffectTime = 0.f;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};

