#include "exporter.h"
#include <locale>

std::string tmpStr = "";

Exporter::Exporter()
	:m_SourceFilePath(""), m_TargetFilePath(""), m_FlagHasTimeCode(true), m_BlockCount(0), m_FlagTargetFileForceOverride(true), m_FlagTargetFileReflush(false), m_FlagIsValidated(false), m_FlagIsAccessed(false), m_TargetExt("srt")
{
}

Exporter::~Exporter()
{
	m_SourceFilePath.clear();
	m_TargetFilePath.clear();
	m_BlockCount = 0;
	m_JsonReader = {};
	m_JsonData = NULL;
	m_FlagHasTimeCode = true;
	m_FlagTargetFileForceOverride = false;
	m_FlagTargetFileReflush = false;
}


void Exporter::SetSourceFilePath(const std::string& path)
{
	m_SourceFilePath = path;
	m_FlagIsValidated = false;
	m_FlagIsAccessed = false;
	Validate();
	GenSourceVideoPath();
}

void Exporter::SetTargetFilePath(const std::string& path)
{
	m_TargetFilePath = path;
	m_FlagIsValidated = false;
	m_FlagIsAccessed = false;
	ResetSubtitle();
}

void Exporter::SetFlagHasTimeCode(bool flag)
{
	if (flag != m_FlagHasTimeCode && m_FlagIsAccessed) {
		ResetSubtitle();
		GenSubtitle();
	}
	m_FlagHasTimeCode = flag;
}

void Exporter::SetTargetExt(const std::string& ext)
{
	m_TargetExt = ext;
}

void Exporter::SetFlagTargetFileForceOverride(bool flag)
{
	m_FlagTargetFileForceOverride = flag;
}

void Exporter::GenSourceVideoPath()
{
	m_SourceVideoPath = m_JsonData["materials"]["videos"][0]["path"].asString();
	m_SourceVideoPath = ReplaceAll(m_SourceVideoPath, "/", "\\");
}

void Exporter::ResetSubtitle()
{
	std::vector<std::string>::iterator iter = m_Subtitle.begin();
	for (; iter != m_Subtitle.end();)
	{
		iter = m_Subtitle.erase(iter);
	}
	m_BlockCount = 0;
	m_Logs.push_back(GenCurrentDateTime() + u8" ��ǰ��Ļ�����");
}

void Exporter::ResetLogs()
{
	std::vector<std::string>::iterator iter = m_Logs.begin();
	for (; iter != m_Logs.end();)
	{
		iter = m_Logs.erase(iter);
	}
}

void Exporter::Validate()
{
	m_FlagIsValidated = true;
	std::string sourceFile = GetSourceFilePath();
	std::string targetFile = GetTargetFilePath();
	if (sourceFile.empty()) {
		m_Logs.push_back(GenCurrentDateTime() + u8" ���޼�ӳԴ��");
	} else {
		std::fstream fs(sourceFile);
		if (!fs.is_open()) {
			m_Logs.push_back(GenCurrentDateTime() + u8" ��ӳԴ�����쳣[" + sourceFile + "]");
		} else {
			if (m_JsonReader.parse(fs, m_JsonData, true)) {
				if (m_JsonData.empty()) {
					m_Logs.push_back(GenCurrentDateTime() + u8" ��ӳԴ��JSON������������, �ݶ��Ƿ��ļ�[" + sourceFile + "]");
				}
				m_FlagIsAccessed = true;
			} else {
				m_Logs.push_back(GenCurrentDateTime() +  u8" ��ӳԴ��JSON�����쳣[" + sourceFile + "]");
			}
		}
		fs.close();
	}

	if (targetFile.empty()) {
		m_Logs.push_back(GenCurrentDateTime() + u8" ���޵����ļ�");
		m_FlagIsAccessed = false;
	} else {
		std::fstream ts(targetFile);
		if (ts.is_open() && !m_FlagTargetFileForceOverride) {
			m_Logs.push_back(GenCurrentDateTime() + u8" �����ļ��Ѵ���[" + targetFile + "]");
			m_FlagIsAccessed = false;
		}
		ts.close();
	}
}

void Exporter::DelTargetFile()
{
	if (m_FlagTargetFileReflush && !m_TargetFilePath.empty()) remove(m_TargetFilePath.c_str());
}

bool Exporter::ExecExport()
{
	if (m_Subtitle.empty()) {
		m_Logs.push_back(GenCurrentDateTime() + u8" ����������Ļ");
		return false;
	}

	std::ofstream target;
	std::string targetFile = GetTargetFilePath();
	target.open(targetFile, std::ios::out | std::ios::trunc);
	if (!target.is_open()) {
		m_Logs.push_back(GenCurrentDateTime() + u8" ����·�����쳣[" + targetFile + "]");
		target.close();
		// _CRT_SECURE_NO_WARNINGS
		std::cout << "��������·���쳣: " << errno << "@" << strerror(errno) << std::endl;
		return false;
	}

	for (size_t i = 0; i < m_Subtitle.size(); i++) {

		if (m_Subtitle[i] == GetBlockSep()) {
			target << std::endl;
			continue;
		}

		target << m_Subtitle[i] << std::endl;
	}

	target.close();
	//std::locale::global(std::locale(loc));

	m_Logs.push_back(GenCurrentDateTime() + u8" ������Ļ�ɹ�[" + targetFile + "]");
	return true;
}

bool Exporter::GenSubtitle()
{
	Validate();
	if (!m_FlagIsAccessed) {
		m_Logs.push_back(GenCurrentDateTime() + u8" δͨ�����");
		m_Logs.push_back(GenCurrentDateTime() + u8" ������Ļʧ��");
		return false;
	}

	if (!m_Subtitle.empty()) {
		ResetSubtitle();
	}

	if (m_JsonData.empty() || !m_JsonData.isMember("materials") || !m_JsonData["materials"].isMember("texts") || !m_JsonData.isMember("tracks")) {
		m_Logs.push_back(GenCurrentDateTime() + u8" ��ӳԴ��JSON�����������������, �ݶ��Ƿ��ļ�[" + GetSourceFilePath() + "]");
		m_Logs.push_back(GenCurrentDateTime() + u8" ������Ļʧ��");
		return false;
	}

	unsigned int count = m_JsonData["materials"]["texts"].size();
	std::string timeStartSwap;
	std::string timeEndSwap;

	for (unsigned int i = 0; i < count; i++) {
		m_BlockCount++;
		std::string text = m_JsonData["materials"]["texts"][i]["content"].asString();
		unsigned int start = m_JsonData["tracks"][1]["segments"][i]["target_timerange"]["start"].asInt();
		unsigned int duration = m_JsonData["tracks"][1]["segments"][i]["target_timerange"]["duration"].asInt();
		MicroS2Str(timeStartSwap, start);
		MicroS2Str(timeEndSwap, start + duration);
		
		m_Subtitle.push_back(std::to_string(m_BlockCount));
		if (m_FlagHasTimeCode) {
			m_Subtitle.push_back(timeStartSwap + " --> " + timeEndSwap);
		}
		m_Subtitle.push_back(text);
		m_Subtitle.push_back(GetBlockSep());
	}

	m_Logs.push_back(GenCurrentDateTime() + u8" ������Ļ�ɹ�, �ܼ�:" + std::to_string(m_BlockCount));
	return true;
}
