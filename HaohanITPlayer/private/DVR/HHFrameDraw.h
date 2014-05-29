#ifndef _HHFRAMEDRAW_H_
#define _HHFRAMEDRAW_H_

#include <gdiplus.h>
#include "Metadata_Types.h"
#include "Sync_Locks.h"

class HHFrameMetadataHelper
{
public:
	int FromMemory(BYTE* pBuf, int nBuyLen, HHV::FrameMetaDataList& metaDataList);

public:
	// Draw Meta Data Functions.
	void DrawFrameMetaData(Gdiplus::Graphics& graphics, const HHV::FrameMetaData& frame, const LONG& lWndWidth, const LONG& lWndHeight);
	// Draw Meta Data Scale
	void DrawDisplayObjectMeta(Gdiplus::Graphics& graphics, const HHV::DisplayObjectMeta& dspObj, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight)
	{
		DrawObjectType(graphics, dspObj.obj, lWndWidth, lWndHeight, nImgWidth, nImgHeight);
		DrawPolyLine(graphics, dspObj.track, lWndWidth, lWndHeight, nImgWidth, nImgHeight);
	}
	void inline DrawPolyLine(Gdiplus::Graphics& graphics, const HHV::PolyLine& line, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight);
	void inline DrawTextMeta(Gdiplus::Graphics& graphics, const HHV::TextMeta& txt, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight);
	void inline DrawPolygon(Gdiplus::Graphics& graphics, const HHV::PolygonM& polygon, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight);
	void inline DrawObjectType(Gdiplus::Graphics& graphics, const HHV::ObjectType& obj, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight);
};

class ScaleFrameMetaDataList
{
public:
	ScaleFrameMetaDataList();
	~ScaleFrameMetaDataList();

	void GetScaledFrameMetaDataList(HHV::FrameMetaDataList& dstFrame, const HHV::FrameMetaDataList& srcFrame, LONG lWndWidth, LONG lWndHeight);
	//void GetScaledFrameMetaDataList(int nPort, HHV::FrameMetaDataList& dstFrame, const HHV::FrameMetaDataList& srcFrame, LONG lDstWidth, LONG lDstHeight);
	static std::string RegionType(const HHV::Attributes& attrs, int index);
private:
	static	int g_ScaleIndex;
	typedef std::map<int, HHV::FrameMetaDataList>	MAP_RRAME;
	//MAP_RRAME m_mapFrameMetaDataList;
	//MAP_RRAME m_mapTmpMetaDataList;
	//std::map<int, LONG>	m_mapDstWidth, m_mapDstHeight;

	HHV::FrameMetaDataList	m_FrameMetaDataList;
	HHV::FrameMetaDataList	m_TmpMetaDataList;
	int	GetWorkFrameMetaData(HHV::FrameMetaDataList& frame, LONG& lDstWidth, LONG& lDstHeight);

	LONG	m_lWndWidth, m_lWndHeight;
	bool	m_bScaling;
	CCritSec	m_ScaleLock;
	CCritSec	m_TmpLock;
	enum	eEvent{eScaleEvent = 0, eExitEvent = 1};
	HANDLE		m_hEvent[2];
	HANDLE		m_hThread;

	static unsigned __stdcall ScaleCallBack(void* pParam);

	static void ScaleFrameMetaData(HHV::FrameMetaData& frame, LONG lWndWidth, LONG lWndHeight)
	{
		ScaleFrameDisplayData(frame.displayData, lWndWidth, lWndHeight);
	}
	static void ScaleAttributes(HHV::FrameMetaData& attributes, HHV::Attributes& attrs, LONG lWndWidth, LONG lWndHeight);
	static void ScaleFrameDisplayData(HHV::FrameDisplayData& fdd,  LONG lWndWidth, LONG lWndHeight);
	static void ScaleDisplayObjectMeta(HHV::DisplayObjectMeta& dom, float xScale, float yScale);
	static void ScalePolyLine(HHV::PolyLine& line, float xScale, float yScale);
	static void ScaleTextMeta(HHV::TextMeta& txt, float xScale, float yScale);
	static void ScalePolygonM(HHV::PolygonM& pg, float xScale, float yScale);
	static void ScaleObjectType(HHV::ObjectType& ot, float xScale, float yScale);

	static std::string ToString(LPCSTR, int num);
	static std::vector<int> ToArray(const HHV::Attributes& attrs, LPCSTR prefix, int index = -1);
	static std::string ShapeType(const HHV::Attributes& attrs, int index);

	static HHV::PolyLine ToPolyLine(const HHV::Attributes& attrs, int index);
	static HHV::ObjectType ToObjectType(const HHV::Attributes& attrs, int index);
	static HHV::PolygonM	ToPolygon(const HHV::Attributes& attrs, int index);
};
#endif