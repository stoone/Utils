#pragma once
#include <string>
#include <list>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <stdio.h>

#define LOGD(fmt, ...)  Log::getInstance()->log(LOG_LEVEL::LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGT(fmt,...)  Log::getInstance()->log(LOG_LEVEL::LOG_LEVEL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGI(fmt,...)  Log::getInstance()->log(LOG_LEVEL::LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGW(fmt,...)  Log::getInstance()->log(LOG_LEVEL::LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGE(fmt,...)  Log::getInstance()->log(LOG_LEVEL::LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGF(fmt,...)  Log::getInstance()->log(LOG_LEVEL::LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
//#define LOG(fmt,...)  Log::log(__FILE__, __LINE__, ##__VA_ARGS__)

#define DEFAULT_PATH  ".\\"

enum class LOG_LEVEL {
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
};

class Log {
public:
	Log();
	~Log();

public:
	static Log* getInstance();
	static void release();
	void log(LOG_LEVEL level, const char* file, int nLineNo, const std::string& info);
	void log(LOG_LEVEL level, const char* file, int nLineNo, const char* pszFmt, ...);
	void log(LOG_LEVEL level, const char* file, int nLineNo, const wchar_t* lpszFmt, ...);

private:
	std::string getLevelText(LOG_LEVEL level);
};

class LogUtils {
public:
	LogUtils();
	~LogUtils();

public:
	static LogUtils* getInstance();
	static void release();
	void start();
	void stop();
	void init(LOG_LEVEL level, const std::string& fileName);
	LOG_LEVEL getDefaultLevel() { return _defaultLevel; }
	bool getLog(std::string& log);
	void setLog(const std::string& log);
	bool writeLog(std::string& log);
	std::string getLogFile();
	bool isFileToday();
private:
	std::thread _thread;
	std::recursive_mutex _mtx;
	std::list<std::string> _logList;
	std::string _sFileName = "";
	std::string _sPath = DEFAULT_PATH;
	std::string _sName = "main.log";
	bool _running = false;
	FILE* _fp = nullptr;
	LOG_LEVEL _defaultLevel = LOG_LEVEL::LOG_LEVEL_DEBUG;
};

