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
#include "../settings/settings.h"

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
        fonts::arial_font[2] = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 16.f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
        fonts::tahoma_font[2] = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Tahoma.ttf", 16.f, &config, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
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
#define GetOriginValue(name) \
DWORD o_##name;\
game_device->GetRenderState(name, &o_##name);
#define SetOriginValue(name) game_device->SetRenderState(name, o_##name);
IDirect3DSurface9* gamesurface = NULL;
IDirect3DTexture9* gametexture = NULL;
IDirect3DVertexBuffer9* v_buffer = NULL;
struct CUSTOMVERTEX {
    FLOAT x, y, z, rhw, u, v;
};

void spec_list()
{
    if (!settings::get_bool("visual_spec") || !get_local_player() || !get_local_player()->is_alive())
        return;
    ImGui::SetNextWindowSize(ImVec2(200.f, 200.f));
    ImGui::Begin("spec_list", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
    {
        std::string names = "";
        static int number_spec;
        for (int i = 0; i < interfaces::entity_list->get_highest_entity_index(); i++)
        {
            auto ent = (c_base_player*)interfaces::entity_list->get_client_entity(i);
            if (ent == nullptr || !ent->is_player() || ent == get_local_player())
                continue;
            if (ent->get_observer_target() != get_local_player())
                continue;
            number_spec = interfaces::entity_list->get_highest_entity_index();

            names += ent->get_name() + "\n";
        }

        auto menu_size = ImGui::GetItemRectSize();
        auto text = std::string(
            "Spectators:\n" + names);
        auto text_size = ImGui::CalcTextSize(text.c_str());
        auto pos = ImVec2{
            ImGui::GetIO().DisplaySize.x - text_size.x - ImGui::GetStyle().WindowPadding.x,
            menu_size.y };
        ImGui::Text(text.c_str());
    }
    ImGui::End();
 
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
    IDirect3DSurface9* backbuffer = 0;
    device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
    D3DSURFACE_DESC desc;
    backbuffer->GetDesc(&desc);
    if (!gamesurface)
    {
        device->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, desc.Format, D3DPOOL_DEFAULT, &gametexture, 0);
        gametexture->GetSurfaceLevel(0, &gamesurface);

        device->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX),
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            (D3DFVF_XYZRHW | D3DFVF_TEX1),
            D3DPOOL_DEFAULT,
            &v_buffer,
            NULL);
    }


    IDirect3DVertexDeclaration9* vertDec = NULL; IDirect3DVertexShader9* vertShader = NULL;
    device->GetVertexDeclaration(&vertDec);
    device->GetVertexShader(&vertShader);
    GetOriginValue(D3DRS_COLORWRITEENABLE);
    GetOriginValue(D3DRS_SRGBWRITEENABLE);
    GetOriginValue(D3DRS_MULTISAMPLEANTIALIAS);
    GetOriginValue(D3DRS_CULLMODE);
    GetOriginValue(D3DRS_LIGHTING);
    GetOriginValue(D3DRS_ZENABLE);
    GetOriginValue(D3DRS_ALPHABLENDENABLE);
    GetOriginValue(D3DRS_ALPHATESTENABLE);
    GetOriginValue(D3DRS_BLENDOP);
    GetOriginValue(D3DRS_SRCBLEND);
    GetOriginValue(D3DRS_DESTBLEND);
    GetOriginValue(D3DRS_SCISSORTESTENABLE);


    DWORD D3DSAMP_ADDRESSU_o;
    device->GetSamplerState(NULL, D3DSAMP_ADDRESSU, &D3DSAMP_ADDRESSU_o);
    DWORD D3DSAMP_ADDRESSV_o;
    device->GetSamplerState(NULL, D3DSAMP_ADDRESSV, &D3DSAMP_ADDRESSV_o);
    DWORD D3DSAMP_ADDRESSW_o;
    device->GetSamplerState(NULL, D3DSAMP_ADDRESSW, &D3DSAMP_ADDRESSW_o);
    DWORD D3DSAMP_SRGBTEXTURE_o;
    device->GetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, &D3DSAMP_SRGBTEXTURE_o);
    DWORD D3DTSS_COLOROP_o;
    device->GetTextureStageState(0, D3DTSS_COLOROP, &D3DTSS_COLOROP_o);
    DWORD D3DTSS_COLORARG1_o;
    device->GetTextureStageState(0, D3DTSS_COLORARG1, &D3DTSS_COLORARG1_o);
    DWORD D3DTSS_COLORARG2_o;
    device->GetTextureStageState(0, D3DTSS_COLORARG2, &D3DTSS_COLORARG2_o);
    DWORD D3DTSS_ALPHAOP_o;
    device->GetTextureStageState(0, D3DTSS_ALPHAOP, &D3DTSS_ALPHAOP_o);
    DWORD D3DTSS_ALPHAARG1_o;
    device->GetTextureStageState(0, D3DTSS_ALPHAARG1, &D3DTSS_ALPHAARG1_o);
    DWORD D3DTSS_ALPHAARG2_o;
    device->GetTextureStageState(0, D3DTSS_ALPHAARG2, &D3DTSS_ALPHAARG2_o);
    DWORD D3DSAMP_MINFILTER_o;
    device->GetSamplerState(NULL, D3DSAMP_MINFILTER, &D3DSAMP_MINFILTER_o);
    DWORD D3DSAMP_MAGFILTER_o;
    device->GetSamplerState(NULL, D3DSAMP_MAGFILTER, &D3DSAMP_MAGFILTER_o);
    DWORD o_FVF;
    device->GetFVF(&o_FVF);

    if (gamesurface && interfaces::engine->is_in_game() && !interfaces::engine->is_drawing_loading_image())
    {
        device->SetRenderTarget(NULL, gamesurface);
        device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

        CUSTOMVERTEX Va[6];
        int b = 0;
        Va[b] = { 0.f, 0.f, 0.0f, 1.0f, 0.f,0.f }; b++;
        Va[b] = { (float)desc.Width, 0.f , 0.0f, 1.0f,1.f,0.f }; b++;
        Va[b] = { 0.f,(float)desc.Height, 0.0f, 1.0f,0.f,1.f }; b++;
        Va[b] = { (float)desc.Width, 0.f, 0.0f, 1.0f,1.f,0.f }; b++;
        Va[b] = { (float)desc.Width,(float)desc.Height , 0.0f, 1.0f,1.f,1.f }; b++;
        Va[b] = { 0.f,(float)desc.Height, 0.0f, 1.0f,0.f,1.f };

        for (auto it = 0; it < 6; it++) {
            Va[it].x -= 0.5f;
            Va[it].y -= 0.5f;
        }
        VOID* pVoid;
        v_buffer->Lock(0, 0, (void**)&pVoid, D3DLOCK_DISCARD);
        memcpy(pVoid, Va, sizeof(Va));
        v_buffer->Unlock();
        device->SetPixelShader(NULL);
        device->SetVertexShader(NULL);
        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, FALSE);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);


        //if (GetAsyncKeyState('V')) {
        //	D3DXSaveTextureToFile("_clean_rt.bmp", D3DXIFF_BMP, game_clean_rt->extract_texture(game_clean_rt), NULL);
        //}
        device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        device->SetFVF((D3DFVF_XYZRHW | D3DFVF_TEX1));
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);


        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);

        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

        //if (GetAsyncKeyState('V')) {
        //	D3DXSaveTextureToFile("_dirty_rt.bmp", D3DXIFF_BMP, game_dirty_rt->extract_texture(game_dirty_rt), NULL);
        //}
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
        /////////
        device->SetRenderTarget(NULL, backbuffer);

        device->SetPixelShader(NULL);
        device->SetVertexShader(NULL);
        device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, FALSE);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);//D3DBLEND_ONE D3DBLEND_ONE = fullbright but flicker's else

        //device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        //device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

        //real copy
        //device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
        //device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
        //device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        //	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        //	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

        device->SetRenderState(D3DRS_ZENABLE, false);
        device->SetRenderState(D3DRS_ZWRITEENABLE, false);
        device->SetRenderState(D3DRS_LIGHTING, false);
        //device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

        device->SetTexture(0, gametexture);
        //if (GetAsyncKeyState('V')) {
        //	D3DXSaveTextureToFile("lol.bmp", D3DXIFF_BMP, gametexture, NULL);
        //}

        device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
        device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        device->SetFVF((D3DFVF_XYZRHW | D3DFVF_TEX1));
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

    }
    device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
    device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
    device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    device->SetRenderState(D3DRS_LIGHTING, false);
    device->SetRenderState(D3DRS_ZENABLE, false);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    menu::draw_menu();
    spec_list();
    directx_render::add_temp_to_draw_list(ImGui::GetBackgroundDrawList());
	
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
    device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

    device->SetPixelShader(NULL);
    device->SetVertexShader(NULL);
    device->SetTexture(NULL, NULL);



    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);


    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    device->SetFVF(o_FVF);
    SetOriginValue(D3DRS_COLORWRITEENABLE);
    SetOriginValue(D3DRS_SRGBWRITEENABLE);
    SetOriginValue(D3DRS_MULTISAMPLEANTIALIAS);
    SetOriginValue(D3DRS_CULLMODE);
    SetOriginValue(D3DRS_LIGHTING);
    SetOriginValue(D3DRS_ZENABLE);
    SetOriginValue(D3DRS_ALPHABLENDENABLE);
    SetOriginValue(D3DRS_ALPHATESTENABLE);
    SetOriginValue(D3DRS_BLENDOP);
    SetOriginValue(D3DRS_SRCBLEND);
    SetOriginValue(D3DRS_DESTBLEND);
    SetOriginValue(D3DRS_SCISSORTESTENABLE);
    device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSU_o);
    device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DSAMP_ADDRESSV_o);
    device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DSAMP_ADDRESSW_o);
    device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, D3DSAMP_SRGBTEXTURE_o);

    //device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTSS_COLOROP_o);
    //device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTSS_COLORARG1_o);
    //device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTSS_COLORARG2_o);
    //device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTSS_ALPHAOP_o);
    //device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG1_o);
    //device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTSS_ALPHAARG2_o);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DSAMP_MINFILTER_o);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DSAMP_MAGFILTER_o);


    device->SetVertexDeclaration(vertDec);
    device->SetVertexShader(vertShader);
    device->SetRenderTarget(NULL, gamesurface);
    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
    device->SetRenderTarget(NULL, backbuffer);
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
