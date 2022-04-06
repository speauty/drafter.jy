#pragma once

#include <iostream>
#include <string.h>
#include <vector>

#define ERR_PRINT(msg) do { std::cerr << "[����] �����ļ�: " << __FILE__ << "@" << __LINE__ << ", �����Ϣ: " << msg << std::endl; } while (0)

/**
 * ������ת����ʱ�����ʽ���ַ��� 
 **/
void MicroS2Str(std::string& msFmtStr, unsigned int microSeconds);

std::string& ReplaceAll(std::string & str, const std::string & old_value, const std::string & new_value);

std::string GenCurrentDateTime();

std::vector<std::string> GetJYInstallDir();