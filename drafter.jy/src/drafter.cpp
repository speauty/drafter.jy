#include "drafter.h"
#include <iostream>
#include "Utils.h"

namespace drafter {
	Drafter::Drafter()
		:m_FileType(drafter::SRT), m_Paths(), m_ForceOverride(true)
	{
	}
	Drafter::~Drafter()
	{
	}

	void Drafter::OnImGuiRender()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // Dock模式
        ImGui::Begin(u8"剪映字幕处理工具");

		ImGui::Text(u8"注意事项");
		ImGui::SameLine();
		ImGui::Text(u8"     系统时间:");
		ImGui::SameLine();
		ImGui::Text(GenCurrentDateTime().c_str());
		ImGui::BulletText(u8"路径采用绝对路径，不要包含中文字符！！！");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button(u8"自动探测剪映源件")) {
			
		}
		ImGui::SameLine();
		ImGui::Text(u8"该功能暂未实现");


		const char* items[] = {u8"暂无剪映源", u8"测试数据方式发回来发哈JFK"};
		static int item_current = 0;
		if (ImGui::Combo(u8"剪映源", &item_current, items, IM_ARRAYSIZE(items), 3) && item_current > 0){
			strcpy_s(m_Paths[0], items[item_current]);
			m_Exporter.SetSourceFilePath(std::string(m_Paths[0]));
		}

		if (ImGui::InputTextWithHint(u8"剪映源路径", u8"请输入剪映源路径", m_Paths[0], IM_ARRAYSIZE(m_Paths[0]))) {
			m_Exporter.SetSourceFilePath(std::string(m_Paths[0]));
		}

		if (ImGui::InputTextWithHint(u8"导出路径", u8"请输入字幕导出路径", m_Paths[1], IM_ARRAYSIZE(m_Paths[1]))) {
			m_Exporter.SetTargetFilePath(std::string(m_Paths[1]));
		}

		if (ImGui::Checkbox(u8"是否强制覆盖导出路径", &m_ForceOverride)) {
			m_Exporter.SetFlagTargetFileForceOverride(m_ForceOverride);
		}

		ImGui::Text(u8"文件格式");
		ImGui::SameLine();
		if (ImGui::RadioButton("srt", (int*)&m_FileType, (int)SRT)) {
			m_Exporter.SetTargetExt("srt");
			m_Exporter.SetFlagHasTimeCode(m_FileType == TXT_NO_TIME);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("txt", (int*)&m_FileType, (int)TXT)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(m_FileType == TXT_NO_TIME);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"txt(无时间码)", (int*)&m_FileType, (int)TXT_NO_TIME)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(m_FileType == TXT_NO_TIME);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text(u8"可选操作");
		ImGui::SameLine();
		if (ImGui::Button(u8"校验")) {
			m_Exporter.Validate();
		}
		
		ImGui::SameLine();
		if (ImGui::Button(u8"重置")) {
			std::cout << "重置" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"生成")) {
			if (m_Exporter.GenSubtitle()) {
				std::vector<std::string> subtitle;
				subtitle = m_Exporter.GetSubtitle();

				for (size_t i = 0; i < subtitle.size(); i++)
				{
					if (subtitle[i] == m_Exporter.GetBlockSep()) {
						std::cout << std::endl;
						continue;
					}
					std::cout << subtitle[i] << std::endl;
				}
			}
			
			std::cout << "生成字幕" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"预览")) {
			std::cout << "预览字幕" << std::endl;
		}
		ImGui::SameLine();
		
		if (ImGui::Button(u8"导出")) {
			std::cout << "导出字幕" << std::endl;
			//mExporter.ExecExport();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextWrapped(
			u8"当前配置信息\n%s: %s\n%s: %s\n%s: %d",
			u8"剪映源", m_Exporter.GetSourceFilePath().c_str(),
			u8"导出路径", m_Exporter.GetTargetFilePath().c_str(),
			u8"验证结果", m_Exporter.GetFlagIsAccessed()
		);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static std::vector<std::string> logs;
		logs = m_Exporter.GetLogs();
		ImGui::Text(u8"输出");
		for (int i = logs.size() - 1; i >= 0; i--)
		{
			ImGui::BulletText(logs[i].c_str());
		}

		

        ImGui::End();
	}
}