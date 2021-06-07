#pragma once
#include "Resource.h" //Anam

struct Preferences {
   int m_nLaneWidthFactor;
   BOOL m_bShowTunnels;
   BOOL m_bShowRoundabouts;
   BOOL m_bShowSpeed;
   BOOL m_bAutoScale;
   int m_nCityInScale;
   int m_nCityOutScale;
   BOOL m_bShowTSPedestrian;
   BOOL m_bShowTSPedestrianCrosswalk;
   BOOL m_bShowTSTrafficLight;
   BOOL m_bShowTSTrafficLightSign;
   BOOL m_bShowTSRightOfWayRoad;
   BOOL m_bShowTSRightOfWay;
   BOOL m_bShowTSYield;
   BOOL m_bShowTSEndOfTown;
   BOOL m_bShowTSIntersection;
   BOOL m_bShowTSOther;
   BOOL m_bShowCustom;
   BOOL m_bDebug;
};

// CPreferencesDialog dialog

class CPreferencesDialog : public CDialog, public Preferences
{
	DECLARE_DYNAMIC(CPreferencesDialog);

public:
	CPreferencesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreferencesDialog();

// Dialog Data
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
