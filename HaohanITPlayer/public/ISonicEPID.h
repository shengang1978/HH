//-----------------------------------------------------------------------------
// ISonicEPID.h
//
// Handles Intel's EPID key exchange
//
// Copyright (c), Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _ISonicEPID_H_
#define _ISonicEPID_H_

#include "CommonTypes.h"

class ISonicEPID
{
public:
    virtual ~ISonicEPID() {}
    
    // Types of EPID objects to create
    enum
    {
        eDxvaUab        = 1,            // Create an EPID object for DXVA UAB encryption
        eMediaVault,                    // Create an EPID object for MediaVault
    };
    
    static ISonicEPID*  CreateEPID(int object_type);
    static void         DestroyEPID(ISonicEPID* pObject);
    
    // Types of session pointers for PerformKeyExchange
    enum
    {
        eSessionCryptoSession9 = 1,     // pSession is a IDirect3DCryptoSession9*
        eSessionD3DManager,             // pSession is a IDirect3DDeviceManager9* used to create an Intel Auxilary Session
        eSessionD3DDevice               // pSession is a IDirect3DDevice9* used to create an Intel Auxilary Session
    };
    
    enum
    {
        eVideoPath = 1,
        eAudioPath,
    };
    
    // Return values (any other values are implementation specific)
    enum
    {
        eEPID_OK                = 0,
        eEPID_InvalidArgs       = -1,
        eEPID_DeriveSigmaKeys   = -2,
        eEPID_CreateHMAC        = -3,
        eEPID_MessageSign       = -4,
        eEPID_VerifyHMAC        = -5,
        eEPID_VerifyCertSig     = -6,
        eEPID_MessageVerify     = -7,
    };
    
    // Object-specific init params set before calling PerformKeyExchange:
    // eDxvaUab: not used
    // eMediaVault: param is a MV_StreamParams_t* filled in by the caller (except pCertificate and pPrivKey, these are filled in internally)
    //
    virtual bool    Init(void* param, size_t param_size) = 0;
    
    // Establish a sigma session (return 0 for success, any other value is an error)
    virtual SInt32  PerformKeyExchange(void* pSession, int session_type) = 0;
    
    // Test the validity of the key. Pass key info via Init like you do prior to calling PerformKeyExchange.  (return 0 for success, any other value is an error)
    virtual SInt32  TestKey(void* pSession, int session_type) = 0;

    // Close the sigma session (return 0 for success, any other value is an error)
    virtual SInt32  CloseSession() = 0;
    
    // Get an audio/video stream key from the active session
    virtual bool    GetStreamKey(int mediaPathId, int streamId, UInt8* pKey, size_t nKeyBufSize, size_t& outKeySize) = 0;
    
    // Get the active session ID
    virtual UInt32  GetSessionID() = 0;
};

#endif //_ISonicEPID_H_


