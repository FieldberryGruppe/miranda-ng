/*
	 ICQ Corporate protocol plugin for Miranda IM.
	 Copyright (C) 2003-2005 Eugene Tarasenko <zlyden13@inbox.ru>

	 This program is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 2 of the License, or
	 (at your option) any later version.

	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with this program; if not, write to the Free Software
	 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
	 */

#include "stdafx.h"

std::vector <ICQUser *> icqUsers;

///////////////////////////////////////////////////////////////////////////////

ICQUser::ICQUser() :
	socket(WM_NETEVENT_USER)
{
}

///////////////////////////////////////////////////////////////////////////////

void ICQUser::setStatus(unsigned short newStatus)
{
	if (statusVal == newStatus)
		return;

	statusVal = newStatus;
	db_set_w(hContact, protoName, "Status", newStatus);
}

///////////////////////////////////////////////////////////////////////////////

void ICQUser::setInfo(char *name, unsigned int data)
{
	if (data && data != 0xFFFFFFFF)
		db_set_dw(hContact, protoName, name, data);
	else
		db_unset(hContact, protoName, name);
}

///////////////////////////////////////////////////////////////////////////////

void ICQUser::setInfo(char *name, unsigned short data)
{
	if (data && data != 0xFFFF)
		db_set_w(hContact, protoName, name, data);
	else
		db_unset(hContact, protoName, name);
}

///////////////////////////////////////////////////////////////////////////////

void ICQUser::setInfo(char *name, unsigned char data)
{
	if (data && data != 0xFF)
		db_set_b(hContact, protoName, name, data);
	else
		db_unset(hContact, protoName, name);
}

///////////////////////////////////////////////////////////////////////////////

void ICQUser::setInfo(char *name, char *data)
{
	if (data[0])
		db_set_s(hContact, protoName, name, data);
	else
		db_unset(hContact, protoName, name);
}

///////////////////////////////////////////////////////////////////////////////

static char* iptoa(unsigned int ip)
{
	struct in_addr addr;
	addr.S_un.S_addr = htonl(ip);
	return inet_ntoa(addr);
}

///////////////////////////////////////////////////////////////////////////////

static void setTextValue(HWND hWnd, int id, const wchar_t *value)
{
	bool unspecified = value == nullptr;

	EnableWindow(GetDlgItem(hWnd, id), !unspecified);
	SetDlgItemText(hWnd, id, unspecified ? TranslateT("<not specified>") : value);
}

///////////////////////////////////////////////////////////////////////////////

static INT_PTR CALLBACK icqUserInfoDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR hdr;

	switch (msg) {
	case WM_INITDIALOG:
		TranslateDialogDefault(hWnd);
		return TRUE;

	case WM_NOTIFY:
		hdr = (LPNMHDR)lParam;
		if (hdr->idFrom == 0 && hdr->code == PSN_INFOCHANGED) {
			wchar_t buffer[64];
			unsigned long ip, port;
			MCONTACT hContact = (MCONTACT)((LPPSHNOTIFY)lParam)->lParam;

			_itow(db_get_dw(hContact, protoName, "UIN", 0), buffer, 10);
			setTextValue(hWnd, IDC_INFO_UIN, buffer);

			ip = db_get_dw(hContact, protoName, "IP", 0);
			setTextValue(hWnd, IDC_INFO_IP, ip ? _A2T(iptoa(ip)) : nullptr);

			ip = db_get_dw(hContact, protoName, "RealIP", 0);
			setTextValue(hWnd, IDC_INFO_REALIP, ip ? _A2T(iptoa(ip)) : nullptr);

			port = db_get_w(hContact, protoName, "Port", 0);
			_itow(port, buffer, 10);
			setTextValue(hWnd, IDC_INFO_PORT, port ? buffer : nullptr);

			setTextValue(hWnd, IDC_INFO_VERSION, nullptr);
			setTextValue(hWnd, IDC_INFO_MIRVER, nullptr);
			setTextValue(hWnd, IDC_INFO_PING, nullptr);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) SendMessage(GetParent(hWnd), msg, wParam, lParam);
		break;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

int icqUserInfoInitialise(WPARAM wParam, LPARAM lParam)
{
	char *proto = GetContactProto(lParam);
	if ((proto == nullptr || mir_strcmp(proto, protoName)) && lParam)
		return 0;

	OPTIONSDIALOGPAGE odp = {};
	odp.position = -1900000000;
	odp.szTitle.a = protoName;
	odp.pfnDlgProc = icqUserInfoDlgProc;
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_INFO_ICQCORP);
	g_plugin.addUserInfo(wParam, &odp);
	return 0;
}
