#pragma once

#include <mutex>
#include <queue>

#include <imgui/im_tools.h>

#include "../../esp/esp.h"
#include "../../../game_sdk/entities/c_base_player.h"

namespace ImGui {
	IMGUI_API bool WittchenCheckbox(const std::string& name, const std::string& var);
	IMGUI_API bool WittchenCheckbox(const std::string& name, bool* var);

	IMGUI_API void WittchenSlider(const std::string& name, const std::string& var, const ImVec2& minmax);
	IMGUI_API void WittchenSlider(const std::string& name, float* var, const ImVec2& minmax);
	IMGUI_API void WittchenSlider(const std::string& name, int* var, const ImVec2& minmax);
	
	IMGUI_API void BeginApplyAlpha(float alpha);
	IMGUI_API void EndApplyAlpha();

	IMGUI_API bool ColorsEdit4(const std::string& str_id, c_color& color, ImGuiColorEditFlags flags);

	IMGUI_API ImVec2 Translate(const ImVec2& from, const ImVec2& to, float speed = 1.f);
}

namespace Wittchen {
	struct WitthcenEspStyleEditor {
		std::mutex esp_editor_mutex;
		esp::c_esp_box temp_box;
		
		WitthcenEspStyleEditor() = default;

		static c_color GetAutoColor(const std::string& name, c_base_entity* entity);
		static c_color GetAutoColor(const std::string& name, c_base_player* player);
	};

	WitthcenEspStyleEditor* GetWittchenEspStyleEditor();
	
	void InitializeEspStyleEditor();
	
	void ApplyStyleToBox(esp::c_esp_box& box);

	void DrawEspEditor();
}
