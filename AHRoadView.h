/** 
 * @file    AHRoadView.h
 * @brief   Plug-In to display Road View.
 * @author  Stéphane Dreher
 * @version 0.1
 * @date    20.11.2004.
 *
 * 18/03/2005: added Crossing Side info (Heading Change Support)
 * 05/2005:    added Speed Limit support
 * 06/2005:    added correct distance calculation
 */

#pragma once

#include "stdafx.h"
#include "PreferencesDialog.h"
#include "ADASRP.Libs\EHPI\EHPlugIn.h"
#include "RVAreas.h"
#include "RVSign.h"
#include "Resource.h"
//#include"ADASRP.Libs\NTMFCUtils\inc\NTLayout.h"



class CAHRoadView : public CEHPlugIn, public Preferences
{

public: // Constructor/Destructor

   CAHRoadView(const Context& context);
   virtual ~CAHRoadView();


public: // CEHPlugIn

   virtual const TCHAR* getTitle() const;
   virtual bool canBeDestroyed(const char* szPlugInTitles);


public: // AH Listener

   virtual Sint16 onClearMsg          (const MASSIVE::AHClearMsg& msg);
   virtual Sint16 onPositionChangedMsg(const MASSIVE::AHPositionChangedMsg& msg);
   virtual Sint16 onRootLinkMsg       (const MASSIVE::AHRootLinkMsg& msg);


public: // VP Listener

   virtual Sint16 onVPMessage(const MASSIVE::VPMessage& rMsg);


public: // Messages

   DECLARE_MESSAGE_MAP()

   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnPaint();
   afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
   afx_msg void OnDestroy();
   afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
   afx_msg void OnConfigure();

private: // Worker method

   void showPreferencesDialog();


   /** Gets all the infos along the MPP to build the Road View (Crossings, Lane changes, Crossing Sides, Prohibited Roads). */
   void getPathInfos();
   /** Gets the number of lane on the root link */
   bool getStartNbOfLanes(std::vector<Uint32>& mpp, ADAS::HorizonAttributes&  pts);
   /** Gets the Side of the Crossing at the end of the link with id nLinkId */
   RVSign::CrossingSideType getCrossingSide(ADAS::HorizonLinks& links, Uint32 nLinkId, std::vector<Uint32>& mpp, RVSign::ProhibitedSideType* nProhibitedSide); // Uint32 nNextLinkId

   /** Creates arrays for the arc-shaped transition in lane change areas */
   int  ArcPoints                   (CArray<CPoint, CPoint>& PolyPtsArray, CArray<CPoint, CPoint>& PolyPtsLineArray, int xPolyRefPoint, int yPolyRefPoint, int nRadius, double fAngle, bool bIsArcUp, bool bIsArcRight, int nVerticalOffset);
   /** To check if attribute points are on the MPP */
	bool isAttributeOnMPP            (ADAS::HorizonAttribute* pt, std::vector<Uint32>& mpp);
   /** Check if the link with given ID is on the MPP */
   bool isIdOnMPP                   (Uint32 nLinkId, std::vector<Uint32>& mpp);
   /** Retrieves the Speed Limit (Current/ADAS or Expected) on the link whose Id is provided */
   Uint32 getSpeedOnLink(Uint32 nLinkId, ADAS::HorizonAttributes&  pts);
   /** Get number of lanes on the link that ahat refers to. Add opposing lanes * 64. */
   int CAHRoadView::getLanes(ADAS::HorizonAttributes &attrs, ADAS::HorizonAttribute &ahat);
   /** Get Traffic Signs along the MPP and store them in an Area strcuture */
   void getTSAreas();
   

private: // Painting

   void paintAll(CDC& dc, const CSize& sizeCanvas);

   /** Paints the Plug-in window Background */
   void paintBackground             (CDC& dc, const CSize& sizeCanvas);
   /** Paints the car bitmap frtom the resources */
   int  paintCar                    (CDC& dc, const CSize& sizeCanvas);
   /** Paints the scale ruler and the City Sign */
   void paintScale                  (CDC& dc, const CSize& sizeCanvas, int wCar);
   /** Paints the Roundabout and Tunnel areas as background rectangles if ShowTunnels or Showroundabouts are set */
   void paintAreas                  (CDC& dc, const CRect& rectRoad, CArray<RVAreas>& areas);
   /** Paints the whole Road View */
   bool paintRoad                   (CDC& dc, const CSize& sizeCanvas, int wCar);
   /** Paints a straight road segment bewteen two transition areas (crossings or lane number changes) */
   void paintRoadSegment            (CDC& dc, const CRect& rectRoad, int nStart, int nEnd, int nNbOfLanes);
   /** Paint the Lane separation lines */
   int  paintLaneLine               (CDC& dc, int nStart, int nEnd, int nY, int nLineLength, int nGapLength);  // int nLastPixels
   /** Paints the Transition areas (Crossings or Lane number change) bewteen two Road segments */
   void paintRoadSegmentTransition  (CDC& dc, const CRect& rectRoad, int nStart, int nEnd, int nPreviousNbOfLanes, int nNextNbOfLanes, float nSizeOfCrossing, RVSign::CrossingSideType nCrossingSide, RVSign::ProhibitedSideType nProhibitedSide, Uint32 nLinkId);
   /** Paints a Crossing on the determined sides */
   void paintCrossing               (CDC& dc, int nStart, int nEnd, int nRoadCenter, int nRoadWidth, int nLaneWidth, int nNextNbOfLanes, RVSign::CrossingSideType nCrossingSide);
   /** Paint methods for lane number change accomodation areas */
   void paintLaneIncrease           (CDC& dc, int nStart, int nEnd, int nRoadCenter, int nRoadWidth, int nLaneWidth, int nPreviousNbOfLanes, int nNextNbOfLanes); // float fSizeOfCrossing
   void paintLaneDecrease           (CDC& dc, int nStart, int nEnd, int nRoadCenter, int nRoadWidth, int nLaneWidth, int nPreviousNbOfLanes, int nNextNbOfLanes); // float fSizeOfCrossing
   /** Paints a hashed area if two crossings are too close */
   void paintComplexCrossing        (CDC& dc, const CRect& rectRoad, int nStart, int nEnd, int nNbOfLanes);
   /** Paints arrows showing the traffic flow direction */
   void paintArrow                  (CDC& dc, int xCenter, int yCenter, bool bLeftToRigth, int nArrowWidthPixels);
   /** Paint text in the Plug-in window (for Debug) */
   bool paintText                   (CDC& dc, const CRect& rectRoad, int nPosition, CString szText, COLORREF color = RGB(0, 0, 0));
   
   /** Various methods to paint signs */
   void paintSigns                  (CDC& dc, const CSize& sizeCanvas, int wCar);
   void paintSign                   (CDC& dc, const CRect& rectSigns, int nTopRoad, const TrafficSign::Sign theSign, UINT nSignParam, int nDistanceM, bool bLength, int nDistFromStart, int &xLast, int iPos, bool bIsSign = false);
   void paintSignPx                 (CDC& dc, const CRect& rectSigns, const TrafficSign::Sign theSign, UINT nSignParam, int nPosition);
   void paintProhibitedSigns        (CDC& dc, const CRect& rectRoad, int nRoadCenter, int nRoadWidth, int nPosition, RVSign::ProhibitedSideType nProhibitedSide);

private: // .INI settings
   CString            m_szCustomSignPath0;
   CString            m_szCustomSignPath1;
   CString            m_szCustomSignPath2;
   CString            m_szCustomSignPath3;
   CString            m_szCustomSignPath4;
   CString            m_szCustomSignPath5;
   CString            m_szCustomSignPath6;
   CString            m_szCustomSignPath7;
   CString            m_szCustomSignPath8;
   CString            m_szCustomSignPath9;
   CString            m_szCustomSignPath10;
   CString            m_szCustomSignPath11;
   CString            m_szCustomSignPath12;
   CString            m_szCustomSignPath13;
   CString            m_szCustomSignPath14;
   CString            m_szCustomSignPath15;
   

private: // Data members

   int                m_nDisplayedLengthCM;
   int                m_nStartNbOfLanes;
   int                m_nLineLength;
   int                m_nLineGapLength;
   bool               m_bIsStartInTunnel;
   bool               m_bIsStartInRoundabout;
   bool               m_bIsInCity;
   int                m_nSpeedOnRootLink;
   bool               m_bIsCurrentSpeed;
   int                m_nMaxLanes;

   CArray<RVSign>     m_signs;
   TrafficSign*       ts;
   /** Areas for Tunnels and Roundabouts */
   CArray<RVAreas>    m_areas;
   /** Areas for Traffic Signs */
   CArray<RVAreas>    m_tsAreas;

   CFont              fontText;
   CFont              fontScale;
   CBitmap            carNT;
   CBrush             brushScale;
   CBrush             brushRoad;
   CBrush             brushComplex;
   CBrush             brushAreaRoundabout;
   CBrush             brushAreaTunnel;
   CBrush             brushAreaTS;
   CBrush             brushArrow;
   CPen               penRoad;
   CPen               penLines;
   CPen               penLinesDash;
   CPen               penArrow;


   

private: // Static data members

   static const TCHAR* INI_SECTION;

};
