#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

#include "util.h"

namespace CEUtil
{
/*
 * Get HKEY from HKCU/Software/ClassicExplorer.
 */
LRESULT GetHkey(HKEY* hkDest){
	return RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\ClassicExplorer"), 0, NULL, 0, KEY_READ|KEY_WRITE, NULL, hkDest, NULL);
}

/*
 * Get a DWORD value from an HKEY. Creates it with the dwDefault if it doesn't exist.
 */
LRESULT DwordFromHkey(HKEY hkSrc, DWORD* dwDest, LPCTSTR szValue, DWORD* dwDefault){
	DWORD cbDest = sizeof(DWORD);
	LRESULT lResult = RegGetValue(hkSrc, TEXT(""), szValue, RRF_RT_DWORD, NULL, dwDest, &cbDest);
	if (lResult == ERROR_FILE_NOT_FOUND) {
		CEUtil::HkeyFromDword(dwDefault, hkSrc, szValue);
	}
	return lResult;
}

/*
 * Sets a DWORD from an HKEY.
 */
LRESULT HkeyFromDword(DWORD* dwSrc, HKEY hkDest, LPCTSTR szValue){
	return RegSetValueEx(hkDest, szValue, 0, REG_DWORD, (BYTE*)dwSrc, sizeof(DWORD));
}

/*
 * FixExplorerSizes: Manually correct the sizes of all children in the explorer
 *                   window.
 * 
 * This is useful for avoiding graphical inconsistencies after certain operations
 * which don't immediately sync with explorer.
 * 
 * Make sure to be mindful of redraw loops in calling this. Avoid calling this function
 * from within size handlers, unless you are absolutely sure that the visual needs to be
 * revalidated, or else it will softlock explorer (and probably every other program too).
 * 
 * See: FixExplorerSizesIfNecessary
 * 
 * NOTE: Find a better way of invalidating the explorer visual?
 */
HRESULT FixExplorerSizes(HWND hWndExplorerChild)
{
	HWND hWndExplorerRoot = GetAncestor(hWndExplorerChild, GA_ROOTOWNER);
	if (!IsWindow(hWndExplorerRoot))
		return E_FAIL;

	HWND hWndTabWindow = FindWindowExW(hWndExplorerRoot, NULL, L"ShellTabWindowClass", NULL);
	if (!hWndTabWindow)
		return E_FAIL;

	RECT rcTabWindow;
	GetClientRect(hWndTabWindow, &rcTabWindow);

	int cxTabWindow = rcTabWindow.right - rcTabWindow.left;
	int cyTabWindow = rcTabWindow.bottom - rcTabWindow.top;

	/*
	 * For some reason, Explorer *may* initialise the tab window at 0x0. This isn't always the
	 * case, but I have experienced it occasionally (and consistently thereafter) during testing.
	 * My guess is that it should be initially scaled to fit the last rebar configuration, but
	 * perhaps this cached value gets discarded or corrupted by something.
	 * 
	 * The size is corrected shortly after the call, but it can result in a race condition where
	 * the rebar bands are almost always coerced into being on their own separate row. During my
	 * testing, the bands would only be correctly positioned on the initial sizing if MessageBox
	 * was called, due to the specific way it blocks the thread which allows the normal Explorer
	 * logic to continue executing just enough to set the size but not calculate the rebar band
	 * positions yet.
	 * 
	 * As a result, I was having difficulties with the throbber band being positioned on its own
	 * row instead of on the right side of whichever nearby band. Explorer would correct the
	 * position the moment you sized the explorer window in any way, but not any sooner.
	 * 
	 * Fortunately, this wasn't entirely difficult to correct. Simply overriding the initial
	 * sizing (which we determine by checking if the width of the tab window is 0) does the job
	 * excellently.
	 */
	bool isInitialSizing = cxTabWindow <= 0;

	SetWindowPos(
		hWndTabWindow,
		NULL,
		NULL,
		NULL,
		isInitialSizing ? 1300 : cxTabWindow + 1,
		isInitialSizing ?  900 : cyTabWindow + 1,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER
	);

	if (!isInitialSizing)
	{
		SetWindowPos(
			hWndTabWindow,
			NULL,
			NULL,
			NULL,
			cxTabWindow,
			cyTabWindow,
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER
		);
	}

	RedrawWindow(hWndExplorerRoot, NULL, NULL, RDW_INVALIDATE);

	return S_OK;
}

HRESULT FixExplorerSizesIfNecessary(HWND explorerChild)
{
	bool shouldResize = false;

	HWND explorerRoot = GetAncestor(explorerChild, GA_ROOTOWNER);
	if (!IsWindow(explorerRoot))
		return E_FAIL;

	//------------------------------------------------------------------------------
	// BEGIN CHECKS
	//------------------------------------------------------------------------------

	// A manual resize is necessary if the height of the ReBar host shell worker is
	// different from the height of the ReBar:
	{
		HWND tab = FindWindowExW(explorerRoot, NULL, L"ShellTabWindowClass", NULL);
		HWND worker = FindWindowExW(tab, NULL, L"WorkerW", NULL);
		HWND rebar = FindWindowExW(worker, NULL, L"ReBarWindow32", NULL);

		// Worker must exist if this can be true.
		if (IsWindow(rebar))
		{
			RECT rcWorker, rcRebar;
			GetWindowRect(worker, &rcWorker);
			GetWindowRect(rebar, &rcRebar);

			int cyWorker = rcWorker.bottom - rcWorker.top;
			int cyRebar = rcRebar.bottom - rcRebar.top;

			if (cyWorker > cyRebar)
			{
				shouldResize = true;
			}
		}
	}

	//------------------------------------------------------------------------------
	// END CHECKS
	//------------------------------------------------------------------------------

	if (shouldResize)
	{
		FixExplorerSizes(explorerChild);
	}

	return S_OK;
}

} // namespace CEUtil
