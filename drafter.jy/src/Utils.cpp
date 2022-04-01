#include "Utils.h"
#include <wtypes.h>
#include <tchar.h>
#include "io.h"

void MicroS2Str(std::string& msFmtStr, unsigned int microSeconds)
{
	/* 定义时分秒&毫秒 */
	unsigned int numH = 0, numM = 0, numS = 0, numMs = 0;
	char swap[4];
	unsigned int tmp;
	tmp = (unsigned int)(microSeconds / 1e3); /* 去掉微秒部分 */
	numMs = tmp % 1000; /* 转存毫秒部分 */
	tmp = (unsigned int)(tmp / 1e3); /* 去掉毫秒部分 */

	/* 计算时分秒 */
	numH = (unsigned int)(tmp / 3600);
	tmp -= numH * 3600;
	if (tmp) {
		numM = tmp / 60;
		numS = tmp - numM * 60;
	}

	msFmtStr = "";

	if (numH > 24) numH = 24;
	if (numH < 10) msFmtStr += '0';
	sprintf_s(swap, "%d", numH);
	msFmtStr += swap;
	msFmtStr += ":";
	if (numM < 10) msFmtStr += "0";
	sprintf_s(swap, "%d", numM);
	msFmtStr += swap;
	msFmtStr += ":";
	if (numS < 10) msFmtStr += "0";
	sprintf_s(swap, "%d", numS);
	msFmtStr += swap;
	msFmtStr += ",";

	if (numMs < 100) msFmtStr += "0";
	if (numMs < 10) msFmtStr += "0";
	sprintf_s(swap, "%d", numMs);
	msFmtStr += swap;

	return;
}

std::string GenCurrentDateTime()
{
	time_t now;
	struct tm tmp;
	char buff[64];

	time(&now);
	localtime_s(&tmp, &now);

	strftime(buff, 64, "%Y-%m-%d %H:%M:%S", &tmp);

	return std::string(buff);
}

std::vector<std::string> GetJYInstallDir()
{
	std::vector<std::string> files;
	HKEY hKEY;
	LPCTSTR subKey = TEXT("SOFTWARE\\Bytedance\\JianyingPro");
	// HKEY_CURRENT_USER\SOFTWARE\Bytedance\JianyingPro
	//访问注册表，hKEY则保存此函数所打开的键的句柄
	long ret0 = RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKEY);
	if (ret0 != ERROR_SUCCESS)//如果无法打开hKEY,打开指定注册表失败，则中止程序的执行，
	{
		std::cout << "打开注册表异常 " << subKey << std::endl;
		return files;
	}
	//查询有关的数据
	TCHAR addrGet[256] = {0};
	DWORD dataType = REG_SZ;//定义数据类型
	DWORD dataLen = sizeof(addrGet);//定义数据长度
	long res = RegQueryValueEx(hKEY, TEXT("installDir"), NULL, &dataType, (LPBYTE)&addrGet, &dataLen);
	if (res != ERROR_SUCCESS)
	{
		std::cout << "查询值异常 installDir" << std::endl;
		return files;
	}
	char tmp[512] = {};
	sprintf_s(tmp, dataLen, "%ls", addrGet);
	
	RegCloseKey(hKEY);
	std::string path(tmp);
	path += "..\\JianyingPro Drafts";

	intptr_t  hFile = 0;
	_finddata_t fileinfo;//用来存储文件信息的结构体   
	std::string tmpPath;
	struct stat tmpStat;

	if ((hFile = _findfirst(tmpPath.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					if (stat(tmpPath.assign(path).append("\\").append(fileinfo.name).append("\\").append("draft_content.json").c_str(), &tmpStat) == 0) {
						files.push_back(tmpPath.assign(path).append("\\").append(fileinfo.name).append("\\").append("draft_content.json"));  // 存入目录
					}
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile); //结束查找  
	}

	return files;
}
