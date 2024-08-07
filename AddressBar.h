#pragma once

#ifndef _ADDRESSBAR_H
#define _ADDRESSBAR_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

class AddressBar : public CWindowImpl<AddressBar>
{
	protected: // Class members:
		CWindow m_toolbar = NULL;
		CComPtr<IShellBrowser> m_pShellBrowser = NULL;

		HWND m_goButton = NULL;
		HWND m_comboBox = NULL;
		HWND m_comboBoxEditCtl = NULL;

		DWORD m_showGoButton = 1;
		DWORD m_memphisStyle = 0;

		HIMAGELIST m_himlGoInactive = NULL;
		HIMAGELIST m_himlGoActive = NULL;

		WCHAR m_displayName[1024] = { 0 };
		WCHAR m_currentPath[1024] = { 0 };
		bool m_locationHasPhysicalPath = false;

		// This Internet Explorer API is also compatible with File Explorer, and
		// provides useful events.
		CComPtr<IWebBrowser2> m_pWebBrowser = NULL;

		friend class AddressBarHostBand;

	public: // Window setup:
		DECLARE_WND_CLASS(L"ClassicExplorer.AddressBar")

		BEGIN_MSG_MAP(AddressBar)
			NOTIFY_CODE_HANDLER(NM_CLICK, OnComponentNotifyClick)
			MESSAGE_HANDLER(WM_CREATE, OnCreate)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		END_MSG_MAP()

	public: // Exported functions:

		HWND GetToolbar(void)
		{
			return m_toolbar.m_hWnd;
		}

		void SetBrowsers(CComPtr<IShellBrowser> pShellBrowser, CComPtr<IWebBrowser2> pWebBrowser);

		HRESULT InitComboBox();
		HRESULT HandleNavigate();
		HRESULT RefreshCurrentAddress();
		BOOL GetCurrentAddressText(CComHeapPtr<WCHAR> &pszText);
		HRESULT Execute();

	protected: // Message handlers:
		LRESULT OnComponentNotifyClick(WPARAM wParam, LPNMHDR notifyHeader, BOOL &bHandled);
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	protected: // Miscellaneous functions:
		LRESULT GetBooleanRegKey(PTCHAR key, bool* boolValue);

		static LRESULT CALLBACK ComboboxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
		static LRESULT CALLBACK RealComboboxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		LRESULT CreateGoButton();

		HRESULT GetCurrentFolderPidl(PIDLIST_ABSOLUTE *pidl);
		HRESULT GetCurrentFolderName(WCHAR *pszName, long length);

		HRESULT ParseAddress(PIDLIST_RELATIVE *pidlOut);
		HRESULT ExecuteCommandLine();
};

#endif // _ADDRESSBAR_H
