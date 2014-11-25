#include "stdafx.h"
#include "GUIControl.h"


GUIControl::GUIControl(const HWND hWnd) : _hWnd(hWnd)
{
	GetClientRect(_hWnd, &_r);
}

GUIControl::GUIControl(const HWND hWnd, const DWORD w, const DWORD h) : _hWnd(hWnd)
{
	GetClientRect(_hWnd, &_r);
	if (w)
		_r.right = _r.left + w;
	if (h)
		_r.bottom = _r.top + h;
}


GUIControl::~GUIControl(void)
{
}
