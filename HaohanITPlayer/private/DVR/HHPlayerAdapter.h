#ifndef _HHPLAYERADAPTER_H_
#define _HHPLAYERADAPTER_H_

#include "DVRSettings.h"
#include "NetDef.h"
class HHPlayerAdapter
{
public:
	virtual bool Init() {return true;};
	virtual void Destroy() {}

	virtual int OpenFile(tstring strFile) = 0;
	virtual int OpenStream(HWND hWnd, HHV_CLIENT_INFO* pClientInfo) = 0;
	virtual int OpenStreamByTime(HWND hWnd, SYSTEM_VIDEO_FILE* recdFile, char* ssIP, int ssPort) = 0;

	// Close the file, stream by the index which is returned by the OpenFile or OpenStream.
	virtual void Close(int nIndex) = 0;

	virtual void Play(int nIndex) = 0;
	virtual void Pause(int nIndex) = 0;
	virtual void Stop(int nIndex) = 0;

	virtual int Fast(int nIndex) = 0;
	virtual int Slow(int nIndex) = 0;

	virtual int ResizeWindow(int nIndex) = 0;

	// Return : the captured pic path if success. empty if error.
	virtual tstring CapturePic(tstring strPicFolder, CDVRSettings::eCapPicType ePicType) = 0;
};

#endif