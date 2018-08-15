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
		%a ���ڼ��ļ�д
		%A ���ڼ���ȫ��
		%b �·ֵļ�д
		%B �·ݵ�ȫ��
		%c ��׼�����ڵ�ʱ�䴮
		%C ��ݵĺ���λ����
		%d ʮ���Ʊ�ʾ��ÿ�µĵڼ���
		%D ��/��/��
		%e �����ַ����У�ʮ���Ʊ�ʾ��ÿ�µĵڼ���
		%F ��-��-��
		%g ��ݵĺ���λ���֣�ʹ�û����ܵ���
		%G ��֣�ʹ�û����ܵ���
		%h ��д���·���
		%H 24Сʱ�Ƶ�Сʱ
		%I 12Сʱ�Ƶ�Сʱ
		%j ʮ���Ʊ�ʾ��ÿ��ĵڼ���
		%m ʮ���Ʊ�ʾ���·�
		%M ʮʱ�Ʊ�ʾ�ķ�����
		%n ���з�
		%p ���ص�AM��PM�ĵȼ���ʾ
		%r 12Сʱ��ʱ��
		%R ��ʾСʱ�ͷ��ӣ�hh:mm
		%S ʮ���Ƶ�����
		%t ˮƽ�Ʊ��
		%T ��ʾʱ���룺hh:mm:ss
		%u ÿ�ܵĵڼ��죬����һΪ��һ�� ��ֵ��0��6������һΪ0��
		%U ����ĵڼ��ܣ�����������Ϊ��һ�죨ֵ��0��53��
		%V ÿ��ĵڼ��ܣ�ʹ�û����ܵ���
		%w ʮ���Ʊ�ʾ�����ڼ���ֵ��0��6��������Ϊ0��
		%W ÿ��ĵڼ��ܣ�������һ��Ϊ��һ�죨ֵ��0��53��
		%x ��׼�����ڴ�
		%X ��׼��ʱ�䴮
		%y �������͵�ʮ������ݣ�ֵ��0��99��
		%Y �����Ͳ��ֵ�ʮ�������
		%z��%Z ʱ�����ƣ�������ܵõ�ʱ�������򷵻ؿ��ַ���
		%% �ٷֺ�
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