#include "EditorConsole.h"
#include "cstdlib"
#include "cctype"
#include "D3E/Debug.h"
#include "D3E/engine/ConsoleManager.h"
#include <iostream>

D3E::EditorConsole::EditorConsole()
{
	open_ = true;
	memset(inputBuffer_, 0, sizeof(inputBuffer_));
	historyPos_ = -1;
	autoScroll_ = true;
	scrollToBottom_ = false;
}

static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

int D3E::EditorConsole::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			Debug::LogMessage("Input text complete callback");
			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = historyPos_;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (historyPos_ == -1)
					historyPos_ = history_.Size - 1;
				else if (historyPos_ > 0)
					historyPos_--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (historyPos_ != -1)
					if (++historyPos_ >= history_.Size)
						historyPos_ = -1;
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != historyPos_)
			{
				const char* history_str = (historyPos_ >= 0) ? history_[historyPos_] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
	}
	return 0;
}
int D3E::EditorConsole::TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
	auto console = (EditorConsole*)data->UserData;
	return console->TextEditCallback(data);
}

void D3E::EditorConsole::Draw()
{
	ImGui::Begin("Console", &open_);
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Close Console"))
			open_ = false;
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &autoScroll_);
		ImGui::EndPopup();
	}

	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");

	ImGui::Separator();

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
	{
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::Selectable("Clear")) ClearLog();
					ImGui::EndPopup();
				}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

		for (const auto& item : items_)
		{
			ImVec4 color;
			bool has_color = false;
//			if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
//			else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
			if (has_color)
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item.c_str());
			if (has_color)
				ImGui::PopStyleColor();
		}

		if (scrollToBottom_ || (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		scrollToBottom_ = false;

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
	ImGui::Separator();

	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText("Input", inputBuffer_, IM_ARRAYSIZE(inputBuffer_), input_text_flags, &TextEditCallbackStub, (void*)this))
	{
		char* s = inputBuffer_;
		if (s[0])
		{
			Strtrim(s);
			Debug::LogMessage(s);
			ConsoleManager::getInstance()->handleConsoleInput(s);
			strcpy(s, "");
		}

		reclaim_focus = true;
	}

	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1);

	ImGui::End();
}
void D3E::EditorConsole::PrintMessage(const eastl::string& str)
{
	items_.push_back(str);
}
void D3E::EditorConsole::ClearLog()
{
	items_.clear();
}

