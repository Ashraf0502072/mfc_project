/** 
 * @file    RVAreas.h
 * @brief   Class that contains Areas informations and coordinates to draw the areas along the road.
 * @author  Stéphane Dreher
 * @version 0.1
 * @date    20.01.2005.
 */


#pragma once

class RVAreas
{
public: // Constructor/Destructor
   
   RVAreas()
   {
      m_sign               = TrafficSign::tsInvalid;
      m_nStart             = 0;
      m_nEnd               = 0;
      m_nWidth             = 0;
      m_nNumber            = 0;
      m_nDistanceOrDuration = 0;
      m_bDuration          = true;
      m_bRealSign = false;
   };


   RVAreas(enum TrafficSign::Sign sign, int nStart, int nEnd, int nWidth, int nNumber = 0, int nDistanceOrDuration = 0, bool bDuration = true, bool bRealSign = false)
   {
      m_sign               = sign;
      m_nStart             = nStart;
      m_nEnd               = nEnd;
      m_nWidth             = nWidth;
      m_nNumber            = nNumber;
      m_nDistanceOrDuration = nDistanceOrDuration;
      m_bDuration          = bDuration;
      m_bRealSign          = bRealSign;
   };



public: // Getters

   enum TrafficSign::Sign getSign()                   const { return m_sign;           };
   Sint32                 getStart()                  const { return m_nStart;         };
   Sint32                 getEnd()                    const { return m_nEnd;           };
   unsigned int           getWidth()                  const { return m_nWidth;         };
   unsigned int           getNumber()                 const { return m_nNumber;        };
   int                    getDistanceOrDuration()     const { return m_nDistanceOrDuration; };
   bool                   isDuration()                const { return m_bDuration;      };
   bool                   isRealSign()                const { return m_bRealSign;      };

   bool operator<(const RVAreas &other) const
   {
      if (m_nStart < other.m_nStart) {
         return true;
      }
      if (m_nStart > other.m_nStart) {
         return false;
      }
      if (m_bRealSign > other.m_bRealSign) { // Reversed.
         return true;
      }
      if (m_bRealSign < other.m_bRealSign) {
         return false;
      }
      if (m_sign < other.m_sign) {
         return true;
      }
      if (m_sign > other.m_sign) {
         return false;
      }
      if (m_nEnd < other.m_nEnd) {
         return true;
      }
      if (m_nEnd > other.m_nEnd) {
         return false;
      }
      if (m_nWidth < other.m_nWidth) {
         return true;
      }
      if (m_nWidth > other.m_nWidth) {
         return false;
      }
      if (m_nNumber < other.m_nNumber) {
         return true;
      }
      if (m_nNumber > other.m_nNumber) {
         return false;
      }
      if (m_nDistanceOrDuration < other.m_nDistanceOrDuration) {
         return true;
      }
      if (m_nDistanceOrDuration > other.m_nDistanceOrDuration) {
         return false;
      }
      if (m_bDuration < other.m_bDuration) {
         return true;
      }
      if (m_bDuration > other.m_bDuration) {
         return false;
      }
      return false;
   }


private: // Data Members
   
   enum TrafficSign::Sign  m_sign;
   Sint32                  m_nStart;
   Sint32                  m_nEnd;
   unsigned int            m_nWidth;
   unsigned int            m_nNumber;
   int                     m_nDistanceOrDuration;
   bool                    m_bDuration;
   bool                    m_bRealSign;
};


