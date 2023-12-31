#include "stdafx.h"
#include "..\Header\StageLoading.h"

#include "Export_Function.h"
#include "Village.h"
#include "Editor.h"

#include "LoadingBackGround.h"
#include "ProgressBar.h"
#include "LoadingPont.h"
#include "BlackIn.h"
#include "Stage.h"

#include "GameManager.h"

CStageLoading::CStageLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStageLoading::~CStageLoading()
{
}

HRESULT CStageLoading::Ready_Scene()
{
	m_eSceneTag = SCENETAG::LOADING;
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(LAYERTAG::ENVIRONMENT), E_FAIL);
	
	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADINGID::LOADING_STAGE);
	//m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADINGID::LOADING_EDITOR);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	HCURSOR Cursor = GetCursor();
	Cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(Cursor);

	return S_OK;
}

Engine::_int CStageLoading::Update_Scene(const _float& fTimeDelta)
{
	_int iExit =__super::Update_Scene(fTimeDelta);

	UIManager()->Update_UI(fTimeDelta);

	CGameObject* pProgessbar = Engine::UIManager()->Get_BasicObject(UI_MIDDLE, UIID_BASIC, 1);
	if (true == m_pLoading->Get_Finish() && static_cast<CProgressBar*>(pProgessbar)->Get_Finish())
	{
		Engine::UIManager()->Delete_BasicObject(Engine::UILAYER::UI_DOWN);
		Engine::UIManager()->Delete_BasicObject(Engine::UILAYER::UI_MIDDLE);

		CScene*		pScene = CVillage::Create(m_pGraphicDev);
		//CScene* pScene = CStage::Create(m_pGraphicDev);
		CGameManager::GetInstance()->PlayMode(PD::ShowVillage);

		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::SceneManager()->Set_Scene(pScene), E_FAIL);
		SceneManager()->Load_Data();
		Octree()->Ready_Octree();
	}

	return iExit;
}

void CStageLoading::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();

	UIManager()->LateUpdate_UI();
}

void CStageLoading::Render_Scene()
{
	// DEBUG �� ����
}

void CStageLoading::Free()
{
	Safe_Release(m_pLoading);

	__super::Free();
}

CStageLoading* CStageLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStageLoading*	pInstance = new CStageLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pInstance;
}

HRESULT CStageLoading::Ready_Prototype()
{
	// �ΰ� ������ ���� ������Ʈ
	//FAILED_CHECK_RETURN(Engine::PrototypeManager()->Ready_Proto(L"Proto_RcTex", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::PrototypeManager()->Ready_Proto(L"Proto_Transform_Logo", CTransform::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::PrototypeManager()->Ready_Proto(L"Proto_Texture_StageBackground", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/SRSource/UI/Loadingscreen/LoadingScreen%d.png",4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::PrototypeManager()->Ready_Proto(L"Proto_Texture_StageLoadingFont", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/SRSource/UI/Loadingscreen/Loadingletters/Loading%d.png", 11)), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::PrototypeManager()->Ready_Proto(L"Proto_Texture_FadeIn", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/SRSource/UI/BlackIn/black%d.png", 10)), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::PrototypeManager()->Ready_Proto(L"Proto_Texture_FadeOut", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/SRSource/UI/BlackOut/black%d.png", 10)), E_FAIL);
	return S_OK;
}

HRESULT CStageLoading::Ready_Layer_Environment(LAYERTAG _eLayerTag)
{
	//Engine::CLayer*		pLayer = Engine::CLayer::Create(_eLayerTag);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//m_mapLayer.insert({ _eLayerTag, pLayer });

	Engine::CGameObject* pGameObject = nullptr;

	// BackGround
	pGameObject = CLoadingBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_BASIC, 0);
	Engine::UIManager()->Add_BasicGameobject(Engine::UILAYER::UI_DOWN, pGameObject);
	//Engine::UIManager()->AddPopupGameobject_UI(Engine::UIPOPUPLAYER::POPUP_STAT, Engine::UILAYER::UI_DOWN, pGameObject);

	pGameObject = CProgressBar::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_BASIC, 1);
	Engine::UIManager()->Add_BasicGameobject(Engine::UILAYER::UI_MIDDLE, pGameObject);

	pGameObject = CLoadingPont::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	static_cast<CTempUI*>(pGameObject)->Set_UIObjID(UIOBJECTTTAG::UIID_BASIC, 2);
	Engine::UIManager()->Add_BasicGameobject(Engine::UILAYER::UI_MIDDLE, pGameObject);

	return S_OK;
}
