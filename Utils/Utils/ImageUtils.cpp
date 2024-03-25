#include "ImageUtils.h"
#include "Utils.h"

namespace Utils {
	namespace ImageUtils {
		int GetEncoderClsid(const std::wstring& format, CLSID* pClsid)
		{
			UINT num = 0;
			UINT size = 0;

			Gdiplus::Status s = Gdiplus::Status::Ok;
			Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;
			s = Gdiplus::GetImageEncodersSize(&num, &size);
			if (s != Gdiplus::Status::Ok || size == 0) 
				return -1;


			pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
			if (pImageCodecInfo == nullptr) return -1;

			s = Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
			if (s != Gdiplus::Status::Ok) return -1;

			int index = -1;
			for (UINT i = 0; i < num; ++i)
			{
				if (_tcscmp(pImageCodecInfo[i].MimeType, format.c_str()) == 0)
				{
					index = i;
					*pClsid = pImageCodecInfo[i].Clsid;
					break;
				}
			}

			free(pImageCodecInfo);
			return index;
		}

		std::wstring GetFormatType(const std::wstring& fileName)
		{
			std::wstring type = L"";
			std::wstring ext = FileUtils::fileExt(fileName);
			if (!ext.empty())
			{
				ext = Utils::WString::toUpper(ext);
				if (ext.compare(L".BMP") == 0)
					type = L"image/bmp";
				else if (ext.compare(L".JPEG") == 0)
					type = L"image/jpeg";
				else if (ext.compare(L".PNG") == 0)
					type = L"image/png";
				else if (ext.compare(L".TIFF") == 0)
					type = L"image/tiff";
				else if (ext.compare(L".GIF") == 0)
					type = L"image/gif";
			}

			return type;
		}


		bool ImageTo(const std::wstring& from, const std::wstring& to)
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;

			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			CLSID encoderClsid;
			Gdiplus::Status s;

			std::wstring format = GetFormatType(to);
			int r = GetEncoderClsid(format, &encoderClsid);
			if (r == -1) return false;

			Gdiplus::Image* pImage = new Gdiplus::Image(from.c_str());
			if (pImage == nullptr) return false;

			s = pImage->Save(to.c_str(), &encoderClsid, NULL);
			if (pImage)
			{
				delete pImage;
				pImage = nullptr;
			}

			Gdiplus::GdiplusShutdown(gdiplusToken);

			return s == Gdiplus::Status::Ok;
		}
	}
}