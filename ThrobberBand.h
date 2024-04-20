#pragma once

#ifndef _THROBBERBAND_H
#define _THROBBERBAND_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

class ATL_NO_VTABLE ThrobberBand :
	public CWindowImpl<ThrobberBand, CWindow, CControlWinTraits>,
	public CComObjectRootEx<CComMultiThreadModelNoCS>,
	public CComCoClass<ThrobberBand, &CLSID_ThrobberBand>,
	public IObjectWithSiteImpl<ThrobberBand>,
	public IDeskBand,
	public IDispEventImpl<1, ThrobberBand, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
{
	private: // Class members:
		CComPtr<IWebBrowser2> m_pWebBrowser = NULL;

		HWND m_parentRebar = NULL;
		HBITMAP m_hBitmap = NULL;
		bool m_subclassedRebar = false;
		bool m_alreadyDeletedSelf = false;
		bool m_shouldManuallyCorrectHeight = false;
		bool m_memphisStyle = false;

		// Width of the current bitmap.
		int m_cxCurBmp = 0;

		// Height of the current bitmap.
		int m_cyCurBmp = 0;

		int m_redrawCounter = 0;
		unsigned int m_latestRedrawTime = 0;
		bool m_disableRedraws = false;

	public: // COM class setup:
		DECLARE_WND_CLASS(L"ClassicExplorer.Throbber")

		~ThrobberBand(void)
		{
			ClearResources();
		}

		DECLARE_REGISTRY_RESOURCEID_V2_WITHOUT_MODULE(IDR_CLASSICEXPLORER, ThrobberBand)

		BEGIN_MSG_MAP(ThrobberBand)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_SIZE, OnSize)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		END_MSG_MAP()

		BEGIN_COM_MAP(ThrobberBand)
			COM_INTERFACE_ENTRY(IOleWindow)
			COM_INTERFACE_ENTRY(IObjectWithSite)
			COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
			COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
		END_COM_MAP()

		BEGIN_SINK_MAP(ThrobberBand)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
		END_SINK_MAP()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct()
		{
			return S_OK;
		}

		void FinalRelease() {}

	protected: // Message handlers:
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	protected: // Miscellaneous functions:
		LRESULT GetBooleanRegKey(PTCHAR key, bool* boolValue);
		void ClearResources();

		static LRESULT CALLBACK RebarParentSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
		static LRESULT CALLBACK RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		void PerformRedrawCheck();
		LRESULT CorrectBandSize();
		bool ShouldRefreshVisual();

		LRESULT LoadBitmapForSize();

	public: // COM method implementations:

		// implement IDeskBand:
		STDMETHOD(GetBandInfo)(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pDbi);

		// implement IObjectWithSite:
		STDMETHOD(SetSite)(IUnknown *pUnkSite);

		// implement IOleWindow:
		STDMETHOD(GetWindow)(HWND *hWnd);
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

		// implement IDockingWindow:
		STDMETHOD(CloseDW)(unsigned long dwReserved);
		STDMETHOD(ResizeBorderDW)(const RECT *pRcBorder, IUnknown *pUnkToolbarSite, BOOL fReserved);
		STDMETHOD(ShowDW)(BOOL fShow);

		// implement DWebBrowserEvents2:
		STDMETHOD(OnNavigateComplete)(IDispatch *pDisp, VARIANT *url);
		STDMETHOD(OnQuit)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(ThrobberBand), ThrobberBand);

#endif // _THROBBERBAND_H