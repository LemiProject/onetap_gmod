#include "render_system.h"

#include <mutex>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>

#include "render_helpers.h"
#include "imgui/imgui_freetype.h"
#include "../features/menu/menu.h"
#include "../globals.h"

#include "../features/menu/themes.h"
#include "../features/menu/widgets/widgets.h"

#include "fonts/nunito_font.h"
#include "fonts/fontasw.h"

#include <d3d9helper.h>

#include "fontawesomium.h"

IDirect3DDevice9* game_device;
bool render_system_initialized = false;
std::mutex render_mutex;

namespace directx_render
{
    inline std::vector<std::shared_ptr<draw_types::draw_data_t>> draw_calls;
    inline std::vector<std::shared_ptr<draw_types::draw_data_t>> safe_draw_calls;

    void add_temp_to_draw_list(ImDrawList* target_list);
}

void render_system::init() {
    ImGui::CreateContext();

    auto* const game_hwnd = FindWindowW(0, L"Garry's Mod (x64)");
    if (game_hwnd)
    {
        ImGui_ImplWin32_Init(game_hwnd);
        ImGui_ImplDX9_Init(game_device);
    	
        ImGui::GetIO().IniFilename = nullptr;

        static ImFontConfig config;
        static ImFontConfig icon_cfg;
        icon_cfg.MergeMode = true;
        icon_cfg.OversampleH = icon_cfg.OversampleV = 1;
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    	
        fonts::menu_font[1] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(nunito_font_compressed_data, nunito_font_compressed_size, 18.5f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fontasw_compressed_data, fontasw_compressed_size, 16.5f, &icon_cfg, icon_ranges);
        fonts::menu_font[2] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(nunito_font_compressed_data, nunito_font_compressed_size, 34.5f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fontasw_compressed_data, fontasw_compressed_size, 32.5f, &icon_cfg, icon_ranges);
        fonts::menu_font[0] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(nunito_font_compressed_data, nunito_font_compressed_size, 16.0f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(fontasw_compressed_data, fontasw_compressed_size, 14.0f, &icon_cfg, icon_ranges);
    	
        fonts::nunito_font[1] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(nunito_font_compressed_data, nunito_font_compressed_size, 18.5f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        fonts::arial_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 14.f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        fonts::nunito_font[2] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(nunito_font_compressed_data, nunito_font_compressed_size, 34.5f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        fonts::nunito_font[0] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(nunito_font_compressed_data, nunito_font_compressed_size, 16.0f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
    	
        ImGuiFreeType::BuildFontAtlas(ImGui::GetIO().Fonts);
    	
        menu_themes::standard_theme();

        Wittchen::InitializeEspStyleEditor();
    }

    render_system_initialized = true;
}

IDirect3DDevice9* render_system::get_device() {
	return game_device;
}

void render_system::on_end_scene(LPDIRECT3DDEVICE9 device, uintptr_t return_address) {
    static uintptr_t game_overlay_return_address = 0;
	
	if (!game_device) game_device = device;
	if (!render_system_initialized) {
		init();
	}

    if (!game_overlay_return_address) {
        MEMORY_BASIC_INFORMATION mi; VirtualQuery((LPVOID)return_address, &mi, sizeof(MEMORY_BASIC_INFORMATION));
    	char mn[MAX_PATH]; GetModuleFileName((HMODULE)mi.AllocationBase, mn, MAX_PATH);
        if (std::string(mn).find("gameoverlay") != std::string::npos) game_overlay_return_address = return_address;
    }
   // if (game_overlay_return_address != (uintptr_t)return_address && settings::states["other::anti_obs"])
       // return;

    device->SetRenderTarget(0, 0);
   // interfaces::mat_render_context->set_render_target(0);
	
    IDirect3DStateBlock9* pixel_state = NULL;
    IDirect3DVertexDeclaration9* vertex_declaration;
    IDirect3DVertexShader9* vertex_shader;
    device->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
    device->GetVertexDeclaration(&vertex_declaration);
    device->GetVertexShader(&vertex_shader);
    device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);

    DWORD state1;
    DWORD state2;
    DWORD state3;
    device->GetRenderState(D3DRS_COLORWRITEENABLE, &state1);
    device->GetRenderState(D3DRS_COLORWRITEENABLE, &state2);
    device->GetRenderState(D3DRS_SRGBWRITEENABLE, &state3);

    device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
    device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
    device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

    device->SetPixelShader(NULL);
    device->SetVertexShader(NULL);

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    menu::draw_menu();
	
    directx_render::add_temp_to_draw_list(ImGui::GetBackgroundDrawList());
	
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    device->SetRenderState(D3DRS_COLORWRITEENABLE, state1);
    device->SetRenderState(D3DRS_COLORWRITEENABLE, state2);
    device->SetRenderState(D3DRS_SRGBWRITEENABLE, state3);

    pixel_state->Apply();
    pixel_state->Release();
    device->SetVertexDeclaration(vertex_declaration);
    device->SetVertexShader(vertex_shader);
}

void directx_render::init_directx_render()
{

}

void directx_render::render_surface(std::function<void()> draw_surface)
{
    draw_calls.clear();
    draw_surface();

    std::unique_lock<std::mutex> l(render_mutex);
    draw_calls.swap(safe_draw_calls);
}

void directx_render::add_temp_to_draw_list(ImDrawList* target_list)
{
    const std::unique_lock<std::mutex> l(render_mutex);

    for (auto i : safe_draw_calls)
        i->draw(target_list);

    safe_draw_calls.clear();
}


void directx_render::filled_rect(const ImVec2& min, const ImVec2& max, c_color color, float round)
{
    auto call = std::make_shared<draw_types::rect_draw_type_t>();
    call->color = color;
    call->filled = true;
    call->min = min;
    call->max = max;
    call->rounding = round;
    draw_calls.push_back(call);
}

void directx_render::bordered_rect(const ImVec2& min, const ImVec2& max, c_color color, float round)
{
    auto call = std::make_shared<draw_types::rect_draw_type_t>();
    call->color = color;
    call->filled = false;
    call->min = min;
    call->max = max;
    call->rounding = round;
    draw_calls.push_back(call);
}

void directx_render::line(const ImVec2& pos1, const ImVec2& pos2, c_color color, float t)
{
    auto call = std::make_shared<draw_types::line_draw_type_t>();
    call->pos1 = pos1;
    call->pos2 = pos2;
    call->color = color;
    call->thickness = t;

    draw_calls.push_back(call);
}

void directx_render::text(ImFont* font, const std::string& text, const ImVec2& pos, float size, c_color color,
    int flags)
{
    auto call = std::make_shared<draw_types::text_draw_type_t>();
    call->flags = flags;
    call->text = text;
    call->pos = pos;
    call->font = font;
    call->size = size;
    call->color = color;
    draw_calls.push_back(call);
}

void directx_render::outlined_circle(const ImVec2& pos, int radius, c_color color, int segments, int t)
{
    auto call = std::make_shared<draw_types::circle_draw_type_t>();
    call->pos = pos;
    call->radius = radius;
    call->color = color;
    call->t = t;
    call->segment_num = segments;
    call->filled = false;

    draw_calls.push_back(call);
}

void directx_render::filled_circle(const ImVec2& pos, int radius, c_color color, int segments, int t)
{
    auto call = std::make_shared<draw_types::circle_draw_type_t>();
    call->pos = pos;
    call->radius = radius;
    call->color = color;
    call->t = t;
    call->segment_num = segments;
    call->filled = true;

    draw_calls.push_back(call);
}

void directx_render::corner_box(const ImVec2& min, const ImVec2& max, c_color color)
{
    std::array<ImVec2, 4> points = {
    min,  ImVec2{min.x, max.y}, max,  ImVec2{max.x, min.y}
    };

    auto draw_corner = [](const ImVec2& center, const ImVec2& add, c_color color)
    {
        ImVec2 inline_padding = { add.x < 0.f ? -1.f : 1.f, add.y < 0.f ? -1.f : 1.f };

        line({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + add.x/* + inline_padding.x*/, center.y + inline_padding.y }, colors::black_color);
        line({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + inline_padding.x, center.y + add.y/* + inline_padding.y */ }, colors::black_color);

        inline_padding = { -inline_padding.x, -inline_padding.y };

        line({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + add.x/* + inline_padding.x*/, center.y + inline_padding.y }, colors::black_color);
        line({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + inline_padding.x, center.y + add.y /*+ inline_padding.y */ }, colors::black_color);

        line(center, { center.x + add.x, center.y }, color);
        line(center, { center.x, center.y + add.y }, color);
    };

    auto w = max.x - min.x;
    auto h = max.y - min.y;
    const ImVec2 side_sizes = { w / 3.f, h / 3.f };

    for (auto i = 0; i < points.size(); ++i)
    {
        auto point = points[i];
        switch (i)
        {
        case 0:
            draw_corner(point, side_sizes, color);
            break;
        case 1:
            draw_corner(point, { side_sizes.x, -side_sizes.y }, color);
            break;
        case 2:
            draw_corner(point, { -side_sizes.x, -side_sizes.y }, color);
            break;
        case 3:
            draw_corner(point, { -side_sizes.x, side_sizes.y }, color);
            break;
        }
    }
}

void imgui_render::corner_box(ImDrawList* list, const ImVec2& min, const ImVec2& max, c_color color) {
    std::array<ImVec2, 4> points = {
min,  ImVec2{min.x, max.y}, max,  ImVec2{max.x, min.y}
    };

    auto draw_corner = [&](const ImVec2& center, const ImVec2& add, c_color color)
    {
        ImVec2 inline_padding = { add.x < 0.f ? -1.f : 1.f, add.y < 0.f ? -1.f : 1.f };

        list->AddLine({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + add.x/* + inline_padding.x*/, center.y + inline_padding.y }, colors::black_color.get_u32());
        list->AddLine({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + inline_padding.x, center.y + add.y/* + inline_padding.y */ }, colors::black_color.get_u32());

        inline_padding = { -inline_padding.x, -inline_padding.y };

        list->AddLine({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + add.x/* + inline_padding.x*/, center.y + inline_padding.y }, colors::black_color.get_u32());
        list->AddLine({ center.x + inline_padding.x, center.y + inline_padding.y }, { center.x + inline_padding.x, center.y + add.y /*+ inline_padding.y */ }, colors::black_color.get_u32());
        
        list->AddLine(center, { center.x + add.x, center.y }, color);
        list->AddLine(center, { center.x, center.y + add.y }, color);
    };

    auto w = max.x - min.x;
    auto h = max.y - min.y;
    const ImVec2 side_sizes = { w / 3.f, h / 3.f };

    for (auto i = 0; i < points.size(); ++i)
    {
        auto point = points[i];
        switch (i)
        {
        case 0:
            draw_corner(point, side_sizes, color);
            break;
        case 1:
            draw_corner(point, { side_sizes.x, -side_sizes.y }, color);
            break;
        case 2:
            draw_corner(point, { -side_sizes.x, -side_sizes.y }, color);
            break;
        case 3:
            draw_corner(point, { -side_sizes.x, side_sizes.y }, color);
            break;
        }
    }
}
