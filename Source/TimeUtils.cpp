#include "stdafx.h"
#include "Utils.h"


namespace Utils {
	namespace TimeUtils {
		time_t TimetFromFileTime(FILETIME ft)
		{
			time_t t;

			ULARGE_INTEGER            ui;
			ui.LowPart = ft.dwLowDateTime;
			ui.HighPart = ft.dwHighDateTime;

			t = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);

			return t;
		}

		FILETIME TimetToFileTime(time_t time)
		{
			FILETIME ft;

			LONGLONG nLL = Int32x32To64(time, 10000000) + 116444736000000000;
			ft.dwLowDateTime = (DWORD)nLL;
			ft.dwHighDateTime = (DWORD)(nLL >> 32);
			return ft;
		}

		time_t TimetFromSystemTime(SYSTEMTIME sTime)
		{
			FILETIME ft;
			SystemTimeToFileTime(&sTime, &ft);

			return TimetFromFileTime(ft);

		}

		SYSTEMTIME TimetToSystemTime(time_t time)
		{
			FILETIME fTime = TimetToFileTime(time);

			SYSTEMTIME sTime;
			FileTimeToSystemTime(&fTime, &sTime);

			return sTime;
		}

		time_t GetCurrentSystemTime()
		{
			SYSTEMTIME sTime;
			GetLocalTime(&sTime);
			time(nullptr);

			return TimetFromSystemTime(sTime);
		}

		time_t GetCurrentSystemTimeUTC()
		{
			SYSTEMTIME sTime;
			GetSystemTime(&sTime);
			time(nullptr);

			return TimetFromSystemTime(sTime);
		}

		FILETIME SystemTimeToFileTime(SYSTEMTIME sTime)
		{
			FILETIME fTime;
			BOOL result = ::SystemTimeToFileTime(&sTime, &fTime);

			return result ? fTime : FILETIME{ 0 };
		}

		SYSTEMTIME SysTemTimeFromFileTime(FILETIME fTime)
		{
			SYSTEMTIME sTime;
			BOOL result = ::FileTimeToSystemTime(&fTime, &sTime);

			return result ? sTime : SYSTEMTIME{ 0 };
		}


		//strftime time format
		/*
		%a 星期几的简写
		%A 星期几的全称
		%b 月分的简写
		%B 月份的全称
		%c 标准的日期的时间串
		%C 年份的后两位数字
		%d 十进制表示的每月的第几天
		%D 月/天/年
		%e 在两字符域中，十进制表示的每月的第几天
		%F 年-月-日
		%g 年份的后两位数字，使用基于周的年
		%G 年分，使用基于周的年
		%h 简写的月份名
		%H 24小时制的小时
		%I 12小时制的小时
		%j 十进制表示的每年的第几天
		%m 十进制表示的月份
		%M 十时制表示的分钟数
		%n 新行符
		%p 本地的AM或PM的等价显示
		%r 12小时的时间
		%R 显示小时和分钟：hh:mm
		%S 十进制的秒数
		%t 水平制表符
		%T 显示时分秒：hh:mm:ss
		%u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
		%U 第年的第几周，把星期日做为第一天（值从0到53）
		%V 每年的第几周，使用基于周的年
		%w 十进制表示的星期几（值从0到6，星期天为0）
		%W 每年的第几周，把星期一做为第一天（值从0到53）
		%x 标准的日期串
		%X 标准的时间串
		%y 不带世纪的十进制年份（值从0到99）
		%Y 带世纪部分的十进制年份
		%z，%Z 时区名称，如果不能得到时区名称则返回空字符。
		%% 百分号
		*/
		std::wstring FormatTime(time_t time, const std::wstring& format)
		{
			wchar_t buf[64];

			struct tm tm_time;
			gmtime_s(&tm_time, &time);
			wcsftime(buf, 64, format.c_str(), &tm_time);
			return buf;
		}
	}
}