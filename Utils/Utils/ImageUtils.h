#pragma once
#include <string>
#include "win.h"

#include <gdiplus.h>
#pragma comment(lib,"Gdiplus.lib")

namespace Utils {
	namespace ImageUtils {
		int GetEncoderClsid(const std::wstring& format, CLSID* pClsid);
		std::wstring GetFormatType(const std::wstring& fileName);
		bool ImageTo(const std::wstring& from, const std::wstring& to);
	}
}