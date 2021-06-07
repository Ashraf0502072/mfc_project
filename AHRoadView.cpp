/** 
 * @file    AHRoadView.cpp
 * @brief   Plug-In to display the Road View.
 * @author  Stéphane Dreher
 * @version 0.1
 * @date    20.11.2004.
 *
 * 18/03/2005: added Crossing Side info (Heading Change Support)
 */

#include "StdAfx.h"
#include "Resource.h"
#include "RVSign.h"
#include "RVAreas.h"
#include "AHRoadView.h"

#include <algorithm>

#ifdef _DEBUG
   #define new DEBUG_NEW
#endif

// Maths constants
static const double  PI             = 3.14159265358979;
// Constants for Arc direction and side (lane transition)
static const bool    ARC_UP         = true;
static const bool    ARC_DOWN       = false;
static const bool    ARC_RIGHT      = true;
static const bool    ARC_LEFT       = false;

///////////////////
// Paint parameters 

static COLORREF COLOR_BACK;
static COLORREF COLOR_ROAD;
static COLORREF COLOR_LINES;
static COLORREF COLOR_SCALE;
static COLORREF COLOR_ARROW;
static COLORREF COLOR_DEBUG;

static COLORREF COLOR_AREA_ROUNDABOUT;
static COLORREF COLOR_AREA_TUNNEL;
static COLORREF COLOR_AREA_SIGNS;

static COLORREF COLOR_COMPLEX        = RGB(170, 170, 170);

/*---------------------------------------------------------------------------------------------
                                            MARGIN_TOP
                                   ______________________________
                                  |___________Signs______________|  <- Lane change + Area Signs
                                  |______________________________|  <- Prohibited Turn Signs
                                          ROAD_SIDE_GAP
                                   ______________________________
               ____               |                              |
MARGIN_LEFT   |wCar| CAR_ROAD_GAP |           Road               | MARGIN_RIGHT
                                  |______________________________|
                                      
                                   _______ROAD_SIDE_GAP__________
                                  |___________Signs______________|  <- Prohibited Turn Signs
                                  |______________________________|  <- Crossing Signs
                                   ______________________________
                                  |___________Scale______________|

                                          MARGIN_BOTTOM
                                          
-----------------------------------------------------------------------------------------------*/

static int     MARGIN_LEFT                   = 30;    // 20
static int     MARGIN_RIGHT                  = 20;
static int     MARGIN_TOP                    = 5;
static int     MARGIN_BOTTOM                 = 10;
static int     CAR_ROAD_GAP                  = 10;    // 5
static int     ROAD_SIDE_GAP                 = 4;     // Gap between Road side and Signs display rectangle
static int     ROAD_LINES_GAP                = 2;     // Gap between continuous border lines and road border

static int     VERTICAL_ROAD_EXTENT          = 80;    // Percentage of the Plug-in window dedicated to the road, the rest will contain the scale 
static int     ARROW_CENTER_FROM_START       = 35;    // Position of the center of the traffic flow dir. arrows with respect to start of segment

static float   m_fCrossingWidthFactorSame    = 1;     // Width factor for each possible crossing road size
static float   m_fCrossingWidthFactorSmall   = 0.5;
static float   m_fCrossingWidthFactorBig     = 1.5;
static int     m_nCrossingLaneExtent         = 10;    // Number of pixels the crossing road extends on the sides of the driving road.
static int     m_nArrowWidthPixels           = 14;


/////////////////////////
// Constructor/Destructor

CAHRoadView::CAHRoadView(const Context& ctx)
     :CEHPlugIn(ctx, INI_SECTION)
{
#pragma warning(disable: 4800)   // Assign BOOL to bool.
   // Get TrafficSign path from .INI file
   CString szSignPath = getProfileText("Path", ".").c_str();
   ts = new TrafficSign(szSignPath);

   // Get INI parameters here
   m_nLaneWidthFactor     = getProfileInt("Max Lanes", 4);
   
   // Show Tunnel and/or Roundabout Areas ?
   m_bShowTunnels         = getProfileBool("Show Tunnels",     true);
   m_bShowRoundabouts     = getProfileBool("Show Roundabouts", true);

   // Show speed sign on root link
   m_bShowSpeed           = getProfileBool("Show Speed",      true);

   // Show Traffic Signs (Areas)
   m_bShowTSPedestrian           = getProfileBool(_T("Show TS Pedestrian"), true);
   m_bShowTSPedestrianCrosswalk  = getProfileBool(_T("Show TS PedestrianCrosswalk"), true);
   m_bShowTSTrafficLight         = getProfileBool(_T("Show TS TrafficLight"), true);
   m_bShowTSTrafficLightSign     = getProfileBool(_T("Show TS TrafficLightSign"), true);
   m_bShowTSRightOfWay           = getProfileBool(_T("Show TS RightOfWay"), true);
   m_bShowTSRightOfWayRoad       = getProfileBool(_T("Show TS RightOfWayRoad"), true);
   m_bShowTSYield                = getProfileBool(_T("Show TS Yield"), true);
   m_bShowTSEndOfTown            = getProfileBool(_T("Show TS EndOfTown"), true);
   m_bShowTSIntersection         = getProfileBool(_T("Show TS Intersection"), true);
   m_bShowTSOther                = getProfileBool(_T("Show Other Signs"),  true);
   m_bShowCustom                 = getProfileBool(_T("Show Custom"), true);
   m_szCustomSignPath0           = getProfileText(_T("CustomSignPath0"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath1           = getProfileText(_T("CustomSignPath1"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath2           = getProfileText(_T("CustomSignPath2"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath3           = getProfileText(_T("CustomSignPath3"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath4           = getProfileText(_T("CustomSignPath4"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath5           = getProfileText(_T("CustomSignPath5"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath6           = getProfileText(_T("CustomSignPath6"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath7           = getProfileText(_T("CustomSignPath7"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath8           = getProfileText(_T("CustomSignPath8"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath9           = getProfileText(_T("CustomSignPath9"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath10          = getProfileText(_T("CustomSignPath10"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath11          = getProfileText(_T("CustomSignPath11"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath12          = getProfileText(_T("CustomSignPath12"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath13          = getProfileText(_T("CustomSignPath13"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath14          = getProfileText(_T("CustomSignPath14"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();
   m_szCustomSignPath15          = getProfileText(_T("CustomSignPath15"), _T("..\\TrafficSigns\\Warning.wmf")).c_str();

   // Auto Scale
   m_bAutoScale           = getProfileBool(_T("Auto-Scale"),false);           // default to No Auto-Scale
   m_nCityInScale         = getProfileInt(_T("City In Scale"),300) * 100;     // Convert in CM
   m_nCityOutScale        = getProfileInt(_T("City Out Scale"),1000) * 100;
   
   // Debug Mode (prints out LinkIds on segment transition areas)
   m_bDebug               = getProfileBool(_T("Debug"),false);

   // Get INI Color parameters
   COLOR_BACK             = getProfileColor("RGB Back",     RGB(255, 255, 255));
   COLOR_ROAD             = getProfileColor("RGB Road",     RGB(  0,   0,   0));
   COLOR_LINES            = getProfileColor("RGB Lines",    RGB(255, 255, 255));
   COLOR_ARROW            = getProfileColor("RGB Arrow",    RGB(210, 210, 210));
   COLOR_SCALE            = getProfileColor("RGB Scale",    RGB(170, 170, 170));
   COLOR_DEBUG            = getProfileColor("RGB Debug",    RGB(255, 127,   0));

   COLOR_AREA_ROUNDABOUT  = getProfileColor("RGB Area Roundabout",    RGB(220, 170, 170));
   COLOR_AREA_TUNNEL      = getProfileColor("RGB Area Tunnel",        RGB( 78, 177, 130));
   COLOR_AREA_SIGNS       = getProfileColor("RGB Area Signs",         RGB(  0, 172, 255));
#pragma warning(default: 4800)   // Assign BOOL to bool.

   // Create painting elements
   fontText.               CreateFont(-16, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");
   fontScale.              CreateFont(-14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");

   penRoad.                CreatePen(PS_SOLID, 1, COLOR_ROAD);
   penLines.               CreatePen(PS_SOLID, 3, COLOR_LINES);
   penLinesDash.           CreatePen(PS_DASH, 3, COLOR_LINES);
   penArrow.               CreatePen(PS_SOLID, 1, COLOR_ARROW);

   brushScale.             CreateSolidBrush(COLOR_SCALE);
   brushRoad.              CreateSolidBrush(COLOR_ROAD);
   brushComplex.           CreateHatchBrush(HS_BDIAGONAL, COLOR_COMPLEX);
   brushAreaRoundabout.    CreateSolidBrush(COLOR_AREA_ROUNDABOUT);
   brushAreaTunnel.        CreateSolidBrush(COLOR_AREA_TUNNEL); 
   brushAreaTS.            CreateSolidBrush(COLOR_AREA_SIGNS);
   brushArrow.             CreateSolidBrush(COLOR_ARROW);

   // Load the Car bitmap from the resources
   VERIFY(carNT.LoadBitmap(IDB_NTCAR));
};

CAHRoadView::~CAHRoadView(void)
{
   delete ts;
   fontText.DeleteObject();
   fontScale.DeleteObject();

   carNT.DeleteObject();

   penRoad.DeleteObject();
   penLines.DeleteObject();
   penLinesDash.DeleteObject();
   penArrow.DeleteObject();

   brushScale.DeleteObject();
   brushRoad.DeleteObject();
   brushComplex.DeleteObject();   
   brushArrow.DeleteObject();
   brushAreaRoundabout.DeleteObject();
   brushAreaTunnel.DeleteObject();
   brushAreaTS.DeleteObject();
};

////////////
// CEHPlugIn

const TCHAR* CAHRoadView::getTitle() const
{
   return _T("Road View");
};


bool CAHRoadView::canBeDestroyed(const char* /*szPlugInTitles*/)
{
   // SAMPLE: Code that ensures that this plug-in is unloaded last!
   //         Only one plug-in in the system may have such behaveiour;
   //         otherwise deadlock will occur!
   //   return strcmp(szPlugInTitles, getTitle()) == 0;
   return true;
};

//////////////
// AH Listener


Sint16 CAHRoadView::onClearMsg(const MASSIVE::AHClearMsg& /*msg*/)
{
   m_tsAreas.RemoveAll();
   getPathInfos();
   getTSAreas();
   return MASSIVE::OK;
};


Sint16 CAHRoadView::onPositionChangedMsg(const MASSIVE::AHPositionChangedMsg& /*msg*/)
{
   m_tsAreas.RemoveAll();
   getPathInfos();
   getTSAreas();
   return MASSIVE::OK;
};

Sint16 CAHRoadView::onRootLinkMsg(const MASSIVE::AHRootLinkMsg& msg)
{
	// The Root link of the Horizon has changed. The ID is in the msg

   ADAS::HorizonContainer* ahc = context.ahc;
   ADAS::HorizonLinks &hl = ahc->getLinks();
	ADAS::HorizonLink &link = hl.getLinkById(msg.nId);
   ADAS::HorizonAttributes&  pts = ahc->getAttributes();

   m_bIsInCity = link.isInCity() ? true : false;

   m_nSpeedOnRootLink = getSpeedOnLink(msg.nId, pts); // This part modified by sdrehe 05/2005 to account for ADAS Speed Limits
   
   // Set the scale if Auto-Scale has been set in INI
   if (m_bAutoScale)
   {  
      m_nDisplayedLengthCM = link.isInCity() ? m_nCityInScale : m_nCityOutScale;
   }

   return MASSIVE::OK;
};

//////////////
// VP Listener

Sint16 CAHRoadView::onVPMessage(const MASSIVE::VPMessage& rMsg)
{
   if (rMsg.m_nValidCandidates > 0)
   {
      // TODO
   }
   else
   {
      // TODO
   }
   return MASSIVE::OK;
};

///////////
// Messages

BEGIN_MESSAGE_MAP(CAHRoadView, CEHPlugIn)
   ON_WM_CREATE()
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_MOUSEWHEEL()
   ON_WM_DESTROY()
   ON_COMMAND(ID_CONFIGURE, &CAHRoadView::OnConfigure)
   ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int CAHRoadView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CEHPlugIn::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_bIsInCity = false;
   m_bIsCurrentSpeed = false;
   m_nDisplayedLengthCM = m_bAutoScale ? m_nCityOutScale : 100000;
   m_nStartNbOfLanes = 1;
   m_nLineLength = 7;
   m_nLineGapLength = 10;
   m_nMaxLanes = 1;

   return 0;
};


void CAHRoadView::OnDestroy()
{
   CEHPlugIn::OnDestroy();
};

void CAHRoadView::OnSize(UINT nType, int cx, int cy)
{
   CEHPlugIn::OnSize(nType, cx, cy);
};

BOOL CAHRoadView::OnEraseBkgnd(CDC* pDC)
{
   pDC;
   return true;
};

void CAHRoadView::OnPaint()
{
   CPaintDC dc(this);
   CRect rect;
   GetClientRect(rect);
   CSize size = rect.Size();

   CDC dcMem;
   dcMem.CreateCompatibleDC(&dc);
      CBitmap bitmap; 
      bitmap.CreateCompatibleBitmap(&dc, size.cx, size.cy);
         CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);
            paintAll(dcMem, size);
            dc.BitBlt(0, 0, size.cx, size.cy, &dcMem, 0, 0, SRCCOPY);
         dcMem.SelectObject(pOldBitmap);
      bitmap.DeleteObject();
   dcMem.DeleteDC();
};

// Changes the scale length which is used in paintScale(), called in turn by paintAll() which is called by OnPaint()
// Note that this method should force a repaint, therefore call an Invalidate()
BOOL CAHRoadView::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
   if ((zDelta < 0) && (m_nDisplayedLengthCM > 10000))         // Min distance 100 m
   {
      m_nDisplayedLengthCM -= 10000; 
   }
   else if ((zDelta > 0) && (m_nDisplayedLengthCM < 1000000))  // Max distance 10.000 m
   {
      m_nDisplayedLengthCM += 10000; 
   };
   
   Invalidate();
   return TRUE;
};


///////////
// Painting

void CAHRoadView::paintAll(CDC& dc, const CSize& sizeCanvas)
{
   paintBackground(dc, sizeCanvas);
   int wCar = paintCar(dc, sizeCanvas);
   paintScale(dc, sizeCanvas, wCar);
   
   if (paintRoad(dc, sizeCanvas, wCar))
   {
      paintSigns(dc, sizeCanvas, wCar);
   };
  
};


void CAHRoadView::paintBackground(CDC& dc, const CSize& sizeCanvas)
{
   dc.FillSolidRect(0, 0, sizeCanvas.cx, sizeCanvas.cy, COLOR_BACK);
};


int CAHRoadView::paintCar(CDC& dc, const CSize& sizeCanvas)
{
   int x = MARGIN_LEFT;
   int y = (int) (sizeCanvas.cy * ((VERTICAL_ROAD_EXTENT/100.0) / 4.0));
   int h = (int) (sizeCanvas.cy * ((VERTICAL_ROAD_EXTENT/100.0) / 2.0));

   int nLaneWidth = (int)(h / (m_nLaneWidthFactor * 2));
   int nRoadCenter = y + (int) (h / 2);

   // Show the Car bitmap (from the resources)
   BITMAP bitmap;
   CBitmap& car = carNT;
   if (car.GetBitmap(&bitmap) != 0)
   {
      int w = bitmap.bmWidth;
      int h1 = bitmap.bmHeight;
      if (sizeCanvas.cx > w * 3)
      {
         y = nRoadCenter - (h1/2) + (m_nStartNbOfLanes * nLaneWidth / 2);
         CDC memDC;
         memDC.CreateCompatibleDC(&dc);
            CBitmap* pOldBitmap = memDC.SelectObject(&car);
               dc.BitBlt(x, y, w, h1, &memDC, 0, 0, SRCCOPY);
            memDC.SelectObject(pOldBitmap);
         memDC.DeleteDC();
         return w;
      };
   };
   return 0;
};

void CAHRoadView::paintScale(CDC& dc, const CSize& sizeCanvas, int wCar)
{
   int xScale = MARGIN_LEFT + wCar + CAR_ROAD_GAP;
   int yScale = min((int) (sizeCanvas.cy * (VERTICAL_ROAD_EXTENT / 100.0)), sizeCanvas.cy - 30);   // Force the Scale top position 
   int hScale = (int) (sizeCanvas.cy * ((100.0 - VERTICAL_ROAD_EXTENT) / 200.0) - MARGIN_BOTTOM);  // The rule is 50% of the scale rect height
   hScale = max(hScale,15);
   int lScale = sizeCanvas.cx - xScale - MARGIN_RIGHT;

   // Paint the scale
   CRect rectScale(CPoint(xScale,yScale), CSize(lScale,hScale));
   rectScale.top    += ROAD_SIDE_GAP;
   rectScale.bottom -= 1;    // small gap between scale and text below

   dc.FrameRect(rectScale, &brushScale);
   dc.FillSolidRect(rectScale.left, rectScale.top, (int) (rectScale.Width()/4), rectScale.Height(), COLOR_SCALE);
   dc.FillSolidRect(rectScale.left + (int) (rectScale.Width()/2), rectScale.top, (int) (rectScale.Width()/4), rectScale.Height(), COLOR_SCALE);

   // Scale anotations
   CRect rectText(CPoint(xScale,yScale + hScale), CSize(lScale, hScale));   // The text takes the lower 50% of the scale
   CString szMax;  szMax. Format(_T("%ld m"), m_nDisplayedLengthCM / 100);
   CString szCenter;  szCenter. Format(_T("%ld"), m_nDisplayedLengthCM / 200);
   CString szZero(_T("0"));
   CFont* pOldFont = dc.SelectObject(&fontScale);
      dc.DrawText(szMax,  rectText, DT_RIGHT  | DT_TOP);
      dc.DrawText(szCenter,  rectText, DT_CENTER  | DT_TOP);
      dc.DrawText(szZero,  rectText, DT_LEFT  | DT_TOP);
   dc.SelectObject(pOldFont);

   // Paint the City Sign on the left of the Scale if appropriate
   if (m_bIsInCity)
   {
      ts->draw(&dc, CRect(CPoint(MARGIN_LEFT, yScale), CSize(wCar, 2*hScale)), TrafficSign::tsUrbanArea, 0, 0, false); // tsCityBW 
   }

   // In Debug mode, paint the Speed Limit Sign of the Root Link on the top left. Speed Limit corresponds to ADAS Speed if available.
   if ((m_nSpeedOnRootLink > 0) && (m_bShowSpeed))
   {
      TrafficSign::Sign speedSign = (m_nSpeedOnRootLink < 997) ?
            (m_bIsCurrentSpeed ? TrafficSign::tsSpeedLimit : TrafficSign::tsExpectedSpeedLimit) :
            TrafficSign::tsSpeedLimitEnd;
      ts->draw(&dc, CRect(CPoint(MARGIN_LEFT, MARGIN_TOP), CSize(wCar, int(2.5*hScale))), speedSign, m_nSpeedOnRootLink, 0, false);
      //CString szADAS = m_bIsCurrentSpeed ? (_T("Current")) : (_T("Expctd"));
      //dc.DrawText(szADAS, CRect(CPoint(MARGIN_LEFT, MARGIN_TOP + int(2.5*hScale)), CSize(wCar, hScale)) , DT_CENTER  | DT_TOP);
   }
};

bool CAHRoadView::paintRoad(CDC& dc, const CSize& sizeCanvas, int wCar)
{
   int   xRoad     = MARGIN_LEFT + wCar + CAR_ROAD_GAP;
   int   yRoad     = (int) (sizeCanvas.cy * ((VERTICAL_ROAD_EXTENT / 100.0) / 4.0));
   int   hRoad     = (int) (sizeCanvas.cy * ((VERTICAL_ROAD_EXTENT / 100.0) / 2.0));  // Road takes half the height of the Plug-in window
   int   lRoad     = sizeCanvas.cx - xRoad - MARGIN_RIGHT;
   CRect rectRoad(CPoint(xRoad, yRoad), CSize(lRoad, hRoad));

   int   leftSegmentLimit        = rectRoad.left;                         // set initial values for the first segment
   int   rightSegmentLimit       = rectRoad.right;
   
   int   nPreviousTransitionWidthPixels   = 0;
   int   nNextTransitionWidthPixels       = 0;
   int   nNextNbOfLanes                   = m_nStartNbOfLanes;            // We got the start nb of lanes from getPathInfos()
   int   nCurrentNbOfLanes                = m_nStartNbOfLanes;
   bool  bThereWasALaneChange             = false;
   float fNextSizeOfCrossing              = 0.0;
   float fPreviousSizeOfCrossing          = 0.0;

   int   nDistanceToPreviousSignPixels    = 0;
   int   nDistanceToNextSignPixels        = 0;

   // paint the Areas rectangles first (in the background of the Road)
   if (m_bShowTunnels || m_bShowRoundabouts)
   {
      paintAreas(dc, rectRoad, m_areas);
   }
   paintAreas(dc, rectRoad, m_tsAreas);
      
   // Paint the road segments using the signs info in RVSign vector. Each segment is painted together with the transition following it.
   for (int i = 0;  i < (int) m_signs.GetSize();  i++)     // loop over the conditions/signs along the MPP
   {
      // Get the nb of Lanes and crossing info
      nNextNbOfLanes       = m_signs[i].getSignLanesParam();
      fNextSizeOfCrossing  = m_signs[i].getSizeOfCrossing();
      
      // Get the crossing side (left, right, or both)
      RVSign::CrossingSideType nCrossingSide = m_signs[i].getCrossingSide();

      // Get the prohibited (wrong-way) sides
      RVSign::ProhibitedSideType nProhibitedSide = m_signs[i].getProhibitedSide();

      // Get the LinkId of the Sign attribute. The LinkId applies to the Link after.
      Uint32 nLinkId = m_signs[i].getLinkId();

      // SET LEFT SEGMENT LIMITS
      if (i > 0)
      {
         nCurrentNbOfLanes = m_signs[i-1].getSignLanesParam();
         nDistanceToPreviousSignPixels = MulDiv(m_signs[i-1].getDistanceToSignM()*100, rectRoad.Width(), m_nDisplayedLengthCM);
         // If there was a lane change, set the transition width to 0 to ignore the Crossing
         fPreviousSizeOfCrossing = bThereWasALaneChange ? 0 : m_signs[i-1].getSizeOfCrossing();
         bThereWasALaneChange = false;
         // Calculate the width in Pixels of the previous transition area (crossing of lane change area) 
         nPreviousTransitionWidthPixels = (int)(((float) hRoad / (float) m_nLaneWidthFactor) * (float) nCurrentNbOfLanes * fPreviousSizeOfCrossing);
         leftSegmentLimit = rectRoad.left + nDistanceToPreviousSignPixels + (nPreviousTransitionWidthPixels / 2);
         if (leftSegmentLimit > rectRoad.right)
         {
            break;  // exit the loop and draw nothing
         }
      }

      // SET RIGHT SEGMENT LIMITS
      nDistanceToNextSignPixels = MulDiv(m_signs[i].getDistanceToSignM()*100, rectRoad.Width(), m_nDisplayedLengthCM);
      if (nCurrentNbOfLanes != nNextNbOfLanes) // m_signs[i].getSizeOfCrossing() == 0
      {
         // If we have no crossing, the width of the transition area depends on the lane difference between the two successive segments
         nNextTransitionWidthPixels = (int)(((float) hRoad / (float) m_nLaneWidthFactor) * abs(nNextNbOfLanes - nCurrentNbOfLanes));
         rightSegmentLimit = rectRoad.left + nDistanceToNextSignPixels - nNextTransitionWidthPixels;
         bThereWasALaneChange = true;  // remember that there was a lane change to determine start transition width for next segment
      }
      else
      {
         // The size of the crossing defines the width of the transition area. The number of Lanes gives the reference width if the
         // crossing is with a similar road. The width is then adjusted as a function of the size/type of the crossing
         nNextTransitionWidthPixels = (int)(((float) hRoad / (float) m_nLaneWidthFactor) * (float) nNextNbOfLanes * fNextSizeOfCrossing);
         rightSegmentLimit = rectRoad.left + nDistanceToNextSignPixels - (nNextTransitionWidthPixels / 2);
      }
      
      if (rightSegmentLimit > rectRoad.right)
      {
         rightSegmentLimit = rectRoad.right;
      }

      // DRAW NOW
      if (rightSegmentLimit > leftSegmentLimit + 1)
      {
         if ((i == 0) && (nDistanceToNextSignPixels <= nNextTransitionWidthPixels / 2))
         {
            // paint a short segment part corresponding to the transition and jump to next Sign
            paintRoadSegment(dc, rectRoad, leftSegmentLimit, rightSegmentLimit + (2 * nNextTransitionWidthPixels), nNextNbOfLanes);
         }
         else
         {
            paintRoadSegment(dc, rectRoad, leftSegmentLimit, rightSegmentLimit, nCurrentNbOfLanes);
            // Do not draw the transition if we exceed the Right drawing Rect limit
            if ((rightSegmentLimit + nNextTransitionWidthPixels) <= rectRoad.right)
            {
               // Note: we pass m_signs[i].getSizeOfCrossing() as the method has to know if we have a crossing or not
               paintRoadSegmentTransition(dc, rectRoad, rightSegmentLimit, rightSegmentLimit + nNextTransitionWidthPixels, nCurrentNbOfLanes, nNextNbOfLanes, m_signs[i].getSizeOfCrossing(), nCrossingSide, nProhibitedSide, nLinkId);
            }
         }
      }
      else  // If two transition areas are too close, paint a Complex Crossing rectangle
      {
         paintComplexCrossing(dc, rectRoad, leftSegmentLimit, rightSegmentLimit + nNextTransitionWidthPixels, nNextNbOfLanes);
      }
   };   // end of for loop over the conditions/signs
 
   return true;
};

void CAHRoadView::paintAreas(CDC& dc, const CRect& rectRoad, CArray<RVAreas>& areas)
{
   int nLaneWidth = (int)(rectRoad.Height() / (m_nLaneWidthFactor * 2));
   int nRoadCenter = rectRoad.top + (int) (rectRoad.Height() / 2);

   int nAreaWidth;
   int nDistanceToAreaStartPx;
   int nDistanceToAreaEndPx;

   int nPreviousDistanceToAreaEndPx = 0;

   // Loop over the area vector
   for(int nAreaIdx = 0; nAreaIdx < areas.GetSize(); nAreaIdx++)
   {
      nAreaWidth = (areas[nAreaIdx].getWidth() * nLaneWidth * 2) + (2 * ROAD_LINES_GAP) + 10;
      nDistanceToAreaStartPx = MulDiv(areas[nAreaIdx].getStart()*100, rectRoad.Width(), m_nDisplayedLengthCM);
      if (nDistanceToAreaStartPx > rectRoad.right)
      {
         break;
      }
      // Check if the this area overlaps with the previous one to draw it larger
      if (nDistanceToAreaStartPx <= nPreviousDistanceToAreaEndPx)
      {
         nAreaWidth += 2;
      }
      nDistanceToAreaEndPx = MulDiv(areas[nAreaIdx].getEnd()*100, rectRoad.Width(), m_nDisplayedLengthCM);
      if (nDistanceToAreaEndPx == nDistanceToAreaStartPx) {
         continue;   // No length - no visible area.
      }
      if ((nDistanceToAreaEndPx > rectRoad.right) || (nDistanceToAreaEndPx == 0))  // == 0 means we found no End along the MPP 
      {
         nDistanceToAreaEndPx = rectRoad.right;
      }
      nPreviousDistanceToAreaEndPx = nDistanceToAreaEndPx;

      // Define the rectangle in which the area is to be painted
      CRect rectArea(CPoint(rectRoad.left + nDistanceToAreaStartPx, nRoadCenter - (int)(nAreaWidth / 2)), CSize(nDistanceToAreaEndPx - nDistanceToAreaStartPx, nAreaWidth));
      
      int bDrawTS = false;
      switch(areas[nAreaIdx].getSign())
      {
         case TrafficSign::tsRoundabout:
            // If it is a Roundabout Area
            if (m_bShowRoundabouts)
            {
               dc.FillRect(rectArea, &brushAreaRoundabout);
            }
            break;
         case TrafficSign::tsTunnel:
            // If it is a Tunnel Area
            if (m_bShowTunnels)
            {
               dc.FillRect(rectArea, &brushAreaTunnel);
            }
            break;
         case TrafficSign::tsRightOfWay:        bDrawTS = m_bShowTSRightOfWayRoad;     break;
         case TrafficSign::tsTrafficLight:      bDrawTS = m_bShowTSTrafficLight;       break;
         case TrafficSign::tsPedestrian:        bDrawTS = m_bShowTSPedestrian;         break;
         case TrafficSign::tsPedestrianCrossing:bDrawTS = m_bShowTSPedestrianCrosswalk;break;
         case TrafficSign::tsPriorityCrossing:  bDrawTS = m_bShowTSRightOfWay;         break;
         case TrafficSign::tsGiveWay:           bDrawTS = m_bShowTSYield;              break;
         case TrafficSign::tsUrbanAreaEnd:      bDrawTS = m_bShowTSEndOfTown;          break;
         case TrafficSign::tsCrossing:          bDrawTS = m_bShowTSIntersection;       break;
         case TrafficSign::tsFree:              bDrawTS = m_bShowCustom;               break;
         default:                               bDrawTS = m_bShowTSOther;                break;
      }
      if (bDrawTS)
      {
         dc.FillRect(rectArea, &brushAreaTS);
      }
   }
  
};

// Method to paint a hashed road segment
void CAHRoadView::paintComplexCrossing(CDC& dc, const CRect& rectRoad, int nStart, int nEnd, int nNbOfLanes)
{
   int nLaneWidth = (int)(rectRoad.Height() / (m_nLaneWidthFactor * 2));
   int nRoadWidth = nNbOfLanes * nLaneWidth * 2;
   int nRoadCenter = rectRoad.top + (int) (rectRoad.Height() / 2);

   CRect rectComplex(CPoint(nStart, nRoadCenter - (int)(nRoadWidth / 2) - ROAD_LINES_GAP), CSize(nEnd - nStart, nRoadWidth + (2*ROAD_LINES_GAP) + 1));
   
   CBrush* pOldBrush = dc.SelectObject(&brushComplex);
      dc.FillRect(rectComplex, &brushComplex);
   dc.SelectObject(pOldBrush);

};

void  CAHRoadView::paintRoadSegment(CDC& dc, const CRect& rectRoad, int nStart, int nEnd, int nNbOfLanes)
{
   int nLaneWidth = (int)(rectRoad.Height() / (m_nLaneWidthFactor * 2));
   int nRoadWidth = nNbOfLanes * nLaneWidth * 2;
   int nRoadCenter = rectRoad.top + (int) (rectRoad.Height() / 2);
   int nLastPixels = 0;

   // Road background
   dc.FillSolidRect(nStart, nRoadCenter - (int)(nRoadWidth / 2) - ROAD_LINES_GAP, nEnd - nStart, nRoadWidth + (2*ROAD_LINES_GAP) + 1, COLOR_ROAD);

   // Central line
   if (nNbOfLanes == 1)
   {
      // If one lane, paint a dashed central line
      nLastPixels = paintLaneLine(dc, nStart, nEnd, nRoadCenter, m_nLineLength, m_nLineGapLength);  //  nLastPixels
   }
   else
   {
      // If more than one lane, paint a solid central line
      CPen* pOldPen = dc.SelectObject(&penLines);
         dc.MoveTo(nStart, nRoadCenter);
         dc.LineTo(nEnd, nRoadCenter);
      dc.SelectObject(pOldPen);
   }
   // Road border lines
   CPen* pOldPen = dc.SelectObject(&penLines);
      // Top
      dc.MoveTo(nStart, nRoadCenter - (int)(nRoadWidth / 2));
      dc.LineTo(nEnd, nRoadCenter - (int)(nRoadWidth / 2));
      // Bottom
      dc.MoveTo(nStart, nRoadCenter + (int)(nRoadWidth / 2));
      dc.LineTo(nEnd, nRoadCenter +(int)(nRoadWidth / 2));
   dc.SelectObject(pOldPen);

   // Lane separation lines
   for (int nLanes = 1; nLanes <= nNbOfLanes; nLanes++)
   {
      CPen* _pOldPen = dc.SelectObject(&penLines);
         if (nLanes < nNbOfLanes)
         {
            nLastPixels = paintLaneLine(dc, nStart, nEnd, nRoadCenter - (nLanes * nLaneWidth), m_nLineLength, m_nLineGapLength); // nLastPixels
            nLastPixels = paintLaneLine(dc, nStart, nEnd, nRoadCenter + (nLanes * nLaneWidth), m_nLineLength, m_nLineGapLength); // nLastPixels
         }
         // Paint traffic flow direction arrows
         if ((nStart + ARROW_CENTER_FROM_START < rectRoad.right) && (nStart + ARROW_CENTER_FROM_START + (m_nArrowWidthPixels / 2) < nEnd))
         {
            paintArrow(dc, nStart + ARROW_CENTER_FROM_START, nRoadCenter - (nLanes * nLaneWidth) + (int)(nLaneWidth / 2), false, m_nArrowWidthPixels);
            paintArrow(dc, nStart + ARROW_CENTER_FROM_START, nRoadCenter + (nLanes * nLaneWidth) - (int)(nLaneWidth / 2), true, m_nArrowWidthPixels);
         }
      dc.SelectObject(_pOldPen);
   }
};

// Method to paint a Dashed Line with the pattern defined by nLineLength and nLineGapLength
int CAHRoadView::paintLaneLine(CDC& dc, int nStart, int nEnd, int nY, int nLineLength, int nLineGapLength)
{
   /*int nLastPixels*/
   CPen* pOldPen = dc.SelectObject(&penLinesDash);
   int nLineIdx = nStart;
   for (; (nLineIdx + nLineLength) < nEnd; nLineIdx += nLineLength + nLineGapLength)
   {
      dc.MoveTo(nLineIdx, nY);
      dc.LineTo(nLineIdx + nLineLength, nY);
   }
   dc.SelectObject(pOldPen);

   return (nEnd - nLineIdx + nLineLength + nLineGapLength);
};

void CAHRoadView::paintRoadSegmentTransition(CDC& dc, const CRect& rectRoad, int nStart, int nEnd, int nPreviousNbOfLanes, int nNextNbOfLanes, float fSizeOfCrossing, RVSign::CrossingSideType nCrossingSide, RVSign::ProhibitedSideType nProhibitedSide, Uint32 nLinkId)
{
   int nLaneWidth = (int)(rectRoad.Height() / (m_nLaneWidthFactor * 2));
   // Set Width of Road to the width of segment with highest number of lanes
   int nRoadWidth = nNextNbOfLanes >= nPreviousNbOfLanes ? nPreviousNbOfLanes : nNextNbOfLanes;
   nRoadWidth *= nLaneWidth * 2;
   int nRoadCenter = rectRoad.top + (int) (rectRoad.Height() / 2);

   // SIMPLE CROSSING
   if ((fSizeOfCrossing > 0) && (nPreviousNbOfLanes == nNextNbOfLanes))
   {
      paintCrossing(dc, nStart, nEnd, nRoadCenter, nRoadWidth, nLaneWidth, nNextNbOfLanes, nCrossingSide);
      // Paint the prohibited Signs
      paintProhibitedSigns(dc, rectRoad, nRoadCenter, nRoadWidth, (nEnd + nStart) / 2, nProhibitedSide);
   }

   // SIMPLE LANE NUMBER DECREASE
   if (nPreviousNbOfLanes > nNextNbOfLanes)
   {
      paintLaneDecrease(dc, nStart, nEnd, nRoadCenter, nRoadWidth, nLaneWidth, nPreviousNbOfLanes, nNextNbOfLanes);  // fSizeOfCrossing
   }
   
   // SIMPLE LANE NUMBER INCREASE
   if (nPreviousNbOfLanes < nNextNbOfLanes)
   {
      paintLaneIncrease(dc, nStart, nEnd, nRoadCenter, nRoadWidth, nLaneWidth, nPreviousNbOfLanes, nNextNbOfLanes); // fSizeOfCrossing
   }

   // Write the Link ID on top of the transition feature
   if (m_bDebug)
   {
      CString szLinkId;
      szLinkId.Format("%d", nLinkId);
      paintText(dc, rectRoad, (nStart + nEnd)/2, szLinkId, COLOR_DEBUG);
   }
};

void CAHRoadView::paintCrossing(CDC& dc, int nStart, int nEnd, int nRoadCenter, int nRoadWidth, int nLaneWidth, int nNextNbOfLanes, RVSign::CrossingSideType nCrossingSide)
{
   // CROSSING_UNKNOWN is drawn as CROSSING_BOTH
   nCrossingSide = nCrossingSide == RVSign::CROSSING_UNKNOWN ? RVSign::CROSSING_BOTH:
                  nCrossingSide;

   // Road background Rectangle limits
   int nBGRectTop = nRoadCenter - (int)(nRoadWidth / 2) - ROAD_LINES_GAP;
   if ((nCrossingSide == RVSign::CROSSING_LEFT) || (nCrossingSide == RVSign::CROSSING_BOTH))
   {
      nBGRectTop -= m_nCrossingLaneExtent ;
   }
   int nBGRectHeight = nRoadWidth + (2*ROAD_LINES_GAP) + m_nCrossingLaneExtent + 1;
   if (nCrossingSide == RVSign::CROSSING_BOTH)
   {
      nBGRectHeight += m_nCrossingLaneExtent - 1;
   }

   CPen* pOldPen = dc.SelectObject(&penLines);
      // Paint Road background
      dc.FillSolidRect(nStart, nBGRectTop, nEnd - nStart, nBGRectHeight, COLOR_ROAD);
      // Paint Top border Lines
      dc.MoveTo(nStart, nRoadCenter - (nNextNbOfLanes * nLaneWidth));
      if ((nCrossingSide == RVSign::CROSSING_LEFT) || (nCrossingSide == RVSign::CROSSING_BOTH))
      {
         dc.LineTo(nStart + ROAD_LINES_GAP, nRoadCenter - (nNextNbOfLanes * nLaneWidth));
         dc.LineTo(nStart + ROAD_LINES_GAP, nRoadCenter - (nNextNbOfLanes * nLaneWidth) - m_nCrossingLaneExtent);
         dc.MoveTo(nEnd - ROAD_LINES_GAP - 1, nRoadCenter - (nNextNbOfLanes * nLaneWidth) - m_nCrossingLaneExtent);
         dc.LineTo(nEnd - ROAD_LINES_GAP - 1, nRoadCenter - (nNextNbOfLanes * nLaneWidth));
      }
      dc.LineTo(nEnd, nRoadCenter - (nNextNbOfLanes * nLaneWidth));

      // Paint Bottom Border Lines
      dc.MoveTo(nStart, nRoadCenter + (nNextNbOfLanes * nLaneWidth));
      if ((nCrossingSide == RVSign::CROSSING_RIGHT) || (nCrossingSide == RVSign::CROSSING_BOTH))
      {
         dc.LineTo(nStart + ROAD_LINES_GAP, nRoadCenter + (nNextNbOfLanes * nLaneWidth));
         dc.LineTo(nStart + ROAD_LINES_GAP, nRoadCenter + (nNextNbOfLanes * nLaneWidth) + m_nCrossingLaneExtent);
         dc.MoveTo(nEnd - ROAD_LINES_GAP - 1, nRoadCenter + (nNextNbOfLanes * nLaneWidth) + m_nCrossingLaneExtent);
         dc.LineTo(nEnd - ROAD_LINES_GAP - 1, nRoadCenter + (nNextNbOfLanes * nLaneWidth));
      }
      dc.LineTo(nEnd, nRoadCenter + (nNextNbOfLanes * nLaneWidth));
         
   dc.SelectObject(pOldPen);
};

void CAHRoadView::paintLaneIncrease(CDC& dc, int nStart, int nEnd, int nRoadCenter, int nRoadWidth, int nLaneWidth, int nPreviousNbOfLanes, int nNextNbOfLanes) // float fSizeOfCrossing
{
   // TOP LANE INCREASE
   int nPtsIdx = 0;
   CArray<CPoint, CPoint>     PolyPtsArray, PolyPtsLineArray;  // Arrays to contain the points of the polygon forming the Lane increase

   int TransitionToRoadRadius = nLaneWidth;
   int xPolyStart = nStart; // - (1.5 * nLaneWidth);
   int yPolyStart = nRoadCenter - (nPreviousNbOfLanes * nLaneWidth) + ROAD_LINES_GAP - 1;
   int xPolyEnd = nEnd ; // + (1.5 * nLaneWidth);
   int yPolyEnd = nRoadCenter - (nNextNbOfLanes * nLaneWidth) - 1;

   // Bottom upward arc points
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyStart, yPolyStart, TransitionToRoadRadius, PI / 4, ARC_UP, ARC_RIGHT, - ROAD_LINES_GAP);
   
   // TODO: int nTransitionIdx = nPtsIdx; // Keep it in memory to add a crossing road here if necessary

   // Top downward arc points
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyEnd, yPolyEnd, TransitionToRoadRadius, PI / 4, ARC_DOWN, ARC_LEFT, - ROAD_LINES_GAP);
   
   // Close the polygon
   PolyPtsArray.Add(CPoint(xPolyEnd, yPolyStart));

   // TODO: In this version, if there are both a crossing and a lane change, only the lane change is drawn.
   // Improvement: If there is also a crossing, add some points to the lane transition polygon for a small crossing road
   // if (fSizeOfCrossing != 0.0)
   // {
   //    PolyPtsArray.InsertAt(nTransitionIdx, CPoint(PolyPtsArray[nTransitionIdx].x, nRoadCenter - (int)(nRoadWidth / 2) - ROAD_LINES_GAP - m_nCrossingLaneExtent)); //m_nCrossingLaneExtent
   //    PolyPtsArray.InsertAt(nTransitionIdx + 1, CPoint(PolyPtsArray[nTransitionIdx + 1].x, nRoadCenter - (int)(nRoadWidth / 2) - ROAD_LINES_GAP - m_nCrossingLaneExtent));
   //    PolyPtsLineArray.Add(CPoint(xPolyEnd, yPolyStart));
   // }

   // Now Draw the filled polygon
   CPen* pOldPen = dc.SelectObject(&penRoad);
      CBrush* pOldBrush = dc.SelectObject(&brushRoad);
         dc.Polygon(PolyPtsArray.GetData(), nPtsIdx + 1);
      dc.SelectObject(pOldBrush);
   dc.SelectObject(pOldPen);

   // Add a road background in the middle
   dc.FillSolidRect(nStart, nRoadCenter - (nPreviousNbOfLanes * nLaneWidth) - ROAD_LINES_GAP, nEnd - nStart, nRoadWidth + (2*ROAD_LINES_GAP), COLOR_ROAD);
   // Draw the border and central lines
   pOldPen = dc.SelectObject(&penLines);
      dc.Polyline(PolyPtsLineArray.GetData(), nPtsIdx);
      dc.MoveTo(nStart, nRoadCenter);
      dc.LineTo(nEnd, nRoadCenter);
   dc.SelectObject(pOldPen);

   // BOTTOM LANE INCREASE
   nPtsIdx = 0;
   PolyPtsArray.RemoveAll();      // Clear the polygon arrays
   PolyPtsLineArray.RemoveAll();

   yPolyStart = nRoadCenter + (nPreviousNbOfLanes * nLaneWidth) - ROAD_LINES_GAP + 1;
   yPolyEnd = nRoadCenter + (nNextNbOfLanes * nLaneWidth) + 1;

   // Top downward arc points
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyStart, yPolyStart, TransitionToRoadRadius, PI / 4, ARC_DOWN, ARC_RIGHT, ROAD_LINES_GAP);
   
   // Bottom upward arc points 
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyEnd, yPolyEnd, TransitionToRoadRadius, PI / 4, ARC_UP, ARC_LEFT, ROAD_LINES_GAP);
  
   // Close the polygon
   PolyPtsArray.Add(CPoint(xPolyEnd, yPolyStart));

   // Draw the filled polygon
   pOldPen = dc.SelectObject(&penRoad);
      pOldBrush = dc.SelectObject(&brushRoad);
         dc.Polygon(PolyPtsArray.GetData(), nPtsIdx + 1);
      dc.SelectObject(pOldBrush);
   dc.SelectObject(pOldPen);

   // Draw the curved border line
   pOldPen = dc.SelectObject(&penLines);
      dc.Polyline(PolyPtsLineArray.GetData(), nPtsIdx);
   dc.SelectObject(pOldPen);
};

// Method to define an arc (PolyPtsArray) and a "border" arc (PolyPtsLineArray) starting at x=0 and stopping at the given angle
// Note: Angle must be in Radian ! Returns the number of points of the arc for a Polygon drawing.
// Example: Right downward arc      Right upward arc
//              x_                            |
//                 \                      x_ /
//                 |
// x is the starting point (xPolyRefPoint, yPolyRefPoint)
//
int CAHRoadView::ArcPoints(CArray<CPoint, CPoint>& PolyPtsArray, CArray<CPoint, CPoint>& PolyPtsLineArray, int xPolyRefPoint, int yPolyRefPoint, int nRadius, double fAngle, bool bIsArcUp, bool bIsArcRight, int nVerticalOffset)
{
   // Set Arc Direction by defining if there will be an addition or a substraction
   int nArcDirectionSign = bIsArcUp ? -1 : 1;

   int nPtsIdx = 0;
   if (bIsArcRight)     // For a Right Arc
   {
      for (int nDrawIdx = 0; nDrawIdx <= cos(fAngle) * nRadius; nDrawIdx++)  // cos(pi/4) = sqrt(2.0)/2.0)
      {
         PolyPtsArray.Add(CPoint(xPolyRefPoint + nDrawIdx,
            yPolyRefPoint + nArcDirectionSign * (-(int) sqrt((nRadius * nRadius) - ((float) nDrawIdx * (float) nDrawIdx)) + nRadius) + nVerticalOffset));
         PolyPtsLineArray.Add(CPoint(xPolyRefPoint + nDrawIdx,
            yPolyRefPoint + nArcDirectionSign * (-(int) sqrt((nRadius * nRadius) - ((float) nDrawIdx * (float) nDrawIdx)) + nRadius) ));
         nPtsIdx++;
      }
   }
   else                 // For a Left Arc
   {
      for (int nDrawIdx = (int)(cos(fAngle) * nRadius); nDrawIdx >= 0 ; nDrawIdx--)   // sqrt(2.0)/2.0)
      {
         PolyPtsArray.Add(CPoint(xPolyRefPoint - nDrawIdx,
            yPolyRefPoint + nArcDirectionSign * (-(int) sqrt((nRadius * nRadius) - ((float) nDrawIdx * (float) nDrawIdx)) + nRadius) + nVerticalOffset));
         PolyPtsLineArray.Add(CPoint(xPolyRefPoint - nDrawIdx,
            yPolyRefPoint + nArcDirectionSign * (-(int) sqrt((nRadius * nRadius) - ((float) nDrawIdx * (float) nDrawIdx)) + nRadius) ));
         nPtsIdx++;
      }
   }
   return nPtsIdx;
};

void CAHRoadView::paintLaneDecrease(CDC& dc, int nStart, int nEnd, int nRoadCenter, int nRoadWidth, int nLaneWidth, int nPreviousNbOfLanes, int nNextNbOfLanes) // float fSizeOfCrossing
{
   // Top lane decrease
   int nPtsIdx = 0;
   CArray<CPoint, CPoint>     PolyPtsArray, PolyPtsLineArray;

   int TransitionToRoadRadius = nLaneWidth;
   int xPolyStart = nStart; // - (1.5 * nLaneWidth);
   int yPolyStart = nRoadCenter - (nPreviousNbOfLanes * nLaneWidth) - 1;
   int xPolyEnd = nEnd ; // + (1.5 * nLaneWidth);
   int yPolyEnd = nRoadCenter - (nNextNbOfLanes * nLaneWidth) + ROAD_LINES_GAP - 1;

   // Define points of Top left Arc
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyStart, yPolyStart, TransitionToRoadRadius, PI / 4, ARC_DOWN, ARC_RIGHT, - ROAD_LINES_GAP);
  
   // Define points of Bottom right Arc
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyEnd, yPolyEnd, TransitionToRoadRadius, PI / 4, ARC_UP, ARC_LEFT, - ROAD_LINES_GAP);
   
   // Close the polygon
   PolyPtsArray.Add(CPoint(xPolyStart, PolyPtsArray[nPtsIdx-1].y));
   
   // Draw the filled polygon
   CPen* pOldPen = dc.SelectObject(&penRoad);
      CBrush* pOldBrush = dc.SelectObject(&brushRoad);
         dc.Polygon(PolyPtsArray.GetData(), nPtsIdx + 1);
      dc.SelectObject(pOldBrush);
   dc.SelectObject(pOldPen);

   // Road background
   dc.FillSolidRect(nStart, nRoadCenter - (nNextNbOfLanes * nLaneWidth) - ROAD_LINES_GAP, nEnd - nStart, nRoadWidth + (2*ROAD_LINES_GAP), COLOR_ROAD);
   // Border and central lines
   pOldPen = dc.SelectObject(&penLines);
      dc.Polyline(PolyPtsLineArray.GetData(), nPtsIdx);
      dc.MoveTo(nStart, nRoadCenter);
      dc.LineTo(nEnd, nRoadCenter);
   dc.SelectObject(pOldPen);

   // Bottom lane decrease
   nPtsIdx = 0;
   PolyPtsArray.RemoveAll();
   PolyPtsLineArray.RemoveAll();

   yPolyStart = nRoadCenter + (nPreviousNbOfLanes * nLaneWidth) + 1;
   yPolyEnd = nRoadCenter + (nNextNbOfLanes * nLaneWidth) - ROAD_LINES_GAP + 1;

   // Bottom upward Arc points 
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyStart, yPolyStart, TransitionToRoadRadius, PI / 4, ARC_UP, ARC_RIGHT, ROAD_LINES_GAP);

   // Top downward Arc points
   nPtsIdx += ArcPoints(PolyPtsArray, PolyPtsLineArray, xPolyEnd, yPolyEnd, TransitionToRoadRadius, PI / 4, ARC_DOWN, ARC_LEFT, ROAD_LINES_GAP);
   
   // Close the polygon
   PolyPtsArray.Add(CPoint(xPolyStart, PolyPtsArray[nPtsIdx-1].y));
  
   // Draw the filled polygon
   pOldPen = dc.SelectObject(&penRoad);
      pOldBrush = dc.SelectObject(&brushRoad);
         dc.Polygon(PolyPtsArray.GetData(), nPtsIdx + 1);
      dc.SelectObject(pOldBrush);
   dc.SelectObject(pOldPen);

   pOldPen = dc.SelectObject(&penLines);
      dc.Polyline(PolyPtsLineArray.GetData(), nPtsIdx);
   dc.SelectObject(pOldPen);
};


void CAHRoadView::paintArrow(CDC& dc, int xCenter, int yCenter, bool bLeftToRight, int nArrowWidthPixels)
{
   // bLeftToRight = true to paint a right arrow, else paint a left arrow
   CArray<CPoint, CPoint>     PolyArrowArray;

   int nArrowHeightPixels = (int)(nArrowWidthPixels / 2);

   //     _ _ _|\
   //    |_ _ _  >
   //          |/

   int nArrowDirectionSign = bLeftToRight ? 1 : -1;

   PolyArrowArray.Add(CPoint(xCenter - nArrowDirectionSign * (int)(nArrowWidthPixels / 2), yCenter - (int)(nArrowHeightPixels / 4)));
   PolyArrowArray.Add(CPoint(xCenter + nArrowDirectionSign * (int)(nArrowWidthPixels / 4), yCenter - (int)(nArrowHeightPixels / 4)));
   PolyArrowArray.Add(CPoint(xCenter + nArrowDirectionSign * (int)(nArrowWidthPixels / 4), yCenter - (int)(nArrowHeightPixels / 2)));
   PolyArrowArray.Add(CPoint(xCenter + nArrowDirectionSign * nArrowWidthPixels, yCenter));
   PolyArrowArray.Add(CPoint(xCenter + nArrowDirectionSign * (int)(nArrowWidthPixels / 4), yCenter + (int)(nArrowHeightPixels / 2)));
   PolyArrowArray.Add(CPoint(xCenter + nArrowDirectionSign * (int)(nArrowWidthPixels / 4), yCenter + (int)(nArrowHeightPixels / 4)));
   PolyArrowArray.Add(CPoint(xCenter - nArrowDirectionSign * (int)(nArrowWidthPixels / 2), yCenter + (int)(nArrowHeightPixels / 4)));
   
   CPen* pOldPen = dc.SelectObject(&penArrow);
      CBrush* pOldBrush = dc.SelectObject(&brushArrow);
         dc.Polygon(PolyArrowArray.GetData(), PolyArrowArray.GetSize());
      dc.SelectObject(pOldBrush);
   dc.SelectObject(pOldPen);

};

void CAHRoadView::paintSignPx(CDC& dc, const CRect& rectSigns, const TrafficSign::Sign theSign, UINT nSignParam, int nPosition)
{
   // Paint signs from RVSign vector using TrafficSign
   if (nPosition > rectSigns.left && nPosition < rectSigns.right)
   {
      int wSign       = rectSigns.Width() / 15;
      CRect rectSign(nPosition - wSign/2, rectSigns.top, nPosition + wSign/2, rectSigns.bottom);
      ts->draw(&dc, rectSign, theSign, nSignParam, 0, false);
   }
};

void CAHRoadView::paintSign(CDC& dc, const CRect& rectSigns, int nTopRoad, const TrafficSign::Sign theSign, UINT nSignParam, int nDistanceM, bool bLength, int nDistFromStart, int &xLast, int iPos, bool bIsSign) // const RVSign& sign
{
   // Paint signs from RVSign vector using TrafficSign
   int xSignCenter = rectSigns.left + MulDiv(nDistFromStart*100, rectSigns.Width(), m_nDisplayedLengthCM);
   if (xSignCenter > rectSigns.left && xSignCenter < rectSigns.right)
   {
      int wSignOrg = /*rectSigns.Width() / 15*/ rectSigns.Height();  ///< Size if first sign at position.
      int wSign = wSignOrg;   ///< Actual size, shrinked if not first sign at position.
      if (0 != iPos) {
         wSign = 8 * wSignOrg / 10;
      }
      int xSignCenterNew = xSignCenter;
      if (xLast + 3 * wSign / 10 > xSignCenter) {
         xSignCenterNew = xLast + 3 * wSign / 10;
      }
      xLast = xSignCenterNew + wSign / 2;
      CRect rectSign(xSignCenterNew - wSign, rectSigns.top, xSignCenterNew + wSign, rectSigns.top + wSign);
      if (nTopRoad > rectSigns.bottom) {
         /* Draw sign pole. */
         int iModeOld = dc.SetBkMode(TRANSPARENT);
         CPen pen(bIsSign ? PS_SOLID : PS_DOT, bIsSign ? rectSigns.Height() / 30 + 1 : 1, COLOR_SCALE);
         CPen *pPenOld = dc.SelectObject(&pen);
         dc.MoveTo(xSignCenter, nTopRoad);
         dc.LineTo(xSignCenterNew, rectSigns.bottom);
         if (bIsSign) {
            dc.LineTo(xSignCenterNew, MARGIN_TOP / 2);
         }
         dc.SelectObject(pPenOld);
         dc.SetBkMode(iModeOld);
      }
      ts->draw(&dc, rectSign, theSign, nSignParam, nDistanceM, bLength, false /*bIsSign*/);
   }
};


void CAHRoadView::paintSigns(CDC& dc, const CSize& sizeCanvas, int wCar)
{
   int x      = MARGIN_LEFT + wCar + CAR_ROAD_GAP;
   int hTotal = (int) (sizeCanvas.cy * ((VERTICAL_ROAD_EXTENT/100.0) / 4.0));
   int h      = (int) (hTotal * (3.0/4.0));
   int l      = sizeCanvas.cx - x - MARGIN_RIGHT;

   // Top signs rectangle
   int yTop = 0;
   CRect rectSignsTop(CPoint(x,yTop), CSize(l,h));
   rectSignsTop.top    += MARGIN_TOP;
   rectSignsTop.bottom -= ROAD_SIDE_GAP;

   // Bottom signs rectangle
   int yBottom = (int) (sizeCanvas.cy * (VERTICAL_ROAD_EXTENT/100.0) / 4.0) * 3;
   yBottom += (hTotal - h);
   CRect rectSignsBottom(CPoint(x,yBottom), CSize(l,h));
   rectSignsBottom.top    += MARGIN_TOP;

   // Paint Lanes signs on Top and Crossing signs on Bottom
   for (int i = (int) m_signs.GetSize();  i > 0;  i--)
   {
      int xLast = -INT_MAX;
//      paintSign(dc, rectSignsTop, hTotal, m_signs[i-1].getSignLanes(), m_signs[i-1].getSignLanesParam(), 9999, false,  m_signs[i-1].getDistanceToSignM(), xLast, 0);
//      xLast = -INT_MAX;
      paintSign(dc, rectSignsBottom, rectSignsBottom.bottom, m_signs[i-1].getSignCrossing(), 0, 0, false, m_signs[i-1].getDistanceToSignM(), xLast, 0);
   };

#if 0
   // Paint the sign corresponding to the Area above the Road (so that it is painted above the other signs)
   for(int nAreaIdx = 0; nAreaIdx < m_areas.GetSize(); nAreaIdx++)
   {      
      int xLast = -INT_MAX;
      paintSign(dc, rectSignsTop, hTotal, m_areas[nAreaIdx].getSign(), 0, 0, false, m_areas[nAreaIdx].getStart(), xLast, 0);
   }
#endif
   // Paint the Traffic Signs in the Area above the Road (so that it is painted above the other signs)
   int xLast = -INT_MAX;   // Maximum x extension of last sign graphics, in pixels.
   int distPrev = -INT_MAX;   // Position of the last sign, in cm.
   int iPos = 0;           // Index of sign at same position.
   for(int nAreaIdx = 0; nAreaIdx < m_tsAreas.GetSize(); nAreaIdx++)
   {      
      BOOL bDrawIt = FALSE;
      bool bIsFree = false;
      switch (m_tsAreas[nAreaIdx].getSign())
      {
         case TrafficSign::tsInvalid:                                                        continue;
         case TrafficSign::tsPedestrian:           bDrawIt = m_bShowTSPedestrian;            break;
         case TrafficSign::tsTrafficLight:         bDrawIt = m_bShowTSTrafficLight;          break;
         case TrafficSign::tsRightOfWay:           bDrawIt = m_bShowTSRightOfWayRoad;        break;
         case TrafficSign::tsPedestrianCrossing:   bDrawIt = m_bShowTSPedestrianCrosswalk;   break;
         case TrafficSign::tsPriorityCrossing:     bDrawIt = m_bShowTSRightOfWay;            break;
         case TrafficSign::tsGiveWay:              bDrawIt = m_bShowTSYield;                 break;
         case TrafficSign::tsUrbanAreaEnd:         bDrawIt = m_bShowTSEndOfTown;             break;
         case TrafficSign::tsCrossing:             bDrawIt = m_bShowTSIntersection;          break;
         case TrafficSign::tsFree:                 bDrawIt = m_bShowCustom; bIsFree = true;  break;
         default:                                  bDrawIt = m_bShowTSOther;                 break;
      }
      if (bDrawIt)
      {
         int dist = m_tsAreas[nAreaIdx].getStart();
         if (distPrev == dist) {
            iPos++;
         } else {
            distPrev = dist;
            iPos = 0;
         }
         if (!bIsFree)
         {
            paintSign(dc, rectSignsTop, hTotal, m_tsAreas[nAreaIdx].getSign(), m_tsAreas[nAreaIdx].getNumber(), m_tsAreas[nAreaIdx].getDistanceOrDuration(), m_tsAreas[nAreaIdx].isDuration(), dist, xLast, iPos , m_tsAreas[nAreaIdx].isRealSign()); // add ,true if grayed rectangles should be painted  
         }
         else
         {
            paintSign(dc, rectSignsTop, hTotal, TrafficSign::tsFree, 
               reinterpret_cast<unsigned int>(m_szCustomSignPath0.GetBuffer(m_szCustomSignPath0.GetLength())), m_tsAreas[nAreaIdx].getDistanceOrDuration(), m_tsAreas[nAreaIdx].isDuration(), dist, xLast, iPos);
         }
      }
   }
};

void CAHRoadView::paintProhibitedSigns(CDC& dc, const CRect& rectRoad, int nRoadCenter, int nRoadWidth, int nPosition, RVSign::ProhibitedSideType nProhibitedSide)
{
   int nSignHeight = 20;

   // Define the top signs rectangle
   int x    = rectRoad.left;
   int h    = nSignHeight;
   int l    = rectRoad.Width();

   // Top signs rectangle
   int yTop = nRoadCenter - (nRoadWidth / 2) - nSignHeight - m_nCrossingLaneExtent - ROAD_SIDE_GAP;
   CRect rectProhibitedTop(CPoint(x,yTop), CSize(l,h));
   //rectProhibitedTop.top    -= ROAD_SIDE_GAP; // -= MARGIN_TOP;
   //rectProhibitedTop.bottom -= ROAD_SIDE_GAP;

   // Bottom signs rectangle
   int yBottom = nRoadCenter + (nRoadWidth / 2) + m_nCrossingLaneExtent + ROAD_SIDE_GAP;
   CRect rectProhibitedBottom(CPoint(x,yBottom), CSize(l,h));

   if ((nProhibitedSide == RVSign::PROHIBITED_LEFT) || (nProhibitedSide == RVSign::PROHIBITED_BOTH))
   {
      paintSignPx(dc, rectProhibitedTop, TrafficSign::tsPrivate, 0, nPosition);
   }
   if ((nProhibitedSide == RVSign::PROHIBITED_RIGHT) || (nProhibitedSide == RVSign::PROHIBITED_BOTH))
   {
      paintSignPx(dc, rectProhibitedBottom, TrafficSign::tsPrivate, 0, nPosition);
   }
};

bool CAHRoadView::paintText(CDC& dc, const CRect& rectRoad, int nPosition, CString szLinkId, COLORREF color)
{
   nPosition -= 16;

   CFont* pOldFont = dc.SelectObject(&fontText);
      COLORREF OldColor = dc.SetTextColor(color);
         int nOldMode = dc.SetBkMode(TRANSPARENT);
            dc.TextOut(nPosition, ((rectRoad.top + rectRoad.bottom) / 2) - (16/2) - 1, szLinkId);
 	      dc.SetBkMode(nOldMode);
      OldColor = dc.SetTextColor(OldColor);
   dc.SelectObject(pOldFont);

   return true;
};

//////////////////
// Worker method
// To get Road infos along the MPP and fill RVSign and RVAreas for later drawing

void CAHRoadView::getPathInfos()
{
   // Get the attributes along the Horizon
   ADAS::HorizonContainer* ahc = context.ahc;
   ADAS::HorizonLinks&     links = ahc->getLinks();
   ADAS::HorizonAttributes&    pts = ahc->getAttributes();
	bool	bRightSideDrive = true;
   // Get the MPP
   std::vector<Uint32> mpp;
   links.getMostProbablePath(mpp);
	if (mpp.size() > 0)
	{
		UDAL::Link *root = context.dal->createLink(links.getLinkById(mpp.at(0)).getInternalId());
		if (root != NULL)
		{
			if (root->getDrivingSide() == 2)
			{
				bRightSideDrive = false;
			}
			context.dal->deleteLink(root);
		}
	}
   
   Uint32 n = pts.getSize();         // the number of Attribute points on the Horizon

   m_signs.RemoveAll();              // Clear the Signs and Areas vectors
   m_areas.RemoveAll();
   
   // RVSign parameters
   struct TrafficSignView::Hint hintLanes;
   struct TrafficSignView::Hint hintCrossing;
   hintLanes.sign                             = TrafficSign::tsInvalid;
   hintLanes.param                            = 0;
   hintCrossing.sign                          = TrafficSign::tsInvalid;
   hintCrossing.param                         = 0;
   float fSizeOfCrossing                      = 0.0;
   RVSign::CrossingSideType nCrossingSide     = RVSign::CROSSING_UNKNOWN;
   RVSign::ProhibitedSideType nProhibitedSide = RVSign::PROHIBITED_NONE;
   Uint32 nLinkId                             = 0;
   Sint32 nLinkLength                         = 0;

   // RVArea parameters
   struct TrafficSignView::Hint hintAreas;
   hintAreas.sign       = TrafficSign::tsInvalid;
   hintAreas.param      = 0;

   getStartNbOfLanes(mpp, pts);   // Get m_nStartNbOfLanes and m_bIsStartInTunnel / m_bIsStartInRoundabout (used here as bIsStartInArea)

   unsigned int nMaxLanes = 0;
   Sint32 nAreaStart = 0;
   Sint32 nAreaEnd = 0;
   bool bIsStartInArea = false;
   bool bFoundAreaContinuation = false;

   unsigned int nPreviousNbOfLanes = m_nStartNbOfLanes;
   unsigned int nLastFoundNbOfLanes = m_nStartNbOfLanes;

   Sint32 nPreviousDist    = 0;           // Float64 = 0.0

   for (Uint32 i = 0;  i < n;  i++)       // loop over the Attribute points on Horizon 
   {
      ADAS::HorizonAttribute ahat;        // to store the info structure of the current point on the Horizon
      Float64  fProbability;              // to store the Probability of the current point on the Horizon
	   
      // get info for the n'th nearest point on Horizon and store it in "fProbability" and "ahat"
      Sint32   nDistCM = pts.getNearest(i, &fProbability, &ahat);  
      Sint32   nDist = nDistCM / 100;
      // TODO: The distance returned in nDistCM is the shortest distance between the Start of the Horizon
      // and the attribute. This distance is not necessarely the distance on the MPP if multiple paths
      // lead to the attribute. The correct distance along the MPP has to be retrieved here.

      if (isAttributeOnMPP(&ahat, mpp) && (nDist > 0))  // checks if point is on MPP and fills the RVSign
	   {
         // We group all the found attributes by distance. So, if there are no more infos for the current nDist,
         // we can add the infos to the RVAreas and RVSign vectors now.
         if ((nDist != nPreviousDist) && (nPreviousDist != 0.0))
         {
            if (bFoundAreaContinuation)   // If the Tunnel/Roundabout area just continues here, then do nothing
            {
               bFoundAreaContinuation = false;
            }
            else  // If we found no Tunnel/Roundabout continuation, this means that the Tunnel/Roundabout end is on the previous link
            {
               // Check if we have Area infos to add
               if ((bIsStartInArea && (nAreaStart == 0)) || (nAreaStart != 0))
               {
                  nAreaEnd = nPreviousDist;
                  m_areas.Add(RVAreas(hintAreas.sign, nAreaStart, nAreaEnd, nMaxLanes));
                  // reset Area parameters
                  nAreaStart = 0;
                  nAreaEnd = 0;
                  nMaxLanes = 0;
                  bIsStartInArea = false;  // to avoid writing the starting Tunnel again
               }
            }
            // Check if we have Lane or Crossing infos to add
            if (((hintLanes.sign != TrafficSign::tsInvalid) || (hintCrossing.sign != TrafficSign::tsInvalid)))
            {  
               // There has to be a number of lanes defined at each crossing! If it is not the case, set it to the last found one.
               if (hintLanes.param == 0)
               {
                  hintLanes.param = nLastFoundNbOfLanes;
               }
               // Now add the attributes to the Sign array              
               m_signs.Add(RVSign(hintLanes.sign, hintLanes.param, hintCrossing.sign, fSizeOfCrossing, nPreviousDist, nCrossingSide, nProhibitedSide, nLinkId));
               if (TrafficSign::tsInvalid != hintLanes.sign) {
                  m_tsAreas.Add(RVAreas(hintLanes.sign, nPreviousDist, nPreviousDist, 0, hintLanes.param));
               }

               nLastFoundNbOfLanes = hintLanes.param;

               // reset Signs infos
               hintLanes.sign      = TrafficSign::tsInvalid;
               hintLanes.param     = 0;
               hintCrossing.sign   = TrafficSign::tsInvalid;
               fSizeOfCrossing     = 0.0;
               nCrossingSide       = RVSign::CROSSING_UNKNOWN;
               nProhibitedSide     = RVSign::PROHIBITED_NONE;
            }           
         }  // End of grouping by distance part

         switch (ahat.type)      // Check kind of attributes we get, look for interesting attributes
         {
            case ADAS::ahatNumberOfLanesFromSC:
               {
                  unsigned int nNew = ahat.info & 0xFFFFL;
                  //unsigned int nOld = (ahat.info & 0x7FFF0000) >> 16;
						hintLanes.sign = nNew == nPreviousNbOfLanes ? TrafficSign::tsInvalid : // Do not draw lanes sign if lanes nb is identical
												nNew > nPreviousNbOfLanes ? (bRightSideDrive ? TrafficSign::tsLanesInc : TrafficSign::tsLanesIncRight)
												: (bRightSideDrive ? TrafficSign::tsLanesDec : TrafficSign::tsLanesDecRight);
                  hintLanes.param = nNew;
                  
                  nPreviousNbOfLanes = nNew;

                  if (nNew > nMaxLanes)
                  {
                     nMaxLanes = nNew;
                     m_nMaxLanes = nMaxLanes;
                  }

                  nLinkId = ahat.nLinkId;
                  nLinkLength = ahat.nLengthCM / 100;

                  break;
               };
            case ADAS::ahatCrossingSame:
               hintCrossing.sign = TrafficSign::tsCrossing;
               fSizeOfCrossing = m_fCrossingWidthFactorSame;
               nCrossingSide = getCrossingSide(links, ahat.nLinkId, mpp, &nProhibitedSide); //  nNextLinkId
               nLinkId = ahat.nLinkId;
               break;

            case ADAS::ahatCrossingSmall:
               hintCrossing.sign = TrafficSign::tsPriorityCrossing;
               fSizeOfCrossing = m_fCrossingWidthFactorSmall;
               nCrossingSide = getCrossingSide(links, ahat.nLinkId, mpp, &nProhibitedSide);
               nLinkId = ahat.nLinkId;
               break;

            case ADAS::ahatCrossingBig:
               hintCrossing.sign = TrafficSign::tsGiveWay;
               fSizeOfCrossing = m_fCrossingWidthFactorBig;
               nCrossingSide = getCrossingSide(links, ahat.nLinkId, mpp, &nProhibitedSide);
               nLinkId = ahat.nLinkId;
               break;

            case ADAS::ahatTunnel:           // Here we define the area limits
            case ADAS::ahatRoundabout:
               hintAreas.sign = ahat.type == ADAS::ahatTunnel ? TrafficSign::tsTunnel : TrafficSign::tsRoundabout;
               bIsStartInArea = ahat.type == ADAS::ahatTunnel ? m_bIsStartInTunnel : m_bIsStartInRoundabout;
               if (nAreaStart == 0.0)
               {
                  if (!bIsStartInArea)
                  {
                     nAreaStart = nDist;
                  }
               }
               bFoundAreaContinuation = true;
               break;

         } // end of switch

         nPreviousDist = nDist;

      } // end of isAttributeOnMPP (on MPP) condition check
   } // end of loop on Horizon points

   // SIGNS: If we found a Crossing or lane change at the end, add it here
   if (((hintLanes.sign != TrafficSign::tsInvalid) || (hintCrossing.sign != TrafficSign::tsInvalid)))
   {  
      // There has to be a number of lanes defined at each crossing! If it is not the case, set it to the last found one.
      if (hintLanes.param == 0)
      {
         hintLanes.param = nLastFoundNbOfLanes;
      }
      // Now add the attributes to the Sign array              
      m_signs.Add(RVSign(hintLanes.sign, hintLanes.param, hintCrossing.sign, fSizeOfCrossing, nPreviousDist, nCrossingSide, nProhibitedSide, nLinkId));
      if (TrafficSign::tsInvalid != hintLanes.sign) {
         m_tsAreas.Add(RVAreas(hintLanes.sign, nPreviousDist, nPreviousDist, 0, hintLanes.param));
      }
   } else {
      // fix 2006/04/03: Create a last entry until the end of the last segment to complete the Horizon to be drawn
      m_signs.Add(RVSign(TrafficSign::tsInvalid, hintLanes.param, TrafficSign::tsInvalid, 0.0, nPreviousDist + nLinkLength, nCrossingSide, nProhibitedSide, nLinkId));
   }
   // AREAS: If we didn't reach the End of the Area, then add the area info to the Area array
   if (((nAreaStart != 0) || (bIsStartInArea && bFoundAreaContinuation && (nAreaStart == 0)) ) && (nAreaEnd == 0.0))
   {
      m_areas.Add(RVAreas(hintAreas.sign, nAreaStart, nAreaEnd, nMaxLanes));
   }

   RVSign *pSignBegin = m_signs.GetData();
   RVSign *pSignEnd = pSignBegin + m_signs.GetCount();   // Must point behind the last element!
   std::sort(pSignBegin, pSignEnd, RVSign::compareByDistance);

   RVAreas *pAreaBegin = m_areas.GetData();
   RVAreas *pAreaEnd = pAreaBegin + m_areas.GetCount();  // Must point behind the last element!
   std::sort(pAreaBegin, pAreaEnd);

   m_tsAreas.Append(m_areas);

   Invalidate();
};

int CAHRoadView::getLanes(ADAS::HorizonAttributes &attrs, ADAS::HorizonAttribute &ahat)
{
   // FIXME: If ahat is at end of link, check number of lanes at next link!
   std::vector<ADAS::HorizonAttribute> attrsLane;
   attrs.getLinkAttributes(ahat.nLinkId, attrsLane, ADAS::ahatADASNumberOfLanes);
   if (1 != attrsLane.size()) {
      return 1;   // For lack of better knowledge.
   }
   int nLanes = attrsLane[0].info;
   attrsLane.clear();
   attrs.getLinkAttributes(ahat.nLinkId, attrsLane, ADAS::ahatRightWay);
   if (1 == attrsLane.size()) {
      // One way road - maybe motorway carriageway ot something similar.
      // In any case, we don't consider the opposing lanes.
      return nLanes;
   }
   attrsLane.clear();
   attrs.getLinkAttributes(ahat.nLinkId, attrsLane, ADAS::ahatADASOppositeNumberOfLanes);
   if (1 == attrsLane.size()) {
      nLanes += 64 * attrsLane[0].info;   // Traffic sign coding.
   }
   return nLanes;
}

// Get the Traffic Signs on the MPP and store them in an Area structure
void CAHRoadView::getTSAreas()
{
   // Get the attributes along the Horizon
   ADAS::HorizonContainer* ahc = context.ahc;
   ADAS::HorizonLinks&     links = ahc->getLinks();
   ADAS::HorizonAttributes&    pts = ahc->getAttributes();

   // Get the MPP
   std::vector<Uint32> mpp;
   links.getMostProbablePath(mpp);
   
   Uint32 n = pts.getSize();         // the number of Attribute points on the Horizon

   // RVSign parameters
   struct TrafficSignView::Hint hintTS;

   hintTS.sign                             = TrafficSign::tsInvalid;
   hintTS.param                            = 0;
   hintTS.bIsSign                          = true;

   for (Uint32 i = 0;  i < n;  i++)       // loop over the Attribute points on Horizon 
   {
      ADAS::HorizonAttribute ahat;        // to store the info structure of the current point on the Horizon
      Float64  fProbability;              // to store the Probability of the current point on the Horizon
	   
      // get info for the n'th nearest point on Horizon and store it in "fProbability" and "ahat"
      Sint32   nDistCM = pts.getNearest(i, &fProbability, &ahat);  
      Sint32   nDist = nDistCM / 100;
      // TODO: The distance returned in nDistCM is the shortest distance between the Start of the Horizon
      // and the attribute. This distance is not necessarely the distance on the MPP if multiple paths
      // lead to the attribute. The correct distance along the MPP has to be retrieved here.

      int nAreaStart = 0;
      int nAreaEnd = 0;

      // Extract the Traffic Sign Information coded in the Attribute info with the TrafficSignInfo enum
      union ADAS::TrafficSignInfo tsi;
      tsi.unit = ahat.info;

      if (isAttributeOnMPP(&ahat, mpp) && (nDist > 0))
      {
         nAreaStart = nDist;
         nAreaEnd = nAreaStart;
         int nDistanceOrDuration = 0;
         bool bDuration = false;

         if (tsi.bits.m_validityFlag == ADAS::TrafficSignValidityFlag::tsValidityStart)
         {
            //nAreaStart += ahat.nLengthCM / 100;
            nDistanceOrDuration = ahat.nLengthCM / 100;
            bDuration = false;
         }
         if (tsi.bits.m_validityFlag == ADAS::TrafficSignValidityFlag::tsValidityDuration)
         {
            nAreaEnd += ahat.nLengthCM / 100;
            nDistanceOrDuration = ahat.nLengthCM / 100;
            bDuration = true;
         }

         switch (ahat.type)      // Check kind of attributes we get, look for interesting attributes
         {
            case ADAS::ahatTSPedestrianXing:
               hintTS.sign  = TrafficSign::tsPedestrian;
               //hintTS.param = tsi.bits.m_nNumber;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));             
               //nLinkId = ahat.nLinkId;
               break;
            case ADAS::ahatTSPedestrianCrosswalk:
               hintTS.sign = TrafficSign::tsPedestrianCrossing;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
            case ADAS::ahatTSTrafficLight:
               hintTS.sign  = TrafficSign::tsTrafficLight;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, 0, false, false));
               break;
            case ADAS::ahatTSTrafficLightSign:
               hintTS.sign = TrafficSign::tsTrafficLight;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRightofWayRoad:
               hintTS.sign  = TrafficSign::tsRightOfWay;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRightOfWayCrossing:
               hintTS.sign = TrafficSign::tsPriorityCrossing;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSEndOfTown:
               hintTS.sign = TrafficSign::tsUrbanAreaEnd;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSEqualIntersection:
               hintTS.sign = TrafficSign::tsCrossing;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSYield:
               hintTS.sign = TrafficSign::tsGiveWay;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSStop:
               hintTS.sign = TrafficSign::tsStop;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSWarning:
               hintTS.sign = TrafficSign::tsWarning;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSharpCurveLeft:
               hintTS.sign = TrafficSign::tsLeftTurn;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSharpCurveRight:
               hintTS.sign = TrafficSign::tsRightTurn;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSCurveLeft:
               hintTS.sign = TrafficSign::tsSCurveLeft;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSCurveRight:
               hintTS.sign = TrafficSign::tsSCurveRight;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSUnevenRoad:
               hintTS.sign = TrafficSign::tsUnevenRoad;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSIcyRoad:
               hintTS.sign = TrafficSign::tsIcyRoad;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSlipperyRoad:
               hintTS.sign = TrafficSign::tsSlipperyRoad;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSFallingRocks:
               hintTS.sign = TrafficSign::tsFallingRocks;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRoadNarrowingLeft:
               hintTS.sign = TrafficSign::tsRoadNarrowingLeft;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRoadNarrowingRight:
               hintTS.sign = TrafficSign::tsRoadNarrowingRight;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRoadNarrowingBothSides:
               hintTS.sign = TrafficSign::tsRoadNarrowingBothSides;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSTrafficCongestion:
               hintTS.sign = TrafficSign::tsTrafficCongestion;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSAnimals:
               hintTS.sign = TrafficSign::tsAnimals;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSChildren:
               hintTS.sign = TrafficSign::tsChildren;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSOvertakeCC:
               hintTS.sign = ahat.bIsStart ? TrafficSign::tsOvertakeAllowed : TrafficSign::tsOvertakeProhibited;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSOvertakeTC:
               hintTS.sign = ahat.bIsStart ? TrafficSign::tsOvertakeTCAllowed : TrafficSign::tsOvertakeTCProhibited;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSEndOfAllProhibitions:
               hintTS.sign = TrafficSign::tsEndOfAllProhibitions;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSEndPriorityRoad:
               hintTS.sign = TrafficSign::tsEndPriorityRoad;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRailwayCrossingGates:
               hintTS.sign = TrafficSign::tsRailwayCrossingGates;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRailwayCrossingNoGates:
               hintTS.sign = TrafficSign::tsRailwayCrossingNoGates;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSTramway:
               hintTS.sign = TrafficSign::tsTramway;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRailwayCrossing:
               hintTS.sign = TrafficSign::tsRailwayCrossing;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSCompulsoryRoundabout:
               hintTS.sign = TrafficSign::tsCompulsoryRoundabout;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSCrossWind:
               hintTS.sign = TrafficSign::tsCrossWind;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSAccidentHazard:
               hintTS.sign = TrafficSign::tsAccidentHazard;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSRiskOfGrounding:
               hintTS.sign = TrafficSign::tsRiskOfGrounding;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSPriorityOncomingTraffic:
               hintTS.sign = TrafficSign::tsPriorityOncomingTraffic;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSYieldOncomingTraffic:
               hintTS.sign = TrafficSign::tsYieldOncomingTraffic;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSteepUphill:
               hintTS.sign = TrafficSign::tsSlope;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, tsi.bits.m_nNumber, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSteepDownhill:
               hintTS.sign = TrafficSign::tsSlopeNeg;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, tsi.bits.m_nNumber, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSpeedLimit:
               hintTS.sign = ahat.bIsStart ? TrafficSign::tsSpeedLimit : TrafficSign::tsSpeedLimitEnd;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, tsi.bits.m_nNumber, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatTSSignLanes:
               {
                  hintTS.sign = TrafficSign::tsLanes;
                  int nCurLanes = getLanes(pts, ahat);
                  m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, (1 == (nCurLanes & 0x3f)) ? (2 + (nCurLanes & ~0x3f)) : nCurLanes - 1, nDistanceOrDuration, bDuration, true));
               }
               break;
            case ADAS::ahatTSSignExtraLaneLeft:
               {
                  hintTS.sign = TrafficSign::tsLanesInc;
                  int nCurLanes = getLanes(pts, ahat) & 0x3f;
                  m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, nCurLanes + 1, nDistanceOrDuration, bDuration, true));
               }
               break;
            case ADAS::ahatTSSignExtraLaneRight:
               {
                  hintTS.sign = TrafficSign::tsLanesIncRight;
                  int nCurLanes = getLanes(pts, ahat) & 0x3f;
                  m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, nCurLanes + 1, nDistanceOrDuration, bDuration, true));
               }
               break;
            case ADAS::ahatTSSignLaneMergeLeft:
               {
                  hintTS.sign = TrafficSign::tsLanesDec;
                  int nCurLanes = getLanes(pts, ahat) & 0x3f;
                  m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, (1 == nCurLanes) ? 1 : nCurLanes - 1, nDistanceOrDuration, bDuration, true));
               }
               break;
            case ADAS::ahatTSSignLaneMergeRight:
               {
                  hintTS.sign = TrafficSign::tsLanesDecRight;
                  int nCurLanes = getLanes(pts, ahat) & 0x3f;
                  m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, (1 == nCurLanes) ? 1 : nCurLanes - 1, nDistanceOrDuration, bDuration, true));
               }
               break;
            case ADAS::ahatTSSignLaneMergeCenter:
               hintTS.sign = TrafficSign::tsLanesDecCenter;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, getLanes(pts, ahat) - 1, nDistanceOrDuration, bDuration, true));
               break;
            case ADAS::ahatCustom3:
               hintTS.sign = TrafficSign::tsFree;
               m_tsAreas.Add(RVAreas(hintTS.sign, nAreaStart, nAreaEnd, m_nMaxLanes, 0, nDistanceOrDuration, bDuration, false));
               break;
            default:
               break;
         }
      }
   }
   // Sort traffic signs by:
   // * position,
   // * sign itself,
   // * additional information.
   RVAreas *pBegin = m_tsAreas.GetData();
   RVAreas *pEnd = pBegin + m_tsAreas.GetCount();  // Must point behind the last element!
   std::sort(pBegin, pEnd);

   // Then eliminate duplicate signs (same sign may be posted e.g. left and right or over multiple lanes).
   int iSign = 0;
   while (iSign < m_tsAreas.GetCount() - 1) { // At most the last but one entry.
      if (m_tsAreas[iSign].getStart() == m_tsAreas[iSign + 1].getStart()
         && m_tsAreas[iSign].getSign() == m_tsAreas[iSign + 1].getSign()) {
            // We don't expect differences in other properties.
            m_tsAreas.RemoveAt(iSign + 1);
      } else {
         iSign++;
      }
   }
};


bool CAHRoadView::getStartNbOfLanes(std::vector<Uint32>& mpp, ADAS::HorizonAttributes&  pts)
{
   Uint32  n = pts.getSize();
   m_bIsStartInTunnel = false;
   m_bIsStartInRoundabout = false;

   // The information about the root link is stored in the attributes of the start node which is behind us.
   // Therefore, we have to search in the negative distance values for the Nb of Lanes attribute of the Root link
   //while((nDist < 0) && (i > 0))
   for (int i = n - 1; i > 0; i--)
   {
      //i--;
      ADAS::HorizonAttribute ahat;   // to store the info structure of the current point on Horizon
      Float64          fProbability;
      Sint32           nDistCM = pts.getNearest(i, &fProbability, &ahat);  

      if ((isAttributeOnMPP(&ahat, mpp)) && (fProbability != 0) && (nDistCM < 0))
      {
         switch(ahat.type)
         {
            case ADAS::ahatNumberOfLanesFromSC:    // Set the start number of Lanes
               m_nStartNbOfLanes = ahat.info & 0xFFFFL;
               break;
            case ADAS::ahatTunnel:           // detect if we are in a Tunnel
               m_bIsStartInTunnel = true;
               break;
            case ADAS::ahatRoundabout:       // detect if we are on a Roundabout
               m_bIsStartInRoundabout = true;
               break;
         }
      }
   }

   return true;
   
};

// Returns the crossing sides between a parent link whose ID is provided and its Child links excluding the next link on the MPP
RVSign::CrossingSideType CAHRoadView::getCrossingSide(ADAS::HorizonLinks& links, Uint32 nCurrentLinkId, std::vector<Uint32>& mpp, RVSign::ProhibitedSideType* nProhibitedSide) // Uint32 nNextLinkId
{
   ADAS::HorizonLink &currentLink = links.getLinkById(nCurrentLinkId);
   RVSign::CrossingSideType nCrossingSide = RVSign::CROSSING_UNKNOWN;

   for (int i = 0; i < currentLink.getChilds(); i++)
   {
      ADAS::HorizonLink& child = links.getLinkById(currentLink.getChild(i));

      if (!isIdOnMPP(child.getId(), mpp)
         && memcmp(child.getInternalId(), currentLink.getInternalId(), currentLink.getInternalIdSize()))  // Ignore the link if it's the next link on the MPP, or if it's a U-turn.
      {
         //ADAS::HorizonLink& child = links.getLinkById(nChildId);
         int nChildTurnAngle = child.getParentTurnAngleDegreesById(nCurrentLinkId);
         if (nChildTurnAngle < 0)  // Left Turn
         {
            nCrossingSide = nCrossingSide == RVSign::CROSSING_LEFT ? RVSign::CROSSING_LEFT:
                            nCrossingSide == RVSign::CROSSING_UNKNOWN ? RVSign::CROSSING_LEFT:
                            nCrossingSide == RVSign::CROSSING_RIGHT ? RVSign::CROSSING_BOTH:
                            RVSign::CROSSING_BOTH;

            // Determine if this link is prohibited (wrong-way)
            if (child.getProbability() == 0.0)
            {
               *nProhibitedSide = *nProhibitedSide == RVSign::PROHIBITED_RIGHT ? RVSign::PROHIBITED_BOTH:
                                  *nProhibitedSide == RVSign::PROHIBITED_BOTH ? RVSign::PROHIBITED_BOTH:
                                   RVSign::PROHIBITED_LEFT;
            }
         }
         if (nChildTurnAngle > 0)  // Right Turn
         {
            nCrossingSide = nCrossingSide == RVSign::CROSSING_LEFT ? RVSign::CROSSING_BOTH:
                            nCrossingSide == RVSign::CROSSING_UNKNOWN ? RVSign::CROSSING_RIGHT:
                            nCrossingSide == RVSign::CROSSING_RIGHT ? RVSign::CROSSING_RIGHT:
                            RVSign::CROSSING_BOTH;

            // Determine if this link is prohibited (wrong-way)
            if (child.getProbability() == 0.0)
            {
               *nProhibitedSide = *nProhibitedSide == RVSign::PROHIBITED_LEFT ? RVSign::PROHIBITED_BOTH:
                                  *nProhibitedSide == RVSign::PROHIBITED_BOTH ? RVSign::PROHIBITED_BOTH:
                                   RVSign::PROHIBITED_RIGHT;
            }
         }

      };
   };

   return nCrossingSide;
};

// returns Current/ADAS or Expected speed on link with provided Id - added by sdrehe to account for ADAS Speed Limits
Uint32 CAHRoadView::getSpeedOnLink(Uint32 nLinkId, ADAS::HorizonAttributes&  pts)
{
   Uint32 nCurrentSpeed = 0;
   Uint32 nExpectedSpeed = 0;

   // Puts the attributes of the Horizon in the ahat vector
   std::vector<ADAS::HorizonAttribute> ahat;    
   pts.getLinkAttributes(nLinkId, ahat);

   // Look for speed attributes
   for (Uint32 i = 0;  i < ahat.size();  i++)
   {
      switch(ahat[i].type)
         {
            case ADAS::ahatCurrentSpeed:
               nCurrentSpeed = ahat[i].info;
               break;
            case ADAS::ahatExpectedSpeedFromSC:
               nExpectedSpeed = ahat[i].info;
               break;
         }
   };
   // If the Current/ADAS Speed Limit is not defined take the Expected Speed Limit (from the Speed Cat)
   Uint32 sp = (nCurrentSpeed != 0) ? nCurrentSpeed : nExpectedSpeed;
   m_bIsCurrentSpeed = (nCurrentSpeed != 0) ? true : false;

   return sp;
};


// Check if attribute is on MPP
bool CAHRoadView::isAttributeOnMPP(ADAS::HorizonAttribute* pt, std::vector<Uint32>& mpp)
{
   bool bIn = false;
   for (Uint32 i = 0;  !bIn && i < mpp.size();  i++)
   {
      bIn = pt->nLinkId == mpp[i];
   };
   if (!bIn)
   {
      return false;
   }
   else
	{
       return true;
   };
};


// Check if the link with given ID is on MPP
bool CAHRoadView::isIdOnMPP(Uint32 nLinkId, std::vector<Uint32>& mpp)
{
   bool bIn = false;
   for (Uint32 i = 0;  !bIn && i < mpp.size();  i++)
   {
      bIn = nLinkId == mpp[i];
   };
   if (!bIn)
   {
      return false;
   }
   else
	{
       return true;
   };
};


void CAHRoadView::showPreferencesDialog()
{
   CPreferencesDialog dlg;
   (Preferences &) dlg = (Preferences &) *this;

   if (IDOK != dlg.DoModal()) {
      return;
   }
   (Preferences &) *this = (Preferences &) dlg;

#pragma warning(disable: 4800)   // Assign BOOL to bool.
   setProfileInt("Max Lanes", m_nLaneWidthFactor);
   setProfileBool("Show Tunnels", m_bShowTunnels);
   setProfileBool("Show Roundabouts", m_bShowRoundabouts);
   setProfileBool("Show Speed", m_bShowSpeed);
   setProfileBool(_T("Show TS Pedestrian"), m_bShowTSPedestrian);
   setProfileBool(_T("Show TS PedestrianCrosswalk"), m_bShowTSPedestrianCrosswalk);
   setProfileBool(_T("Show TS TrafficLight"), m_bShowTSTrafficLight);
   setProfileBool(_T("Show TS TrafficLightSign"), m_bShowTSTrafficLightSign);
   setProfileBool(_T("Show TS RightOfWay"), m_bShowTSRightOfWay);
   setProfileBool(_T("Show TS RightOfWayRoad"), m_bShowTSRightOfWayRoad);
   setProfileBool(_T("Show TS Yield"), m_bShowTSYield);
   setProfileBool(_T("Show TS EndOfTown"), m_bShowTSEndOfTown);
   setProfileBool(_T("Show TS Intersection"), m_bShowTSIntersection);
   setProfileBool(_T("Show Other Signs"), m_bShowTSOther);
   setProfileBool(_T("Show Custom"), m_bShowCustom);
   setProfileBool(_T("Auto-Scale"), m_bAutoScale);
   setProfileInt(_T("City In Scale"), m_nCityInScale / 100);
   setProfileInt(_T("City Out Scale"), m_nCityOutScale / 100);
   setProfileBool(_T("Debug") ,m_bDebug);
#pragma warning(default: 4800)   // Assign BOOL to bool.
}

void CAHRoadView::OnConfigure()
{
   showPreferencesDialog();
}

void CAHRoadView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
   CMenu menu;
   menu.LoadMenu(IDR_MENU_CONTEXT);
   CMenu *pPopup = menu.GetSubMenu(0);
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
}


//////////////////////
// Static data members
// Define the section name in the INI file

const TCHAR* CAHRoadView::INI_SECTION = _T("Road View");


///////////////////////////////////////////////
// Factory

extern "C" __declspec(dllexport) CEHPlugIn* createPlugIn(const CEHPlugIn::Context& context)
{
   return new CAHRoadView(context);
};

