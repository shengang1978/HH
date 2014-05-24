//-----------------------------------------------------------------------------
// ProductFeaturesAuthoring.cpp
// Copyright (c) 2004 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// ProductFeaturesAuthoring.cpp : implementation file
//

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040915]: safe input param; MBCS internally; no dangerous API; no registry/sys folder/temp file 
#if defined _WIN32
#include "ProductFeaturesAuthoring.h"
#include "SonicOptions.h"
#include "SonicTextBase.h"
#include "SonicException.h"

	const SonicText FeatureInitText::PConfigFileNotFound				(SonicTextBase::FeatureInitText - 1, "Config file not found");
	const SonicText FeatureInitText::SonicKeyNotFound					(SonicTextBase::FeatureInitText - 2, "Sonic key not found");
	const SonicText FeatureInitText::DongleNotFound						(SonicTextBase::FeatureInitText - 3, "Warning. Unable to detect security dongle. Check that the dongle is installed, and the drivers have been installed correctly.");
	const SonicText FeatureInitText::InvalidAvidDongle					(SonicTextBase::FeatureInitText - 4, "This dongle is not licensed to run this software. Please contact your Avid representative for assistance.");

// Base class for configuring features. Consumer apps use PConfigFeaturesAuthoring and init with the path to the
// pconfig.dcf file. Pro products use XxxxDongleFeatures, Xxxx is company or other designator, and implement
// dongle calls to determine feature set.
//
static ProductFeaturesAuthoring* GlobalProductFeatures = NULL;

ProductFeaturesAuthoring* ProductFeaturesAuthoring::GetProductFeatures()
{
	return GlobalProductFeatures;
}

ProductFeaturesAuthoring::ProductFeaturesAuthoring(const FileSpec& /*inPath*/)
{
	// There should only be one of these global objects in the app.
	assert(NULL == GlobalProductFeatures);
	GlobalProductFeatures = this;
}

ProductFeaturesAuthoring::~ProductFeaturesAuthoring()
{
	GlobalProductFeatures = NULL;
}

bool ProductFeaturesAuthoring::IsWin98orMEPlatform()
{
	OSVERSIONINFO	versionInfo;

	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);

	return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

}

bool ProductFeaturesAuthoring::IsMePlatform()
{
	OSVERSIONINFO	versionInfo;

	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);

	return (versionInfo.dwMinorVersion  == 90);

}

bool ProductFeaturesAuthoring::IsNTPlatfrom()
{
	OSVERSIONINFO	versionInfo;

	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);

	return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);

}

bool ProductFeaturesAuthoring::IsXPorGreater()
{
	OSVERSIONINFO	versionInfo;

	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);
	return ((5 < versionInfo.dwMajorVersion)
		|| ((5 == versionInfo.dwMajorVersion) 
		&& (1 <= versionInfo.dwMinorVersion)));
}

SInt32 ProductFeaturesAuthoring::IsDVDEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDVDPlusVREnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDVDMinusVREnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsVCDEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSVCDEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDVDPlusVRImportEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDVDMinusVRImportEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsQuickTimeImportEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsAC3Enabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsMultiAC3ImportEnabled() const
{
   return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsPCMEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsMPEG2Enabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsJPEGEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsMCEEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsCaptureEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsAnalogCaptureEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDVCaptureEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSlideShowEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsTrimmingEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsVideoEditingEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSceneDetectionEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsChapterPointEditingEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsD2DEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsMotionMenusEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsRecordToHardDiscEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDVDEditingEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsMultiCopyBurnEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsRegistrationEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSonicUniversityEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDiscBrowserEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsGetTiVoFilesEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsGetPCastFilesEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsRecordNowExposed() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSBUExposed() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsMPEG1Only() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsIVTSupportEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsReEncodeMovieEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsEditSegmentDEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsConstantQualityVBREnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSegmentBasedReEndocingEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsReturnToUmbrellaEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDLTOutputEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDualLayerBurningEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDMACheckingEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsFitToDVDEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsQualitySettingsEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsBurnToDiscImageEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsPictureCaptureEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsFreeButtonLayoutEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDiffSplashForJPNEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsUsingPacRimSplashEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsLiteEdition() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsPhotoEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsVCDOnly() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsAutoDetect() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsAC3ImportEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsOpenDVDDefaultEnabled() const
{
   return ProductFeaturesBase::eFeatureNotEnabled;
}

UInt32 ProductFeaturesAuthoring::GetProductEdition() const
{
	return 0;
}

SInt32 ProductFeaturesAuthoring::IsProjectVersionMustMatchEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsLabelerEnabled() const
{
	return ProductFeaturesBase::eFeatureEnabled;
}

// How many audio tracks should be displayed?
UInt32 ProductFeaturesAuthoring::GetAudioTrackCount() const
{
	return (0);
}

// How many subtitle tracks should be displayed?
UInt32 ProductFeaturesAuthoring::GetSubtitleTrackCount() const
{
	return (0);
}

SInt32 ProductFeaturesAuthoring::IsSATSupportEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsLCESupportEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsMenuSubpictureInOutEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDualLayerProjectCreationEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDiscProtectionEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsVolumeInfoEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsTrackPathEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsJacketPictureEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetHighlightFromLinkEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetHighlightFromEndActionEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetAudioStreamFromLinkEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetAudioStreamFromEndActionEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetAudioStreamInPlaylistsEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetSubtitleStreamFromLinkEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetSubtitleStreamFromEndActionEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsSetSubtitleStreamInPlaylistsEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsPlaylistsEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsRemoteActionsEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsButtonNumberingEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsPreviewWithParheliaEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

bool ProductFeaturesAuthoring::AreWindowsDockedByDefault() const
{
	return false;
}

SInt32 ProductFeaturesAuthoring::IsSaveEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureEnabled;
}
SInt32 ProductFeaturesAuthoring::IsDisplayEULA() const
{
	return ProductFeaturesAuthoring::eFeatureEnabled;
}
SInt32 ProductFeaturesAuthoring::IsSimulateEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureEnabled;
}

SInt32 ProductFeaturesAuthoring::IsHDMVAuthoringEnabled() const
{
	return ProductFeaturesAuthoring::eFeatureNotEnabled;
}

bool ProductFeaturesAuthoring::GetProductFullName(std::string& /*name*/) const
{
	return false;
}

bool ProductFeaturesAuthoring::GetProductName(std::string& /*name*/) const
{
	return false;
}

SInt32 ProductFeaturesAuthoring::IsUsingEuropeanSplashEnabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsDisableEjectAfterBurn()const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsLaunchRNBackup() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsRoxioLogo() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
SInt32 ProductFeaturesAuthoring::IsShowLabeler() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}

SInt32 ProductFeaturesAuthoring::IsAnalogCaptureTunerDisabled() const
{
	return ProductFeaturesBase::eFeatureNotEnabled;
}
#endif

