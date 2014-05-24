//-----------------------------------------------------------------------------
// ieee80.h
// Copyright (c) 2002 - 2004, Haohanit.  All rights reserved.
//-----------------------------------------------------------------------------

//SR FS: Reviewed [JAW  20040912]
//SR FS: Reviewed [wwt 20040914] 

#ifndef __IEEE80_H__
#define __IEEE80_H__

namespace ieee80
{

void double_to_extended80(unsigned char *outRate, double inRate);
void extended80_to_double(double *outRate, const unsigned char* inRate);

}

#endif
