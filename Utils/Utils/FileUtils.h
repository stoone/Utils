#pragma once
#include <string>
#include <vector>
#include <functional>
#include "win.h"


//#define _USE_32BIT_TIME_T

namespace Utils {
	class FileUtils {
	protected:
	public:
		static std::wstring FullPath(const std::wstring& path, const std::wstring& fileName);
		static std::wstring ParentPath(const std::wstring& fileFullName);
		static std::wstring FileName(const std::wstring& fileFullName, bool bIncludeExt = true);
		static int SplitPath(const std::wstring& fileFullName, std::wstring& path, std::wstring& fileName);
		static int SplitPathEx(const std::wstring& fileFullName, std::wstring& path, std::wstring& driver, std::wstring& fileName, std::wstring& extension);
		static std::wstring TrimSlash(std::wstring& path);

		static std::wstring GetCurrentPath();
		static std::wstring fileExt(const std::wstring& fileName);

		// return 0: not exist; 1, file; 2, folder
		static int FileExists(const std::wstring& file);

		// if file is folder, will not delete it if it is not empty, and will not delete files recursive
		static bool DeleteFileForce(const std::wstring& file, bool bForce = false);
		static bool DeletePath(const std::wstring& path);
		static bool CreatePath(const std::wstring& path);
		static bool ReversePath(const std::wstring& path, std::vector<std::wstring>& files, bool bRecursive = true);
		static bool ReversePath(const std::wstring& path, std::vector<std::wstring>& files, std::vector<std::wstring>& folders, bool bRecursive = true, std::function<bool(const std::wstring&)> filter = nullptr);
		static bool CopyPath(const std::wstring& src, const std::wstring& dst);
		static bool MovePath(const std::wstring& src, const std::wstring& dst);
		static bool CopyDirectory(const std::wstring& src, const std::wstring& dst);

		//复制文件或文件夹, 使用SHFileOperation，可以设置参数来定义各种行为
		static bool SHCopyFile(const std::wstring& src, const std::wstring& dst);
		static size_t FileSize(const std::wstring& file);

		static FILETIME GetFileLastWriteTime(const std::wstring& file);
		static FILETIME GetFileCreateTime(const std::wstring& file);
		
		static bool WriteStringToFile(const std::wstring& file, const std::wstring& data);
		static bool ReadStringFromFile(const std::wstring& file, std::wstring& data);
		static bool WriteDataToFile(const std::wstring& file, const std::string& data);
		static bool ReadDataFromFile(const std::wstring& file, std::string& data);
		static bool WriteLine(const std::string& file, const std::string& data);
		static bool ReadLine(const std::string& file, std::string& data);
	private:

	};

}
