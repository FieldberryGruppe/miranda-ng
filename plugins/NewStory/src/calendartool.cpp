#include "stdafx.h"

struct CalendarToolData
{
	int x, y;
};

int CALLBACK CalendarToolDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CalendarToolData *data = (CalendarToolData *)GetWindowLong(hwnd, GWL_USERDATA);
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			data = (CalendarToolData *)lParam;
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)data);

// This causes ALL miranda dialogs to have drop-shadow enabled. That's bad =(
//			SetClassLong(hwnd, GCL_STYLE, GetClassLong(hwnd, GCL_STYLE)|CS_DROPSHADOW);

			SetWindowPos(hwnd, HWND_TOP, data->x, data->y, 0, 0, SWP_NOSIZE);
			return TRUE;
		}

	    case WM_ACTIVATE:
			if (wParam == WA_INACTIVE)
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDOK:
				{
					EndDialog(hwnd, 0);
					return TRUE;
				}

				case IDCANCEL:
				{
					EndDialog(hwnd, 0);
					return TRUE;
				}
			}
			break;
		}

		case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR)lParam;
			if ((hdr->idFrom = IDC_MONTHCALENDAR1) && (hdr->code == MCN_SELECT))
			{
				LPNMSELCHANGE lpnmsc = (LPNMSELCHANGE)lParam;
				struct tm tm_sel;
				tm_sel.tm_hour = tm_sel.tm_min = tm_sel.tm_sec = 0;
				tm_sel.tm_isdst = 1;
				tm_sel.tm_mday = lpnmsc->stSelStart.wDay;
				tm_sel.tm_mon = lpnmsc->stSelStart.wMonth - 1;
				tm_sel.tm_year = lpnmsc->stSelStart.wYear - 1900;
				EndDialog(hwnd, mktime(&tm_sel));
			}
			return TRUE;
		}

		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return TRUE;
		}

		case WM_DESTROY:
		{
			delete data;
			data = 0;
			SetWindowLong(hwnd, GWL_USERDATA, 0);
		}
	}
	return FALSE;
}

time_t CalendarTool_Show(HWND hwnd, int x, int y)
{
	CalendarToolData *data = new CalendarToolData;
	data->x = x;
	data->y = y;

//	HWND hwndTool = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_CALENDARTOOL), 0, CalendarToolDlgProc, (LPARAM)data);
//	ShowWindow(hwndTool, SW_SHOWNORMAL);
	return DialogBoxParam(g_plugin.getInst(), MAKEINTRESOURCE(IDD_CALENDARTOOL), 0, CalendarToolDlgProc, (LPARAM)data);
}