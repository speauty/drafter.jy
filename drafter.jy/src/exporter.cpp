#include "exporter.h"

std::string tmpStr = "";

Exporter::Exporter()
	:m_SourceFilePath(""), m_TargetFilePath(""), m_FlagHasTimeCode(true), m_BlockCount(0), m_FlagTargetFileForceOverride(false), m_FlagTargetFileReflush(false), m_FlagIsValidated(false), m_FlagIsAccessed(false), m_TargetExt("srt")
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
}

void Exporter::SetTargetFilePath(const std::string& path)
{
	m_TargetFilePath = path;
}

void Exporter::SetFlagHasTimeCode(bool flag)
{
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

void Exporter::Validate()
{
	m_FlagIsValidated = true;
	std::string sourceFile = GetSourceFilePath();
	std::string targetFile = GetTargetFilePath();
	if (sourceFile.empty()) {
		m_Logs.push_back(GenCurrentDateTime() + u8" 暂无剪映源件");
	} else {
		std::fstream fs(sourceFile);
		if (!fs.is_open()) {
			m_Logs.push_back(GenCurrentDateTime() + u8" 剪映源件打开异常[" + sourceFile + "]");
		} else {
			if (m_JsonReader.parse(fs, m_JsonData, true)) {
				if (m_JsonData.empty()) {
					m_Logs.push_back(GenCurrentDateTime() + u8" 剪映源件JSON解析暂无数据, 暂定非法文件[" + sourceFile + "]");
				}
				m_FlagIsAccessed = true;
			} else {
				m_Logs.push_back(GenCurrentDateTime() +  u8" 剪映源件JSON解析异常[" + sourceFile + "]");
			}
		}
		fs.close();
	}

	if (targetFile.empty()) {
		m_Logs.push_back(GenCurrentDateTime() + u8" 暂无导出文件");
		m_FlagIsAccessed = false;
	} else {
		std::fstream ts(targetFile);
		if (ts.is_open() && !m_FlagTargetFileForceOverride) {
			m_Logs.push_back(GenCurrentDateTime() + u8" 导出文件已存在[" + targetFile + "]");
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
	std::ofstream out;
	out.open(m_TargetFilePath, std::ios::out|std::ios::trunc);
	if (!out.is_open()) {
		tmpStr.clear();
		tmpStr = "转存文件打开异常[" + m_TargetFilePath + "]";
		ERR_PRINT(tmpStr);
		out.close(); return false;
	}
	m_FlagTargetFileReflush = true;
	
	std::ifstream source(m_SourceFilePath);

	if (!m_JsonReader.parse(source, m_JsonData)) {
		tmpStr.clear();
		tmpStr = "草稿文件JSON解析异常[" + m_SourceFilePath + "]";
		ERR_PRINT(tmpStr);
		source.close(); 
		out.close(); 
		DelTargetFile();
		return false;
	}

	if (m_JsonData.empty() || !m_JsonData.isMember("materials") || !m_JsonData["materials"].isMember("texts") || !m_JsonData.isMember("tracks")) {
		tmpStr.clear();
		tmpStr = "草稿文件解析数据无相关索引, 暂定非法文件[" + m_SourceFilePath + "]";
		ERR_PRINT(tmpStr);
		source.close();
		out.close();
		DelTargetFile();
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
		out << m_BlockCount << std::endl
			<< timeStartSwap << " --> " << timeEndSwap << std::endl
			<< text << std::endl
			<< std::endl;
	}

	source.close();
	out.close();

	return true;
}

bool Exporter::GenSubtitle()
{
	Validate();
	if (!m_FlagIsAccessed) {
		m_Logs.push_back(GenCurrentDateTime() + u8" 未通过检测");
		m_Logs.push_back(GenCurrentDateTime() + u8" 生成字幕失败");
		return false;
	}

	if (m_JsonData.empty() || !m_JsonData.isMember("materials") || !m_JsonData["materials"].isMember("texts") || !m_JsonData.isMember("tracks")) {
		m_Logs.push_back(GenCurrentDateTime() + u8" 剪映源件JSON解析数据无相关索引, 暂定非法文件[" + GetSourceFilePath() + "]");
		m_Logs.push_back(GenCurrentDateTime() + u8" 生成字幕失败");
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

	m_Logs.push_back(GenCurrentDateTime() + u8" 生成字幕成功, 总计:" + std::to_string(m_BlockCount));
	return true;
}
