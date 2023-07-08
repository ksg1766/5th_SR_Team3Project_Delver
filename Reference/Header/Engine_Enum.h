#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum WINMODE			{ MODE_FULL, MODE_WIN };

	// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	enum COMPONENTID		{ ID_DYNAMIC, ID_STATIC, ID_END };

	enum INFO				{ INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION			{ ROT_X, ROT_Y, ROT_Z, ROT_END };

	enum TEXTUREID			{ TEX_NORMAL, TEX_CUBE, TEX_END };

	enum RENDERID			{ RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_EFFECT, RENDER_UI, RENDER_END };

	enum MOUSEKEYSTATE		{ DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE		{ DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum class LIGHT_TYPE	{ LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_SPOT, LIGHT_END };

	enum class EVENT_TYPE	{ CREATE_OBJECT, DELETE_OBJECT, SCENE_CHANGE, EVENT_END };

	enum class LAYERTAG		{ EVERYTHING, ENVIRONMENT, GAMELOGIC, EFFECT, UI, IGNORERAY, LAYER_END };
	//enum class LAYERTAG	{ EVERYTHING, ENVIRONMENT, DYNAMIC_GAMEOBJECT, STATIC_GAMEOBJECT, EFFECT, UI, IGNORERAY, LAYER_END };
	enum class OBJECTTAG	{ BACKGROUND, TERRAIN, BLOCK, PLAYER, ITEM, MONSTER, CAMERA, UI, SKYBOX, OBJECT_END };
	enum class COMPONENTTAG	{ BUFFER, TEXTURE0, TEXTURE1, TRANSFORM, COLLIDER, RIGIDBODY, LIGHT, CAMERA, BASICSTAT, MONSTERAI,
								ANIMATOR, PARTICLE, BILLBOARD, STATEMACHINE, COMPONENT_END };
	
	// STATE
	enum class STATE { IDLE, ROMIMG, ATTACK, HIT, DEAD, STATE_END };


	enum UILAYER            { UI_DOWN, UI_MIDDLE, UI_UP, UI_END };
	enum UIPOPUPLAYER       { POPUP_MOUSE, POPUP_INVEN, POPUP_STAT, POPUP_MAP, POPUP_ESC, POPUP_END };
	//enum UISLOTTAG          { POPUP_MOUSE, POPUP_INVEN, POPUP_STAT, POPUP_MAP, POPUP_ESC, POPUP_END };
}
#endif // Engine_Enum_h__
