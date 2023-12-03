#pragma once

#include "imgui.h"

namespace D3E
{
	class EditorConsole
	{
	private:
		bool open_;
		char inputBuffer_[256];
		ImVector<char*> items_;
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
		void ClearLog();
	};
}

