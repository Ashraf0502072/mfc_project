// EHPIAHRoadView.h : main header file for the EHPIAHRoadView DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CEHPIAHRoadViewApp
// See EHPIAHRoadView.cpp for the implementation of this class
//

class CEHPIAHRoadViewApp : public CWinApp
{
public:
	CEHPIAHRoadViewApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
