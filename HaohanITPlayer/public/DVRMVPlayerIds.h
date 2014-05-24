//*****************************************************************************
//
// Sonic Playback Engine On Intel Media Vault Platform 
// Copyright (C) Haohanit.  All Rights Reserved.
//
// FileName:            DVRMVPlayerIds.h 
//
// Abstract:            Dispatch ID definitions for all public Interfaces
//
//*****************************************************************************

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////
#define DISPID_DELTA                            50


//////////////////////////////////////////////////////////////////////
// IDVRMVPlayer
//////////////////////////////////////////////////////////////////////
#define DISPID_IMVPLAYER_BASE                   0
#define DISPID_IMVPLAYER_URL                    (DISPID_IMVPLAYER_BASE + 1)
#define DISPID_IMVPLAYER_FULLSCREEN             (DISPID_IMVPLAYER_BASE + 2)
#define DISPID_IMVPLAYER_UIMODE                 (DISPID_IMVPLAYER_BASE + 3)
#define DISPID_IMVPLAYER_STATUS                 (DISPID_IMVPLAYER_BASE + 4)
#define DISPID_IMVPLAYER_PLAYSTATE              (DISPID_IMVPLAYER_BASE + 5)
#define DISPID_IMVPLAYER_VERSIONINFO            (DISPID_IMVPLAYER_BASE + 6)
#define DISPID_IMVPLAYER_CLOSE                  (DISPID_IMVPLAYER_BASE + 7)
#define DISPID_IMVPLAYER_CONTROLS               (DISPID_IMVPLAYER_BASE + 8)
#define DISPID_IMVPLAYER_SETTINGS               (DISPID_IMVPLAYER_BASE + 9)
#define DISPID_IMVPLAYER_CURRENTMEDIA           (DISPID_IMVPLAYER_BASE + 10)

//DVR Interfaces
#define DISPID_DVRPLAYER_OPENFILE				(DISPID_IMVPLAYER_BASE + 11)
#define DISPID_DVRPLAYER_LOGIN					(DISPID_IMVPLAYER_BASE + 12)
#define DISPID_DVRPLAYER_LOGOUT					(DISPID_IMVPLAYER_BASE + 13)
#define DISPID_DVRPLAYER_SETMEDIASERVER			(DISPID_IMVPLAYER_BASE + 14)
#define DISPID_DVRPLAYER_GETMEDIASERVER			(DISPID_IMVPLAYER_BASE + 15)
#define DISPID_DVRPLAYER_STARTMONITOR			(DISPID_IMVPLAYER_BASE + 16)
#define DISPID_DVRPLAYER_STOPMONITOR			(DISPID_IMVPLAYER_BASE + 17)
#define DISPID_DVRPLAYER_CAPPIC					(DISPID_IMVPLAYER_BASE + 18)
#define DISPID_DVRPLAYER_GETCAPTUREPATH			(DISPID_IMVPLAYER_BASE + 19)
#define DISPID_DVRPLAYER_SETCAPTUREPATH			(DISPID_IMVPLAYER_BASE + 20)
#define DISPID_DVRPLAYER_SHOWMETADATA			(DISPID_IMVPLAYER_BASE + 21)
#define DISPID_DVRPLAYER_SETWNDCHANNEL			(DISPID_IMVPLAYER_BASE + 22)
#define DISPID_DVRPLAYER_GET_FULLSCREEN			(DISPID_IMVPLAYER_BASE + 23)
#define DISPID_DVRPLAYER_SET_FULLSCREEN			(DISPID_IMVPLAYER_BASE + 24)
#define DISPID_DVRPLAYER_AddPlayFile			(DISPID_IMVPLAYER_BASE + 25)
#define DISPID_DVRPLAYER_ClosePlayFile			(DISPID_IMVPLAYER_BASE + 26)
#define DISPID_DVRPLAYER_PlayNextFile			(DISPID_IMVPLAYER_BASE + 27)
#define DISPID_DVRPLAYER_StartPlayback			(DISPID_IMVPLAYER_BASE + 28)
#define DISPID_DVRPLAYER_StopPlayback			(DISPID_IMVPLAYER_BASE + 29)
#define DISPID_DVRPLAYER_GET_DURATION			(DISPID_IMVPLAYER_BASE + 30)
#define DISPID_DVRPLAYER_GET_CURRENT_POSITION	(DISPID_IMVPLAYER_BASE + 31)
#define DISPID_DVRPLAYER_GET_DURATION_STRING	(DISPID_IMVPLAYER_BASE + 32)
#define DISPID_DVRPLAYER_GET_CURRENT_POSITION_STRING	(DISPID_IMVPLAYER_BASE + 33)
#define DISPID_DVRPLAYER_SET_POSITION			(DISPID_IMVPLAYER_BASE + 34)


//////////////////////////////////////////////////////////////////////
// IDVRMVControls
//////////////////////////////////////////////////////////////////////
#define DISPID_IMVCONTROLS_BASE                  (DISPID_IMVPLAYER_BASE + DISPID_DELTA)
#define DISPID_IMVCONTROLS_PLAY                  (DISPID_IMVCONTROLS_BASE + 1)
#define DISPID_IMVCONTROLS_STOP                  (DISPID_IMVCONTROLS_BASE + 2)
#define DISPID_IMVCONTROLS_PAUSE                 (DISPID_IMVCONTROLS_BASE + 3)
#define DISPID_IMVCONTROLS_FASTFORWARD           (DISPID_IMVCONTROLS_BASE + 4)
#define DISPID_IMVCONTROLS_FASTREVERSE           (DISPID_IMVCONTROLS_BASE + 5)
#define DISPID_IMVCONTROLS_CURRENTPOSITION       (DISPID_IMVCONTROLS_BASE + 6)
#define DISPID_IMVCONTROLS_CURRENTPOSITIONSTRING (DISPID_IMVCONTROLS_BASE + 7)
#define DISPID_IMVCONTROLS_ISAVAILABLE           (DISPID_IMVCONTROLS_BASE + 8)


//////////////////////////////////////////////////////////////////////
// IDVRMVSettings
//////////////////////////////////////////////////////////////////////
#define DISPID_IMVSETTINGS_BASE                 (DISPID_IMVCONTROLS_BASE + DISPID_DELTA)
#define DISPID_IMVSETTINGS_MUTE                 (DISPID_IMVSETTINGS_BASE + 1)
#define DISPID_IMVSETTINGS_VOLUME               (DISPID_IMVSETTINGS_BASE + 2)


//////////////////////////////////////////////////////////////////////
//  IDVRMVMedia
//////////////////////////////////////////////////////////////////////
#define DISPID_IMVMEDIA_BASE                   (DISPID_IMVSETTINGS_BASE + DISPID_DELTA)
#define DISPID_IMVMEDIA_IMAGESOURCEWIDTH       (DISPID_IMVMEDIA_BASE + 1)
#define DISPID_IMVMEDIA_IMAGESOURCEHEIGHT      (DISPID_IMVMEDIA_BASE + 2)
#define DISPID_IMVMEDIA_DURATION               (DISPID_IMVMEDIA_BASE + 3)
#define DISPID_IMVMEDIA_DURATIONSTRING         (DISPID_IMVMEDIA_BASE + 4)


//////////////////////////////////////////////////////////////////////
// DIID_IDVRMVPlayerCtrlEvents
//
// These are the events that will be fired from ActiveX Control itself
//////////////////////////////////////////////////////////////////////
#define DISPID_IMVPLAYERCTRLEVENTS_BASE              6500
#define DISPID_IMVPLAYERCTRLEVENTS_DOUBLECLICK      (DISPID_IMVPLAYERCTRLEVENTS_BASE + 1)
#define DISPID_IMVPLAYERCTRLEVENTS_KEYDOWN          (DISPID_IMVPLAYERCTRLEVENTS_BASE + 2)
#define DISPID_IMVPLAYERCTRLEVENTS_KEYPRESS         (DISPID_IMVPLAYERCTRLEVENTS_BASE + 3)
#define DISPID_IMVPLAYERCTRLEVENTS_HDCPERROR        (DISPID_IMVPLAYERCTRLEVENTS_BASE + 4)



//*****************************************************************************
//
//  End of File
//
//*****************************************************************************
