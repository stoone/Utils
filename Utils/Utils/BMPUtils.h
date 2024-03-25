#pragma once
#include <string>
#include "win.h"

namespace Utils {
	class BMPUtils {
	public:
		BMPUtils()
		{
		};
		~BMPUtils()
		{
			FreeBMP();
		};

		static BMPUtils* get();
		static void Release();

		bool LoadBMP(const std::string& bmpFile);
		bool SaveRawData(const std::string& file);
		bool GetRawDataFromFile(const std::string& bmpFile, unsigned char* pRawData, int& width, int& height);
		void GetRawData(unsigned char* pRawData, int len);
		bool CutBMP(const std::string& file, const std::string& newFile, int x, int y, int width, int height);
		bool CutRawData(unsigned char* pRawData, unsigned char* pDataOut, int width, int height, int x, int y, int cutWidth, int cutHeight);
		int GetWidth();
		int GetHeight();
		void FreeBMP();

		static bool SaveRawDataFile(unsigned char* pRawData, const std::string& file, int width, int height);

	private:
		unsigned char* _rawData{ nullptr };
		RGBQUAD _palette[256] = { 0 };
		BITMAPFILEHEADER _fileHeader;
		BITMAPINFOHEADER _infoHeader;
		int m_iWidth{ 0 };
		int m_iHeight{ 0 };
		bool m_bPalette{ false };
	};
}

