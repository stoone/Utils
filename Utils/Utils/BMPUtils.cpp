#include "BMPUtils.h"
#include "Utility.h"
#include "Utils.h"

//#define   WIDTHBYTES(bits) (((bits)+31)/32*4)//用于使图像宽度所占字节数为4byte的倍数

namespace Utils {

	BMPUtils* g_bmpUtils = nullptr;

	BMPUtils* BMPUtils::get()
	{
		if (g_bmpUtils == nullptr)
		{
			g_bmpUtils = new BMPUtils();
		}

		return g_bmpUtils;
	}

	void BMPUtils::Release()
	{
		if (g_bmpUtils)
		{
			g_bmpUtils->FreeBMP();
			delete g_bmpUtils;
			g_bmpUtils = nullptr;
		}
	}

	inline int WIDTHBYTES(int width)
	{
		return (width + 3) / 4 * 4;
	}

	inline int MEMALIGN(int width)
	{
		return  (width / 4 + (width % 4 ? 1 : 0)) * 4;
	}

	bool BMPUtils::LoadBMP(const std::string& bmpFile)
	{
		if (Utils::FileUtils::FileExists(Utils::AnsiToUnicode(bmpFile).c_str()) != 1)  return false;

		FreeBMP();

		unsigned char* lineData = nullptr;
		int ex = 0;
		int iWidthAlign = m_iWidth;

		int size = 0;
		FILE* file = nullptr;
		errno_t err = fopen_s(&file, bmpFile.c_str(), "rb");
		if (err != 0) goto _FAILED;

		// read BITMAPFILEHEADER
		fseek(file, 0, SEEK_SET);
		size = fread(&_fileHeader, 1, sizeof(BITMAPFILEHEADER), file);
		if (_fileHeader.bfType != 0x4D42 || size != sizeof(BITMAPFILEHEADER) ||
			_fileHeader.bfOffBits < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
		{
			goto _FAILED;
		}

		// read BITMAPINFOHEADER
		size = fread(&_infoHeader, 1, sizeof(BITMAPINFOHEADER), file);
		if (size != sizeof(BITMAPINFOHEADER))
		{
			goto _FAILED;
		}
		m_iWidth = ::abs(_infoHeader.biWidth);
		m_iHeight = ::abs(_infoHeader.biHeight);

		// read color palette
		if (_fileHeader.bfOffBits == sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 1024)
		{
			m_bPalette = true;
			for (unsigned int i = 0; i < 256; i++)
			{
				//存储的时候，一般去掉保留字rgbReserved   
				fread((char *)&_palette[i].rgbBlue, 1, sizeof(BYTE), file);
				fread((char *)&_palette[i].rgbGreen, 1, sizeof(BYTE), file);
				fread((char *)&_palette[i].rgbRed, 1, sizeof(BYTE), file);
				fread((char *)&_palette[i].rgbReserved, 1, sizeof(BYTE), file);
			}
		}

//		long lset = ftell(file);

		iWidthAlign = MEMALIGN(m_iWidth);
		ex = iWidthAlign - m_iWidth;     // 因为内存对齐，每一行填充的字节数
		_rawData = new unsigned char[iWidthAlign * m_iHeight];
		memset(_rawData, 0, m_iWidth * m_iHeight);
		lineData = new unsigned char[iWidthAlign];
		memset(lineData, 0, iWidthAlign);
		for (int i = 0; i < m_iHeight; i++)
		{
			memset(lineData, 0, iWidthAlign);
			fread(lineData, sizeof(unsigned char), iWidthAlign, file);
			if (_infoHeader.biHeight > 0)
				memcpy(_rawData + (m_iHeight - i - 1) * m_iWidth, lineData, m_iWidth);
			else
				memcpy(_rawData + i * m_iWidth, lineData, m_iWidth);
		}

		if (lineData)
		{
			delete[] lineData;
			lineData = nullptr;
		}
	
_FAILED:
		if (file) fclose(file);

		return false;
	}

	bool BMPUtils::GetRawDataFromFile(const std::string& bmpFile, unsigned char* pRawData, int& width, int& height)
	{
		if (pRawData == nullptr) return false;
		if (!LoadBMP(bmpFile)) return false;

		memcpy(pRawData, _rawData, width * height);
		return true;
	}


	void BMPUtils::FreeBMP()
	{
		if (_rawData)
		{
			delete[] _rawData;
			_rawData = nullptr;
		}

// 		RELEASE_MEM_SAFE(_bmpBuffer);
// 		RELEASE_MEM_SAFE(_rawData);
	}

	bool BMPUtils::SaveRawData(const std::string& file)
	{
		CHECK_ZERO_FALSE(_rawData);

		FILE* p = fopen("e:\\test\\1.raw", "wb");
		if (p)
		{
			fwrite(_rawData, 1, m_iWidth * m_iHeight, p);
			fclose(p);
		}
		return true;
		
		return SaveRawDataFile(_rawData, file,m_iWidth, m_iHeight);
	}

	
/*	void BMPUtils::ShowBMP(CDC* pDc, const std::string& bmpFile, const CRect& rc)
	{
		HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, Utils::StringUtils::AnsiToUnicode(bmpFile).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if (hBitmap == nullptr) return;

		CBitmap cBit;

		cBit.Attach(hBitmap);

		CDC MemDC;

		MemDC.CreateCompatibleDC(pDc); //创建与当前设备描述表相适应的内存DC

		BITMAP bitmap;

		cBit.GetBitmap(&bitmap);

		CBitmap *oldBit;

		oldBit = MemDC.SelectObject(&cBit);

		//	pDc->BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);  // 原尺寸
		pDc->StretchBlt(0, 0, rc.Width(), rc.Height(), &MemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY); //拉伸
	}
*/

	void BMPUtils::GetRawData(unsigned char* pRawData, int len)
	{
		if (pRawData == nullptr) return;

		memcpy(pRawData, _rawData, len);
	}

	int BMPUtils::GetWidth()
	{
		return m_iWidth;
	}

	int BMPUtils::GetHeight()
	{
		return m_iHeight;
	}

	// pRawData 为灰度值
	// 24位图，就是说一个像素的颜色信息用24位来表示，也就是说，对于三原色BRG，每一个颜色都用以字节（8）位来表示。
	// 除了24位图，还有1位（单色），2位（4色，CGA），4位（16色，VGA），8位（256色），16位（增强色），24位（真彩色）和32位等
	bool BMPUtils::SaveRawDataFile(unsigned char* pRawData, const std::string& file, int width, int height)
	{
		/*BITMAPINFOHEADER中biBitCount为每个像素所需的位数
			当biBitCount = 1时，8个像素占1个字节；
			当biBitCount = 4时，2个像素占1个字节；
			当biBitCount = 8时，1个像素占1个字节；
			当biBitCount = 24时，1个像素占3个字节；*/
		/*当biBitCount = 1时，为2色图像，BMP位图中有2个数据结构RGBQUAD，一个调色板占用4字节数据，所以2色图像的调色板长度为2 * 4为8字节。
			当biBitCount = 4时，为16色图像，BMP位图中有16个数据结构RGBQUAD，一个调色板占用4字节数据，所以16像的调色板长度为16 * 4为64字节。
			当biBitCount = 8时，为256色图像，BMP位图中有256个数据结构RGBQUAD，一个调色板占用4字节数据，所以256色图像的调色板长度为256 * 4为1024字节。
			当biBitCount = 16，24或32时，没有颜色表。*/
		// 每个像素点几个字节，8位图是1，24位图是3.

		const int count = 1;
		const int biBitCount = 8;
		//位图文件头结构     
		BITMAPFILEHEADER   bmfHdr;
		//位图信息头结构       
		BITMAPINFOHEADER   bi;
		unsigned char* pFillin = nullptr;

		DWORD dwPaletteSize = 256 * 4;
		int nLineByte = WIDTHBYTES(width);
		DWORD dwBmBitsSize = nLineByte * height;
		int ex = nLineByte - width;

		FILE * fp = NULL;
		errno_t err = fopen_s(&fp, file.c_str(), "wb");
		if (err != 0) return false;

		bmfHdr.bfType = 0x4D42;   //   ;BM;  
		DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
		bmfHdr.bfSize = dwDIBSize;
		bmfHdr.bfReserved1 = 0;
		bmfHdr.bfReserved2 = 0;
		bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

		fwrite(&bmfHdr, 8, 1, fp);
		fwrite(&bmfHdr.bfReserved2, sizeof(bmfHdr.bfReserved2), 1, fp);
		fwrite(&bmfHdr.bfOffBits, sizeof(bmfHdr.bfOffBits), 1, fp);

		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = biBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrImportant = 0;
		bi.biClrUsed = 0;

		fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, fp);


		RGBQUAD *pColorTable = new RGBQUAD[256];
		memset(pColorTable, 0, 256);
		for (int i = 0; i < 256; i++)
		{
			pColorTable[i].rgbBlue = i; // _palette[i].rgbBlue;
			pColorTable[i].rgbGreen = i; // _palette[i].rgbGreen;
			pColorTable[i].rgbRed = i; // _palette[i].rgbRed;
			pColorTable[i].rgbReserved = 0;
		}
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);

		delete pColorTable;
		pColorTable = nullptr;


		for (int i = 0; i < height; i++)
		{
			fwrite(pRawData + (height - i - 1) * width, 1, nLineByte, fp);
		}
//		fwrite(pRawData, dwBmBitsSize, 1, fp);
		fclose(fp);

		return true;
	}

	bool BMPUtils::CutBMP(const std::string& file, const std::string& newFile, int x, int y, int width, int height)
	{
		bool ret = LoadBMP(file);
		if (!ret) return false;

		BITMAPFILEHEADER fileHeader;
		fileHeader = _fileHeader;

		BITMAPINFOHEADER infoHeader;
		infoHeader = _infoHeader;

		infoHeader.biHeight = height;
		infoHeader.biWidth = width;

		int mywritewidth = WIDTHBYTES(infoHeader.biWidth*_infoHeader.biBitCount);//BMP图像实际位图数据区的宽度为4byte的倍数，在此计算实际数据区宽度
		infoHeader.biSizeImage = mywritewidth * infoHeader.biHeight;//计算位图实际数据区大小

		fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * 4 + _infoHeader.biSizeImage;

		int l_width = WIDTHBYTES(width*_infoHeader.biBitCount);//计算位图的实际宽度并确保它为4byte的倍数
		int write_width = WIDTHBYTES(infoHeader.biWidth*infoHeader.biBitCount);//计算写位图的实际宽度并确保它为4byte的倍数

		BYTE    *pColorDataMid = (BYTE *)malloc(mywritewidth*height);//开辟内存空间存储图像处理之后数据
		memset(pColorDataMid, 0, mywritewidth*height);

		long write_nData = mywritewidth * width;//截取的位图数据区长度定义

		for (int hnum = _infoHeader.biHeight - y - height; hnum < _infoHeader.biHeight - y; hnum++)//由于BMP图像的数据存储格式起点是图像的左下角，所以需要进行坐标换算操作
			for (int wnum = x; wnum < x + width; wnum++)
			{
				int pixel_point = hnum * l_width + wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点
				int write_pixel_point = (hnum - _infoHeader.biHeight + y + height)*mywritewidth + (wnum - x) * 3;
				pColorDataMid[write_pixel_point] = _rawData[pixel_point];
				pColorDataMid[write_pixel_point + 1] = _rawData[pixel_point + 1];
				pColorDataMid[write_pixel_point + 2] = _rawData[pixel_point + 2];
			}

//		ret = SaveRawDataFile(pColorDataMid, newFile, width, height);

		char strFilesave[50] = "E:\\test\\test_cut.bmp";//处理后图像存储路径
		FILE * wfile = NULL;
		errno_t err = fopen_s(&wfile, strFilesave, "wb");
		if (err != 0) return false;

// 		WORD fileType = 0x4d42;
// 		fwrite(&fileType, 1, sizeof(WORD), wfile);
		
		fwrite(&fileHeader, 8, 1, wfile);//写回位图文件头信息到输出文件
		fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, wfile);//写回位图信息头信息到输出文件
		fwrite(pColorDataMid, 1, write_nData, wfile);
		fclose(wfile);

		return ret;
	}

	bool BMPUtils::CutRawData(unsigned char* pRawData, unsigned char* pDataOut, int width, int height, int x, int y, int cutWidth, int cutHeight)
	{
		if (x < 0 || y < 0 || width <= 0 || height <= 0 || cutWidth <= 0 || cutHeight <= 0)
			return false;

		if (x + cutWidth > width || y + cutHeight > height) return false;

		if (pRawData == nullptr || pDataOut) return false;

		return true;
	}

}

