#include "stdafx.h"
#include "..\Header\BossStage.h"
#include "Export_Function.h"
#include "PoolManager.h"
#include "Monstergroup.h"
#include "Itemgroup.h"
#include "UIUseShop_Trander.h"
#include "UIShop.h"
#include "SpawningPool.h"
#include "Blade_Trap_Body.h"
#include "StrikeDown_Trap_Body.h"
#include "Plate_Trap_Body.h"
#include "SoundManager.h"
#include "Jump_Plate.h"
#include "BlackIn.h"
#include "Lava.h"
#include "CameraManager.h"
#include "GameManager.h"
#include "KingSpiderWeb.h"
#include "KingSpiderGrabWeb.h"
#include "FlyingCamera.h"
#include "EffectStar.h"
#include "MoonBoss.h"
#include "UIbosshp.h"

CBossStage::CBossStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CBossStage::~CBossStage()
{
}

HRESULT CBossStage::Ready_Scene()
{
	Engine::CGameObject* pGameObject = CBlackIn::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_BASIC, 0);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_BLACK, Engine::UILAYER::UI_DOWN, pGameObject);

	m_eSceneTag = SCENETAG::BOSSSTAGE;
	FAILED_CHECK_RETURN(Ready_Layer_Environment(LAYERTAG::ENVIRONMENT), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(LAYERTAG::GAMELOGIC), E_FAIL);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	CSoundManager::GetInstance()->StopAll();
	CSoundManager::GetInstance()->PlayBGM(L"DK-7.mp3", 1.f);

	Engine::Renderer()->Set_FogUse(true);
	Engine::Renderer()->Set_FogColor(1, 100, 0, 0);
	Engine::Renderer()->Set_FogDistance(1.f, 120.0f);

	return S_OK;
}

Engine::_int CBossStage::Update_Scene(const _float& fTimeDelta)
{
	__super::Update_Scene(fTimeDelta);

	CPlayer* pPlayer = Engine::SceneManager()->Get_Scene()->Get_MainPlayer();

#pragma region KSG

	//if (BOSSPHASE::PHASE3 == m_pBoss->Get_Phase() && m_pBoss->Get_BasicStat()->Get_Stat()->fHP <= 5.f && !m_bExecuteBoss)
	//if (BOSSPHASE::PHASE1 == m_pBoss->Get_Phase() && m_pBoss->Get_BasicStat()->Get_Stat()->fHP <= 5.f && !m_bExecuteBoss)
	//if (BOSSPHASE::LASTPHASE == m_pBoss->Get_Phase() && m_pBoss->Get_BasicStat()->Get_Stat()->fHP <= 5.f && !m_bExecuteBoss)
	if (BOSSPHASE::LASTPHASE == m_pBoss->Get_Phase() && STATE::BOSS_CRAWL == m_pBoss->Get_StateMachine()->Get_State() && m_pBoss->Get_HekiReki() && !m_bExecuteBoss)
	{
		if (m_pBoss && D3DXVec3Length(&(m_pBoss->m_pTransform->m_vInfo[INFO_POS] - pPlayer->m_pTransform->m_vInfo[INFO_POS])) < 25.f)
		{
			CGameManager::GetInstance()->PlayMode(PD::HekirekiIssen);
			m_bExecuteBoss = true;
		}
	}

	if (!m_bHekiReki)
	{
		//if (m_bExecuteBoss && m_pBoss->Get_BasicStat()->Get_Stat()->fHP == 0.f)
		if((PD::HekirekiIssen_SideView == CGameManager::GetInstance()->Get_PlayMode())&&(m_bExecuteBoss))
		{
			// 보스 사망 처리
			m_pBoss->Get_StateMachine()->Set_State(STATE::BOSS_DYING);
			m_bHekiReki = true;
			m_pBoss->Get_BasicStat()->Get_Stat()->fHP = 0.f;
		}
	}

	if ((!m_bGate)&&(STATE::BOSS_DEAD == m_pBoss->Get_StateMachine()->Get_State()))
	{
		m_fGateTime += fTimeDelta;
		CGameObject* pGameObject = nullptr;
		if(3.f< m_fGateTime)
		{	
			CFlyingCamera* pCamera = static_cast<CFlyingCamera*>(CCameraManager::GetInstance()->Get_CurrentCam());
			pCamera->Set_ShakeForce(0.f, 0.02f, 2.f, 2.f);
			pCamera->Shake_Camera();

			CSoundManager::GetInstance()->StopSound(CHANNELID::SOUND_BOSS);
			CSoundManager::GetInstance()->PlaySound(L"KingSpider_Appear.mp3", CHANNELID::SOUND_BOSS, 1.f);

			pGameObject = CDimensionGate::Create(m_pGraphicDev);
			static_cast<CDimensionGate*>(pGameObject)->m_pTransform->m_vInfo[INFO_POS] = _vec3(-100.f, 32.f, 1.f);
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
			m_bGate = true;
		}
		if (!m_bOrbSPawn)
		{
			m_bOrbSPawn = true;
			pGameObject = COrb::Create(m_pGraphicDev, true);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			static_cast<COrb*>(pGameObject)->m_pTransform->m_vInfo[INFO_POS] = _vec3(-77.f, 34.5f, 2.f);
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		}
	}

#pragma endregion KSG

	CGameManager::GetInstance()->Update_Game(fTimeDelta);
	CCameraManager::GetInstance()->Update_Camera(fTimeDelta);

	UIManager()->Update_UI(fTimeDelta);
	if (10.f >= pPlayer->m_pTransform->m_vInfo[INFO_POS].y)
	{
		switch (static_cast<CSkeletonKing*>(m_pBoss)->Get_Phase())
		{
		case BOSSPHASE::PHASE1:
			pPlayer->m_pTransform->m_vInfo[INFO_POS] = _vec3(100.f, 22.f, 0.f);
			m_pBoss->Get_StateMachine()->Set_State(STATE::BOSS_SLEEP);
			break;
		case BOSSPHASE::PHASE2:
			pPlayer->m_pTransform->m_vInfo[INFO_POS] = _vec3(-72.5f, 36.f, 75.5f);
			m_pBoss->Get_StateMachine()->Set_State(STATE::BOSS_SLEEP);
			break;
		case BOSSPHASE::PHASE3:
			pPlayer->m_pTransform->m_vInfo[INFO_POS] = _vec3(-72.f, 36.f, -75.f);
			m_pBoss->Get_StateMachine()->Set_State(STATE::BOSS_SLEEP);
			break;
		case BOSSPHASE::LASTPHASE:
			pPlayer->m_pTransform->m_vInfo[INFO_POS] = _vec3(-60.f, 35.f, 0.f);
			m_pBoss->Get_StateMachine()->Set_State(STATE::BOSS_SLEEP);
			break;
		}
	}

	return 0;
}

void CBossStage::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();

	CollisionManager()->LateUpdate_Collision();
	CCameraManager::GetInstance()->LateUpdate_Camera();

	UIManager()->LateUpdate_UI();
}

void CBossStage::Render_Scene()
{
	// DEBUG 용 렌더
}

void CBossStage::Free()
{
	CPoolManager::DestroyInstance();

	__super::Free();
}

CBossStage* CBossStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossStage*	pInstance = new CBossStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("CBossStage Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CBossStage::Ready_Prototype()
{

	return S_OK;
}

HRESULT CBossStage::Ready_Layer_Environment(LAYERTAG _eLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(_eLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

#pragma region 첫 번째 씬에서 받아오는 오브젝트
	// DynamicCamera
	//pGameObject = CDynamicCamera::Create(m_pGraphicDev, 
	//										&_vec3(0.f, 0.f, 0.f),
	//										&_vec3(0.f, 0.f, 1.f),
	//										&_vec3(0.f, 1.f, 0.f),
	//										D3DXToRadian(90.f), 
	//										(_float)WINCX / WINCY,
	//										0.1f, 
	//										1000.f);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
#pragma endregion 첫 번째 씬에서 받아오는 오브젝트

	// Moon
	pGameObject = CMoonBoss::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	// SkyBox
	pGameObject = CSkyBoxBoss::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
	m_mapLayer.insert({ _eLayerTag, pLayer });

	for (_uint i = 0; i < 50; ++i)
	{
		pGameObject = CEffectStar::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
	}

	return S_OK;
}

HRESULT CBossStage::Ready_Layer_GameLogic(LAYERTAG _eLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(_eLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	m_mapLayer.insert({ _eLayerTag, pLayer });

	CPoolManager::GetInstance()->Ready_Pool();

	Engine::CGameObject*		pGameObject = nullptr;

	// Boss
	pGameObject = CSkeletonKing::Create(m_pGraphicDev);
	m_pBoss = static_cast<CSkeletonKing*>(pGameObject);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->m_pTransform->Translate(_vec3(-80.f, 35.f, 0.f));
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	return S_OK;
}

HRESULT CBossStage::Ready_Layer_UI(LAYERTAG _eLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create(_eLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

#pragma region 첫 번째 씬에서 받아오는 오브젝트
	// 기본 인벤토리 5칸
	for (_uint i = 0; i < 5; ++i)
	{
		pGameObject = CUIbasicslot::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		pGameObject->m_pTransform->m_vInfo[INFO_POS].x = 520.f + (60.f * i);
		pGameObject->m_pTransform->m_vInfo[INFO_POS].y = WINCY - 30.f;
		static_cast<CTempUI*>(pGameObject)->WorldMatrix(pGameObject->m_pTransform->m_vInfo[INFO_POS].x, pGameObject->m_pTransform->m_vInfo[INFO_POS].y, pGameObject->m_pTransform->m_vLocalScale.x, pGameObject->m_pTransform->m_vLocalScale.y);
		static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_SLOTBASIC, i);
		static_cast<CTempUI*>(pGameObject)->Set_Setup(true);
		Engine::UIManager()->Add_BasicGameobject(Engine::UILAYER::UI_DOWN, pGameObject);
	}

	_uint Index = 0;
	// 장비슬롯
	for (_uint iy = 0; iy < 3; ++iy)
	{
		for (_uint ix = 0; ix < 2; ++ix)
		{

			Index = iy * 2 + ix;
			pGameObject = CUIequipmentslot::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->m_pTransform->m_vInfo[INFO_POS].x = 350.f + (60.f * ix);
			pGameObject->m_pTransform->m_vInfo[INFO_POS].y = WINCY - 135.f - (60.f * iy);
			static_cast<CTempUI*>(pGameObject)->WorldMatrix(pGameObject->m_pTransform->m_vInfo[INFO_POS].x, pGameObject->m_pTransform->m_vInfo[INFO_POS].y, pGameObject->m_pTransform->m_vLocalScale.x, pGameObject->m_pTransform->m_vLocalScale.y);
			static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_SLOTEQUIPMENT, Index);
			static_cast<CTempUI*>(pGameObject)->Set_Setup(true);
			Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_EQUIPMENT, Engine::UILAYER::UI_DOWN, pGameObject);
		}
	}

	// 메인 인벤토리
	for (_uint iy = 0; iy < 3; ++iy)
	{
		for (_uint ix = 0; ix < 6; ++ix)
		{

			pGameObject = CUIemptyslot::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->m_pTransform->m_vInfo[INFO_POS].x = 490.f + (60.f * ix);
			pGameObject->m_pTransform->m_vInfo[INFO_POS].y = WINCY - 135.f - (60.f * iy);
			static_cast<CTempUI*>(pGameObject)->WorldMatrix(pGameObject->m_pTransform->m_vInfo[INFO_POS].x, pGameObject->m_pTransform->m_vInfo[INFO_POS].y, pGameObject->m_pTransform->m_vLocalScale.x, pGameObject->m_pTransform->m_vLocalScale.y);
			static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_SLOTEMPTY, iy * 6 + ix);
			static_cast<CTempUI*>(pGameObject)->Set_Setup(true);
			Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_INVEN, Engine::UILAYER::UI_DOWN, pGameObject);
		}
	}

	// 인벤토리 및 스탯 버튼
	for (_uint iy = 0; iy < 2; ++iy)
	{
		pGameObject = CUIbutton::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		pGameObject->m_pTransform->m_vInfo[INFO_POS].x = 850.f;
		pGameObject->m_pTransform->m_vInfo[INFO_POS].y = 595.f - (40 * iy);
		static_cast<CTempUI*>(pGameObject)->WorldMatrix(pGameObject->m_pTransform->m_vInfo[INFO_POS].x, pGameObject->m_pTransform->m_vInfo[INFO_POS].y, pGameObject->m_pTransform->m_vLocalScale.x, pGameObject->m_pTransform->m_vLocalScale.y);
		static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_INVENBUTTON, iy);
		if (iy == 0)
		{
			static_cast<CTempUI*>(pGameObject)->Set_UIImage(1);
		}
		else
		{
			static_cast<CTempUI*>(pGameObject)->Set_UIImage(3);
		}

		Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_EQUIPMENT, Engine::UILAYER::UI_DOWN, pGameObject);
	}

	pGameObject = CUIplayerstat::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_STAT, Engine::UILAYER::UI_DOWN, pGameObject);

	// 조준점
	HCURSOR Cursor = nullptr;
	SetCursor(Cursor);
	pGameObject = CUIaimpoint::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_MOUSE, Engine::UILAYER::UI_DOWN, pGameObject);

	// 플레이어 hp bar
	pGameObject = CUIplayerhp::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_BasicGameobject(Engine::UILAYER::UI_DOWN, pGameObject);

	// 큰 지도
	pGameObject = CUIbigmap::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_MAP, Engine::UILAYER::UI_DOWN, pGameObject);

	// Esc 배경
	pGameObject = CUIEscBackground::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_ESC, Engine::UILAYER::UI_DOWN, pGameObject);

	// Esc Resum 버튼
	pGameObject = CUIResumeButton::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_ESC, Engine::UILAYER::UI_DOWN, pGameObject);

	// Esc Option 버튼
	pGameObject = CUIOptionButton::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_ESC, Engine::UILAYER::UI_DOWN, pGameObject);

	// Esc Quition 버튼
	pGameObject = CUIQuitButton::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_ESC, Engine::UILAYER::UI_DOWN, pGameObject);

	// Shop
	pGameObject = CUIShop::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_SHOP, Engine::UILAYER::UI_DOWN, pGameObject);

	//BossHPBar
	pGameObject = CUIbosshp::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_BOSSHP, Engine::UILAYER::UI_DOWN, pGameObject);

	// Speech Bubble Test
	pGameObject = CUIspeech_OldMan::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_SPEECH, Engine::UILAYER::UI_DOWN, pGameObject);

	pGameObject = CUIUseShop_Trander::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_SPEECH, Engine::UILAYER::UI_DOWN, pGameObject);

	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_EQUIPMENT);
	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_INVEN);
	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_STAT);
	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_MAP);
	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_ESC);
	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_SPEECH);	// Speech Bubble Test
	Engine::UIManager()->Hide_PopupUI(Engine::UIPOPUPLAYER::POPUP_SHOP);	// Speech Bubble Test
#pragma endregion 첫 번째 씬에서 받아오는 오브젝트
	
	m_mapLayer.insert({ _eLayerTag, pLayer });

	return S_OK;
}

HRESULT CBossStage::Load_Data()
{
	CLayer* pLayer = m_mapLayer[LAYERTAG::GAMELOGIC];
	for (int i = 0; i < (UINT)OBJECTTAG::OBJECT_END; ++i)
	{
		// 일단 블록만
		if (OBJECTTAG::BLOCK != (OBJECTTAG)i)
			continue;

		vector<CGameObject*>& refObjectList = pLayer->Get_ObjectList((OBJECTTAG)i);
		for_each(refObjectList.begin(), refObjectList.end(), [&](CGameObject* pObj) { EventManager()->DeleteObject(pObj); });
		refObjectList.clear();
	}
	//HANDLE hFile = CreateFile(L"../Bin/Data/Sewer_TrapTest.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//HANDLE hFile = CreateFile(L"../Bin/Data/Sewer.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HANDLE hFile = CreateFile(L"../Bin/Data/BossStage.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//HANDLE hFile = CreateFile(L"../Bin/Data/BossStage.dat", GENERIC_READ,	0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//HANDLE hFile = CreateFile(L"../Bin/Data/BossStage_3rd.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	OBJECTTAG eTag = OBJECTTAG::OBJECT_END;

	DWORD		dwByte = 0;
	_float		fX, fY, fZ;
	_ubyte		byTextureNumber = 0;

	MONSTERTAG	eSpawnerTag = MONSTERTAG::MONSTER_END;  //
	_int		iPoolCapacity = 5;
	_float		fSpawnRadius = 10.0f;
	_float		fSpawnTime = 10.0f;

	while (true)
	{
		// key 값 저장
		ReadFile(hFile, &eTag, sizeof(OBJECTTAG), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		// if문 추가
		if (OBJECTTAG::BLOCK == eTag)
		{
			// value값 저장
			ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

			ReadFile(hFile, &byTextureNumber, sizeof(_ubyte), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CGameObject* pGameObject = nullptr;

			if (40 == byTextureNumber)
			{
				pGameObject = CLava::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				static_cast<CLava*>(pGameObject)->Set_TextureNumber(0);
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
				//EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
			}
			else
			{
				pGameObject = CCubeBlock::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				static_cast<CCubeBlock*>(pGameObject)->Set_TextureNumber(byTextureNumber);
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
				//EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
			}
		}
		else if (OBJECTTAG::SPAWNINGPOOL == eTag)
		{
			// value값 저장
			ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

			ReadFile(hFile, &eSpawnerTag, sizeof(MONSTERTAG), &dwByte, nullptr);
			ReadFile(hFile, &iPoolCapacity, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &fSpawnRadius, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fSpawnTime, sizeof(_float), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CGameObject* pGameObject = CSpawningPool::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			static_cast<CSpawningPool*>(pGameObject)->Set_MonsterTag(eSpawnerTag);
			static_cast<CSpawningPool*>(pGameObject)->Set_PoolCapacity(iPoolCapacity);
			static_cast<CSpawningPool*>(pGameObject)->Set_SpawnRadius(fSpawnRadius);
			static_cast<CSpawningPool*>(pGameObject)->Set_SpawnTime(fSpawnTime);
			pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
			pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
			//EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		}
		else if (OBJECTTAG::MONSTER == eTag)
		{
			// value값 저장
			ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

			TRAPTAG eTrapTag;
			ReadFile(hFile, &eTrapTag, sizeof(TRAPTAG), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CGameObject* pGameObject = nullptr;

			switch (eTrapTag)
			{
			case TRAPTAG::BLADE:
				pGameObject = CBlade_Trap::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				static_cast<CBlade_Trap*>(pGameObject)->Create_Blade();
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				break;

			case TRAPTAG::STRIKEDOWN:
				pGameObject = CStrikeDown_Trap::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				static_cast<CStrikeDown_Trap*>(pGameObject)->Set_InitailHeight(15.f);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				break;

			case TRAPTAG::JUMP:
				pGameObject = CJump_Plate::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				break;
			}

			pGameObject->m_pTransform->m_vInfo[INFO_POS] = _vec3(fX, fY + 1.f, fZ);
			pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
			//EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		}
		else if (OBJECTTAG::TRAP == eTag)
		{
			// value값 저장
			ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

			TRAPTAG eTrapTag;
			ReadFile(hFile, &eTrapTag, sizeof(TRAPTAG), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CGameObject* pGameObject = nullptr;

			switch (eTrapTag)
			{
			case TRAPTAG::BLADE:
				pGameObject = CBlade_Trap::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				static_cast<CBlade_Trap*>(pGameObject)->Create_Blade();
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				break;

			case TRAPTAG::STRIKEDOWN:
				pGameObject = CStrikeDown_Trap::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				static_cast<CStrikeDown_Trap*>(pGameObject)->Set_InitailHeight(10.f);
				static_cast<CStrikeDown_Trap*>(pGameObject)->Set_MinHeight(10.f);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				break;

			case TRAPTAG::JUMP:
				pGameObject = CJump_Plate::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				break;
			}

			pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
			//EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
		}
	}
	CloseHandle(hFile);
	return S_OK;
}
