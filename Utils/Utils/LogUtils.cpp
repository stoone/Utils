#include "LogUtils.h"
#include <time.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include  <io.h>
#include "Utils.h"

//#ifdef _USE_32BIT_TIME_T
//#undef  _USE_32BIT_TIME_T
//#endif

bool isFileExist(const std::string& fileName) {
	return (_access(fileName.c_str(), 0)) != -1;
}

std::string getTimeStamp() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t st = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&st);
	auto stamp = std::put_time(&tm, "%F %T");

	std::stringstream ss;
	ss << stamp;

	std::string sTime(ss.str());
	return sTime;
}

time_t getCreateTime(const std::string& filePath) {
	struct _stat buf;
	int result;

	result = _stat(filePath.c_str(), &buf);
	if (result == 0)
	{
		return buf.st_ctime;
	}
	return 0;
}


Log::Log() {

}

Log::~Log() {

}

Log* instance = nullptr;
std::mutex _mtx;
Log* Log::getInstance() {
	/*if (instance == nullptr)
	{
		std::lock_guard<std::mutex> lk(_mtx);
		if (instance == nullptr)
		{
			instance = new Log();
		}
	}*/
	static Log instance;
	return &instance;
}

void Log::release() {
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}

std::string Log::getLevelText(LOG_LEVEL level) {
	std::string text = "UNKNOWN";
	if (level == LOG_LEVEL::LOG_LEVEL_DEBUG)
		text = "DEBUG";
	else if (level == LOG_LEVEL::LOG_LEVEL_INFO)
		text = "INFO";
	else if (level == LOG_LEVEL::LOG_LEVEL_TRACE)
		text = "TRACE";
	else if (level == LOG_LEVEL::LOG_LEVEL_WARN)
		text = "WARN";
	else if (level == LOG_LEVEL::LOG_LEVEL_ERROR)
		text = "ERROR";
	else if (level == LOG_LEVEL::LOG_LEVEL_FATAL)
		text = "FATAL";

	return text;
}

void Log::log(LOG_LEVEL level, const char* file, int nLineNo, const wchar_t* lpszFmt, ...) {
	if (level < LogUtils::getInstance()->getDefaultLevel())
		return;

	va_list ap;
	va_start(ap, lpszFmt);
	int nLogMsgLength = _vsctprintf(lpszFmt, ap);

	wchar_t* pBuffer = new wchar_t[nLogMsgLength + 1];
	wmemset(pBuffer, 0, nLogMsgLength + 1);
	vswprintf_s(pBuffer, nLogMsgLength + 1, lpszFmt, ap);
	va_end(ap);

	std::string info = Utils::UnicodeToAnsi(pBuffer);
	if (pBuffer)
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}

	log(level, file, nLineNo, info);
}

void Log::log(LOG_LEVEL level, const char* file, int nLineNo, const char* pszFmt, ...) {
	if (level < LogUtils::getInstance()->getDefaultLevel())
		return;

	va_list ap;
	va_start(ap, pszFmt);
	int nLogMsgLength = _vscprintf(pszFmt, ap);

	char* pBuffer = new char[nLogMsgLength + 1];
	memset(pBuffer, 0, nLogMsgLength + 1);
	vsprintf_s(pBuffer, nLogMsgLength + 1, pszFmt, ap);
	va_end(ap);

	std::string info(pBuffer);
	if (pBuffer)
	{
		delete[] pBuffer;
		pBuffer = nullptr;
	}

	log(level, file, nLineNo, info);
}

void Log::log(LOG_LEVEL level, const char* file, int nLineNo, const std::string& info) {

	char buffer[10] = { 0 };
	_itoa(nLineNo, buffer, 10);
	std::string strLogMsg = getTimeStamp() + ", " + getLevelText(level) + ", " + file + ": " + buffer + ", " + info;



	LogUtils::getInstance()->setLog(strLogMsg);
}

LogUtils::LogUtils()
{
	start();
}


LogUtils::~LogUtils()
{
	stop();
}

LogUtils* g_instance = nullptr;
std::mutex _mutex;
LogUtils* LogUtils::getInstance() {
	if (g_instance == nullptr)
	{
		std::lock_guard<std::mutex> lk(_mutex);
		if (g_instance == nullptr)
		{
			g_instance = new LogUtils();
		}
	}
	return g_instance;
}

void LogUtils::release() {
	if (g_instance)
	{
		LogUtils::getInstance()->stop();
		Log::release();

		delete g_instance;
		g_instance = nullptr;
	}
}

void LogUtils::start() {
	if (_running) return;

	_running = true;
	_thread = std::thread([this]() {
		this->getLogFile();
		while (true)
		{
			std::string log;
			while (getLog(log))
			{
				writeLog(log);
			}

			if (!_running && _logList.empty())
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}		
	});
}

void LogUtils::stop() {
	if (_running)
	{
		_running = false;
		if (_thread.joinable())
		{
			_thread.join();
		}

		if (_fp)
		{
			fclose(_fp);
			_fp = nullptr;
		}
	}
}

void LogUtils::init(LOG_LEVEL level, const std::string& fileName) {
	int pos = fileName.find_last_of('\\');
	if (pos != std::string::npos)
	{
		_sPath = fileName.substr(0, pos);
		_sName = fileName.substr(pos);

		if (_sPath.empty())
		{
			_sPath = DEFAULT_PATH;
		}
	}

	_sFileName = _sPath + _sName;
	_defaultLevel = level;
}

bool LogUtils::getLog(std::string& log) {
	std::lock_guard<std::recursive_mutex> lk(_mtx);
	if (_logList.empty())
		return false;

	log = _logList.front();
	_logList.pop_front();
	return true;
}

void LogUtils::setLog(const std::string& log) {
	std::lock_guard<std::recursive_mutex> lk(_mtx);
	_logList.push_back(log);

	int size = _logList.size();
	size++;
}

bool LogUtils::isFileToday() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t st = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&st);

	time_t t = getCreateTime(_sFileName);
	if (t == 0)
	{
		return false;
	}

	std::tm tt = *std::localtime(&t);

	if (tm.tm_year == tt.tm_year && tm.tm_mon == tt.tm_mon && tm.tm_mday == tt.tm_mday)
	{
		return true;
	}

	return false;
}

std::string LogUtils::getLogFile() {
	if (isFileExist(_sFileName))
	{
		bool isToday = isFileToday();
		if (!isToday)
		{
			int pos = _sFileName.rfind('\\');
			if (pos != std::string::npos)
			{
				std::string path = _sFileName.substr(0, pos + 1);
				std::string filename = _sFileName.substr(pos + 1);
				pos = filename.rfind('.');
				if (pos != std::string::npos)
				{
					std::string name = filename.substr(0, pos);
					std::string ext = filename.substr(pos);

	// 				time_t tm;
	// 				time(&tm);

					std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
					std::time_t st = std::chrono::system_clock::to_time_t(today);
					std::tm tm = *std::localtime(&st);
					auto stamp = std::put_time(&tm, "%F");

					std::stringstream ss;
					ss << stamp;

	// 				char tmp[64];
	// 				strftime(tmp, sizeof(tmp), "%Y-%m-%d", localtime(&tm));
				
					std::string newName = path + name + ss.str() + ext;
					::rename(_sFileName.c_str(), newName.c_str());
				}
			}
		}
	}

	std::string sFileName = _sPath + _sName;
	_fp = fopen(sFileName.c_str(), "ab");

	return sFileName;
}

bool LogUtils::writeLog(std::string& log) {
	if (_fp)
	{
		std::string p = log.substr(log.length() - 2);
		if (log.substr(log.length() - 2) != "\r\n")
		{
			log += "\r\n";
		}

		fwrite(log.c_str(), log.length(), 1, _fp);
		fflush(_fp);
	}

	return true;
}

