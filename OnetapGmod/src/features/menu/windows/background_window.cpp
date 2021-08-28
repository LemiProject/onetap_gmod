#include "background_window.h"

#include <imgui/im_tools.h>

#include "glua_loader_window.h"
#include "../../../render_system/render_system.h"

using namespace ImGui;

bool draw_envirompment_list = false;
bool draw_glua_loader = false;


std::string current_time()
{
	time_t now = time(NULL);
	struct tm tstruct;
	char buf[40];
	tstruct = *localtime(&now);
	//format: HH:mm:ss
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

void menu_tab_selectable(const std::string& name, bool* var) {
	auto text_size = CalcTextSize(name.c_str(), 0, true);

	//PushStyleColor(ImGuiCol_Text, *var ? GetStyleColorVec4(ImGuiCol_TextSelectedBg) : GetStyleColorVec4(ImGuiCol_Text));
	if (TopbarSelectable(name.c_str(), *var, 0, { text_size.x + 16, 0 })) {
		*var = !*var;
	}
	//PopStyleColor();
	SameLine(0, 2);
	GetWindowDrawList()->AddLine({ GetItemRectMax().x + 1, GetItemRectMin().y }, { GetItemRectMax().x + 1, GetItemRectMax().y }, GetColorU32(ImGuiCol_Separator), 2);
	
}

void background_window::draw_background_window() {
	auto& io = GetIO();

	PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f, 3.f });
	PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });
	
	if (BeginMainMenuBar()) {
		menu_tab_selectable("glua loader##WITTCHEN_GLUA_LOADER", &draw_glua_loader);
		menu_tab_selectable("envirompment list##WITTCHEN_ENV_LIST", &draw_envirompment_list);		

		{
			const std::string time = current_time();
			SetCursorPosX(io.DisplaySize.x - CalcTextSize(time.c_str()).x - GetStyle().ItemSpacing.x);
			GetWindowDrawList()->AddLine({ GetCursorPosX() - 4, 0 }, { GetCursorPosX() - 4, GetWindowSize().y }, GetColorU32(ImGuiCol_Separator), 2);
			Text(time.c_str());
		}
		
		EndMainMenuBar();
	}
	
	PopStyleVar(2);
	
	Begin(u8"Измени мой стайл))))");

	ShowStyleEditor();

	End();

#ifdef _DEBUG
	ShowMetricsWindow();
#endif

	glua_loader_window::draw_glua_loader_window(draw_glua_loader);
	
}
