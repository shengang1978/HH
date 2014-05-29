#include "StdAfx.h"
#include "HHFrameDraw.h"
#include "StreamParser.h"
// Draw Functions
int HHFrameMetadataHelper::FromMemory(BYTE* pBuf, int nBuyLen, HHV::FrameMetaDataList& metaDataList)
{
	U32 readSize = 0;
	BYTE* pFMDStartPos = pBuf;
	while(readSize < nBuyLen)
	{
		HHV::FrameMetaData fmd;
		pFMDStartPos = fmd.FromMemory(pFMDStartPos);
		metaDataList.push_back(fmd);
		readSize += fmd.memsize();
	}

	return (int)metaDataList.size();
}
void HHFrameMetadataHelper::DrawFrameMetaData(Gdiplus::Graphics& graphics, const HHV::FrameMetaData& frame, const LONG& lWndWidth, const LONG& lWndHeight)
{
#ifdef TEST_PERFORMANCE
	FreeProfilerRecordCodeBlock(0x6, "")
#endif
#ifdef _DEBUG
	TRACE2("Frame Image: width(%d), height(%d)\n", frame.displayData.image_width, frame.displayData.image_height);
	TRACE(_T("Frame Attributes:"));
	DWORD dwBegin = GetTickCount();
#endif
	//DrawAttributes(graphics, frame.attributes, lWndWidth, lWndHeight);

	//for (HHV::Attributes::const_iterator it = frame.attributes.begin(); it != frame.attributes.end(); ++it)
	//{
	//	TRACE(_T("Attr Key(%s), Value(%s)\n"), (LPTSTR)CA2T(it->first.c_str()), (LPTSTR)CA2T(it->second.c_str()));
	//}
	for (HHV::DisplayObjectMetaList::const_iterator itDspObj = frame.displayData.disp_obj_list.begin(); 
		itDspObj != frame.displayData.disp_obj_list.end(); 
		++itDspObj)
	{
		TRACE1("Draw DisplayObjectMeta: id=%s\n", (LPTSTR)CA2T(itDspObj->id.c_str()));
		DrawDisplayObjectMeta(graphics, *itDspObj, lWndWidth, lWndHeight, frame.displayData.image_width, frame.displayData.image_height);
	}

	for (HHV::PolyLines::const_iterator itLine = frame.displayData.line_list.begin(); 
		itLine != frame.displayData.line_list.end(); 
		++itLine)
	{
		DrawPolyLine(graphics, *itLine, lWndWidth, lWndHeight, frame.displayData.image_width, frame.displayData.image_height);
	}

	for (HHV::Texts::const_iterator itTxt = frame.displayData.text_list.begin();
		itTxt != frame.displayData.text_list.end();
		++itTxt)
	{
		DrawTextMeta(graphics, *itTxt, lWndWidth, lWndHeight, frame.displayData.image_width, frame.displayData.image_height);
	}

	for (HHV::Polygons::const_iterator itPolygon = frame.displayData.polygon_list.begin();
		itPolygon != frame.displayData.polygon_list.end();
		++itPolygon)
	{
		DrawPolygon(graphics, *itPolygon, lWndWidth, lWndHeight, frame.displayData.image_width, frame.displayData.image_height);
	}

	for (HHV::GUIObjects::const_iterator itGUIObj = frame.displayData.gui_object_list.begin();
		itGUIObj != frame.displayData.gui_object_list.end();
		++itGUIObj)
	{
		DrawObjectType(graphics, *itGUIObj, lWndWidth, lWndHeight, frame.displayData.image_width, frame.displayData.image_height);
	}
#ifdef _DEBUG
	DWORD dwEnd = GetTickCount();
	TRACE1("\nDrawFrameMetaData: %d\n", dwEnd-dwBegin);
#endif
}
void HHFrameMetadataHelper::DrawPolyLine(Gdiplus::Graphics& graphics, const HHV::PolyLine& line, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight)
{
#ifdef TEST_PERFORMANCE
	FreeProfilerRecordCodeBlock(0x5, "")
#endif
	if (line.lines.size() > 1)
	{
		if (line.end_style == 0)
		{
			HDC dc = graphics.GetHDC();
			HPEN hPen = ::CreatePen(PS_SOLID, line.thickness, RGB(line.color.r, line.color.g, line.color.b));
			//CPen gdiPen(PS_SOLID, line.thickness, RGB(line.color.r, line.color.g, line.color.b));
			HGDIOBJ hOldPen = ::SelectObject(dc, hPen);
			POINT* ptLines = new POINT[line.lines.size()];
			for (int i = 0; i < line.lines.size(); ++i)
			{
				ptLines[i].x = line.lines[i].x;
				ptLines[i].y = line.lines[i].y;
			}		
			::Polyline(dc, ptLines, line.lines.size());
			::SelectObject(dc, hOldPen);
			::DeleteObject(hPen);
			graphics.ReleaseHDC(dc);
			delete[] ptLines;
		}
		else
		{
			Gdiplus::Pen gPlusPen(Gdiplus::Color(line.color.r, line.color.g, line.color.b), line.thickness);
			//Gdiplus::SolidBrush gPlusBrush(Gdiplus::Color(line.color.r, line.color.g, line.color.b));

			Gdiplus::Point* ptLines = new Gdiplus::Point[line.lines.size()];
			int i = 0;
			for (HHV::Points::const_iterator itPt = line.lines.begin();
				itPt != line.lines.end();
				++itPt, ++i)
			{
				ptLines[i].X = itPt->x;
				ptLines[i].Y = itPt->y;
			}

			switch(line.end_style)
			{
			case 1:
				gPlusPen.SetCustomEndCap(&Gdiplus::AdjustableArrowCap(line.arrow_length, line.arrow_width, FALSE));
				break;
			case 2:
				gPlusPen.SetCustomEndCap(&Gdiplus::AdjustableArrowCap(line.arrow_length, line.arrow_width, TRUE));
				break;
			case 0:
			default:
				break;
			}
			graphics.DrawLines(&gPlusPen, ptLines, line.lines.size());

			delete[] ptLines;
		}

	}
}

void HHFrameMetadataHelper::DrawTextMeta(Gdiplus::Graphics& graphics, const HHV::TextMeta& txt, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight)
{
#ifdef TEST_PERFORMANCE
	FreeProfilerRecordCodeBlock(0x4, "")
#endif
	HDC dc = graphics.GetHDC();
	int nTxtLen = txt.text.length();
	WCHAR* pBuf = new WCHAR[nTxtLen+1];
	memset(pBuf, 0, sizeof(WCHAR)*(nTxtLen+1));
	::MultiByteToWideChar(CP_ACP, 0, txt.text.c_str(), -1, pBuf, nTxtLen+1);
	HFONT hFont;
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	//lf.lfHeight = FW_NORMAL;   
	//lf.lfWeight = FW_NORMAL;
	lf.lfHeight = txt.size;
	hFont = ::CreateFontIndirect(&lf);
	//if(!hFont.CreateFontIndirect(&lf))
	//	return;
	HFONT hOldFont = (HFONT)::SelectObject(dc, hFont);
	::SetBkMode(dc, TRANSPARENT);
	::SetTextColor(dc, RGB(txt.color.r, txt.color.g, txt.color.b));
	::TextOutW(dc, txt.x, txt.y, pBuf, nTxtLen);
	::SelectObject(dc, hOldFont);
	::DeleteObject(hFont);
	graphics.ReleaseHDC(dc);
	delete[] pBuf;
}
void HHFrameMetadataHelper::DrawPolygon(Gdiplus::Graphics& graphics, const HHV::PolygonM& polygon, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight)
{
#ifdef TEST_PERFORMANCE
	FreeProfilerRecordCodeBlock(0x3, "")
#endif
	const HHV::DrawingStyle& style = polygon.style;

	if (style.alpha == 100 && !polygon.style.bFill)
	{
		HDC dc = graphics.GetHDC();
		HPEN hPen = ::CreatePen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
		//CPen gdiPen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
		HGDIOBJ hOldPen = ::SelectObject(dc, hPen);
		POINT* ptPolygons = new POINT[polygon.points.size() + 1];
		for (int i = 0; i < polygon.points.size(); ++i)
		{
			ptPolygons[i].x = polygon.points[i].x;
			ptPolygons[i].y = polygon.points[i].y;
		}
		//闭合多边形（最后一个点和第一个点重合）
		ptPolygons[polygon.points.size()].x = polygon.points[0].x;
		ptPolygons[polygon.points.size()].y = polygon.points[0].y;
		::Polyline(dc, ptPolygons, polygon.points.size() + 1);
		::SelectObject(dc, hOldPen);
		::DeleteObject(hPen);
		graphics.ReleaseHDC(dc);
		delete[] ptPolygons;
	}
	else
	{
		Gdiplus::Pen gPlusPen(Gdiplus::Color(style.alpha*255/100, style.color.r, style.color.g, style.color.b), style.thickness);

		Gdiplus::Point* ptPolygons = new Gdiplus::Point[polygon.points.size()];
		for (int i = 0; i < polygon.points.size(); ++i)
		{
			ptPolygons[i].X = polygon.points[i].x;
			ptPolygons[i].Y = polygon.points[i].y;
		}
		if (polygon.style.bFill)
		{
			Gdiplus::SolidBrush gPlusBrush(Gdiplus::Color(style.alpha*255/100, style.fill_color.r, style.fill_color.g, style.fill_color.b));
			graphics.FillPolygon(&gPlusBrush, ptPolygons, polygon.points.size());
		}

		graphics.DrawPolygon(&gPlusPen, ptPolygons, polygon.points.size());
		delete[] ptPolygons;
	}
}
void HHFrameMetadataHelper::DrawObjectType(Gdiplus::Graphics& graphics, const HHV::ObjectType& obj, const LONG& lWndWidth, const LONG& lWndHeight, const LONG& nImgWidth, const LONG& nImgHeight)
{
#ifdef TEST_PERFORMANCE
	FreeProfilerRecordCodeBlock(0x2, "")
#endif
	const HHV::DrawingStyle& style = obj.style;

	Gdiplus::Pen gPlusPen(Gdiplus::Color(style.alpha*255/100, style.color.r, style.color.g, style.color.b), style.thickness);

	switch(obj.type)
	{
	case 0:		//line_segment
		{
			//去掉透明度
			HDC dc = graphics.GetHDC();
			HPEN hPen = ::CreatePen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			//CPen gdiPen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			HGDIOBJ hOldPen = ::SelectObject(dc, hPen);
			::MoveToEx(dc, obj.x0, obj.y0, NULL);
			::LineTo(dc, obj.x1, obj.y1);
			::SelectObject(dc, hOldPen);
			::DeleteObject(hPen);
			graphics.ReleaseHDC(dc);
		}
		/*
		if (style.alpha == 100)
		{
			HDC dc = graphics.GetHDC();
			HPEN hPen = ::CreatePen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			//CPen gdiPen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			HGDIOBJ hOldPen = ::SelectObject(dc, hPen);
			::MoveToEx(dc, obj.x0, obj.y0, NULL);
			::LineTo(dc, obj.x1, obj.y1);
			::SelectObject(dc, hOldPen);
			::DeleteObject(hPen);
			graphics.ReleaseHDC(dc);
		}
		else
		{
			graphics.DrawLine(&gPlusPen, obj.x0, obj.y0, obj.x1, obj.y1);
		}
		*/
		break;
	case 1:		//rectangle
		if (style.bFill)
		{
			Gdiplus::SolidBrush gPlusBrush(Gdiplus::Color(style.alpha*255/100, style.fill_color.r, style.fill_color.g, style.fill_color.b));
			graphics.FillRectangle(&gPlusBrush, obj.x0 + obj.style.thickness/2, obj.y0 + obj.style.thickness/2, 
				obj.x1 - obj.x0 - obj.style.thickness, obj.y1 - obj.y0 - obj.style.thickness);
		}
		{
			//去掉透明度
			HDC dc = graphics.GetHDC();
			HPEN hPen = ::CreatePen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			//CPen gdiPen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			HGDIOBJ hOldPen = ::SelectObject(dc, hPen);
			::MoveToEx(dc, obj.x0, obj.y0, NULL);
			::LineTo(dc, obj.x1, obj.y0);
			::LineTo(dc, obj.x1, obj.y1);
			::LineTo(dc, obj.x0, obj.y1);
			::LineTo(dc, obj.x0, obj.y0);
			::SelectObject(dc, hOldPen);
			::DeleteObject(hPen);
			graphics.ReleaseHDC(dc);
		}
		/*
		if (style.alpha == 100)
		{
			HDC dc = graphics.GetHDC();
			HPEN hPen = ::CreatePen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			//CPen gdiPen(PS_SOLID, style.thickness, RGB(style.color.r, style.color.g, style.color.b));
			HGDIOBJ hOldPen = ::SelectObject(dc, hPen);
			::MoveToEx(dc, obj.x0, obj.y0, NULL);
			::LineTo(dc, obj.x1, obj.y0);
			::LineTo(dc, obj.x1, obj.y1);
			::LineTo(dc, obj.x0, obj.y1);
			::LineTo(dc, obj.x0, obj.y0);
			::SelectObject(dc, hOldPen);
			::DeleteObject(hPen);
			graphics.ReleaseHDC(dc);
		}
		else
		{
			graphics.DrawRectangle(&gPlusPen, obj.x0, obj.y0, obj.x1 - obj.x0, obj.y1 - obj.y0);	
		}
		*/
		break;
	case 2:		//ellipse
		if (style.bFill)
		{
			Gdiplus::SolidBrush gPlusBrush(Gdiplus::Color(style.alpha*255/100, style.fill_color.r, style.fill_color.g, style.fill_color.b));

			graphics.FillEllipse(&gPlusBrush, (obj.x0 + obj.style.thickness/2), (obj.y0 + obj.style.thickness/2), 
				(obj.x1 - obj.x0 - obj.style.thickness), (obj.y1 - obj.y0 - obj.style.thickness));
		}

		graphics.DrawEllipse(&gPlusPen, obj.x0, obj.y0, obj.x1 - obj.x0, obj.y1 - obj.y0);
		break;
	case -1:	//no display
	default:
		//
		break;
	}
}

int ScaleFrameMetaDataList::g_ScaleIndex = 0;

ScaleFrameMetaDataList::ScaleFrameMetaDataList()
	: m_bScaling(false)
{
	CString csEvent;
	csEvent.Format(_T("ScaleEvent_%d"), g_ScaleIndex);
	m_hEvent[eScaleEvent] = ::CreateEvent(NULL, TRUE, FALSE, csEvent);
	csEvent.Format(_T("ExitScaleThread_%d"), g_ScaleIndex);
	m_hEvent[eExitEvent] = ::CreateEvent(NULL, TRUE, FALSE, csEvent);
	++g_ScaleIndex;

	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, ScaleCallBack, this, 0, NULL);
	//memset(&m_lWndWidth, 0, sizeof(m_lWndWidth));
	//memset(&m_lWndHeight, 0, sizeof(m_lWndHeight));
}

ScaleFrameMetaDataList::~ScaleFrameMetaDataList()
{
	::SetEvent(m_hEvent[eExitEvent]);
	::WaitForSingleObject(m_hThread, 10000);
	::CloseHandle(m_hThread);
	::CloseHandle(m_hEvent[eScaleEvent]);
	::CloseHandle(m_hEvent[eExitEvent]);
}
void ScaleFrameMetaDataList::GetScaledFrameMetaDataList(HHV::FrameMetaDataList& dstFrame, const HHV::FrameMetaDataList& srcFrame, LONG lWndWidth, LONG lWndHeight)
{
	if (!m_bScaling)
	{
		m_TmpLock.Lock();
		m_TmpMetaDataList = srcFrame;
		m_lWndWidth = lWndWidth;
		m_lWndHeight = lWndHeight;
		SetEvent(m_hEvent[eScaleEvent]);
		m_TmpLock.Unlock();
	}
	m_ScaleLock.Lock();
	dstFrame = m_FrameMetaDataList;
	m_ScaleLock.Unlock();
}

//void ScaleFrameMetaDataList::GetScaledFrameMetaDataList(int nPort, HHV::FrameMetaDataList& dstFrame, const HHV::FrameMetaDataList& srcFrame, LONG lDstWidth, LONG lDstHeight)
//{
//	TRACE_LOG("执行GetScaledFrameMetaDataLis函数 \r\n");
//	TRACE_LOG("GetScaledFrameMetaDataLis->m_bScaling %d\r\n", m_bScaling);
//	TRACE_LOG("GetScaledFrameMetaDataLis->srcFrame的大小 %d\r\n", (int)srcFrame.size());
//	TRACE_LOG("GetScaledFrameMetaDataLis->nPort的通道号 %d\r\n", nPort);
//	if (!m_bScaling)
//	{
//		m_TmpLock.Lock();
//		m_mapTmpMetaDataList[nPort] = srcFrame;
//		m_mapDstWidth[nPort] = lDstWidth;
//		m_mapDstHeight[nPort] = lDstHeight;
//		m_TmpLock.Unlock();
//		SetEvent(m_hEvent[eScaleEvent]);
//	}
//	m_ScaleLock.Lock();
//	TRACE_LOG("GetScaledFrameMetaDataLis->m_mapFrameMetaDataList的大小 %d\r\n", (int)m_mapFrameMetaDataList.size());
//	std::map<int, HHV::FrameMetaDataList>::iterator it = m_mapFrameMetaDataList.find(nPort);
//	if (it != m_mapFrameMetaDataList.end())
//	{
//		dstFrame = it->second;
//		TRACE_LOG("GetScaledFrameMetaDataList->dstFrame %d\r\n", (int)dstFrame.size());
//		m_mapFrameMetaDataList.erase(it);
//	}
//	m_ScaleLock.Unlock();
//}

//int	ScaleFrameMetaDataList::GetWorkFrameMetaData(HHV::FrameMetaDataList& frame, LONG& lDstWidth, LONG& lDstHeight)
//{
//	static int nCurPort = 0;
//	int nStartQueryPort = nCurPort;
//	MAP_RRAME::iterator it = m_mapTmpMetaDataList.find(nCurPort);
//	TRACE_LOG("GetWorkFrameMetaData->m_mapTmpMetaDataList的大小 %d\r\n", (int)m_mapTmpMetaDataList.size());
//	while (it == m_mapTmpMetaDataList.end() || it->second.size() == 0)
//	{
//		++nCurPort;
//		TRACE_LOG("GetWorkFrameMetaData->++nCurPort %d\r\n", nCurPort);
//		if (nCurPort >= MAX_PLAYER)
//		{
//			TRACE_LOG("GetWorkFrameMetaData->nCurPort >= MAX_PLAYER %d\r\n", nCurPort);
//			nCurPort = 0;
//		}
//		if (nCurPort == nStartQueryPort)
//		{
//			TRACE_LOG("break\r\n", nCurPort);
//			TRACE_LOG("GetWorkFrameMetaData->nCurPort %d\r\n", nCurPort);
//			TRACE_LOG("GetWorkFrameMetaData->nStartQueryPort %d\r\n", nStartQueryPort);
//			break;
//		}
//		it = m_mapTmpMetaDataList.find(nCurPort);
//	}
//	
//	if (it != m_mapTmpMetaDataList.end() && it->second.size() > 0)
//	{
//		frame = it->second;
//		lDstWidth = m_mapDstWidth[it->first];
//		lDstHeight = m_mapDstHeight[it->first];
//		return it->first;
//	}
//	return -1;
//}
unsigned ScaleFrameMetaDataList::ScaleCallBack(void* pParam)
{
	ScaleFrameMetaDataList* pThis = (ScaleFrameMetaDataList*)pParam;
	BOOL bExit = FALSE;
	while(!bExit)
	{
		DWORD dwEventIndex = ::WaitForMultipleObjects(sizeof(pThis->m_hEvent)/sizeof(HANDLE), pThis->m_hEvent, FALSE, 1000);
		switch(dwEventIndex)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0+eScaleEvent:
			{
				pThis->m_TmpLock.Lock();
				//HHV::FrameMetaDataList frame;// = pThis->m_TmpMetaDataList;
				HHV::FrameMetaDataList frame = pThis->m_TmpMetaDataList;
				//LONG lWndWidth;// = pThis->m_lWndWidth;
				//LONG lWndHeight;// = pThis->m_lWndHeight;
				LONG lWndWidth = pThis->m_lWndWidth;
				LONG lWndHeight = pThis->m_lWndHeight;
				//int nPort = pThis->GetWorkFrameMetaData(frame, lWndWidth, lWndHeight);
				pThis->m_TmpLock.Unlock();
				if (frame.size() > 0)
				{
					HHV::FrameMetaData attributes;
					pThis->m_bScaling = true;
					for (HHV::FrameMetaDataList::iterator it = frame.begin(); it != frame.end(); ++it)
					{
						ScaleAttributes(attributes, it->attributes, lWndWidth, lWndHeight);
						ScaleFrameMetaData(*it, lWndWidth, lWndHeight);
					}
					if (attributes.displayData.disp_obj_list.size() > 0 ||
						attributes.displayData.gui_object_list.size() > 0 ||
						attributes.displayData.line_list.size() > 0 ||
						attributes.displayData.polygon_list.size() > 0 ||
						attributes.displayData.text_list.size() > 0)
					{
						attributes.displayData.image_width = lWndWidth;
						attributes.displayData.image_height = lWndHeight;
						frame.push_back(attributes);
					}
				}

				pThis->m_ScaleLock.Lock();
				//if (nPort >= 0)
				//{
				if (frame.size() > 0)
				{
					pThis->m_FrameMetaDataList = frame;
					//pThis->m_mapFrameMetaDataList[nPort] = frame;
				}
				//else
				//{
				//pThis->m_mapFrameMetaDataList.erase(nPort);
				//}
				//}
				pThis->m_ScaleLock.Unlock();
				ResetEvent(pThis->m_hEvent[eScaleEvent]);
			}
			break;
		case WAIT_OBJECT_0+eExitEvent:
			bExit = TRUE;
			ResetEvent(pThis->m_hEvent[eExitEvent]);
			break;
		}
		pThis->m_bScaling = false;
	}
	::_endthreadex(0);
	return 0;
}

void ScaleFrameMetaDataList::ScaleAttributes(HHV::FrameMetaData& attributes, HHV::Attributes& attrs, LONG lWndWidth, LONG lWndHeight)
{
#ifdef TEST_PERFORMANCE
	FreeProfilerRecordCodeBlock(0x7, "")
#endif

		HHV::Attributes::const_iterator it = attrs.find("NumberOfRegion");
	if (it != attrs.end() && !it->first.empty())
	{
		int nNumberOfRegion = atoi(it->second.c_str());
		std::vector<int> imgSize = ToArray(attrs, "region.imagesize");
		if (imgSize.size() == 2)
		{
			float xScale = (float)lWndWidth/imgSize[0];
			float yScale = (float)lWndHeight/imgSize[1];
			for (int i = 1; i <= nNumberOfRegion; ++i)
			{
				std::string shape = ShapeType(attrs, i);
				std::string type = RegionType(attrs, i);
				if (type != "Effective")//Effective 不需要描画
				{
					if (shape == "rectangle" || shape == "line")
					{
						HHV::ObjectType ot = ToObjectType(attrs, i);
						ScaleObjectType(ot, xScale, yScale);
						attributes.displayData.gui_object_list.push_back(ot);
					}
					else if (shape == "polygon")
					{
						HHV::PolygonM pm = ToPolygon(attrs, i);
						ScalePolygonM(pm, xScale, yScale);
						attributes.displayData.polygon_list.push_back(pm);
					}
				}

				//else if (shape == "line")
				//{
				//	CvPoint imgSize = ImageSize(attrs, i);
				//	if (imgSize.x >0 && imgSize.y > 0 )
				//	{
				//		DrawPolyLine(graphics, ToPolyLine(attrs, i), lWndWidht, lWndHeight, imgSize.x, imgSize.y);
				//	}
				//}
			}
		}
	}
}

void ScaleFrameMetaDataList::ScaleFrameDisplayData(HHV::FrameDisplayData& fdd,  LONG lWndWidth, LONG lWndHeight)
{
	float xScale = (float)lWndWidth/fdd.image_width;
	float yScale = (float)lWndHeight/fdd.image_height;

	for (HHV::DisplayObjectMetaList::iterator it = fdd.disp_obj_list.begin(); it != fdd.disp_obj_list.end(); ++it)
	{
		ScaleDisplayObjectMeta(*it, xScale, yScale);
	}

	for (HHV::PolyLines::iterator it = fdd.line_list.begin(); it != fdd.line_list.end(); ++it)
	{
		ScalePolyLine(*it, xScale, yScale);
	}

	for (HHV::Texts::iterator it = fdd.text_list.begin(); it != fdd.text_list.end(); ++it)
	{
		ScaleTextMeta(*it, xScale, yScale);
	}

	for (HHV::Polygons::iterator it = fdd.polygon_list.begin(); it != fdd.polygon_list.end(); ++it)
	{
		ScalePolygonM(*it, xScale, yScale);
	}

	for (HHV::GUIObjects::iterator it = fdd.gui_object_list.begin(); it != fdd.gui_object_list.end(); ++it)
	{
		ScaleObjectType(*it, xScale, yScale);
	}

	fdd.image_width = lWndWidth;
	fdd.image_height = lWndHeight;
}
void ScaleFrameMetaDataList::ScaleDisplayObjectMeta(HHV::DisplayObjectMeta& dom, float xScale, float yScale)
{
	ScaleObjectType(dom.obj, xScale, yScale);
	ScalePolyLine(dom.track, xScale, yScale);
}
void ScaleFrameMetaDataList::ScalePolyLine(HHV::PolyLine& line, float xScale, float yScale)
{
	//line.thickness = (int)line.thickness*xScale;
	for (HHV::Points::iterator it = line.lines.begin(); it != line.lines.end(); ++it)
	{
		it->x = (int)it->x * xScale;
		it->y = (int)it->y * yScale;
	}
}
void ScaleFrameMetaDataList::ScaleTextMeta(HHV::TextMeta& txt, float xScale, float yScale)
{

	txt.size *= xScale;
	txt.x *= xScale;
	txt.y *= yScale;

}
void ScaleFrameMetaDataList::ScalePolygonM(HHV::PolygonM& pg, float xScale, float yScale)
{
	//pg.style.thickness *= xScale;
	for (HHV::Points::iterator it = pg.points.begin(); it != pg.points.end(); ++it)
	{
		it->x *= xScale;
		it->y *= yScale;
	}
}
void ScaleFrameMetaDataList::ScaleObjectType(HHV::ObjectType& ot, float xScale, float yScale)
{
	//ot.style.thickness *= xScale;
	switch(ot.type)
	{
	case 0:
	case 1:
		ot.x0 *= xScale;
		ot.y0 *= yScale;
		ot.x1 *= xScale;
		ot.y1 *= yScale;
		break;
	case 2:
		ot.x0 *= xScale;
		ot.y0 *= yScale;
		ot.x1 *= xScale;
		ot.y1 *= yScale;
		ot.x2 *= xScale;
		ot.y2 *= yScale;
		ot.x3 *= xScale;
		ot.y3 *= yScale;
		break;
	default:
		break;
	}
}

std::string ScaleFrameMetaDataList::ToString(LPCSTR prefix, int num)
{
	CStringA csRet;
	csRet.Format("%s.%d", prefix, num);
	return (LPCSTR)csRet;
}

std::vector<int> ScaleFrameMetaDataList::ToArray(const HHV::Attributes& attrs, LPCSTR prefix, int index)
{
	std::vector<int> ary;
	HHV::Attributes::const_iterator it = attrs.find((index == -1) ? prefix : ToString(prefix, index));
	if (it != attrs.end())
	{
		int nStartPos = 0;
		int nPos = it->second.find(',', nStartPos);
		while (nPos != std::string::npos)
		{
			ary.push_back(atoi(it->second.substr(nStartPos, nPos).c_str()));
			nStartPos = nPos+1;
			nPos = it->second.find(',', nStartPos);
		}
		if (nStartPos != 0)
		{
			ary.push_back(atoi(it->second.substr(nStartPos, it->second.length()-nStartPos).c_str()));
		}
	}
	return ary;
}
std::string ScaleFrameMetaDataList::ShapeType(const HHV::Attributes& attrs, int index)
{
	HHV::Attributes::const_iterator it = attrs.find(ToString("shape-type", index));
	if (it != attrs.end())
	{
		return it->second;
	}
	return "";
}
std::string ScaleFrameMetaDataList::RegionType(const HHV::Attributes& attrs, int index)
{
	HHV::Attributes::const_iterator it = attrs.find(ToString("region-types", index));
	if (it != attrs.end())
	{
		return it->second;
	}
	return "";
}	
HHV::PolyLine ScaleFrameMetaDataList::ToPolyLine(const HHV::Attributes& attrs, int index)
{
	return HHV::PolyLine();
}
HHV::ObjectType ScaleFrameMetaDataList::ToObjectType(const HHV::Attributes& attrs, int index)
{
	HHV::ObjectType obj;

	std::string shape = ShapeType(attrs, index);
	obj.style.bFill = false;
	obj.type = -1;
	obj.style.alpha = 100;
	obj.style.color.r = 1;
	obj.style.color.g = 162;
	obj.style.color.b = 232;
	obj.style.thickness = 1;

	if (shape == "line")
	{
		std::vector<int> ary = ToArray(attrs, "coordinate", index);
		if (ary.size() >= 4)
		{
			obj.type = 0;
			obj.x0 = ary[0];
			obj.y0 = ary[1];
			obj.x1 = ary[2];
			obj.y1 = ary[3];	
		}
	}
	else if (shape == "rectangle")
	{
		std::vector<int> ary = ToArray(attrs, "coordinate", index);
		if (ary.size() >= 4)
		{
			obj.type = 1;
			obj.x0 = ary[0];
			obj.y0 = ary[1];
			obj.x1 = ary[0] + ary[2];
			obj.y1 = ary[1] + ary[3];
			//obj.x2 = ary[4];
			//obj.y2 = ary[5];
			//obj.x3 = ary[6];
			//obj.y3 = ary[7];
		}
	}

	return obj;
}
HHV::PolygonM ScaleFrameMetaDataList::ToPolygon(const HHV::Attributes& attrs, int index)
{
	HHV::PolygonM pm;
	pm.style.bFill = false;
	pm.style.bFlash = false;
	pm.style.alpha = 100;
	pm.style.thickness = 1;
	pm.style.color.r = 237;
	pm.style.color.g = 28;
	pm.style.color.b = 36;

	std::vector<int> ary = ToArray(attrs, "coordinate", index);
	if (0 == (ary.size() & 1))
	{
		for (int i = 0; i < ary.size(); i += 2)
		{
			pm.points.push_back(cvPoint(ary[i], ary[i+1]));
		}
	}

	return pm;
}