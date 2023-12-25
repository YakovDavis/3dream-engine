#pragma once

#include "D3E/Debug.h"
#include "EASTL/map.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "imgui.h"

namespace D3E
{
	class EditorConsole
	{
	private:
		bool open_;
		char inputBuffer_[256];
		eastl::vector<eastl::string> items_;
		eastl::map<eastl::string, D3E::Debug::TextColor> itemColors_;
		ImVector<char*> history_;
		int historyPos_;
		ImGuiTextFilter filter_;
		bool autoScroll_;
		bool scrollToBottom_;
	private:
		int TextEditCallback(ImGuiInputTextCallbackData* data);
		static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	public:
		EditorConsole();
		void Draw();
		void PrintMessage(const eastl::string& str, D3E::Debug::TextColor color);
		void ClearLog();
	};
}

