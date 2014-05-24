//-----------------------------------------------------------------------------
// PConfigFeatures.h
// Copyright (c) 2004 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// PConfigFeatures.h : definition file
//
// This ProductFeaturesAuthoring derived class provides the implementation
// that uses an RWPConfig instance for determining which features are enabled.
//
//-----------------------------------------------------------------------

//SR FS: Reviewed [wwt 20040914] 

#ifndef __PConfigFeatures_H
#define __PConfigFeatures_H

#include "ProductFeaturesAuthoring.h"
#include "pconfig.h"
#include "safe_ptr.h"

//SR FS: Reviewed [wwt 20040914] 

class PConfigFeatures : public ProductFeaturesAuthoring
{
private:
	typedef ProductFeaturesAuthoring INHERITED;

public:
	PConfigFeatures(const FileSpec& inPath);

	virtual ~PConfigFeatures();

	enum ECommonFeature
	{
		eDVD				= 0x00000001,
		eDVDPlusVR			= 0x00000002,
		eDVDMinusVR			= 0x00000004,
		eVCD				= 0x00000008,
		eSVCD				= 0x00000010,
		eDVDPlusVRImport	= 0x00000020,
		eDVDMinusVRImport	= 0x00000040,
		eQuickTimeImport	= 0x00000080,
		eAC3				= 0x00000100,
		ePCM				= 0x00000200,
		eMPEG2				= 0x00000400,
		eJPEG 				= 0x00000800,
		eMCE				= 0x00001000,
		eAnalogCapture		= 0x00002000,
		eDVCapture			= 0x00004000,
		eCapture			= 0x00002000 & 0x00004000,
		eSlideshow			= 0x00008000,
		eTrimming			= 0x00010000,
		eVideoEditing		= 0x00020000,
		eSceneDetection		= 0x00040000,
		eChapterPointEditing = 0x00080000,
		eD2D				= 0x00100000,
		eMotionMenus		= 0x00200000,
		eRecordToHardDisc	= 0x00400000,
		eDVDEditing			= 0x00800000,
		eMultiCopyBurn		= 0x01000000,
		eRegistration		= 0x02000000,
		eSonicUniversity	= 0x04000000,
		eDiscBrowser		= 0x08000000,
		eGetTiVoFiles		= 0x10000000,
		eExposeRecordNow	= 0x20000000,
		eExposeSBU			= 0x40000000,
		eMPEG1Only			= 0x80000000,  // OEM for Adaptec, have DVD feature but any MPEG2 feature is not allowed, by cjh.		
	};

	enum EOtherFeature
	{
		eIVTSupport				= 0x00000001,
		eReEncodeMovie			= 0x00000002,
		eEditSegment			= 0x00000004,
		eConstantQualityVBR		= 0x00000008,
		eSegmentBasedReEncode	= 0x00000010,
		eLaunchRNBackup			= 0x00000020,
		eReturnToUmbrella		= 0x00000040,
		eDLTOutput				= 0x00000080,
		eDualLayerBurning		= 0x00000100,
		eDMAChecking			= 0x00000200,
		eFitToDVD				= 0x00004000,
		eQualitySettings		= 0x00008000,
		eBurnToDiscImage		= 0x00010000,
		ePictureCapture			= 0x00020000,
		eFreeButtonLayout		= 0x00040000,
		eGetPCastFiles			= 0x00080000,

		eBurnToDVDJukebox		= 0x00100000,
		eSlideshowBurning		= 0x00200000,
		eAllAudioBitRatesMode	= 0x00400000,
		eAllTransitionsMode		= 0x00800000,
		eDiffSplashForJPN		= 0x01000000,
		eBurnMP3CD			= 0x02000000,
		eRippingCD			= 0x04000000,
		eUsingPacRimSplash	= 0x08000000,
		eUsingEuropeanSplash	= 0x10000000,
		eDisableEjectAfterBurn  = 0x20000000,
		eShowLabeler			= 0x40000000,
		eRoxioLogo				= 0x80000000
	};

	enum EOtherFeature2
	{
		eDisableAnalogCaptureTuner = 0x00000001,
	};

	enum EProductFeature
	{
		eIsPhoto			= 0x00000001,
		eIsVCD				= 0x00000002,
		eIsAutodetect		= 0x00000004,
		eDisableAC3Import	= 0x00000008,
		eIsLiteEdition		= 0x00000010,
		eSureThingLabeler	= 0x00000020,
		eDeluxeMode			= 0x00000040,
		eDisableMultiAC3Import = 0x00000080,
		eAudioTrackCount	= 0x00000100,
		eSubtitleTrackCount	= 0x00000200,
		eSATSupport			= 0x00000400,
		eLCESupport			= 0x00000800,
		eMenuSubpictureInOut = 0x00001000,
		eDualLayerProjects  = 0x00002000,
		eDiscProtection		= 0x00004000,
		eVolumeInfo			= 0x00008000,
		eTrackPath			= 0x00010000,
		eJacketPicture		= 0x00020000,
		eHighlightFromLink	= 0x00040000,
		eHighlightFromEndAction = 0x00080000,
		eSetAudioFromLink	= 0x00100000,
		eSetAudioFromEndAction = 0x00200000,
		eSetAudioInPlaylists = 0x00400000,
		eSetSubtitleFromLink = 0x00800000,
		eSetSubtitleFromEndAction = 0x01000000,
		eSetSubtitleInPlaylists = 0x02000000,
		ePlaylists			= 0x04000000,
		eRemoteActions		= 0x08000000,
		eButtonNumbering	= 0x10000000,
		ePreviewParhelia	= 0x20000000,
		eDockByDefault		= 0x40000000,
		eOpenDVDEnabled     = 0x80000000
	};

	enum EProductFeature2
	{
		eHDMVAuthoring		= 0x00000001,
	};

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
	virtual SInt32 IsGetPCastFilesEnabled() const;
	virtual SInt32 IsUsingPacRimSplashEnabled() const;
	virtual SInt32 IsUsingEuropeanSplashEnabled() const;
	virtual SInt32 IsDisableEjectAfterBurn() const;
	virtual SInt32 IsLaunchRNBackup() const;
	virtual SInt32 IsRoxioLogo() const;
	virtual SInt32 IsShowLabeler() const;

	virtual SInt32 IsBurnToDVDJukeboxEnabled() const;
	virtual SInt32 IsSlideshowBurningEnabled() const;
	virtual SInt32 IsAllAudioBitRatesModeEnabled() const;
	virtual SInt32 IsAllTransitionsModeEnabled() const;
	virtual SInt32 IsBurnMP3CDEnabled() const;
	virtual SInt32 IsRippingCDEnabled() const;

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
	// Use base class: virtual SInt32 IsProjectVersionMustMatchEnabled() const;
	virtual SInt32 IsLabelerEnabled() const;
	virtual SInt32 IsSaveEnabled() const;
	virtual SInt32 IsSimulateEnabled() const;

	// Use base class: virtual SInt32 IsProducerHDEnabled() const;
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
	// Use base class: virtual SInt32 IsDisplayEULA() const;
	virtual bool AreWindowsDockedByDefault() const;

	// Product Features 2
	virtual SInt32 IsHDMVAuthoringEnabled() const;

	virtual bool GetProductFullName(std::string& name) const;
	virtual bool GetProductName(std::string& name) const;

protected:

	bool FeatureEnabled(ECommonFeature eFeature) const;
	bool FeatureEnabled(EOtherFeature eFeature) const;
	bool FeatureEnabled(EOtherFeature2 eFeature) const;
	bool FeatureEnabled(EProductFeature eFeature) const;
	bool FeatureEnabled(EProductFeature2 eFeature) const;

	virtual UInt32 GetDataOffset(){return sizeof(m_nCommonFeatures) + sizeof(m_nOtherFeatures) + sizeof(m_nProductFeatures);}

	UInt32 m_nCommonFeatures;
	UInt32 m_nOtherFeatures;
	UInt32 m_nOtherFeatures2;
	UInt32 m_nProductFeatures;
	UInt32 m_nProductFeatures2;
	UInt32 m_nProductEdition;
	UInt32 m_nAudioTrackCount;
	UInt32 m_nSubtitleTrackCount;
	bool m_bDockByDefault;
	
private:
	bool FeatureEnabled(const char * pszSection, const char * pszFeature) const;

	PConfigFeatures(const PConfigFeatures&);
	PConfigFeatures& operator =(const PConfigFeatures&);

	safe_ptr<RWPConfigBase>	m_pPconfig;
};

#endif
