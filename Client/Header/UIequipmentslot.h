#pragma once

#include "Export_Function.h"
#include "TempUI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CUIequipmentslot : public CTempUI
{
private:
	explicit CUIequipmentslot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIequipmentslot();

public:
	void		Set_UseSlot(_bool _bUse) { m_bUse = _bUse; }
	void        Set_FindSlot(_bool bFind) 
	{ 
		m_bFind = bFind; 
		if (m_bFind) {
			switch (m_UINumber)
			{
			case 0:
				m_fCurrentImage = 4;
				break;
			case 1:
				m_fCurrentImage = 13;
				break;
			case 2:
				m_fCurrentImage = 7;
				break;
			case 3:
				m_fCurrentImage = 19;
				break;
			case 4:
				m_fCurrentImage = 10;
				break;
			case 5:
				m_fCurrentImage = 16;
				break;
			}
		}
	}

public:
	HRESULT		Ready_Object();
	_int		Update_Object(const _float& fTimeDelta);
	void		LateUpdate_Object(void);
	void		Render_Object();

private:
	HRESULT		Add_Component(void);
	void	    Key_Input(void);

private:
	_bool       m_bFind = false;
	_bool		m_bUse = false;

public:
	static CUIequipmentslot*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	
private:
	virtual void Free();

};

