#pragma warning(disable:4996)
#ifdef WINDOWS
#include <ELAWindows.h>
#include <tlhelp32.h>
#else
#include <sys/timeb.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <SystemUtil.h>
#ifdef WINDOWS
#endif
#include <StringUtil.h>

namespace SystemUtil
{
	void ForEachMatchingFile(std::function<void(const char*,_WIN32_FIND_DATAA)> callback, const char* filePath, std::vector<std::string> patterns)
	{
		for (std::string spattern : patterns)
		{
			char *pattern = const_cast<char*>(spattern.c_str());
#ifdef	WINDOWS
			HANDLE hFind;
			WIN32_FIND_DATAA data;
			char* filePattern = (char*)calloc(1, (strlen(filePath) + strlen(pattern) + 3));
			strcat(filePattern, filePath);
			strcat(filePattern, pattern);
			strcat(filePattern, "*");
			hFind = FindFirstFileA(filePattern, &data);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (strcmp(".", data.cFileName) != 0 && strcmp("..", data.cFileName) != 0)
					{
						callback(filePath, data);
					}
				}
				while (FindNextFileA(hFind, &data));

				FindClose(hFind);
			}

			free(filePattern);
#else
			DIR *ptrDIR = NULL;
			struct dirent *ptrDirent = NULL;
			ptrDIR = opendir(filePath);
			if (ptrDIR)
			{
				while ((ptrDirent = readdir(ptrDIR)) != NULL)
				{
					if (StringUtil::IsSubString(pattern, ptrDirent->d_name))
					{
						callback(filePath, ptrDirent->d_name);
					}
				}
				closedir(ptrDIR);
			}
#endif
		}
	}

	void RemoveFile(const char* path, const char* filename)
	{
		char deleteFile[100];
		sprintf(deleteFile, "%s%s", path, filename);
		remove(deleteFile);
	}

	long long GetFileSizeInBytes(char* fileName)
	{
		long long fileSize = 0;
#ifdef WINDOWS
		HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
		                           FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			fileSize = GetFileSize(hFile, nullptr);
		}
		else
			//ErrorLogMessage("\n Unable to get Filesize");
			CloseHandle(hFile);
#else
		struct stat lf;
		if (stat(fileName, &lf) == 0)
		{
			fileSize = lf.st_size;
		}
		else
			//ErrorLogMessage("\n Unable to get Filesize");
#endif
		return fileSize;
	}

	std::string ExecuteSystemCommand(const char* cmd)
	{
		FILE* pPipe;
#ifdef WINDOWS
		pPipe = _popen(cmd, "rt");
#else
		pPipe = popen(cmd, "r");
#endif
		std::string pipeoutput = "";

		if (pPipe != nullptr)
		{
			char buffer[1025];
			while (!feof(pPipe))
			{
				if (fgets(buffer, 1024, pPipe) != nullptr)
				{
					pipeoutput.append(buffer);
				}
			}
#ifdef WINDOWS
			_pclose(pPipe);
#else
			pclose(pPipe);
#endif
		}
		return pipeoutput;
	}

#ifdef WINDOWS
	void CreateWinProcess(std::string processname, char* uploaderArg)
	{
		STARTUPINFOA stInfo;
		PROCESS_INFORMATION procInfo;
		BOOL uploaderRetVal;

		ZeroMemory(&stInfo, sizeof(stInfo));
		stInfo.cb = sizeof(stInfo);
		ZeroMemory(&procInfo, sizeof(procInfo));

		DWORD pid = 0;

		if ((pid = IsWin32ProcessRunning(processname.c_str())))
		{
			procInfo.dwProcessId = pid;
			procInfo.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (procInfo.hProcess)
			{
				uploaderRetVal = true;
			}
		}
		else
		{
			uploaderRetVal = CreateProcessA(nullptr, uploaderArg, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &stInfo,
			                                &procInfo);
		}

		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
	}

	std::string WSToMBC(LPWSTR utf16string)
	{
		if (utf16string == nullptr)
			return std::string();
		int cw = lstrlenW(utf16string);
		if (cw == 0)
		{
			return std::string();
		}
		int cc = WideCharToMultiByte(CP_UTF8, 0, utf16string, cw, nullptr, 0, nullptr, nullptr);
		if (cc == 0)
			return std::string();
		CHAR* psz = static_cast<char *>(malloc(cc + 1));
		cc = WideCharToMultiByte(CP_UTF8, 0, utf16string, cw, psz, cc, nullptr, nullptr);
		if (cc == 0)
		{
			free(psz);
			return std::string();
		}
		psz[cc] = '\0';
		std::string multibyteString(psz);
		free(psz);
		return multibyteString;
	}

	DWORD IsWin32ProcessRunning(const char* processname)
	{
		DWORD processid = 0;
		PROCESSENTRY32W entry;
		entry.dwSize = sizeof(PROCESSENTRY32W);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32FirstW(snapshot, &entry))
			while (Process32NextW(snapshot, &entry))
			{
				std::string process = WSToMBC(entry.szExeFile);
				if (!_strcmpi(process.c_str(), processname))
				{
					processid = entry.th32ProcessID;
					break;
				}
			}

		CloseHandle(snapshot);

		return processid;
	}
#endif
}
