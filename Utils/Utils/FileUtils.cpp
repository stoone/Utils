#include "Utils.h"
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include "FileUtils.h"
#include "StringUtils.h"

using namespace std;

namespace Utils {
		std::wstring FileUtils::FullPath(const std::wstring& path, const std::wstring& fileName)
		{
			if (path.empty()) return fileName;
			if (fileName.empty()) return path;

			std::wstring p = Utils::StringUtils<std::wstring, wchar_t>::Trim(path);
			std::wstring f = Utils::StringUtils<std::wstring, wchar_t>::Trim(fileName);

			if (Utils::StringUtils<std::wstring, wchar_t>::EndsWith(p, L"\\"))
			{
				p = p.substr(0, p.length() - 1);
			}

			if (Utils::StringUtils<std::wstring, wchar_t>::StartsWith(fileName, L"\\"))
			{
				f = f.substr(1, f.length());
			}

			return p + L"\\" + f;
		}

		std::wstring FileUtils::ParentPath(const std::wstring& fileFullName)
		{
			if (!fileFullName.empty())
			{
				wchar_t delimiter = '\\';
				if (fileFullName.find('/') != std::wstring::npos)
					delimiter = '/';

				size_t pos = fileFullName.rfind(delimiter);
				if (pos != std::wstring::npos)
					return fileFullName.substr(0, pos);
			}

			return L"";
		}

		std::wstring FileUtils::FileName(const std::wstring& fileFullName, bool bIncludeExt)
		{
			if (fileFullName.empty())
				return L"";

			std::wstring name;
			char delimiter = fileFullName.find('/') == std::wstring::npos ? '\\' : '/';
			size_t pos = fileFullName.rfind(delimiter);
			if (pos != std::wstring::npos)
			{
				name = fileFullName.substr(pos + 1);
			}
			else
			{
				name = fileFullName;
			}

			if (!bIncludeExt)
			{
				pos = name.rfind('.');
				if (pos != std::wstring::npos)
				{
					name = name.substr(0, pos);
				}
			}

			return name;
		}

		int FileUtils::SplitPath(const std::wstring& fileFullName, std::wstring& path, std::wstring& fileName)
		{
			path = L"";
			fileName = L"";

			if (fileFullName.empty())
			{
				return 0;
			}

			int pos = fileFullName.find_last_of(L"/\\");
			if (pos == std::wstring::npos)
			{
				return 0;
			}

			path = fileFullName.substr(0, pos);
			fileName = fileFullName.substr(pos + 1);

			return 0;
		}


		int FileUtils::SplitPathEx(const std::wstring& fileFullName, std::wstring& path, std::wstring& driver, std::wstring& fileName, std::wstring& extension)
		{
			wchar_t drive[_MAX_DRIVE] = { 0 };
			wchar_t dir[_MAX_DIR] = { 0 };
			wchar_t name[_MAX_FNAME] = { 0 };
			wchar_t ext[_MAX_EXT] = { 0 };

			int ret = _wsplitpath_s(fileFullName.c_str(), drive, drive ? _MAX_DRIVE : 0,
				dir, dir ? _MAX_DIR : 0, name, name ? _MAX_FNAME : 0, ext, ext ? _MAX_EXT : 0);
			if (ret == 0)
			{
				path = dir;
				driver = drive;
				fileName = name;
				extension = ext;
			}

			return ret;
		}

		std::wstring FileUtils::TrimSlash(std::wstring& path)
		{
			path = Utils::StringUtils<std::wstring, wchar_t>::Trim(path, L"/");
			path = Utils::StringUtils<std::wstring, wchar_t>::Trim(path, L"\\");

			return path;
		}


		std::wstring FileUtils::GetCurrentPath()
		{
			TCHAR buffer[MAX_PATH] = { 0 };
			GetModuleFileName(nullptr, buffer, MAX_PATH);
			wcsrchr(buffer, '\\')[1] = 0;
			return std::wstring(buffer);
		}
		
		std::wstring FileUtils::fileExt(const std::wstring& fileName)
		{
			std::wstring ext = L"";
			int pos = fileName.rfind('.');
			if (pos != std::wstring::npos)
			{
				ext = fileName.substr(pos);
			}

			return ext;
		}

		/*
		use PathFileExists need Shlwapi.dll
		*/
		int FileUtils::FileExists(const std::wstring& file)
		{
			if (file.empty()) return 0;

#ifdef _WINDOWS_
			DWORD dwAttr = GetFileAttributes(file.c_str());
			if (dwAttr == INVALID_FILE_ATTRIBUTES) return 0;

			return (dwAttr & FILE_ATTRIBUTE_DIRECTORY) ? 2 : 1;   // return 1 is file, return 2 is folder, >0 is exist  ==0 not exist
#else
			int e = _waccess(file.c_str(), 0);
			return e == -1 ? 0 : 1;
#endif
		}

		bool FileUtils::CreatePath(const std::wstring& path)
		{
			if (path.empty()) return false;

			if (_taccess(path.c_str(), 0) != 0 && _wmkdir(path.c_str()) != 0)
			{
				std::wstring parentPath = Utils::FileUtils::ParentPath(path);
				CreatePath(parentPath);
				return _wmkdir(path.c_str()) == 0 ? true : false;
			}

			return true;
		}

		bool FileUtils::DeleteFileForce(const std::wstring& file, bool bForce)
		{
			BOOL result;
			if (FileExists(file) == 2)
			{
				result = RemoveDirectory(file.c_str());
			}
			else
			{
				result = ::DeleteFile(file.c_str());
				if (!result && bForce)
				{
					// 自读文件会导致删除失败
					SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
					result = ::DeleteFile(file.c_str());
				}
			}
			return result ? true : false;
		}


		bool FileUtils::DeletePath(const std::wstring& path)
		{
			if (path.empty()) return false;

			bool result = true;
			std::vector<std::wstring> files;
			if (ReversePath(path, files, false))
			{
				for (const std::wstring& file : files)
				{
					if (FileExists(file) == 2)
					{
						result = DeletePath(file);
					}
					else
					{
						result = ::DeleteFile(file.c_str());
					}
				}
			}

			result = RemoveDirectory(path.c_str());

			return result;
		}

		bool FileUtils::ReversePath(const std::wstring& path, std::vector<std::wstring>& files, bool bRecursive)
		{
			intptr_t hFile;
			_wfinddata_t fileinfo;
			const std::wstring& pattern = path + L"\\*.*";
			if ((hFile = _wfindfirst(pattern.c_str(), &fileinfo)) != -1)
			{
				do
				{
					std::wstring pathName = path + L"\\" + fileinfo.name;


					if (fileinfo.attrib & _A_SUBDIR)
					{
						if (wcscmp(fileinfo.name, L".") != 0 && wcscmp(fileinfo.name, L"..") != 0)
						{
							files.push_back(pathName);

							if (bRecursive)
							{
								ReversePath(pathName, files, bRecursive);
							}
						}
					}
					else
					{
						files.push_back(pathName);
					}


				} while (_wfindnext(hFile, &fileinfo) == 0);

				_findclose(hFile);
				return true;
			}

			return false;
		}

		bool FileUtils::ReversePath(const std::wstring& path, 
			std::vector<std::wstring>& files, std::vector<std::wstring>& folders, 
			bool bRecursive, std::function<bool(const std::wstring&)> filter)
		{
			intptr_t hFile;
			_wfinddata_t fileinfo;
			const std::wstring& pattern = path + L"\\*.*";
			if ((hFile = _wfindfirst(pattern.c_str(), &fileinfo)) != -1)
			{
				do
				{
					std::wstring pathName = path + L"\\" + fileinfo.name;


					if (fileinfo.attrib & _A_SUBDIR)
					{
						if (wcscmp(fileinfo.name, L".") != 0 && wcscmp(fileinfo.name, L"..") != 0)
						{
							folders.push_back(pathName);

							if (bRecursive)
							{
								ReversePath(pathName, files, folders, bRecursive, filter);
							}
						}
					}
					else
					{
						if (filter)
						{
							if (filter(pathName))
								files.push_back(pathName);
						}
						else
						{
							files.push_back(pathName);
						}
					}
				

				} while (_wfindnext(hFile, &fileinfo) == 0);

				_findclose(hFile);
				return true;
			}

			return false;
		}

		bool FileUtils::CopyPath(const std::wstring& src, const std::wstring& dst)
		{
			if (src.empty() || dst.empty()) return false;
	
			std::wstring srcPath = Utils::FileUtils::TrimSlash(const_cast<std::wstring&>(src));
			std::wstring dstPath = Utils::FileUtils::TrimSlash(const_cast<std::wstring&>(dst));

			if (Utils::StringUtils<std::wstring, wchar_t>::toLower(srcPath) == Utils::StringUtils<std::wstring, wchar_t>::toLower(dstPath)) return true;

			std::vector<std::wstring> files;
			if (ReversePath(srcPath, files, true))
			{
				size_t sourcePathLen = srcPath.length();
				for (const std::wstring& file : files)
				{
					int srcPathLen = srcPath.length();
					const std::wstring& targetFileName = Utils::FileUtils::FullPath(dstPath, file.substr(srcPathLen));
					if (FileExists(file) == 2)
					{
						CreatePath(targetFileName);
						continue;
					}
					else
					{
						const std::wstring& path = Utils::FileUtils::ParentPath(targetFileName);
						if (FileExists(path) == 0)
						{
							CreatePath(path);
						}
					}					

					BOOL r = ::CopyFile(file.c_str(), targetFileName.c_str(), false);
					if (!r) return false;  // return when copy file failed
				}
			}

			return true;
		}

		bool FileUtils::CopyDirectory(const std::wstring& src, const std::wstring& dst)
		{
			CreatePath(dst);

			bool bRet = false;
			_wfinddata_t file;
			const std::wstring& path = src + L"\\*.*";
			intptr_t handle = _wfindfirst(path.c_str(), &file);
			while (handle != -1)
			{
				if (file.attrib & _A_SUBDIR)
				{
					//判断是否为"."当前目录，".."上一层目录
					if ((_tcscmp(file.name, L".") != 0) && (_tcscmp(file.name, L"..") != 0))
					{
						std::wstring srcPath = src + L"\\" + file.name;
						std::wstring newPath = dst + L"\\" + file.name;
						bRet = CopyDirectory(srcPath, newPath);
					}
				}
				else
				{
					std::wstring srcFile = src + L"\\" + file.name;
					bRet = ::CopyFile(srcFile.c_str(), (dst + L"\\" + (std::wstring)file.name).c_str(), FALSE);
				}

				if (_wfindnext(handle, &file) != 0)
					break;
			}
			_findclose(handle);

			return bRet;
		}

		bool FileUtils::SHCopyFile(const std::wstring& src, const std::wstring& dst)
		{
			SHFILEOPSTRUCT FileOp = { 0 };
			FileOp.fFlags = FOF_NOCONFIRMATION |   //不出现确认对话框
				FOF_NOCONFIRMMKDIR; //需要时直接创建一个文件夹,不需用户确定
			FileOp.pFrom = src.c_str();
			FileOp.pTo = dst.c_str();
			FileOp.wFunc = FO_COPY;
			return SHFileOperation(&FileOp) == 0;
		}

		bool FileUtils::MovePath(const std::wstring& src, const std::wstring& dst)
		{
			if (src.empty() || dst.empty()) return false;

			std::wstring srcPath = Utils::FileUtils::TrimSlash(const_cast<std::wstring&>(src));
			std::wstring dstPath = Utils::FileUtils::TrimSlash(const_cast<std::wstring&>(dst));

			if (Utils::StringUtils<std::wstring, wchar_t>::toLower(srcPath) == Utils::StringUtils<std::wstring, wchar_t>::toLower(dstPath)) return true;

			std::vector<std::wstring> files;
			if (ReversePath(srcPath, files, true))
			{
				size_t sourcePathLen = srcPath.length();
				for (const std::wstring& file : files)
				{
					int srcPathLen = srcPath.length();
					const std::wstring& targetFileName = Utils::FileUtils::FullPath(dstPath, file.substr(srcPathLen));
					if (FileExists(file) == 2)
					{
						CreatePath(targetFileName);
						continue;
					}
					else
					{
						const std::wstring& path = Utils::FileUtils::ParentPath(targetFileName);
						if (FileExists(path) == 0)
						{
							CreatePath(path);
						}
					}

					BOOL r = ::MoveFile(file.c_str(), targetFileName.c_str());
					if (!r) return false;
				}
				bool remove = DeletePath(srcPath.c_str());
				if (!remove)
				{
					int i = 0;
					i++;
				}
			}

			return true;
		}

		size_t FileUtils::FileSize(const std::wstring& file)
		{
			if (file.empty()) return 0;

			size_t size = 0;
			int r = FileExists(file.c_str());
			if (r == 1)
			{
				FILE* f;
				_wfopen_s(&f, file.c_str(), _T("rb"));
				if (f)
				{
					fseek(f, 0, SEEK_END);
					size = ftell(f);
					fclose(f);
				}
			}
			
			return size;
		}


		FILETIME FileUtils::GetFileLastWriteTime(const std::wstring& file)
		{
			WIN32_FILE_ATTRIBUTE_DATA data;
			data.ftLastWriteTime.dwHighDateTime = 0;
			data.ftLastWriteTime.dwLowDateTime = 0;
			GetFileAttributesEx(file.c_str(), GetFileExInfoStandard, &data);
			return data.ftLastWriteTime;
		}

		FILETIME FileUtils::GetFileCreateTime(const std::wstring& file)
		{
			WIN32_FILE_ATTRIBUTE_DATA data;
			data.ftCreationTime.dwHighDateTime = 0;
			data.ftCreationTime.dwLowDateTime = 0;
			GetFileAttributesEx(file.c_str(), GetFileExInfoStandard, &data);
			return data.ftCreationTime;
		}
		
		bool FileUtils::WriteStringToFile(const std::wstring& file, const std::wstring& data)
		{
			if (file.empty()) return false;

			return WriteDataToFile(file, Utils::UnicodeToAnsi(data));
		}

		bool FileUtils::WriteDataToFile(const std::wstring& file, const std::string& data)
		{
			if (file.empty()) return false;
			const std::wstring& path = Utils::FileUtils::ParentPath(file);
			if (path.empty()) return false;
			if (FileExists(path) == 0)
			{
				CreatePath(path);
			}

			FILE *f;
			errno_t err;
			err = _wfopen_s(&f, file.c_str(), L"w+");
			if (err == 0)
			{
				int len = fwrite(data.c_str(), sizeof(char), data.length(), f);
				if (f) fclose(f);

				if (len != data.length())
				{
					return false;
				}

				return true;
			}

			return false;
		}

		bool FileUtils::ReadStringFromFile(const std::wstring& file, std::wstring& data)
		{
			if (FileExists(file) != 1) return false;

			std::string sData;
			bool result = ReadDataFromFile(file, sData);
			if (result)
			{
				data = Utils::AnsiToUnicode(sData);
			}

			return result;
		}


		bool FileUtils::ReadDataFromFile(const std::wstring& file, std::string& data)
		{
			if (FileExists(file) != 1) return false;

			FILE* f;
			errno_t err = _wfopen_s(&f, file.c_str(), L"r");
			if (err == 0)
			{
				char buffer[256] = { 0 };
				int len = fread(buffer, sizeof(char), 256, f);
				if (f) fclose(f);

				data = buffer;
				return true;
			}

			return false;
		}

		bool FileUtils::WriteLine(const std::string& file, const std::string& data)
		{
			std::ofstream fs(file, ios::app);
			if (fs.is_open())
			{
				fs << data.c_str();
				fs.close();
				return true;
			}
			return false;
		}

		bool FileUtils::ReadLine(const std::string& file, std::string& data)
		{
			ifstream fs(file, ios::in);
			if (fs.is_open())
			{
				::getline(fs, data);
				return true;
			}

			return false;
		}
}