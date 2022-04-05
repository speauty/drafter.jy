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

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // Dockģʽ
		//ImGuiID dockspaceID = ImGui::GetID(ID().c_str());
        ImGui::Begin(u8"��ӳ��Ļ������", NULL, window_flags);
		//ImGuiDockNode* Node = ImGui::DockBuilderGetNode(ImGui::GetHoveredID());
		//Node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
		ImGui::Text(u8"ע������");
		ImGui::SameLine();
		ImGui::Text(u8"     ϵͳʱ��:");
		ImGui::SameLine();
		ImGui::Text(GenCurrentDateTime().c_str());
		ImGui::BulletText(u8"·�����þ���·������Ҫ���������ַ�������");
		ImGui::BulletText(u8"��֧�����ӳ��װ·��ͬ���Ĳݸ�·��������");
		ImGui::BulletText(u8"����Ĳݸ�·��ָJianyingPro Drafts������");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		static std::vector<std::string> files;
		static const char* items[10] = { u8"���޼�ӳԴ", "", "", "", "", "", "", "", "", "" };
		//static const char* tmpStr = u8"���޼�ӳԴ\0";
		if (ImGui::Button(u8"�Զ�̽���ӳԴ��")) {
			files = GetJYInstallDir();
			//std::string str;
			for (size_t i = 0; i < files.size() && i < 10; i++) {
				items[i] = files[i].c_str();
				//str += files[i] + "\\0";
			}
			//tmpStr = str.c_str();
			//std::cout << tmpStr << std::endl;
		}

		static int itemCurrent = 0;
		// const char* items_separated_by_zeros
		if (ImGui::Combo(u8"��ӳԴ", &itemCurrent, items, IM_ARRAYSIZE(items), 3) && items[itemCurrent] != "") {
		//if (ImGui::Combo(u8"��ӳԴ", &itemCurrent, tmpStr, 3) && items[itemCurrent] != "") {
			strcpy_s(m_Paths[0], items[itemCurrent]);
			m_Exporter.SetSourceFilePath(std::string(m_Paths[0]));
		}

		if (ImGui::InputTextWithHint(u8"��ӳԴ·��", u8"�������ӳԴ·��", m_Paths[0], IM_ARRAYSIZE(m_Paths[0]))) {
			m_Exporter.SetSourceFilePath(std::string(m_Paths[0]));
		}

		if (ImGui::InputTextWithHint(u8"����·��", u8"��������Ļ����·��", m_Paths[1], IM_ARRAYSIZE(m_Paths[1]))) {
			m_Exporter.SetTargetFilePath(std::string(m_Paths[1]));
		}

		if (ImGui::Checkbox(u8"�Ƿ�ǿ�Ƹ��ǵ���·��", &m_ForceOverride)) {
			m_Exporter.SetFlagTargetFileForceOverride(m_ForceOverride);
		}

		ImGui::Text(u8"�ļ���ʽ");
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
		if (ImGui::RadioButton(u8"txt(��ʱ����)", (int*)&m_FileType, (int)TXT_NO_TIME)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(false);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button(u8"�����־")) {
			m_Exporter.ResetLogs();
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"У��")) {
			m_Exporter.Validate();
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"����")) {
			m_Exporter.ResetSubtitle();
			std::cout << "����" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"����")) {
			m_Exporter.GenSubtitle();
			std::cout << "������Ļ" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"����")) {
			std::cout << "������Ļ" << std::endl;
			m_Exporter.ExecExport();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::TextWrapped(
			u8"��ǰ������Ϣ\n%s: %s\n%s: %s\n%s: %d",
			u8"��ӳԴ", m_Exporter.GetSourceFilePath().c_str(),
			u8"����·��", m_Exporter.GetTargetFilePath().c_str(),
			u8"��֤���", m_Exporter.GetFlagIsAccessed()
		);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static std::vector<std::string> logs;
		logs = m_Exporter.GetLogs();
		ImGui::Text(u8"��־");
		ImGui::BeginChild("Log");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGuiTextBuffer log;
		for (int i = logs.size() - 1; i >= 0; i--) {
			log.appendf("%-2d %s\n", i+1, logs[i].c_str());
		}
		ImGui::TextUnformatted(log.begin(), log.end());
		ImGui::PopStyleVar();
		ImGui::EndChild();
		

		

        ImGui::End();
	}
}