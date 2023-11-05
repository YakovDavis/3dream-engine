#include "App.h"

LRESULT D3E::App::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	App* pThis;

	if (umessage == WM_NCCREATE)
	{
		pThis = static_cast<App*>(reinterpret_cast<CREATESTRUCT*>(lparam)->lpCreateParams);  // NOLINT(performance-no-int-to-ptr)

		SetLastError(0);
		if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<App*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));  // NOLINT(performance-no-int-to-ptr)
	}

	if (pThis)
	{
		return pThis->MsgProc(hwnd, umessage, wparam, lparam);
	}
	else
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

LRESULT D3E::App::MsgProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umessage, wparam, lparam);
}
