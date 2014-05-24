#pragma once

#include <vector>
#include <string>
#include <map>
#include <cv.h>
#include <fstream>
#include <iostream>


namespace HHV {

	using std::string;
	using std::vector;

	struct RGBColor;
	typedef std::map<std::string, std::string> Attributes;
	typedef std::vector <CvPoint> Points;  // a list of line segments

#define INT_SIZE		4
#define BOOL_SIZE		1
#define RGBCOLOR_SIZE	(sizeof(RGBColor)/sizeof(int)*4)

#define BEGIN_SERIALIZE(x)	\
	byte* pNext = x;

#define END_SERIALIZE()	\
	return pNext;

#define ReadBOOL(y)	\
	pNext = MetaDataHelper::ReadBool(pNext, y)

#define WriteBOOL(y) \
	pNext = MetaDataHelper::WriteBool(pNext, y)

#define ReadInt32(y)	\
	pNext = MetaDataHelper::ReadBytes<int, 4>(pNext, y)

#define WriteInt32(value) \
	pNext = MetaDataHelper::WriteBytes<int, 4>(pNext, value)

#define ReadRGBColor(value)	\
	pNext = ReadInt32(value.r);	\
	pNext = ReadInt32(value.g);	\
	pNext = ReadInt32(value.b);

#define WriteRGBColor(value)	\
	pNext = WriteInt32(value.r);	\
	pNext = WriteInt32(value.g);	\
	pNext = WriteInt32(value.b)
	
#define ReadCvRect(box)	\
	pNext = ReadInt32(box.x);	\
	pNext = ReadInt32(box.y);	\
	pNext = ReadInt32(box.width);	\
	pNext = ReadInt32(box.height)

#define WriteCvRect(box)	\
	pNext = WriteInt32(box.x);	\
	pNext = WriteInt32(box.y);	\
	pNext = WriteInt32(box.width);	\
	pNext = WriteInt32(box.height)

#define ReadCvPoints(x)	\
	pNext = MetaDataHelper::ReadPoints(pNext, x);

#define WriteCvPoints(x)	\
	pNext = MetaDataHelper::WritePoints(pNext, x);

#define ReadCvScalar(x)	\
	for (int i = 0; i < 4; ++i)	\
		pNext = MetaDataHelper::ReadBytes<double, sizeof(double)>(pNext, x.val[i]);

#define WriteCvScalar(x)	\
	for (int i = 0; i < 4; ++i)	\
		pNext = MetaDataHelper::WriteBytes<double, sizeof(double)>(pNext, x.val[i]);

#define ReadString(x)	\
	pNext = MetaDataHelper::ReadSTDString(pNext, x);

#define WriteString(x)	\
	pNext = MetaDataHelper::WriteSTDString(pNext, x);

	class MetaDataHelper
	{
	private:
		//For memory block
		template<class TByte, int ByteCount>
		static byte* ReadBytes(byte* pBase, TByte& value)
		{
			memset(&value, 0, sizeof(TByte));
			memcpy(&value, pBase, ByteCount);

			return pBase+ByteCount;
		}
		template<class TByte, int ByteCount>
		static byte* WriteBytes(byte* pBase, TByte value)
		{
			memcpy(pBase, &value, ByteCount);
			return pBase+ByteCount;
		}

		static byte* ReadBool(byte* pBase, bool& value)
		{
			value = *pBase == 1;

			return pBase+1;
		}
		static byte* WriteBool(byte* pBase, bool value)
		{
			*pBase = value ? 1 : 0;

			return pBase + 1;
		}

		static byte* ReadSTDString(byte* pBase, std::string& value)
		{
			int nLen = 0;
			byte* pNext = ReadBytes<int, 4>(pBase, nLen);
			if (nLen > 0)
			{
				char* pStr = new char[nLen+1];
				memcpy(pStr, pNext, nLen);
				pStr[nLen] = 0;
				value = pStr;
				delete [] pStr;
				pNext = pNext + nLen;
			}
			return pNext;
		}

		static byte* WriteSTDString(byte* pBase, std::string value)
		{
			byte* pNext = WriteBytes<int, 4>(pBase, (int)value.length());
			memcpy(pNext, value.c_str(), value.length());
			return pNext+value.length();
		}
		static byte* ReadPoints(byte* pBase, Points& pts)
		{
			int nLen = 0;
			byte* pNext = ReadBytes<int, 4>(pBase, nLen);
			if (nLen > 0)
			{
				for (int i = 0; i < nLen; ++i)
				{
					CvPoint pt;
					pNext = ReadBytes<int, 4>(pNext, pt.x);
					pNext = ReadBytes<int, 4>(pNext, pt.y);

					pts.push_back(pt);
				}
			}
			return pNext;
		}

		static byte* WritePoints(byte* pBase, Points& pts)
		{
			byte* pNext = WriteBytes<int, 4>(pBase, (int)pts.size());
			if (pts.size() > 0)
			{
				for (Points::iterator it = pts.begin(); it != pts.end(); ++it)
				{
					pNext = WriteBytes<int, 4>(pNext, it->x);
					pNext = WriteBytes<int, 4>(pNext, it->y);
				}
			}
			return pNext;
		}
		template<class T>
		static byte* Read(byte* pBase, std::vector<T>& values)
		{
			int nLen = 0;
			byte* pNext = ReadBytes<int, 4>(pBase, nLen);
			if (nLen > 0)
			{
				for (int i = 0; i < nLen; ++i)
				{
					T t;
					pNext = t.FromMemory(pNext);
					values.push_back(t);
				}
			}

			return pNext;
		}
		template<class T>
		static byte* Write(byte* pBase, std::vector<T>& values)
		{
			byte* pNext = WriteBytes<int, 4>(pBase, (int)values.size());
			for (std::vector<T>::iterator it = values.begin(); it != values.end(); ++it)
			{
				pNext = it->ToMemory(pNext);
			}

			return pNext;
		}
		static int MemSize(std::string str)
		{
			return 4 + str.length();
		}
		static int MemSize(Points& pts)
		{
			return 4 + pts.size() * 8;
		}
		template<class T>
		static int MemSize(std::vector<T>& values)
		{
			int nSize = 4;
			for (std::vector<T>::iterator it = values.begin(); it != values.end(); ++it)
			{
				nSize += it->memsize();
			}

			return nSize;
		}

	private:
		friend struct ObjectMeta;
		friend struct TextMeta;
		friend struct DrawingStyle;
		friend struct PolyLine;
		friend struct PolygonM;
		friend struct ObjectType;
		friend struct DisplayObjectMeta;
		friend struct FrameDisplayData;
		friend struct FrameMetaData;
	};

	struct ObjectMeta {
		std::string	         id;		// unique object identifier
		std::string           label; // object text label
		std::string           type; // object class 
		bool			 bAlerted;	// object alert tag
		CvRect			 box;
		CvScalar	     color; // color used to draw object tracking / bounding box

		int memsize()
		{
			return MetaDataHelper::MemSize(id) 
				+ MetaDataHelper::MemSize(label)
				+ MetaDataHelper::MemSize(type)
				+ BOOL_SIZE 
				+ INT_SIZE * 4
				+ sizeof(CvScalar);
		}

		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			ReadString(id);
			ReadString(label);
			ReadString(type);
			ReadBOOL(bAlerted);
			ReadInt32(box.x);
			ReadInt32(box.y);
			ReadInt32(box.width);
			ReadInt32(box.height);
			ReadCvScalar(color);

			END_SERIALIZE();
		}

		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			WriteString(id);
			WriteString(label);
			WriteString(type);
			WriteBOOL(bAlerted);
			WriteCvRect(box);
			WriteCvScalar(color);

			END_SERIALIZE();
		}

	};

	struct VideoEvent {
		enum VideoEventLevel { VE_NONE=0, VE_ALERT=1, VE_EVENT=2, VE_STATUS =3, VE_DEBUG= 4 };
		__int64 time_stamp;
		VideoEventLevel event_level;
		std::string event_name;
		Attributes attributes;
		vector<ObjectMeta>	obj_meta;
	};

	struct Event {
		__int64 time_stamp;
		std::string src_id;
		std::string topic;
		Attributes attributes;
	};

	// display meta data
	struct RGBColor {
		int r,g,b; //0-255
	};

	struct Attribute {
		std::string name;
		std::string value;
	};

	struct TextMeta {
		int x, y;
		RGBColor color;
		int size; // size in height
		std::string text;
		int memsize()
		{
			return INT_SIZE + INT_SIZE + RGBCOLOR_SIZE + INT_SIZE + MetaDataHelper::MemSize(text);
		}

		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			ReadInt32(x);
			ReadInt32(y);
			ReadRGBColor(color);
			ReadInt32(size);
			ReadString(text);
			
			END_SERIALIZE();
		}

		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			WriteInt32(x);
			WriteInt32(y);
			WriteRGBColor(color);
			WriteInt32(size);
			WriteString(text);

			END_SERIALIZE();
		}

	};

	struct DrawingStyle {
		RGBColor color;
		int thickness;
		bool bFill;
		RGBColor fill_color;
		int alpha;  // 0 to 100, the actual color = alpha*color+(1-alpha)*background_color
		bool bFlash;
		
		int memsize()
		{
			return RGBCOLOR_SIZE + INT_SIZE + BOOL_SIZE + RGBCOLOR_SIZE + INT_SIZE + BOOL_SIZE;
		}
		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			ReadRGBColor(color);
			ReadInt32(thickness);
			ReadBOOL(bFill);
			ReadRGBColor(fill_color);
			ReadInt32(alpha);
			ReadBOOL(bFlash);
			
			END_SERIALIZE();
		}

		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			WriteRGBColor(color);
			WriteInt32(thickness);
			WriteBOOL(bFill);
			WriteRGBColor(fill_color);
			WriteInt32(alpha);
			WriteBOOL(bFlash);

			END_SERIALIZE();
		}

	} ;

	struct PolyLine {
		RGBColor color;
		int thickness;
		int end_style; // 0 no style; 1: arrow solid, 2: arrow line
		int arrow_length;
		int arrow_width;
		Points lines;
		int memsize()
		{
			return RGBCOLOR_SIZE + 
				   INT_SIZE +
				   INT_SIZE +
				   INT_SIZE +
				   INT_SIZE +
				   MetaDataHelper::MemSize(lines);
		}
		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			ReadRGBColor(color);
			ReadInt32(thickness);
			ReadInt32(end_style);
			ReadInt32(arrow_length);
			ReadInt32(arrow_width);
			ReadCvPoints(lines);

			END_SERIALIZE();
		}

		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			WriteRGBColor(color);
			WriteInt32(thickness);
			WriteInt32(end_style);
			WriteInt32(arrow_length);
			WriteInt32(arrow_width);
			WriteCvPoints(lines);

			END_SERIALIZE();
		}
	};

	struct PolygonM {
		DrawingStyle style;
		Points points;
		int memsize()
		{
			return style.memsize() + MetaDataHelper::MemSize(points);
		}
		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			pNext = style.FromMemory(pBase);
			ReadCvPoints(points);

			END_SERIALIZE();
		}
		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			pNext = style.ToMemory(pBase);
			WriteCvPoints(points);

			END_SERIALIZE();
		}
	};

	struct ObjectType {  
		DrawingStyle style;
		// object: -1: no display, 0: line_segment, 1: rectangle, 2: ellipse
		int type; 
		int x0, y0, x1,y1;
		int x2,y2, x3, y3;  // for use with ellipse only, 4 points bounds the ellipse

		int memsize()
		{
			return style.memsize() + INT_SIZE +
				INT_SIZE + INT_SIZE + INT_SIZE + INT_SIZE +
				INT_SIZE + INT_SIZE + INT_SIZE + INT_SIZE;
		}
		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			pNext = style.FromMemory(pNext);
			ReadInt32(type);
			ReadInt32(x0);
			ReadInt32(y0);
			ReadInt32(x1);
			ReadInt32(y1);
			ReadInt32(x2);
			ReadInt32(y2);
			ReadInt32(x3);
			ReadInt32(y3);

			END_SERIALIZE();
		}
		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			pNext = style.ToMemory(pNext);
			WriteInt32(type);
			WriteInt32(x0);
			WriteInt32(y0);
			WriteInt32(x1);
			WriteInt32(y1);
			WriteInt32(x2);
			WriteInt32(y2);
			WriteInt32(x3);
			WriteInt32(y3);

			END_SERIALIZE();
		}

	};

	struct ObjectLoc {
		int x,y,w,h;
	};

	struct DisplayObjectMeta {
		std::string id;
		ObjectType obj;  // 
		PolyLine track;  // track of the objecct

		int memsize()
		{
			return MetaDataHelper::MemSize(id) +
					obj.memsize() +
					track.memsize();
		}
		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			ReadString(id);
			pNext = obj.FromMemory(pNext);
			pNext = track.FromMemory(pNext);

			END_SERIALIZE();
		}
		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			WriteString(id);
			pNext = obj.ToMemory(pNext);
			pNext = track.ToMemory(pNext);

			END_SERIALIZE();
		}
	};

	typedef std::vector <DisplayObjectMeta> DisplayObjectMetaList; // display object info
	typedef std::vector <PolyLine> PolyLines;

	typedef std::vector <TextMeta> Texts;     // a list of texts
	typedef std::vector <PolygonM> Polygons;     // a list of polygons
	typedef std::vector <ObjectType> GUIObjects;     // a list of basic objects

	struct FrameDisplayData {
		int image_width;
		int image_height;
		DisplayObjectMetaList disp_obj_list;

		PolyLines line_list;
		Texts text_list;
		Polygons polygon_list;
		GUIObjects gui_object_list;

		int memsize()
		{
			
			return  INT_SIZE +
					INT_SIZE +
					MetaDataHelper::MemSize<DisplayObjectMeta>(disp_obj_list) +
					MetaDataHelper::MemSize<PolyLine>(line_list) +
					MetaDataHelper::MemSize<TextMeta>(text_list) +
					MetaDataHelper::MemSize<PolygonM>(polygon_list) +
					MetaDataHelper::MemSize<ObjectType>(gui_object_list);
		}

		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			ReadInt32(image_width);
			ReadInt32(image_height);
			pNext = MetaDataHelper::Read<DisplayObjectMeta>(pNext, disp_obj_list);
			pNext = MetaDataHelper::Read<PolyLine>(pNext, line_list);
			pNext = MetaDataHelper::Read<TextMeta>(pNext, text_list);
			pNext = MetaDataHelper::Read<PolygonM>(pNext, polygon_list);
			pNext = MetaDataHelper::Read<ObjectType>(pNext, gui_object_list);

			END_SERIALIZE();
		}
		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			WriteInt32(image_width);
			WriteInt32(image_height);
			pNext = MetaDataHelper::Write<DisplayObjectMeta>(pNext, disp_obj_list);
			pNext = MetaDataHelper::Write<PolyLine>(pNext, line_list);
			pNext = MetaDataHelper::Write<TextMeta>(pNext, text_list);
			pNext = MetaDataHelper::Write<PolygonM>(pNext, polygon_list);
			pNext = MetaDataHelper::Write<ObjectType>(pNext, gui_object_list);

			END_SERIALIZE();
		}

		void clear()
		{
			disp_obj_list.clear();
			line_list.clear();
			text_list.clear();
			polygon_list.clear();
			gui_object_list.clear();
		}
	};

	struct FrameMetaData {
		FrameDisplayData displayData;    // For display
		Attributes attributes;       // additional attributes

		// FromMemory
		//	Read data from memory.
		byte* FromMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			pNext = displayData.FromMemory(pBase);
			int nSize = 0;
			ReadInt32(nSize);
			if (nSize > 0)
			{
				for (int i = 0; i <nSize; ++i)
				{
					std::string key, val;
					ReadString(key);
					ReadString(val);

					attributes[key] = val;
				}
			}

			END_SERIALIZE();
		}

		// memsize
		//	calculate the FrameMetaData struct size (byte unit).
		//	Alloc the memory according to this method.
		//	e.g.
		//		FrameMetaData fmd;
		//		fmd...	do some work.
		//		byte* pFMDBlock = new byte[fmd.memsize()];
		//		fmd.ToMemory(pFMDBlock);
		int memsize()
		{
			int size = displayData.memsize();
			size += 4;

			for (Attributes::iterator it = attributes.begin(); it != attributes.end(); ++it)
			{
				size += MetaDataHelper::MemSize(it->first);
				size += MetaDataHelper::MemSize(it->second);
			}
			
			return size;
		}

		// ToMemory
		//	Write the data to memory.
		byte* ToMemory(byte* pBase)
		{
			BEGIN_SERIALIZE(pBase);

			pNext = displayData.ToMemory(pBase);
			WriteInt32((int)attributes.size());
			for(Attributes::iterator it = attributes.begin(); it != attributes.end(); ++it)
			{
				WriteString(it->first);
				WriteString(it->second);
			}

			END_SERIALIZE();
		}

		void clear()
		{
			displayData.clear();
			attributes.clear();
		}
	};

	typedef std::vector <FrameMetaData > FrameMetaDataList;   

};

#define ATTRIBUTE_REGION "regions"

