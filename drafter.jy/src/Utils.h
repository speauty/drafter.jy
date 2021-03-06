#pragma once

#include <iostream>
#include <string.h>
#include <vector>

#define ERR_PRINT(msg) do { std::cerr << "[错误] 所在文件: " << __FILE__ << "@" << __LINE__ << ", 相关信息: " << msg << std::endl; } while (0)

/**
 * 将毫秒转化成时分秒格式的字符串 
 **/
void MicroS2Str(std::string& msFmtStr, unsigned int microSeconds);

std::string& ReplaceAll(std::string & str, const std::string & old_value, const std::string & new_value);

std::string GenCurrentDateTime();

std::vector<std::string> GetJYInstallDir();