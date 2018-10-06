/* $Id: GTSort.h,v 1.1 2010/05/19 16:22:06 tl Exp $ */

#define TRUE 1
#define FALSE 0
#define NOTDEF -1
#define STRLEN 256

#define EOE          0xaaaaaaaa
#define HDRLENBYTES  52
#define HDRLENWORDS  26
#define HDRLENINTS   13
#define MAXLENINTS   525
#define LENEOVWORDS  2

#define NMOD  30
#define NGE   NMOD*10

/* GT event */
/* __contains both raw and */
/* __processed quantities! */
//-- means directly from hdr
typedef struct DigiOS_HEADER_struct
{
  unsigned short int      id;//--
  unsigned short int      board_id;//--
  int                     rawE;//--
  unsigned long long int  LEDts;//--
  double                  LEDtsAbs;// Absolute time stamp [sec]
  unsigned long long int  CFDts;//--
  unsigned long long int  PEAKts;//--
  int                     PreEnergy;//--
  int                     PostEnergy;//--
  unsigned short int      M1BeginSample;//--
  unsigned short int      M1EndSample;//--
  unsigned short int      M2BeginSample;//--
  unsigned short int      M2EndSample;//--
  unsigned short int      PeakSample;//--
  unsigned short int      BaseSample;//--
  unsigned short int      pu;//--
  int                     CFDp1;//--
  int                     CFDp2;//--
}  DigiOS_HEADER;

typedef struct DigiOS_TRACE_struct
{
  unsigned short int      Len;//--
  short int               Data[1025];//--
  float                   tLED[1025];
  int                     LEDp1;// ts before cross
  int                     LEDp2;// ts after cross
  float                   LEDts;// linear interp of ts
  double                  LEDtsRel;//Relative LEDts to ATLAS rf
  float                   tCFD[1025];
  int                     CFDp1;// same as for led
  int                     CFDp2;
  float                   CFDts;
  float                   PreEnergy;
  float                   PostEnergy;
  float                   Energy;
  float                   Smooth[1025];
  float                   RiseMin;
  float                   RiseMinTS10;
  float                   RiseMax;
  float                   RiseMaxTS90;
  float                   RiseTime;//10% - 90% of smoothed (5 pt) trace
}  DigiOS_TRACE;

typedef struct DigiOS_EVENT_struct
{
  int                     EnergyMult[2]; //
  float                   Energy[2]; // Raw energy value [channels]
  double                  EnergyTimeStamp[2]; // Real ts value [sec]
  float                   EnergyRiseTime[2]; //10% - 90% from Smooth (5pt) [ns]
  int                     XNearMult[2];
  float                   XNear[2];
  double                  XNearTimeStamp[2];
  float                   XNearRiseTime[2];
  int                     XFarMult[2];
  float                   XFar[2];
  double                  XFarTimeStamp[2];
  float                   XFarRiseTime[2];
  int                     RingMult[2];
  float                   Ring[2];
  double                  RingTimeStamp[2];
  float                   RingRiseTime[2];
} DigiOS_EVENT;
