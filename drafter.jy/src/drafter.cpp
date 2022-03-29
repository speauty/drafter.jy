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
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // Dockģʽ
        ImGui::Begin(u8"��ӳ��Ļ������");

		ImGui::Text(u8"ע������");
		ImGui::SameLine();
		ImGui::Text(u8"     ϵͳʱ��:");
		ImGui::SameLine();
		ImGui::Text(GenCurrentDateTime().c_str());
		ImGui::BulletText(u8"·�����þ���·������Ҫ���������ַ�������");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button(u8"�Զ�̽���ӳԴ��")) {
			
		}
		ImGui::SameLine();
		ImGui::Text(u8"�ù�����δʵ��");


		const char* items[] = {u8"���޼�ӳԴ", u8"�������ݷ�ʽ����������JFK"};
		static int item_current = 0;
		if (ImGui::Combo(u8"��ӳԴ", &item_current, items, IM_ARRAYSIZE(items), 3) && item_current > 0){
			strcpy_s(m_Paths[0], items[item_current]);
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
			m_Exporter.SetFlagHasTimeCode(m_FileType == TXT_NO_TIME);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("txt", (int*)&m_FileType, (int)TXT)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(m_FileType == TXT_NO_TIME);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton(u8"txt(��ʱ����)", (int*)&m_FileType, (int)TXT_NO_TIME)) {
			m_Exporter.SetTargetExt("txt");
			m_Exporter.SetFlagHasTimeCode(m_FileType == TXT_NO_TIME);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text(u8"��ѡ����");
		ImGui::SameLine();
		if (ImGui::Button(u8"У��")) {
			m_Exporter.Validate();
		}
		
		ImGui::SameLine();
		if (ImGui::Button(u8"����")) {
			std::cout << "����" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"����")) {
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
			
			std::cout << "������Ļ" << std::endl;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"Ԥ��")) {
			std::cout << "Ԥ����Ļ" << std::endl;
		}
		ImGui::SameLine();
		
		if (ImGui::Button(u8"����")) {
			std::cout << "������Ļ" << std::endl;
			//mExporter.ExecExport();
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
		ImGui::Text(u8"���");
		for (int i = logs.size() - 1; i >= 0; i--)
		{
			ImGui::BulletText(logs[i].c_str());
		}

		

        ImGui::End();
	}
}