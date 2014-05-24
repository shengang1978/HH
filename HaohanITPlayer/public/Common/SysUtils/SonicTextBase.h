///////////////////////////////////////////////////////////////////////////////
// SonicTextBase.h
// Copyright (c) 2004, Haohanit.  All rights reserved.
///////////////////////////////////////////////////////////////////////////////

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __SonicTextBase_h__
#define __SonicTextBase_h__

#include "SonicText.h"

namespace SonicTextBase
{
	const SInt32 AuthorScriptAPIText			= 0;
	const SInt32 SonicExceptionText				= -1;
	const SInt32 HighLevelText					= -100;
	const SInt32 DVDImagerLibMsgText			= -1000;
	const SInt32 TranscodeMsgText				= -3000;
	const SInt32 TimecodeText					= -4800;
	const SInt32 PrimeTimeStyleText				= -5000;
	const SInt32 SonicSimpleAlertText			= -6000;
	const SInt32 block_device_text				= -9000;
	const SInt32 FileSystemText					= -10000;
	const SInt32 AC3Text						= -12000;
	const SInt32 SSEResourcesErrorBase			= -13000;
	const SInt32 SSEAFErrorBase					= -15600;
	const SInt32 SSEAudioIOMgrErrorBase			= -15700;
	const SInt32 MOFS_Text						= -16000;
	const SInt32 SSEAPIErrorBase				= -16200;
	const SInt32 DVDError						= -19000;
	const SInt32 DVDEngineText					= -19100;
	const SInt32 LegalizerText					= -19300;
	const SInt32 AVSchedulerText				= -19400;
	const SInt32 AVSegmentText					= -19500;
	const SInt32 MachineControlText				= -19600;
	const SInt32 EncEngineText					= -19700;
	const SInt32 DVDSessionText					= -19800;
	const SInt32 DVDStreamText					= -19900;
	const SInt32 MPEGBoardText					= -20200;
	const SInt32 MPEGEncoderText				= -20300;
	const SInt32 MovieBuilderText				= -20400;
#define	kVOBDemuxErrorBase			-20600		// mpc
	const SInt32 VOBPlayerErrorText				= -20900;
	const SInt32 VBRProfilerErrorText			= -21000;
	const SInt32 AudioRecorderText				= -21100;
	const SInt32 VideoRecorderText				= -21200;
	const SInt32 PostProcessorErrorText			= -22000;
	const SInt32 MQModelErrorText				= -23000;
	const SInt32 VBVModelErrorText				= -24000;
	const SInt32 MPEGVideoLegalizerText			= -25100;
	const SInt32 SourceText						= -26000;
	const SInt32 SortedContainerText			= -26100;
	const SInt32 WAVText						= -26200;
	const SInt32 MPEGNavigationProcessingText	= -26300;
	const SInt32 DVDItLegalizerText				= -26500;
	const SInt32 DraxEncoderDeviceText			= -26700;
	const SInt32 DraxDecoderText				= -26800;
	const SInt32 DDPPackageText					= -27000;
	const SInt32 DVDImagerLibTargetText			= -27200;
	const SInt32 DVDImagerLibErrorText			= -27300;
	const SInt32 PrePassDataCreatorText			= -27600;
	const SInt32 MPEG1AudioText					= -27900;
	const SInt32 primo_text						= -28100;
	const SInt32 ProofText						= -29000;
	const SInt32 FormatLibText					= -31000;
	const SInt32 AdvancedMuxText				= -32000;
	const SInt32 I2OSText						= -33000;
	const SInt32 PluginCoderText				= -34000;
	const SInt32 SonicRTStreamEncText			= -35000;
	const SInt32 MetadataText					= -36000;
	const SInt32 PlaylistText					= -36100;
	const SInt32 SonicLuaText					= -36200;
	const SInt32 StyleScriptText				= -36500;
	const SInt32 MenuCoreMacroText				= -37000;
	const SInt32 MenuCoreText					= -38000;
	const SInt32 DVDitPaletteText				= -41000;
	const SInt32 DVDitSelectionText				= -42000;
	const SInt32 DVDitCoreMacroText				= -43000;
	const SInt32 DVDitPaletteMacroText			= -44000;
	const SInt32 AuthorCoreMacroText			= -45000;
	const SInt32 MenuCompilerText				= -46000;
	const SInt32 AVDecoderText					= -47000;
	const SInt32 MyDVDCoreMacroText				= -48000;
	const SInt32 SVCDFormatText					= -50000;
	const SInt32 rt_stream_error				= -51000;
	const SInt32 PreviewCompilerText			= -53000;
	const SInt32 SceneDetectionText				= -54000;
	const SInt32 SonicSurfaceText				= -55000;
	const SInt32 CompositorText					= -56000;
	const SInt32 IFFFileText					= -57000;
	const SInt32 SlideshowTranscoderText		= -58000;
	const SInt32 DirectShowUtilsText			= -59000;
	const SInt32 DShowCaptureText				= -60000;
	const SInt32 AuthorScriptCEText				= -61000;
	const SInt32 AVCaptureText					= -62000;
	const SInt32 TranscodeText					= -63000;
	const SInt32 ICodecAPIText					= -63100;
	const SInt32 TimelineTranscoderText			= -64000;
	const SInt32 MediaPipeText					= -65000;
	const SInt32 RTControllerText				= -66000;
	const SInt32 DashVRVolumeText				= -67000;
	const SInt32 AVRecorderText					= -68000;
	const SInt32 storageStreamFSText		    = -69000;
	const SInt32 DVDCommandText					= -70000;
	const SInt32 FileUtilsText					= -71000;
	const SInt32 MuxerLibText					= -72000;
	const SInt32 FeatureInitText				= -73000;
	const SInt32 WMVOutputText					= -73100;
	const SInt32 ImageScriptText				= -84000;
	const SInt32 SimpleBurnEngineText			= -90000;
	const SInt32 SimpleBurnUIText				= -91000;
	const SInt32 storageDeviceManagerText		= -92000;
	const SInt32 HDDVDText						= -93000;
	const SInt32 HDRendererText					= -94000;
	const SInt32 BDAVRecorderText				= -95000;
	const SInt32 XMLDOMText						= -96000;
	const SInt32 AS_StorageText					= -97000;
	const SInt32 PlayerUtilitiesText			= -98000;
	const SInt32 QuickTimeText					= -99000;
	const SInt32 SenseCodeText					= 0x7f000000;
};

#endif //__SonicTextBase_h__
