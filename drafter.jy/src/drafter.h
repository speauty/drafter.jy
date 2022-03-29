#pragma once
#include <imgui/imgui.h>
#include "exporter.h"

namespace drafter {
	enum FileTypes
	{
		SRT = 1, TXT = 2, TXT_NO_TIME = 3
	};


	class Drafter {
	private:
		char m_Paths[2][512]; // 0Ϊ��ĻԴ·�� 1Ϊ����·��
		FileTypes m_FileType;
		bool m_ForceOverride;
		Exporter m_Exporter;

		public:
			Drafter();
			~Drafter();
			void OnImGuiRender();
	};
}