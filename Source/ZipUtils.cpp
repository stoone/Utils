#include "stdafx.h"
#include "Utils.h"
#include "../include/zip/zip.h"
#include "../include/zip/unzip.h"
#include <strsafe.h>

namespace Utils {
	namespace ZipUtils {

		bool zipFolder(HZIP& hz, const std::wstring& folder, const std::wstring& basePath)
		{
			if (hz == 0) return false;
			if (folder.empty() || basePath.empty()) return false;

			std::wstring rePath = L"";   // 文件夹名称，相对路径
			HANDLE hFile;
			WIN32_FIND_DATA fData;
			TCHAR buffer[MAX_PATH] = { 0 };
			StringCchCopy(buffer, MAX_PATH, folder.c_str());
			StringCchCat(buffer, MAX_PATH, TEXT("/*"));
			hFile = FindFirstFile(buffer, &fData);
			if (hFile == INVALID_HANDLE_VALUE) return false;
			FindNextFile(hFile, &fData);
			while (FindNextFile(hFile, &fData) != 0)
			{
				if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					std::wstring subFolder = Utils::FilePathUtils::FullPath(folder, fData.cFileName);
					std::wstring dstPath = subFolder.substr(basePath.length() + 1);

					ZRESULT zr = ZipAddFolder(hz, dstPath.c_str());
					if (zr != ZR_OK) return false;

					zipFolder(hz, subFolder, basePath);
				}
				else
				{
					std::wstring fileName = Utils::FilePathUtils::FullPath(folder, fData.cFileName);
					std::wstring dstFileName = fileName.substr(basePath.length() + 1);
					ZRESULT zr = ZipAdd(hz, dstFileName.c_str(), fileName.c_str());
					if (zr != ZR_OK) return false;
				}
			}

			return true;
		}

		bool ZipDirectory(const std::wstring& directory, const std::wstring& zipName)
		{
			if (directory.empty()) return false;

			std::wstring dir = directory;
			dir = Utils::FilePathUtils::TrimSlash(dir);
			std::wstring srcName, srcPath;
			Utils::FilePathUtils::SplitPath(dir, srcPath, srcName);

			std::wstring dstName = zipName;
			if (zipName.empty())
			{
				dstName = Utils::FilePathUtils::FullPath(srcPath, srcName);
				dstName = dstName + L".zip";
			}

			HZIP hz = CreateZip(dstName.c_str(), 0);
			if (hz == 0) return false;

			bool ret = zipFolder(hz, dir, srcPath);

			CloseZip(hz);

			return ret;
		}

		bool UnzipFile(const std::wstring& zipName, const std::wstring& path)
		{
			if (zipName.empty()) return false;

			ZIPENTRY ze;
			HZIP hz = OpenZip(zipName.c_str(), 0);
			if (hz == 0) return false;

			ZRESULT zr = GetZipItem(hz, -1, &ze);
			if (zr != ZR_OK)
			{
				CloseZip(hz);
				return false;
			}

			std::wstring dstPath = path;
			if (path.empty())
			{
				std::wstring srcPath, srcName, srcExt, driver;
				Utils::FilePathUtils::SplitPathEx(zipName, srcPath, driver, srcName, srcExt);
				dstPath = srcPath == L"/" ? driver + srcPath + srcName : driver + L"/" + srcPath + srcName;
			}

			SetUnzipBaseDir(hz, dstPath.c_str());
			int total = ze.index;
			for (int zi = 0; zi < total; zi++)
			{
				zr = GetZipItem(hz, zi, &ze);
				if (zr != ZR_OK) break;

				zr = UnzipItem(hz, zi, ze.name);
				if (zr != ZR_OK) break;
			}
			CloseZip(hz);

			return zr == ZR_OK ? true : false;
		}


		// support absolute and relative path
		bool ZipFile(const std::wstring& fileName, const std::wstring& zipName)
		{
			if (fileName.empty()) return false;
			if (Utils::FileUtils::FileExists(fileName) != 1) return false;

			std::wstring fullZipName = zipName;
			std::wstring srcDir, srcName, dir, name;
			Utils::FilePathUtils::SplitPath(fileName, srcDir, srcName);

			if (zipName.find_first_of(L"/\\") != std::wstring::npos)
			{
				fullZipName = zipName;
			}
			else
			{
				if (zipName.empty())
				{
					std::wstring nameNoExt = Utils::FilePathUtils::FileName(fileName, false);
					fullZipName = nameNoExt + L".zip";
				}

				fullZipName = Utils::FilePathUtils::FullPath(srcDir, fullZipName);
			}
			
			
			std::wstring dstPath, dstName;
			Utils::FilePathUtils::SplitPath(fullZipName, dstPath, dstName);
			if (Utils::FileUtils::FileExists(dstPath) == 0)
				Utils::FileUtils::CreatePath(dstPath);

			std::wstring dstFileName = srcName;

			HZIP hz = CreateZip(fullZipName.c_str(), nullptr);
			if (hz == 0) return false;
			ZipAdd(hz, dstFileName.c_str(), fileName.c_str());
			CloseZip(hz);


			return true;
		}

		bool ZipFiles(const std::vector<std::wstring>& files, const std::wstring& zipName, const std::wstring& basePath, bool quitIfFail)
		{
			if (files.empty()) return false;
			if (zipName.empty()) return false;

			HZIP hz = CreateZip(zipName.c_str(), 0);
			if (hz == 0) return false;

			ZRESULT zr = ZR_OK;
			for (const std::wstring& item : files)
			{
				std::wstring fileName = item.substr(basePath.length() + 1);
				if (Utils::FileUtils::FileExists(item) == 2)   // is folder
					zr = ZipAddFolder(hz, fileName.c_str());
				else
					zr = ZipAdd(hz, fileName.c_str(), item.c_str());

				if (zr != ZR_OK)
				{
					if (quitIfFail)
						break;
					else
						continue;
				}
			}

			CloseZip(hz);

			return zr == ZR_OK ? true : false;
		}
	}
}