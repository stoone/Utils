#pragma once
#include <string>
#include "win.h"

namespace Utils {
	namespace TimeUtils {

		time_t TimetFromFileTime(FILETIME ftime);
		FILETIME TimetToFileTime(time_t time);

		time_t TimetFromSystemTime(SYSTEMTIME sTime);
		SYSTEMTIME TimetToSystemTime(time_t time);

		FILETIME SystemTimeToFileTime(SYSTEMTIME sTime);
		SYSTEMTIME SystemTimeFromFileTime(FILETIME fTime);
		time_t GetCurrentSystemTimeLocal();

		// these two are same
		time_t GetCurrentSystemTimeUTC();

		time_t getCurrentTime();

		uint64_t GetCurrentTimeMillSeconds();

		std::wstring GetTimeStamp();

		std::wstring FormatTime(time_t time, const std::wstring& format = L"%Y-%m-%d %H:%M:%S");
	}
}