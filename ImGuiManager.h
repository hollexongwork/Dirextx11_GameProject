#pragma once

#include "main.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <wrl.h>
using Microsoft::WRL::ComPtr;

struct DepthDrawData
{
    ImVec2 pos;
    ImVec2 drawSize;
};

class ImGuiManager
{
private:

public:
    static void Init(HWND hWnd);
    static void Uninit();
    static void NewFrame();
    static void Render();
    static void DrawDebugWindows();
};