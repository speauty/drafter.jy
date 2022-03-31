#include "drafter.h"
#include <iostream>
#include "Utils.h"
#include <imgui/imgui_internal.h>

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
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // Dock模式
		//ImGuiID dockspaceID = ImGui::GetID(ID().c_str());
        ImGui::Begin(u8"剪映字幕处理工具", NULL, window_flags);
		//ImGuiDockNode* Node = ImGui::DockBuilderGetNode(ImGui::GetHoveredID());
		//Node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
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
			m_Exporter.SetFlagHasTimeCode(true);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("txt", (int*)&m_FileType, (int)TXT)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(true);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"txt(无时间码)", (int*)&m_FileType, (int)TXT_NO_TIME)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(false);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button(u8"清除日志")) {
			m_Exporter.ResetLogs();
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"校验")) {
			m_Exporter.Validate();
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"重置")) {
			m_Exporter.ResetSubtitle();
			std::cout << "重置" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"生成")) {
			m_Exporter.GenSubtitle();
			std::cout << "生成字幕" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"导出")) {
			std::cout << "导出字幕" << std::endl;
			m_Exporter.ExecExport();
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
		ImGui::Text(u8"日志");
		ImGui::BeginChild("Log");
			// Multiple calls to Text(), not clipped (slow)
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		for (int i = logs.size() - 1; i >= 0; i--) {
			ImGui::Text(logs[i].c_str());
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();
		

		

        ImGui::End();
	}
}