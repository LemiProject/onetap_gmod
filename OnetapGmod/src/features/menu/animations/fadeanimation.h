#pragma once

#include <imgui/im_tools.h>

class c_fade_animation {
	float alpha = 1.f;
	float smooth;
	ImGuiCol color_idx;
public:
	c_fade_animation() = default;
	c_fade_animation(ImGuiCol color_idx, float smooth) : color_idx(color_idx), smooth(smooth) {}
	
	void push() const;
	static void pop();

	void handle_add();
	void handle_sub();
};

inline void c_fade_animation::push() const {
	auto color = ImGui::GetStyleColorVec4(color_idx);
	color.w = alpha;

	ImGui::PushStyleColor(color_idx, color);
}

inline void c_fade_animation::pop() {
	ImGui::PopStyleColor();
}

inline void c_fade_animation::handle_add() {
	alpha = ImMin(alpha + ImGui::GetIO().DeltaTime * smooth, 1.f);
}

inline void c_fade_animation::handle_sub() {
	alpha = ImMax(alpha - ImGui::GetIO().DeltaTime * smooth, 0.f);
}
