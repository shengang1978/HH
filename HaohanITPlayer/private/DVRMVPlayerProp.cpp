// DVRMVPlayerProp.cpp : Implementation of CDVRMVPlayerProp

#include "stdafx.h"
#include "DVRMVPlayerProp.h"


// CDVRMVPlayerProp

LRESULT CDVRMVPlayerProp::OnBnClickedButtonBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	WCHAR szFileName[MAX_PATH];
	szFileName[0] = L'\0';
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = L"MP4 Files\0*.mp4\0All Files\0*.*\0";
	ofn.nFilterIndex = 1 ;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = L"Open MP4 moive\0";
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileNameW(&ofn))
	{
		SetDlgItemText(IDC_EDIT_URL,szFileName);
		SetDirty(TRUE);
	}

	return 0;
}

LRESULT CDVRMVPlayerProp::OnEnChangeEditUrl(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetDirty(TRUE);
	return 0;
}
