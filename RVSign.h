/** 
 * @file    RVSign.h
 * @brief   Class that describes Crossing and Lane number change Signs on the most probable path.
 * @author  Stéphane Dreher
 * @version 0.1
 * @date    07.01.2005.
 *
 * updated 18/03/2005: added Crossing Side info (Heading Change Support)
 */


#pragma once

class RVSign
{
public: // Constants

   /* Crossing Side Values */
   enum CrossingSideType {
      CROSSING_UNKNOWN, // 0
      CROSSING_LEFT,    // 1
      CROSSING_RIGHT,   // 2
      CROSSING_BOTH     // 3    
   };

   /* Prohibited Side Values */
   enum ProhibitedSideType {
      PROHIBITED_NONE,  // 0
      PROHIBITED_LEFT,  // 1
      PROHIBITED_RIGHT, // 2
      PROHIBITED_BOTH   // 3    
   };

public: // Constructor/Destructor
   
   RVSign()
   {
      m_signLanes          = TrafficSign::tsInvalid;
      m_nSignLanesParam    = 0;
      m_signCrossing       = TrafficSign::tsInvalid;
      m_fSizeOfCrossing    = 0.0;
      m_nDistanceToSignM   = 0;
      m_nCrossingSide      = CROSSING_UNKNOWN;
      m_nProhibitedSide    = PROHIBITED_NONE;
      m_nLinkId            = 0;
   };


   RVSign(enum TrafficSign::Sign signLanes, UINT nSignLanesParam, enum TrafficSign::Sign signCrossing, float fSizeOfCrossing, int nDistanceToSignM, CrossingSideType nCrossingSide, ProhibitedSideType nProhibitedSide, Uint32 nLinkId)
   {
      m_signLanes        = signLanes;
      m_nSignLanesParam  = nSignLanesParam;
      m_signCrossing     = signCrossing;
      m_fSizeOfCrossing  = fSizeOfCrossing;
      m_nDistanceToSignM = nDistanceToSignM;
      m_nCrossingSide    = nCrossingSide;
      m_nProhibitedSide  = nProhibitedSide;
      m_nLinkId          = nLinkId;
   };

public: // Getters

   enum TrafficSign::Sign  getSignLanes()            const { return m_signLanes;       };
   UINT                    getSignLanesParam()       const { return m_nSignLanesParam; };
   enum TrafficSign::Sign  getSignCrossing()         const { return m_signCrossing;       };
   float                   getSizeOfCrossing()       const { return m_fSizeOfCrossing;  };
   int                     getDistanceToSignM()      const { return m_nDistanceToSignM; };
   enum CrossingSideType   getCrossingSide()         const { return m_nCrossingSide; };
   enum ProhibitedSideType getProhibitedSide()       const { return m_nProhibitedSide; };
   Uint32                  getLinkId()               const { return m_nLinkId; };

   static bool compareByDistance(const RVSign &left, const RVSign &right) {
      return left.m_nDistanceToSignM < right.m_nDistanceToSignM;
   }

private: // Data Members
   
   enum TrafficSign::Sign  m_signLanes;
   UINT                    m_nSignLanesParam;
   enum TrafficSign::Sign  m_signCrossing;
   float                   m_fSizeOfCrossing;
   int                     m_nDistanceToSignM;
   enum CrossingSideType   m_nCrossingSide;
   enum ProhibitedSideType m_nProhibitedSide;
   Uint32                  m_nLinkId;

};


