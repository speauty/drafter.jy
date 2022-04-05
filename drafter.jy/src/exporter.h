#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>
#include "Utils.h"

class Exporter
{
private:
	std::string m_SourceFilePath;
	std::string m_TargetFilePath;
	std::string m_TargetExt;
	unsigned int m_BlockCount;

	bool m_FlagHasTimeCode;
	bool m_FlagTargetFileReflush;
	bool m_FlagTargetFileForceOverride;
	bool m_FlagIsValidated;
	bool m_FlagIsAccessed;

	std::vector<std::string> m_Logs;

	std::vector<std::string> m_Subtitle;

	Json::Reader m_JsonReader;
	Json::Value m_JsonData;

public:
	Exporter();
	~Exporter();

	void SetSourceFilePath(const std::string& path);
	void SetTargetFilePath(const std::string& path);
	void SetFlagHasTimeCode(bool flag);
	void SetTargetExt(const std::string& ext);
	void SetFlagTargetFileForceOverride(bool flag);

	void ResetSubtitle();
	void ResetLogs();

	void Validate();
	void DelTargetFile();
	bool ExecExport();
	bool GenSubtitle();

	std::string GetSourceFilePath() const { return m_SourceFilePath; }
	std::string GetTargetFilePath() const {
		if (m_TargetFilePath.empty()) {
			return "";
		}
		return m_TargetFilePath + "." + m_TargetExt; 
	}
	bool GetFlagHasTimeCode() const { return m_FlagHasTimeCode; }
	bool GetFlagIsValidated() const { return m_FlagIsValidated; }
	bool GetFlagIsAccessed() const { return m_FlagIsAccessed; }
	unsigned int GetBlockCount() const { return m_BlockCount; }
	std::vector<std::string> GetSubtitle() const { return m_Subtitle; }
	std::vector<std::string> GetLogs() const { return m_Logs; }

	std::string GetBlockSep() const { return "###########"; }
};
