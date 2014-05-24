//-----------------------------------------------------------------------------
// ProductFeaturesAuthoring.h
// Copyright (c) 2004 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// ProductFeaturesAuthoring.h : definition file
//
//
//-----------------------------------------------------------------------
//SR FS: Reviewed [wwt 20040914] 
//SR FS: Reviewed [DDT 20040928] Second pass.
//SR FS: Reviewed [JAW 20040928] 

#ifndef __ProductFeaturesAuthoring_H
#define __ProductFeaturesAuthoring_H

#include "SonicText.h"
#include <safe_ptr.h>

namespace FeatureInitText {
	extern const SonicText PConfigFileNotFound;
	extern const SonicText SonicKeyNotFound;
	extern const SonicText DongleNotFound;
	extern const SonicText InvalidAvidDongle;
}

class FileSpec;

class ProductFeaturesBase {
public:
	enum 
	{
		eFeatureEnabled = 0,
		eFeatureNotEnabled = -1,
		eFeatureEnabledNoOSSupport = -2,

	};

	ProductFeaturesBase() {};	// Just a base class for using the enum through the code

private:
	ProductFeaturesBase(const ProductFeaturesBase&);
};

class ProductFeaturesAuthoring : public ProductFeaturesBase
{
public:
	static ProductFeaturesAuthoring* GetProductFeatures();

	ProductFeaturesAuthoring(const FileSpec& inPath);

	virtual ~ProductFeaturesAuthoring();

	static bool IsMePlatform();
	static bool IsWin98orMEPlatform();
	static bool IsNTPlatfrom();
	static bool IsXPorGreater();

	//common features
	virtual SInt32 IsDVDEnabled() const;
	virtual SInt32 IsDVDPlusVREnabled() const;
	virtual SInt32 IsDVDMinusVREnabled() const;
	virtual SInt32 IsVCDEnabled() const;
	virtual SInt32 IsSVCDEnabled() const;
	virtual SInt32 IsDVDPlusVRImportEnabled() const;
	virtual SInt32 IsDVDMinusVRImportEnabled() const;
	virtual SInt32 IsQuickTimeImportEnabled() const;
	virtual SInt32 IsAC3Enabled() const;
	virtual SInt32 IsPCMEnabled() const;
	virtual SInt32 IsMPEG2Enabled() const;
	virtual SInt32 IsJPEGEnabled() const;
	virtual SInt32 IsMCEEnabled() const;
	virtual SInt32 IsCaptureEnabled() const;
	virtual SInt32 IsAnalogCaptureEnabled() const;
	virtual SInt32 IsDVCaptureEnabled() const;
	virtual SInt32 IsSlideShowEnabled() const;
	virtual SInt32 IsTrimmingEnabled() const;
	virtual SInt32 IsVideoEditingEnabled() const;
	virtual SInt32 IsSceneDetectionEnabled() const;
	virtual SInt32 IsChapterPointEditingEnabled() const;
	virtual SInt32 IsD2DEnabled() const;
	virtual SInt32 IsMotionMenusEnabled() const;
	virtual SInt32 IsRecordToHardDiscEnabled() const;
	virtual SInt32 IsDVDEditingEnabled() const;
	virtual SInt32 IsMultiCopyBurnEnabled() const;
	virtual SInt32 IsRegistrationEnabled() const;
	virtual SInt32 IsSonicUniversityEnabled() const;
	virtual SInt32 IsDiscBrowserEnabled() const;
	virtual SInt32 IsGetTiVoFilesEnabled() const;
	virtual SInt32 IsGetPCastFilesEnabled() const;
	virtual SInt32 IsRecordNowExposed() const;
	virtual SInt32 IsSBUExposed() const;
	virtual SInt32 IsMPEG1Only() const;

	//other features
	virtual SInt32 IsIVTSupportEnabled() const;
	virtual SInt32 IsReEncodeMovieEnabled() const;
	virtual SInt32 IsEditSegmentDEnabled() const;
	virtual SInt32 IsConstantQualityVBREnabled() const;
	virtual SInt32 IsSegmentBasedReEndocingEnabled() const;
	virtual SInt32 IsReturnToUmbrellaEnabled() const;
	virtual SInt32 IsDLTOutputEnabled() const;
	virtual SInt32 IsDualLayerBurningEnabled() const;
	virtual SInt32 IsDMACheckingEnabled() const;
	virtual SInt32 IsFitToDVDEnabled() const;
	virtual SInt32 IsQualitySettingsEnabled() const;
	virtual SInt32 IsBurnToDiscImageEnabled() const;
	virtual SInt32 IsPictureCaptureEnabled() const;
	virtual SInt32 IsFreeButtonLayoutEnabled() const;
	virtual SInt32 IsDiffSplashForJPNEnabled() const;
	virtual SInt32 IsUsingPacRimSplashEnabled() const;
	virtual SInt32 IsUsingEuropeanSplashEnabled() const;
	virtual SInt32 IsDisableEjectAfterBurn() const;
	virtual SInt32 IsLaunchRNBackup() const;
	virtual SInt32 IsRoxioLogo() const;
	virtual SInt32 IsShowLabeler() const;

	// Other Features 2
	virtual SInt32 IsAnalogCaptureTunerDisabled() const;

	//product information
	virtual SInt32 IsLiteEdition() const;
	virtual SInt32 IsPhotoEnabled() const;
	virtual SInt32 IsVCDOnly() const;
	virtual SInt32 IsAutoDetect() const;
	virtual UInt32 GetProductEdition() const;
	// Is the user allowed to import Dolby Digital (AC3) files into the DVDit palette?
	virtual SInt32 IsAC3ImportEnabled() const;
	//Is the product allowed to import multiDolby Digital(AC3) files into the DVDit palette?
	virtual SInt32 IsMultiAC3ImportEnabled() const;
	//Is OpenDVD the default setting for the product? 
    virtual SInt32 IsOpenDVDDefaultEnabled() const;
	virtual SInt32 IsProjectVersionMustMatchEnabled() const;
	virtual SInt32 IsLabelerEnabled() const;
	virtual SInt32 IsSaveEnabled() const;
	virtual SInt32 IsSimulateEnabled() const;
	
	// How many audio tracks should be displayed?
	virtual UInt32 GetAudioTrackCount() const;
	// How many subtitle tracks should be displayed?
	virtual UInt32 GetSubtitleTrackCount() const;

	virtual SInt32 IsSATSupportEnabled() const;
	virtual SInt32 IsLCESupportEnabled() const;
	virtual SInt32 IsMenuSubpictureInOutEnabled() const;
	virtual SInt32 IsDualLayerProjectCreationEnabled() const;
	virtual SInt32 IsDiscProtectionEnabled() const;
	virtual SInt32 IsVolumeInfoEnabled() const;
	virtual SInt32 IsTrackPathEnabled() const;
	virtual SInt32 IsJacketPictureEnabled() const;
	virtual SInt32 IsSetHighlightFromLinkEnabled() const;
	virtual SInt32 IsSetHighlightFromEndActionEnabled() const;
	virtual SInt32 IsSetAudioStreamFromLinkEnabled() const;
	virtual SInt32 IsSetAudioStreamFromEndActionEnabled() const;
	virtual SInt32 IsSetAudioStreamInPlaylistsEnabled() const;
	virtual SInt32 IsSetSubtitleStreamFromLinkEnabled() const;
	virtual SInt32 IsSetSubtitleStreamFromEndActionEnabled() const;
	virtual SInt32 IsSetSubtitleStreamInPlaylistsEnabled() const;
	virtual SInt32 IsPlaylistsEnabled() const;
	virtual SInt32 IsRemoteActionsEnabled() const;
	virtual SInt32 IsButtonNumberingEnabled() const;
	virtual SInt32 IsPreviewWithParheliaEnabled() const;
	virtual SInt32 IsDisplayEULA() const;
	virtual bool AreWindowsDockedByDefault() const;

	// Product Features 2
	virtual SInt32 IsHDMVAuthoringEnabled() const;

	virtual bool GetProductFullName(std::string& name) const;
	virtual bool GetProductName(std::string& name) const;

private:
	ProductFeaturesAuthoring(const ProductFeaturesAuthoring&);
	ProductFeaturesAuthoring& operator =(const ProductFeaturesAuthoring&);
};

#endif
