// PreferencesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EHPIAHRoadView.h"
#include "PreferencesDialog.h"


// CPreferencesDialog dialog

IMPLEMENT_DYNAMIC(CPreferencesDialog, CDialog)

CPreferencesDialog::CPreferencesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesDialog::IDD, pParent)
{

}

CPreferencesDialog::~CPreferencesDialog()
{
}

void CPreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_MAX_LANES, m_nLaneWidthFactor);
   DDV_MinMaxInt(pDX, m_nLaneWidthFactor, 1, 10);
   DDX_Check(pDX, IDC_SHOW_TUNNELS, m_bShowTunnels);
   DDX_Check(pDX, IDC_SHOW_ROUNDABOUTS, m_bShowRoundabouts);
   DDX_Check(pDX, IDC_SHOW_SPEED_LIMIT, m_bShowSpeed);
   DDX_Check(pDX, IDC_AUTO_SCALE, m_bAutoScale);
   DDX_Text(pDX, IDC_SCALE_IN_CITY, m_nCityInScale);
   DDV_MinMaxInt(pDX, m_nCityInScale, 1000, 999999);
   DDX_Text(pDX, IDC_SCALE_OUTOF_CITY, m_nCityOutScale);
   DDV_MinMaxInt(pDX, m_nCityOutScale, 1000, 999999);
   DDX_Check(pDX, IDC_SHOW_PEDESTRIAN, m_bShowTSPedestrian);
   DDX_Check(pDX, IDC_SHOW_PEDESTRIAN_CROSSWALK, m_bShowTSPedestrianCrosswalk);
   DDX_Check(pDX, IDC_SHOW_TRAFFIC_LIGHT, m_bShowTSTrafficLight);
   DDX_Check(pDX, IDC_SHOW_TRAFFIC_LIGHT_SIGN, m_bShowTSTrafficLightSign);
   DDX_Check(pDX, IDC_SHOW_PRIORITY_ROAD, m_bShowTSRightOfWayRoad);
   DDX_Check(pDX, IDC_SHOW_PRIORITY_INTEERSECTION, m_bShowTSRightOfWay);
   DDX_Check(pDX, IDC_SHOW_GIVE_WAY, m_bShowTSYield);
   DDX_Check(pDX, IDC_SHOW_END_BUILTUP_AREA, m_bShowTSEndOfTown);
   DDX_Check(pDX, IDC_SHOW_INTERSECTION, m_bShowTSIntersection);
   DDX_Check(pDX, IDC_SHOW_OTHER_SIGNS, m_bShowTSOther);
   DDX_Check(pDX, IDC_SHOW_CUSTOM_SIGNS, m_bShowCustom);
   DDX_Check(pDX, IDC_DEBUG, m_bDebug);
}


BEGIN_MESSAGE_MAP(CPreferencesDialog, CDialog)
END_MESSAGE_MAP()


// CPreferencesDialog message handlers
