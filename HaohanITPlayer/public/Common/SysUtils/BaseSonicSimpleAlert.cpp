//-----------------------------------------------------------------------------
// BaseSonicSimpleAlert.cpp
// Copyright (c) 1997 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW 20040911]
//SR FS: Reviewed [wwt 20040914] 

#include "BaseSonicSimpleAlert.h"
#include "SonicTextBase.h"

const SonicText BaseSonicSimpleAlert::Note(SonicTextBase::SonicSimpleAlertText - 0, "Note");
const SonicText BaseSonicSimpleAlert::Caution(SonicTextBase::SonicSimpleAlertText - 1, "Warning");
const SonicText BaseSonicSimpleAlert::Stop(SonicTextBase::SonicSimpleAlertText - 2, "Error");

BaseSonicSimpleAlert::BaseSonicSimpleAlert() 
{
}

BaseSonicSimpleAlert::~BaseSonicSimpleAlert() 
{
}
