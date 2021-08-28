#include "widgets.h"

#include "lemi_utils.h"
#include "../../../settings/settings.h"
#include "../menu.h"

#include "../../esp/esp.h"
#include "../../../render_system/render_system.h"
#include "../../../render_system/render_helpers.h"
#include "../../../utils/input_system.h"

#include <imgui/imgui_internal.h>

using namespace ImGui;

bool ImGui::WittchenCheckbox(const std::string& name, const std::string& var) {
	return WittchenCheckbox(name, &settings::get_bool(var));
}

ImVec2 CalcBindListPopupChildSize() {
	auto ts = CalcTextSize("AAA").y;
	ts += GetStyle().ItemSpacing.y * 2 + GetStyle().ItemInnerSpacing.y * 2 + GetStyle().WindowPadding.y;
	return { GetWindowSize().x, GetWindowSize().y - ts * 2 };
}

bool ImGui::WittchenCheckbox(const std::string& name, bool* var) {
	static float animations[100000000];
	
	auto& v = *var;
	auto id = GenerateAnimationId();
	auto io = GetIO();
	
	auto bc = GetStyleColorVec4(ImGuiCol_Border);
	auto bcs = GetStyleColorVec4(ImGuiCol_BorderShadow);
	bc.w = animations[id];
	bcs.w = 0.f;

	auto check_box_id = GetCurrentWindow()->GetID(name.c_str());
	auto bind_id = bind_system::generate_bind_id(check_box_id);
	
	if (!bind_system::bool_binds[bind_id].empty() && bind_system::bool_binds[bind_id][0].type != (int)bind_system::bind_type::none)
		bc.x = animations[id];
	
	
	PushStyleColor(ImGuiCol_Border, bc);
	PushStyleColor(ImGuiCol_BorderShadow, bcs);
	PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
	auto ret = Checkbox(name.c_str(), &v);
	PopStyleVar();
	PopStyleColor(2);

	
	
	if (IsItemHovered()) {
		animations[id] = ImMin(animations[id] + io.DeltaTime * ELEMENT_FADE_ANIMATION_SPEED, 1.f);
	}
	else {
		animations[id] = ImMax(animations[id] - io.DeltaTime * ELEMENT_FADE_ANIMATION_SPEED, 0.f);
	}

	if (BeginPopupContextItem()) {
		auto& current_binds = bind_system::bool_binds[bind_id];
		if (current_binds.empty())
			input_system::add_bind(bind_id, 0, &v, bind_system::bind_type::none, !v);
		
		//BeginChild("CHECKBOX_BINDS_CHILD", {300, 100});
		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 4 });
		
		auto n = 0;
		for (auto& i : current_binds) {
			Hotkey(("##KEY" + std::to_string(check_box_id) + std::to_string(n * 99)).c_str(), &i.key, {120.f, 0});
			SameLine();
			
			PushItemWidth(120.f);
			Combo(("##BINDTYPE" + std::to_string(check_box_id) + std::to_string(n * 99)).c_str(), &i.type, bind_system::bind_type_string, 4);
			PopItemWidth();
			n++;
		}
		PopStyleVar();

		Text("Bind id is %i", bind_id);
		
		EndPopup();
	}
	
	return ret;
}
void ImGui::WittchenSlider(const std::string& name, const std::string& var, const ImVec2& minmax) {
	WittchenSlider(name, &settings::get_int(var), minmax);
}

void ImGui::WittchenSlider(const std::string& name, float* var, const ImVec2& minmax) {
	
}

void ImGui::WittchenSlider(const std::string& name, int* var, const ImVec2& minmax) {
	SliderInt(name.c_str(), var, minmax.x, minmax.y);

	auto check_box_id = GetCurrentWindow()->GetID(name.c_str());
	auto bind_id = bind_system::generate_bind_id(check_box_id);

	if (BeginPopupContextItem()) {
		auto& current_binds = bind_system::int_binds[bind_id];
		if (current_binds.empty())
			input_system::add_bind(bind_id, 0, var, bind_system::bind_type::none, *var);

		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4, 4 });

		auto n = 0;
		for (auto& i : current_binds) {
			Hotkey(("##KEY" + std::to_string(check_box_id) + std::to_string(n * 99)).c_str(), &i.key, { 120.f, 0 });
			SameLine();

			PushItemWidth(120.f);
			Combo(("##BINDTYPE" + std::to_string(check_box_id) + std::to_string(n * 99)).c_str(), &i.type, bind_system::bind_type_string, 4);
			SameLine();
			SliderInt(("##NEWVAL" + std::to_string(check_box_id) + std::to_string(n * 99)).c_str(), &i.bind_value, minmax.x, minmax.y);
			PopItemWidth();
			n++;
		}
		PopStyleVar();

		Text("Bind id is %i", bind_id);

		EndPopup();
	}
}

void ImGui::BeginApplyAlpha(float alpha) {
	alpha = menu::menu_alpha == 1.f ? alpha : menu::menu_alpha;
	PushStyleVar(ImGuiStyleVar_Alpha, alpha);
}

void ImGui::EndApplyAlpha() {
	PopStyleVar();
}

bool ImGui::ColorsEdit4(const std::string& str_id, c_color& color, ImGuiColorEditFlags flags) {
	std::array<float, 4> tmp = color.get_clamped();
	const auto ret = ColorEdit4(str_id.c_str(), tmp.data(), flags);
	color = c_color(tmp);
	return ret;
}

ImVec2 ImGui::Translate(const ImVec2& from, const ImVec2& to, float speed) {
	if (from.x == to.x && from.y == to.y) return from;
	return { ImLinearSweep(from.x, to.x, ImGui::GetIO().DeltaTime * speed), ImLinearSweep(from.y, to.y, ImGui::GetIO().DeltaTime * speed) };
}

static Wittchen::WitthcenEspStyleEditor g_style_editor;

c_color Wittchen::WitthcenEspStyleEditor::GetAutoColor(const std::string& name, c_base_entity* entity) {
	return colors::white_color;
}

c_color Wittchen::WitthcenEspStyleEditor::GetAutoColor(const std::string& name, c_base_player* player) {
	if (name == "%health") {
		float g = 255 * (player->get_health_procentage() / 100.f);
		return { 255 - g, g, 0 };
	}
	if (name == "%name") {
		return player->get_team_color();
	}
	
	//TODO: IMPL OTHER

	return colors::white_color;
}

Wittchen::WitthcenEspStyleEditor* Wittchen::GetWittchenEspStyleEditor() {
	return &g_style_editor;
}

void Wittchen::InitializeEspStyleEditor() {
	g_style_editor.temp_box.min = { 0, 0 };
	g_style_editor.temp_box.max = { 200, 300 };
	g_style_editor.temp_box.color = colors::white_color;
	g_style_editor.temp_box.border_color = colors::black_color;
	
	g_style_editor.temp_box.text_storage.strings.insert({ esp::c_esp_box::generate_id(), esp::esp_text_t{
		"%name", directx_render::e_font_flags::font_outline, -1.f, colors::white_color, true, (int)esp::e_esp_text_position::top
	} });
	g_style_editor.temp_box.text_storage.strings.insert({ esp::c_esp_box::generate_id(), esp::esp_text_t{
	"%health", directx_render::e_font_flags::font_outline, -1.f, colors::white_color, true, (int)esp::e_esp_text_position::right
	} });
	
}


void Wittchen::ApplyStyleToBox(esp::c_esp_box& box) {
	
	//box style
	box.color = g_style_editor.temp_box.color;
	box.rounding = g_style_editor.temp_box.rounding;
	box.type =  settings::get_int("esp_type");
	box.border_color = g_style_editor.temp_box.border_color;
	
	//text applying
	box.text_storage.strings = g_style_editor.temp_box.text_storage.strings;
}

constexpr auto player_name = "voidptr_t";
constexpr auto player_health = 100;

std::string formatPreviewText(const std::string& str) {
	
	std::string out = str;
	out = replace_all(out, "%name", player_name);
	out = replace_all(out, "%health", std::to_string(player_health));
	
	return out;
}

int getTopSize() {
	auto n = 1;
	for (auto& i : g_style_editor.temp_box.text_storage.strings) {
		if (i.second.relative_position == 0) {
			n++;
		}
	}
	return n;
}

int getDownSize() {
	auto n = 1;
	for (auto& i : g_style_editor.temp_box.text_storage.strings) {
		if (i.second.relative_position == (int)esp::e_esp_text_position::down) {
			n++;
		}
	}
	return n;
}

float getLeftSize(ImFont* font, float ts) {
	auto s = 3;
	for (auto& i : g_style_editor.temp_box.text_storage.strings) {
		if (i.second.relative_position == (int)esp::e_esp_text_position::left && i.second.text.length() > s) {
			s = i.second.text.length();
		}
	}

	std::string st;
	for (auto i = 0; i <= s; ++i) {
		st += "A";
	}
	
	return font->CalcTextSizeA(ts, FLT_MAX, 0, st.c_str()).x;
}

void renderEspText(const esp::esp_text_t& text, ImFont* font, float size, const ImVec2& pos) {
	draw_types::text_draw_type_t t;
	t.text = text.text;
	t.color = text.color;
	t.flags = text.flags;
	t.font = font;
	t.size = size;
	t.pos = pos;
	t.draw(GetWindowDrawList());
}

std::string getButtonIdByEspID(uint64_t id) {
	return std::string("ESP_INTERNAL_BUTTON##" + std::to_string(id));
}

esp::esp_text_t& get_esp_text_by_id(uint64_t id) {
	return g_style_editor.temp_box.text_storage.strings[id];
}

__forceinline void CustomiseEspTextDropTarget(int new_position) {
	if (BeginDragDropTarget()) {
		auto text = AcceptDragDropPayload("EspText##ESPCUSTOMTEXT", ImGuiDragDropFlags_AcceptBeforeDelivery);
		if (text)
			if ((uint64_t*)(text->Data) != nullptr)
				get_esp_text_by_id(*(uint64_t*)(text->Data)).relative_position = new_position;
		EndDragDropTarget();
	}
}

__forceinline void CustomiseEspTextDropSource(uint64_t current_id) {
	if (BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
		SetDragDropPayload("EspText##ESPCUSTOMTEXT", &current_id, sizeof(uint64_t));

		//Text("Drag to %i", get_esp_text_by_id(current_id).relative_position);
		
		EndDragDropSource();
	}
}

void Wittchen::DrawEspEditor() {
	static bool draw_preview = false;
	static int rp = 0;
	
	BeginGroup();
	{

		InputInt("POS", &rp);
		
		if (Button("Add health")) {
			g_style_editor.temp_box.text_storage.strings.insert({esp::c_esp_box::generate_id(), esp::esp_text_t {"%health", 0, 16.f, c_color(), true, rp } });
		}
		SameLine();
		if (Button("Add name")) {
			g_style_editor.temp_box.text_storage.strings.insert({ esp::c_esp_box::generate_id(), esp::esp_text_t{ "%name", 0, 16.f, colors::white_color, true, rp } });
		}

		
		if (Button("Open preview")) draw_preview = !draw_preview;

		if (draw_preview) {
			Begin("ESP PREVIEW", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

			auto temp_box = g_style_editor.temp_box;
			
			const ImVec2 box_size = {
				temp_box.max.x - temp_box.min.x,
				temp_box.max.y - temp_box.min.y
			};

			ImVec2 center_pos;
			//auto font_size = text.size == -1 ? calc_font_size(box) : text.size;
			auto text_size = render_system::fonts::nunito_font[2]->CalcTextSizeA(16.f, FLT_MAX, 0.f, player_name);

			PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });
			PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2.f, 2.f });
			
			BeginGroup();
			
			Dummy({ GetContentRegionAvail().x, text_size.y* getTopSize() }); //top
			CustomiseEspTextDropTarget(0);
			
			Dummy({getLeftSize(render_system::fonts::nunito_font[2], 16.f), box_size.y }); SameLine(); //left
			CustomiseEspTextDropTarget(3);
			
			InvisibleButton("BoxDummy", box_size); SameLine(); center_pos = GetItemRectMin(); //center
			if (BeginPopupContextItem()) {
				ColorsEdit4("Box color##ESPBOXCOLOR", g_style_editor.temp_box.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				SliderFloat("Rounding##ESPBOX_ROUNDING", &g_style_editor.temp_box.rounding, 0.f, 12.f, "%.0f", 1.f);
				constexpr const char* const types[] = {"filled", "border", "corner"};
				Combo("Box type##ESPBOXTYPE", &g_style_editor.temp_box.type, types, 3);
				if (g_style_editor.temp_box.type == (int)esp::box_type::border)
					ColorsEdit4("Border color##ESPBOXCOLOR", g_style_editor.temp_box.border_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
					
				
				EndPopup();
			}
			
			Dummy( { text_size.x, box_size.y }); //right
			CustomiseEspTextDropTarget(1);
			Dummy({ GetContentRegionAvail().x, text_size.y * getDownSize() }); //down
			CustomiseEspTextDropTarget(2);

			EndGroup();
			
			PopStyleVar(2);

			temp_box.min = center_pos;
			temp_box.max = { center_pos.x + box_size.x, center_pos.y + box_size.y };


			{

				switch ((esp::box_type)temp_box.type) {
				case esp::box_type::filled:
					GetWindowDrawList()->AddRect(temp_box.min, temp_box.max, temp_box.color.get_u32(), temp_box.rounding);
					break;
				case esp::box_type::border:
					GetWindowDrawList()->AddRect({ temp_box.min.x - 1.f, temp_box.min.y - 1.f }, { temp_box.max.x + 1.f, temp_box.max.y + 1.f }, temp_box.border_color, temp_box.rounding);
					GetWindowDrawList()->AddRect({ temp_box.min.x + 1.f, temp_box.min.y + 1.f }, { temp_box.max.x - 1.f, temp_box.max.y - 1.f }, temp_box.border_color, temp_box.rounding);
					GetWindowDrawList()->AddRect(temp_box.min, temp_box.max, temp_box.color.get_u32(), temp_box.rounding);
					break;
				case esp::box_type::corner: {
					imgui_render::corner_box(GetWindowDrawList(), temp_box.min, temp_box.max, temp_box.color);
					break;
				}
				}
			}
			
			//format strings first
			for (auto& i : temp_box.text_storage.strings) {
				if (i.second.is_auto_color) {
					if (i.second.text == "%health") {
						float g = 255 * (player_health / 100.f);
						i.second.color = { 255 - g, g, 0 };
					}
				}
				
				i.second.text = formatPreviewText(i.second.text);
			}

			//constants
			temp_box.text_storage.last_positions.fill({ -1, -1 });

			BeginGroup();
			//render strings
			{
				for (auto& i : temp_box.text_storage.strings) {
					auto position = esp::c_esp_box::calc_text_position(temp_box, i.second, temp_box.text_storage.last_positions);
					const auto font_size = (i.second.size == -1 || i.second.size == 0) ? 16.f : i.second.size;
					auto text_size = render_system::fonts::nunito_font[2]->CalcTextSizeA(font_size, FLT_MAX, 0, i.second.text.c_str());
					
					renderEspText(i.second, render_system::fonts::nunito_font[2], font_size, temp_box.get_screen_position(position));

					auto pos = position;
					if (i.second.flags & directx_render::e_font_flags::font_centered_x)
						pos.x -= text_size.x / 2.f;
					if (i.second.flags & directx_render::e_font_flags::font_centered_y)
						pos.y -= text_size.y / 2.f;

					auto& element = g_style_editor.temp_box.text_storage.strings[i.first];
					
					SetCursorScreenPos( temp_box.get_screen_position(pos) );
					InvisibleButton(getButtonIdByEspID(i.first).c_str(), text_size);
					CustomiseEspTextDropSource(i.first);
					if (BeginPopupContextItem()) {
						auto id = getButtonIdByEspID(i.first);
						WittchenCheckbox(("Try associate color##" + id).c_str(),
						         &element.is_auto_color);
						ColorsEdit4(("Text color##" + id).c_str(), element.color,
						            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar |
						            ImGuiColorEditFlags_AlphaPreview);

						SliderFloat(("Text size##" + id).c_str(), &element.size, 0.f, render_system::fonts::nunito_font[2]->FontSize, "%.0f", 1.f);
						
						EndPopup();
					}
				}
			}
			EndGroup();
			
			End();
		}
		
	} EndGroup();
}
