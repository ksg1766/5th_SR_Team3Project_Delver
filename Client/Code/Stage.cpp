#include "stdafx.h"
#include "..\Header\Stage.h"

#include "Export_Function.h"
#include "PoolManager.h"

#include "Monstergroup.h"
#include "SpiderRay.h"
#include "Itemgroup.h"
#include "Npc_Trader.h"
#include "UIUseShop_Trander.h"
#include "UIShop.h"
#include "SoundManager.h"
#include "WaterFallTriger.h"
#include "HellDoor.h"
#include "DoorCube.h"
#include "BookCube.h"
#include "BookDoor.h"
#include "Puzzle.h"
#include "Puzzle_Part.h"
#include "UIPuzzleBack.h"

#include "SpawningPool.h"
#include "BoxCube.h"
#include "EquipBox.h"
#include "EffectSquare.h"
#include "EffectBubble.h"

#include "Blade_Trap_Body.h"
#include "StrikeDown_Trap_Body.h"
#include "Plate_Trap_Body.h"
#include "Phantom.h"
#include "UISpeech_Phantom.h"

#include "Pot.h"
#include "Tree.h"
#include "Pumpkin.h"
#include "Rock.h"
#include "Grass.h"
#include "Mushroom.h"
#include "ImmortalSprite.h"

#include "EffectBrokenbox.h"
#include "EffectDamage.h"
#include "EffectDamageStar.h"
#include "EffectBlood.h"
#include "EffectProjectileTrace.h"
#include "EffectTwinkle.h"
#include "SoundManager.h"
#include "CameraManager.h"

#include "Jump_Plate.h"
#include "Boss_Lightning.h"
#include "BlackIn.h"
#include "BossStage.h"
#include "KingSpider.h"
// 연출 테스트 // 성공시 보스 씬으로 이동
#include "GameManager.h"
#include "EffectWaterfall.h"
#include "GameManager.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	Engine::CGameObject* pGameObject = CBlackIn::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_BASIC, 0);
	Engine::UIManager()->Add_PopupGameobject(Engine::UIPOPUPLAYER::POPUP_BLACK, Engine::UILAYER::UI_DOWN, pGameObject);

	m_eSceneTag = SCENETAG::STAGE;
	m_fSound = 1.f;

	FAILED_CHECK_RETURN(Ready_Layer_Environment(LAYERTAG::ENVIRONMENT), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(LAYERTAG::GAMELOGIC), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(LAYERTAG::UI), E_FAIL);

	CSoundManager::GetInstance()->StopAll();

	Engine::Renderer()->Set_FogUse(true);
	Engine::Renderer()->Set_FogColor(1, 5, 25, 40);
	Engine::Renderer()->Set_FogDistance(1.f, 80.0f);

	return S_OK;
}

Engine::_int CStage::Update_Scene(const _float& fTimeDelta)
{
	__super::Update_Scene(fTimeDelta);

	CGameManager::GetInstance()->Update_Game(fTimeDelta);
	CCameraManager::GetInstance()->Update_Camera(fTimeDelta);

	UIManager()->Update_UI(fTimeDelta);

	return 0;
}

void CStage::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();

	if(m_bSoundOn)
		CSoundManager::GetInstance()->PlaySoundLoop(L"chase_sewers.mp3", CHANNELID::SOUND_BGM, m_fSound);

	CollisionManager()->LateUpdate_Collision();
	CCameraManager::GetInstance()->LateUpdate_Camera();

	UIManager()->LateUpdate_UI();

	if (m_pPlayer && m_pPlayer->m_pTransform->m_vInfo[INFO_POS].y > 250)
	{
		m_pPlayer->Get_RigidBody()->Set_Force(_vec3(0.f, 0.f, 0.f));

		CScene* pScene = CBossStage::Create(m_pGraphicDev);
		Engine::SceneManager()->Change_Scene(pScene);

		CGameManager::GetInstance()->PlayMode(PD::ShowBoss);
	}
}

void CStage::Render_Scene()
{
	// DEBUG 용 렌더
}

void CStage::Free()
{
	__super::Free();
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage*	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CStage::Ready_Prototype()
{
	return S_OK;
}

HRESULT CStage::Ready_Layer_Environment(LAYERTAG _eLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(_eLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	// SkyBox
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	pGameObject = CWaterFallTriger::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	// 폭포 이펙트
	pGameObject = CEffectWaterfall::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->m_pTransform->Translate(_vec3(-37, 13.f, 15.f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_BoundingBox(_vec3(-2.f, 14.f, -1.f), _vec3(2.f, 20.f, 1.f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectMoveScale(5, _vec3(.1f, .1f, .1f), _vec3(.5f, .5f, .5f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectBubbleScale(7, _vec3(.2f, .2f, .2f), _vec3(1.f, 1.f, 1.f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectDropHeight(1.f);
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectMoveSet(0);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	// 작은 폭포 이펙트
	pGameObject = CEffectWaterfall::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->m_pTransform->Translate(_vec3(1.5, 10.f, -46.f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_BoundingBox(_vec3(-2.f, 10.f, -1.f), _vec3(2.f, 20.f, 1.f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectMoveScale(5, _vec3(.1f, .1f, .1f), _vec3(2.f, 2.f, 2.f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectBubbleScale(10, _vec3(.1f, .1f, .1f), _vec3(.5f, .5f, .5f));
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectDropHeight(-2.f);
	static_cast<CEffectWaterfall*>(pGameObject)->Set_EffectMoveSet(2);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	m_mapLayer.insert({ _eLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(LAYERTAG _eLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create(_eLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CHellDoor::Create(m_pGraphicDev, pLayer);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	pGameObject = CBookDoor::Create(m_pGraphicDev, pLayer);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);

	m_mapLayer.insert({ _eLayerTag, pLayer });

	CPoolManager::GetInstance()->Ready_Pool();

	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(LAYERTAG _eLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create(_eLayerTag);
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CUISpeech_Phantom::Create(m_pGraphicDev);
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

HRESULT CStage::Load_Data()
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
	HANDLE hFile = CreateFile(L"../Bin/Data/Sewer_Test_v1.0.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//HANDLE hFile = CreateFile(L"../Bin/Data/BossStage_3rd.dat", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	OBJECTTAG eTag = OBJECTTAG::OBJECT_END;

	DWORD		dwByte = 0;
	_float		fX, fY, fZ;
	_ubyte		byTextureNumber = 0;

	MONSTERTAG	eSpawnerTag = MONSTERTAG::MONSTER_END;
	_int		iSpawnerLife = 10;
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

			if (32 == byTextureNumber)
			{
				pGameObject = CWaterFall::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				static_cast<CWaterFall*>(pGameObject)->Set_TextureNumber(0);
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
			}
			else if(31 == byTextureNumber)
			{
				pGameObject = CWater::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				static_cast<CWater*>(pGameObject)->Set_TextureNumber(byTextureNumber);
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
			}
			else
			{
				pGameObject = CCubeBlock::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				static_cast<CCubeBlock*>(pGameObject)->Set_TextureNumber(byTextureNumber);
				pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
				pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
			}
		}
		else if (OBJECTTAG::SPAWNINGPOOL == eTag)
		{
			// value값 저장
			ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

			ReadFile(hFile, &eSpawnerTag, sizeof(MONSTERTAG), &dwByte, nullptr);
			ReadFile(hFile, &iSpawnerLife, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &iPoolCapacity, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &fSpawnRadius, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fSpawnTime, sizeof(_float), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CGameObject* pGameObject = CSpawningPool::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			static_cast<CSpawningPool*>(pGameObject)->Set_MonsterTag(eSpawnerTag);
			static_cast<CSpawningPool*>(pGameObject)->Set_LifeCount(iSpawnerLife);
			static_cast<CSpawningPool*>(pGameObject)->Set_PoolCapacity(iPoolCapacity);
			static_cast<CSpawningPool*>(pGameObject)->Set_SpawnRadius(fSpawnRadius);
			static_cast<CSpawningPool*>(pGameObject)->Set_SpawnTime(fSpawnTime);
			pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
			pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
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
		}
		else if (OBJECTTAG::IMMORTAL == eTag)
		{
			// value값 저장
			ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

			_float  fCX = 0.f, fCY = 0.f, fCZ = 0.f;
			ReadFile(hFile, &fCX, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fCY, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &fCZ, sizeof(_float), &dwByte, nullptr);

			ENVIRONMENTTAG eEnvTag;
			ReadFile(hFile, &eEnvTag, sizeof(ENVIRONMENTTAG), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CGameObject* pGameObject = nullptr;

			switch (eEnvTag)
			{
				case ENVIRONMENTTAG::TREE:
				{
					_uint iTreeNumber = 0;

					ReadFile(hFile, &iTreeNumber, sizeof(_uint), &dwByte, nullptr);

					pGameObject = CTree::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
					static_cast<CTree*>(pGameObject)->Set_TreeNumber(iTreeNumber);
				}
				break;

				case ENVIRONMENTTAG::ROCK:
				{
					_uint iRockNumber = 0;

					ReadFile(hFile, &iRockNumber, sizeof(_uint), &dwByte, nullptr);

					pGameObject = CRock::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
					static_cast<CRock*>(pGameObject)->Set_RockNumber(iRockNumber);
					break;
				}
				case ENVIRONMENTTAG::GRASS:
				{
					_uint iGrassNumber = 0;

					ReadFile(hFile, &iGrassNumber, sizeof(_uint), &dwByte, nullptr);

					pGameObject = CGrass::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
					static_cast<CGrass*>(pGameObject)->Set_GrassNumber(iGrassNumber);
					break;
				}
				case ENVIRONMENTTAG::MUSHROOM:
				{
					_uint iMushroomNumber = 0;

					ReadFile(hFile, &iMushroomNumber, sizeof(_uint), &dwByte, nullptr);

					pGameObject = CMushroom::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
					static_cast<CMushroom*>(pGameObject)->Set_MushroomNumber(iMushroomNumber);
					break;
				}
				case ENVIRONMENTTAG::PUMPKIN:
				{
					_uint iPumpkinNumber = 0;

					ReadFile(hFile, &iPumpkinNumber, sizeof(_uint), &dwByte, nullptr);

					pGameObject = CPumpkin::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
					static_cast<CPumpkin*>(pGameObject)->Set_PumpkinNumber(iPumpkinNumber);
					break;
				}
				case ENVIRONMENTTAG::ETC:
				{
					_uint iSpriteNumber = 0;

					ReadFile(hFile, &iSpriteNumber, sizeof(_uint), &dwByte, nullptr);

					pGameObject = CImmortalSprite::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
					static_cast<CImmortalSprite*>(pGameObject)->Set_SpriteNumber(iSpriteNumber);
					break;
				}
			}

			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->m_pTransform->Scale(_vec3(fCX, fCY, fCZ));
			pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
			pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
		}
		else if (OBJECTTAG::FRAGILE == eTag)
		{
		// value값 저장
		ReadFile(hFile, &fX, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &fY, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &fZ, sizeof(_float), &dwByte, nullptr);

		FRAGILETAG eFragileTag;
		ReadFile(hFile, &eFragileTag, sizeof(FRAGILETAG), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CGameObject* pGameObject = nullptr;

		switch (eFragileTag)
		{
			case FRAGILETAG::RANDOMBOX:
			{
				pGameObject = CBoxCube::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				break;
			}
			case FRAGILETAG::EQUIPBOX:
			{
				pGameObject = CEquipBox::Create(CGraphicDev::GetInstance()->Get_GraphicDev());
				break;
			}
		}
		NULL_CHECK_RETURN(pGameObject, E_FAIL);

		pGameObject->m_pTransform->Translate(_vec3(fX, fY + 1.f, fZ));
		pLayer->Add_GameObject(pGameObject->Get_ObjectTag(), pGameObject);
		}
	}
	CloseHandle(hFile);

	return S_OK;
}
