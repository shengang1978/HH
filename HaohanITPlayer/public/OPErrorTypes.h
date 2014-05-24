//-----------------------------------------------------------------------------
// OPErrorTypes.h
//
// Copyright (c) Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------
// Define errors for clients to be notified

#pragma once

// Custom HRESULT return values
// From MSDN: Vendors can supply their own values for this field, as 
// long as the C bit (0x20000000) is set, indicating it is a customer code.

#define E_COPP_CERTIFICATE_FAILURE					0xA0040A00
#define E_COPP_OUTPUT_PROTECTION_FAILURE			0xA0040A01
//#define E_UNSECURE_PLAYER_ENV						0xA0040A02
#define E_COPP_DOT_WITH_ANALOG_OUTPUT   			0xA0040A03
#define E_COPP_NO_HDCP_WITH_DIGITAL_OUTPUT 			0xA0040A04
#define E_COPP_UNKNOWN_OUTPUT_TYPE       			0xA0040A05
#define E_COPP_ICT_WITH_ANALOG_NO_DOWN_REZ   		0xA0040A06
#define E_COPP_ACP_OUTPUT_PROTECTION_FAILURE		0xA0040A07
#define E_COPP_CGMS_OUTPUT_PROTECTION_FAILURE		0xA0040A08
#define E_COPP_REPEATER_ON_DIGITAL_OUTPUT	        0xA0040A09
#define E_AACS_GUEST_UPDATE_FAILURE     	        0xA0040A0A
#define E_UAB_VLD_ACCELERATION_CANCELLED   	        0xA0040A0B
#define E_COPP_IMAGE_CONSTRAINED_TOKEN   	        0xA0040A0C
#define E_COPP_CLONE_MODE				   	        0xA0040A0D
#define E_COPP_DWM_ACTIVE_PAVP  		   	        0xA0040A0E
