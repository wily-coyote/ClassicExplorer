#pragma once
#ifndef _UTIL_H
#define _UTIL_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

namespace CEUtil
{
	LRESULT GetHkey(HKEY* hkDest);
	LRESULT DwordFromHkey(HKEY hkSrc, DWORD* dwDest, LPCTSTR szValue, DWORD* dwDefault);
	LRESULT HkeyFromDword(DWORD* dwSrc, HKEY hkDest, LPCTSTR szValue);
	
	HRESULT FixExplorerSizes(HWND explorerChild);
	HRESULT FixExplorerSizesIfNecessary(HWND explorerChild);
}

#endif
