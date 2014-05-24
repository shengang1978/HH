//-----------------------------------------------------------------------------
// PConfigFeatures.cpp
// Copyright (c) 2004 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// PConfigFeatures.cpp : implementation file
//

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; MBCS internally; dangerous API for reading registry; no registry/sys folder/temp file 
#if defined _WIN32

//remove _T macro becuase of MBCS internally
#define _T(x)	x

#include "PConfigFeatures.h"

#include "FilePath.h"
#include "FileSpec.h"
#include "ProductFeaturesAuthoring.h"
#include "SonicException.h"
#include "UnicodeUtilities.h"
#include "WinRegistryUtils.h"

#define PCF_COMMON_SECTION_NAME _T("Common")
#define PCF_OTHER_SECTION_NAME _T("Other")
#define PCF_PRODUCT_SECTION_NAME _T("Product")
#define PCF_ALL_SECTION_NAME _T("All")

bool PConfigFeatures::FeatureEnabled(const char * pszSection, const char * pszFeature) const
{
	std::string csResult = m_pPconfig->get_value(pszSection, pszFeature);

	return (atoi(csResult.c_str()) != 0);
}

PConfigFeatures::PConfigFeatures(const FileSpec& inPath) : INHERITED(inPath),
	m_pPconfig(new RWPConfig()),
	m_nCommonFeatures(0x00000000),
	m_nOtherFeatures(0x00000000),
	m_nOtherFeatures2(0x00000000),
	m_nProductFeatures(0x00000000),
	m_bDockByDefault(false)
{
	if (!m_pPconfig->open(unicode::to_string(inPath.GetPath().c_str()).c_str()))
		throw SonicException(FeatureInitText::PConfigFileNotFound);

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DVD")))
		m_nCommonFeatures |= eDVD;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DVDPlusVR")))
		m_nCommonFeatures |= eDVDPlusVR;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DVDMinusVR")))
		m_nCommonFeatures |= eDVDMinusVR;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("VCD")))
		m_nCommonFeatures |= eVCD;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("SVCD")))
		m_nCommonFeatures |= eSVCD;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DVDPlusVRImport")))
		m_nCommonFeatures |= eDVDPlusVRImport;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DVDMinusVRImport")))
		m_nCommonFeatures |= eDVDMinusVRImport;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("QuicktimeImport")))
		m_nCommonFeatures |= eQuickTimeImport;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("AC3")))
		m_nCommonFeatures |= eAC3;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("PCM")))
		m_nCommonFeatures |= ePCM;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("MPEG2")))
		m_nCommonFeatures |= eMPEG2;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("JPEG")))
		m_nCommonFeatures |= eJPEG;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("MCE")))
		m_nCommonFeatures |= eMCE;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Analog Capture")))
		m_nCommonFeatures |= eAnalogCapture;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DV Capture")))
		m_nCommonFeatures |= eDVCapture;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Slideshow")))
		m_nCommonFeatures |= eSlideshow;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Trimming")))
		m_nCommonFeatures |= eTrimming;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Video Editing")))
		m_nCommonFeatures |= eVideoEditing;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Scene Detection")))
		m_nCommonFeatures |= eSceneDetection;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Chapter Point Editing")))
		m_nCommonFeatures |= eChapterPointEditing;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Direct to Disc")))
		m_nCommonFeatures |= eD2D;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Motion Menus")))
		m_nCommonFeatures |= eMotionMenus;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Record to Harddisc")))
		m_nCommonFeatures |= eRecordToHardDisc;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("DVD Editing")))
		m_nCommonFeatures |= eDVDEditing;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Multi-copy burn")))
		m_nCommonFeatures |= eMultiCopyBurn;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Registration")) ||
		FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("EnableRegistration")))
		m_nCommonFeatures |= eRegistration;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Sonic University")))
		m_nCommonFeatures |= eSonicUniversity;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("Disc Browser")))
		m_nCommonFeatures |= eDiscBrowser;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("GetTiVoFiles")))
		m_nCommonFeatures |= eGetTiVoFiles;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("ExposeRecordNow")))
		m_nCommonFeatures |= eExposeRecordNow;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("ExposeSBU")))
		m_nCommonFeatures |= eExposeSBU;

	if (FeatureEnabled(PCF_COMMON_SECTION_NAME,_T("MPEG1Only")))
	{
		m_nCommonFeatures &= ~eMPEG2;
		m_nCommonFeatures |= eMPEG1Only;
	}

	//other features
	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("IVT Support")))
		m_nOtherFeatures |= eIVTSupport;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Reencode Movie")))
		m_nOtherFeatures |= eReEncodeMovie;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Edit Segment")))
		m_nOtherFeatures |= eEditSegment;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Constant Quality VBR")))
		m_nOtherFeatures |= eConstantQualityVBR;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Segment Based Reencode")))
		m_nOtherFeatures |= eSegmentBasedReEncode;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("ReturnToUmbrella")))
		m_nOtherFeatures |= eReturnToUmbrella;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("DLT Output")))
		m_nOtherFeatures |= eDLTOutput;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Dual Layer Burning")))
		m_nOtherFeatures |= eDualLayerBurning;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("DMA Checking")))
		m_nOtherFeatures |= eDMAChecking;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Fit to DVD")))
		m_nOtherFeatures |= eFitToDVD;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Quality Settings")))
		m_nOtherFeatures |= eQualitySettings;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Burn to Disc Image")))
		m_nOtherFeatures |= eBurnToDiscImage;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Picture Capture")))
		m_nOtherFeatures |= ePictureCapture;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Free Button Layout")))
		m_nOtherFeatures |= eFreeButtonLayout;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Different Splash for JPN")))
		m_nOtherFeatures |= eDiffSplashForJPN;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("GetPCastTVFiles")))
		m_nOtherFeatures |= eGetPCastFiles;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("DVDJukeBox")))
		m_nOtherFeatures |= eBurnToDVDJukebox;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("SlideShowBurning")))
		m_nOtherFeatures |= eSlideshowBurning;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("AudioBitRate")))
		m_nOtherFeatures |= eAllAudioBitRatesMode;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("MP3CDBurning")))
		m_nOtherFeatures |= eBurnMP3CD;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("RippingCD")))
		m_nOtherFeatures |= eRippingCD;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Transitions")))
		m_nOtherFeatures |= eAllTransitionsMode;

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("UsingEuropeanSplash")))
		m_nOtherFeatures |= eUsingEuropeanSplash;

	if(FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Disable Eject After Burn")))
		m_nOtherFeatures |= eDisableEjectAfterBurn;
	
	if(FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("Show Labeler")))
		m_nOtherFeatures |= eShowLabeler;

	if(FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("LaunchRNBackup")))
		m_nOtherFeatures |= eLaunchRNBackup;

	if(FeatureEnabled(PCF_OTHER_SECTION_NAME,_T("RoxioLogo")))
		m_nOtherFeatures |= eRoxioLogo;

	//remove tuner from analog capture device for HP
	if(FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("DisableAnalogCaptureTuner")))
		m_nOtherFeatures2 |= eDisableAnalogCaptureTuner;

	//product information
	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("IsPhoto")))
	{
		// MyDVD Photo Edition excludes all support for VR.
		m_nCommonFeatures &= ~eDVDPlusVR;
		m_nProductFeatures |= eIsPhoto;
	}
	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("IsLiteEdition")))
	{
		m_nProductFeatures |= eIsLiteEdition;
	}
	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("VCD Only")))
	{
		m_nProductFeatures |= eIsVCD;
	}
	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Autodetect")))
	{
		m_nProductFeatures |= eIsAutodetect;
	}
	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME, _T("DisableAC3Import")))
	{
		m_nProductFeatures |= eDisableAC3Import;
	}
	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME, _T("DisableMultiAC3Import")))
	{
		m_nProductFeatures |= eDisableMultiAC3Import;
	}
	if(FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Default OpenDVD")))
	{
        m_nProductFeatures |= eOpenDVDEnabled;
	}


	//////////////////////////////////////////////////////////////////////////
	// Get ProductEdition Value, will map it into Edition Enum in MyDVD.
	// Yuchuan Wang.
	std::string csResult;
	csResult = m_pPconfig->get_value(PCF_PRODUCT_SECTION_NAME, _T("ProductEdition"));
	if(csResult.size() > 0)
	{
		m_nProductEdition = atoi(csResult.c_str());
	}
	else
	{
		// Default to MyDVD Standard Edition
		m_nProductEdition = 1;
	}
	//////////////////////////////////////////////////////////////////////////

	if (FeatureEnabled(PCF_OTHER_SECTION_NAME, _T("Sure Thing Labeler")))
	{
		m_nProductFeatures |= eSureThingLabeler;
	}

	csResult = m_pPconfig->get_value(PCF_PRODUCT_SECTION_NAME, _T("Audio Track Count"));
	m_nAudioTrackCount = (csResult.size() > 0) ? atoi(csResult.c_str()) : 1;

	csResult = m_pPconfig->get_value(PCF_PRODUCT_SECTION_NAME, _T("Subtitle Track Count"));
	m_nSubtitleTrackCount = (csResult.size() > 0) ? atoi(csResult.c_str()) : 1;

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("SAT Support")))
	{
		m_nProductFeatures |= eSATSupport;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("LCE Support")))
	{
		m_nProductFeatures |= eLCESupport;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Menu Subpicture In/Out Support")))
	{
		m_nProductFeatures |= eMenuSubpictureInOut;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Dual Layer Project Creation")))
	{
		m_nProductFeatures |= eDualLayerProjects;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Disc Protection Support")))
	{
		m_nProductFeatures |= eDiscProtection;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Volume Information Support")))
	{
		m_nProductFeatures |= eVolumeInfo;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Track Path Support")))
	{
		m_nProductFeatures |= eTrackPath;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Jacket Picture Support")))
	{
		m_nProductFeatures |= eJacketPicture;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Highlight From Link")))
	{
		m_nProductFeatures |= eHighlightFromLink;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Highlight From EndAction")))
	{
		m_nProductFeatures |= eHighlightFromEndAction;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Audio Stream From Link")))
	{
		m_nProductFeatures |= eSetAudioFromLink;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Audio Stream From EndAction")))
	{
		m_nProductFeatures |= eSetAudioFromEndAction;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Audio Stream in Playlists")))
	{
		m_nProductFeatures |= eSetAudioInPlaylists;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Subtitle Stream From Link")))
	{
		m_nProductFeatures |= eSetSubtitleFromLink;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Subtitle Stream From EndAction")))
	{
		m_nProductFeatures |= eSetSubtitleFromEndAction;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set Subtitle Stream in Playlists")))
	{
		m_nProductFeatures |= eSetSubtitleInPlaylists;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Playlist Support")))
	{
		m_nProductFeatures |= ePlaylists;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Set DVD Remote Actions")))
	{
		m_nProductFeatures |= eRemoteActions;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Allow Button Numbering")))
	{
		m_nProductFeatures |= eButtonNumbering;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Preview With Parhelia")))
	{
		m_nProductFeatures |= ePreviewParhelia;
	}

	if (FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("HDMV Authoring")))
	{
		m_nProductFeatures2 |= eHDMVAuthoring;
	}

	m_bDockByDefault = FeatureEnabled(PCF_PRODUCT_SECTION_NAME,_T("Default Dock State"));
}

PConfigFeatures::~PConfigFeatures()
{
}

bool PConfigFeatures::FeatureEnabled(ECommonFeature eFeature) const
{
	return (eFeature & m_nCommonFeatures) != 0;
}

bool PConfigFeatures::FeatureEnabled(EOtherFeature eFeature) const
{
	return (eFeature & m_nOtherFeatures) != 0;
}

bool PConfigFeatures::FeatureEnabled(EOtherFeature2 eFeature) const
{
	return (eFeature & m_nOtherFeatures2) != 0;
}

bool PConfigFeatures::FeatureEnabled(EProductFeature eFeature) const
{
	return (eFeature & m_nProductFeatures) != 0;
}

bool PConfigFeatures::FeatureEnabled(EProductFeature2 eFeature) const
{
	return (eFeature & m_nProductFeatures2) != 0;
}

SInt32 PConfigFeatures::IsDVDEnabled() const
{
	return FeatureEnabled(eDVD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDVDPlusVREnabled() const
{
	return FeatureEnabled(eDVDPlusVR) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDVDMinusVREnabled() const
{
	return FeatureEnabled(eDVDMinusVR) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsVCDEnabled() const
{
	return FeatureEnabled(eVCD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSVCDEnabled() const
{
	return FeatureEnabled(eSVCD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDVDPlusVRImportEnabled() const
{
	return FeatureEnabled(eDVDPlusVRImport) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDVDMinusVRImportEnabled() const
{
	return FeatureEnabled(eDVDMinusVRImport) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsQuickTimeImportEnabled() const
{
	return FeatureEnabled(eQuickTimeImport) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsAC3Enabled() const
{
	return FeatureEnabled(eAC3) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsPCMEnabled() const
{
	return FeatureEnabled(ePCM) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsMPEG2Enabled() const
{
	return FeatureEnabled(eMPEG2) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsJPEGEnabled() const
{
	return FeatureEnabled(eJPEG) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsMCEEnabled() const
{
	return FeatureEnabled(eMCE) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsCaptureEnabled() const
{
	return FeatureEnabled(eCapture) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsAnalogCaptureEnabled() const
{
	return FeatureEnabled(eAnalogCapture) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDVCaptureEnabled() const
{
	return FeatureEnabled(eDVCapture) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSlideShowEnabled() const
{
	return FeatureEnabled(eSlideshow) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsTrimmingEnabled() const
{
	return FeatureEnabled(eTrimming) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsVideoEditingEnabled() const
{
	SInt32 nResult = ProductFeaturesBase::eFeatureNotEnabled;
	if (ProductFeaturesAuthoring::IsWin98orMEPlatform())
		nResult = ProductFeaturesBase::eFeatureEnabledNoOSSupport;
	else
		nResult = FeatureEnabled(eVideoEditing) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;

	return nResult;
}

SInt32 PConfigFeatures::IsSceneDetectionEnabled() const
{
	return FeatureEnabled(eSceneDetection) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsChapterPointEditingEnabled() const
{
	return FeatureEnabled(eChapterPointEditing) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsD2DEnabled() const
{
	return FeatureEnabled(eD2D) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsMotionMenusEnabled() const
{
	return FeatureEnabled(eMotionMenus) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsRecordToHardDiscEnabled() const
{
	return FeatureEnabled(eRecordToHardDisc) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDVDEditingEnabled() const
{
	return FeatureEnabled(eDVDEditing) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsMultiCopyBurnEnabled() const
{
	return FeatureEnabled(eMultiCopyBurn) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsRegistrationEnabled() const
{
	return FeatureEnabled(eRegistration) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSonicUniversityEnabled() const
{
	return FeatureEnabled(eSonicUniversity) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDiscBrowserEnabled() const
{
	return FeatureEnabled(eDiscBrowser) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsIVTSupportEnabled() const
{
	return FeatureEnabled(eIVTSupport) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsVCDOnly() const
{
	return FeatureEnabled(eIsVCD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsAutoDetect() const
{
	return FeatureEnabled(eIsAutodetect) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsReEncodeMovieEnabled() const
{
	return FeatureEnabled(eReEncodeMovie) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsEditSegmentDEnabled() const
{
	return FeatureEnabled(eEditSegment) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsConstantQualityVBREnabled() const
{
	return FeatureEnabled(eConstantQualityVBR) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSegmentBasedReEndocingEnabled() const
{
	return FeatureEnabled(eSegmentBasedReEncode) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsGetTiVoFilesEnabled() const
{
	return FeatureEnabled(eGetTiVoFiles) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsGetPCastFilesEnabled() const
{
	SInt32 nResult =  FeatureEnabled(eGetPCastFiles) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
	if (nResult == eFeatureEnabled)
	{
		std::string subKey = _T("Software\\BUFFALO INC.\\NetTV\\Settings");
 		if (!WinRegOpenKey(HKEY_CURRENT_USER, subKey))
 		{
 			nResult = eFeatureEnabledNoOSSupport;
 		}
 	}
 
 	return nResult;
}

SInt32 PConfigFeatures::IsRecordNowExposed() const
{
	return FeatureEnabled(eExposeRecordNow) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSBUExposed() const
{
	return FeatureEnabled(eExposeSBU) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsMPEG1Only() const
{
	return FeatureEnabled(eMPEG1Only) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsPhotoEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

// Is the user allowed to import Dolby Digital (AC3) files into the DVDit palette?  Note the reversal
// of the meaning between the pconfig flag and this method - if the pconfig flag DisableAC3Import == 1 then
// IsAC3ImportEnabled will return eFeatureNotEnabled.
SInt32 PConfigFeatures::IsAC3ImportEnabled() const
{
	return FeatureEnabled(eDisableAC3Import) ? ProductFeaturesBase::eFeatureNotEnabled : ProductFeaturesBase::eFeatureEnabled;
}
SInt32 PConfigFeatures::IsMultiAC3ImportEnabled() const
{
	return FeatureEnabled(eDisableMultiAC3Import) ? ProductFeaturesBase::eFeatureNotEnabled : ProductFeaturesBase::eFeatureEnabled;
}
SInt32 PConfigFeatures::IsOpenDVDDefaultEnabled() const
{
	return FeatureEnabled(eOpenDVDEnabled) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsLiteEdition() const
{
	return FeatureEnabled(eIsLiteEdition) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsReturnToUmbrellaEnabled() const
{
	return FeatureEnabled(eReturnToUmbrella) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDLTOutputEnabled() const
{
	return FeatureEnabled(eDLTOutput) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDualLayerBurningEnabled() const
{
	return FeatureEnabled(eDualLayerBurning) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDisableEjectAfterBurn()const
{
	return FeatureEnabled(eDisableEjectAfterBurn) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsShowLabeler() const
{
	return FeatureEnabled(eShowLabeler) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsLaunchRNBackup() const
{
	return FeatureEnabled(eLaunchRNBackup)? ProductFeaturesBase::eFeatureEnabled: ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsRoxioLogo() const
{
	return FeatureEnabled(eRoxioLogo)? ProductFeaturesBase::eFeatureEnabled: ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDMACheckingEnabled() const
{
	return FeatureEnabled(eDMAChecking) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsFitToDVDEnabled() const
{
	return FeatureEnabled(eFitToDVD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsQualitySettingsEnabled() const
{
	return FeatureEnabled(eQualitySettings) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsBurnToDiscImageEnabled() const
{
	return FeatureEnabled(eBurnToDiscImage) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsPictureCaptureEnabled() const
{
	return FeatureEnabled(ePictureCapture) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsFreeButtonLayoutEnabled() const
{
	return FeatureEnabled(eFreeButtonLayout) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDiffSplashForJPNEnabled() const
{
	return FeatureEnabled(eDiffSplashForJPN) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsUsingPacRimSplashEnabled() const
{
	return FeatureEnabled(eUsingPacRimSplash) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 PConfigFeatures::IsLabelerEnabled() const
{
	return FeatureEnabled(eSureThingLabeler) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

UInt32 PConfigFeatures::GetAudioTrackCount() const
{
	return (m_nAudioTrackCount);
}
UInt32 PConfigFeatures::GetSubtitleTrackCount() const
{
	return (m_nSubtitleTrackCount);
}

SInt32 PConfigFeatures::IsSATSupportEnabled() const
{
	return FeatureEnabled(eSATSupport) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsLCESupportEnabled() const
{
	return FeatureEnabled(eLCESupport) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsMenuSubpictureInOutEnabled() const
{
	return FeatureEnabled(eMenuSubpictureInOut) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDualLayerProjectCreationEnabled() const
{
	return FeatureEnabled(eDualLayerProjects) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsDiscProtectionEnabled() const
{
	return FeatureEnabled(eDiscProtection) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsVolumeInfoEnabled() const
{
	return FeatureEnabled(eVolumeInfo) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsTrackPathEnabled() const
{
	return FeatureEnabled(eTrackPath) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsJacketPictureEnabled() const
{
	return FeatureEnabled(eJacketPicture) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetHighlightFromLinkEnabled() const
{
	return FeatureEnabled(eHighlightFromLink) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetHighlightFromEndActionEnabled() const
{
	return FeatureEnabled(eHighlightFromEndAction) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetAudioStreamFromLinkEnabled() const
{
	return FeatureEnabled(eSetAudioFromLink) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetAudioStreamFromEndActionEnabled() const
{
	return FeatureEnabled(eSetAudioFromEndAction) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetAudioStreamInPlaylistsEnabled() const
{
	return FeatureEnabled(eSetAudioInPlaylists) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetSubtitleStreamFromLinkEnabled() const
{
	return FeatureEnabled(eSetSubtitleFromLink) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetSubtitleStreamFromEndActionEnabled() const
{
	return FeatureEnabled(eSetSubtitleFromEndAction) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSetSubtitleStreamInPlaylistsEnabled() const
{
	return FeatureEnabled(eSetSubtitleInPlaylists) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsPlaylistsEnabled() const
{
	return FeatureEnabled(ePlaylists) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsRemoteActionsEnabled() const
{
	return FeatureEnabled(eRemoteActions) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsButtonNumberingEnabled() const
{
	return FeatureEnabled(eButtonNumbering) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsPreviewWithParheliaEnabled() const
{
	return FeatureEnabled(ePreviewParhelia) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

bool PConfigFeatures::AreWindowsDockedByDefault() const
{
	return m_bDockByDefault;
}

SInt32 PConfigFeatures::IsBurnToDVDJukeboxEnabled() const
{
	return FeatureEnabled(eBurnToDVDJukebox) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsSlideshowBurningEnabled() const
{
	return FeatureEnabled(eSlideshowBurning) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsAllAudioBitRatesModeEnabled() const
{
	return FeatureEnabled(eAllAudioBitRatesMode) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsAllTransitionsModeEnabled() const
{
	return FeatureEnabled(eAllTransitionsMode) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsRippingCDEnabled() const
{
	return FeatureEnabled(eRippingCD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsBurnMP3CDEnabled() const
{
	return FeatureEnabled(eBurnMP3CD) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 PConfigFeatures::IsUsingEuropeanSplashEnabled() const
{
	return FeatureEnabled(eUsingEuropeanSplash) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

UInt32 PConfigFeatures::GetProductEdition() const
{
	return m_nProductEdition;
}

SInt32 PConfigFeatures::IsSaveEnabled() const
{
	return ProductFeaturesBase::eFeatureEnabled;
}

SInt32 PConfigFeatures::IsSimulateEnabled() const
{
	return ProductFeaturesBase::eFeatureEnabled;
}

SInt32 PConfigFeatures::IsHDMVAuthoringEnabled() const
{
	return FeatureEnabled(eHDMVAuthoring) ? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}

bool PConfigFeatures::GetProductFullName(std::string& name) const
{
	name = m_pPconfig->get_value(PCF_ALL_SECTION_NAME, _T("FullName"));
	return !name.empty();
}

bool PConfigFeatures::GetProductName(std::string& name) const
{
	name = m_pPconfig->get_value(PCF_ALL_SECTION_NAME, _T("Name"));
	return !name.empty();
}

SInt32 PConfigFeatures::IsAnalogCaptureTunerDisabled() const
{
	return FeatureEnabled(eDisableAnalogCaptureTuner)? ProductFeaturesBase::eFeatureEnabled : ProductFeaturesBase::eFeatureNotEnabled;
}
#endif
