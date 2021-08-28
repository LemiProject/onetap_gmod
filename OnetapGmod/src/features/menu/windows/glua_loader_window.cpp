#include "glua_loader_window.h"

#include <thread>

#include "imgui/im_tools.h"
#include "imgui/TextEditor.h"

#include "../../lua_futures/lua_futures.h"

#include "../widgets/widgets.h"

using namespace ImGui;

bool glua_loader_initialized;

TextEditor text_editor;

void initialize() {
	text_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	text_editor.SetShowWhitespaces(false);
	text_editor.SetDrawTooltip(false);
	
	glua_loader_initialized = true;
}

ImVec2 calc_text_editor_size() {
	auto ts = CalcTextSize("AAA").y;
	ts += GetStyle().ItemSpacing.y * 2 + GetStyle().ItemInnerSpacing.y * 2 + GetStyle().WindowPadding.y;
	return { GetWindowSize().x, GetWindowSize().y - ts * 2 };
}

void add_to_run() {
	auto text = text_editor.GetText();
	if (!text.empty()) {
		lua_futures::add_code_to_run(text);
	}
}

void glua_loader_window::draw_glua_loader_window(bool& draw_glua_loader) {
	static float alpha;
	
	if (!glua_loader_initialized) {
		initialize();
	}

	if (draw_glua_loader) {
		alpha = ImMin(alpha + GetIO().DeltaTime * 4.f, 1.f);
	} else {
		alpha = ImMax(alpha - GetIO().DeltaTime * 4.f, 0.f);
	}

	if (draw_glua_loader || alpha > 0.f) {
		BeginApplyAlpha(alpha);
		Begin(u8"Glua##LUA_EXECUTOR_WINDOW");

		text_editor.Render("##LUA_EXECUTOR", calc_text_editor_size());

		if (CenterButton("Execute", GetContentRegionAvail())) {
			std::thread(add_to_run).detach();
		}

		End();
		EndApplyAlpha();
	}
}
