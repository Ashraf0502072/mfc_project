/** 
 * @file    PathInfo.h local copy of DPPSign.h
 * @brief   Class that describes DPP sign on most probable path.
 * @author  Sinisa Durekovic
 * @version 0.1
 * @date    23.06.2004.
 */


#pragma once

class PathInfo
{
public: // Constructor/Destructor
   
   PathInfo()
   {
      m_sign             = TrafficSign::tsInvalid;
      m_nSignParam       = 0;
      m_nDistanceToSignM = 0;
   };


   PathInfo(enum TrafficSign::Sign sign, UINT nSignParam, int nDistanceToSignM)
   {
      m_sign             = sign;
      m_nSignParam       = nSignParam;
      m_nDistanceToSignM = nDistanceToSignM;
   };



public: // Getters

   enum TrafficSign::Sign getSign()            const { return m_sign;       };
   UINT                   getSignParam()       const { return m_nSignParam; };
   int                    getDistanceToSignM() const { return m_nDistanceToSignM; };


private: // Data Members
   
   enum TrafficSign::Sign m_sign;
   UINT                   m_nSignParam;
   int                    m_nDistanceToSignM;
};


