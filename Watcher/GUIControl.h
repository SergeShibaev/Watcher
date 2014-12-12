#pragma once
class GUIControl
{
	RECT _r;
	HWND _hWnd;
public:
	GUIControl(const HWND hWnd);
	GUIControl(const HWND hWnd, const DWORD w, const DWORD h);
	~GUIControl(void);
	DWORD width() { return _r.right - _r.left; }
	DWORD height() { return _r.bottom - _r.top; }

	GUIControl* const left(DWORD pos) { _r.left = pos; return this; }
	GUIControl* const top(DWORD pos) { _r.top = pos; return this; }
	GUIControl* const right(DWORD pos) { _r.right = pos; return this; }
	GUIControl* const bottom(DWORD pos) { _r.bottom = pos; return this; }
	GUIControl* const width(DWORD w) { _r.right = _r.left + w; return this; }
	GUIControl* const rwidth(DWORD w) { _r.left = _r.right - w; return this; }
	GUIControl* const height(const DWORD h) { _r.bottom = _r.top + h; return this; }

	DWORD GetTop() const { return _r.top; }
	DWORD GetBottom() const { return _r.bottom; }
	DWORD GetRight() const { return _r.right; }

	void Move() { MoveWindow(_hWnd, _r.left, _r.top, this->width(), this->height(), TRUE); }
	void Move(const DWORD left, const DWORD top) 
	{
		_r.left = left; _r.top = top; 
		_r.right += left; _r.bottom += top;
		Move();
	}
	void Maximize()
	{
		GetClientRect(GetParent(_hWnd), &_r);
		Move();
	}
	void Hide() const { ShowWindow(_hWnd, SW_HIDE); }
	void Show() const { ShowWindow(_hWnd, SW_NORMAL); }
};

