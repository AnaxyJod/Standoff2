
#include "draw.h"
#include <android/native_window_jni.h>
#include <android/log.h>
#include "../touch/touch_manager.h"
#include "../../fonts.h"
#include "../../sans_font.h"
#include "../../hitmarker_font.h"

namespace draw {

    static EGLDisplay display = EGL_NO_DISPLAY;
    static EGLConfig config;
    static EGLSurface surface = EGL_NO_SURFACE;
    static EGLContext context = EGL_NO_CONTEXT;
    static ANativeWindow *native_window = nullptr;
    static bool g_Initialized = false;
    static bool g_BackendAndroidReady = false;
    static bool g_BackendGlReady = false;
    static bool g_FrameReady = false;
    static DisplayInfo g_displayInfo = {};

    DisplayInfo getDisplayInfo() {
        return g_displayInfo;
    }

    bool initialize(bool log) {
        if (g_Initialized)
            return true;

        auto info = android::ANativeWindowCreator::GetDisplayInfo();
        if (info.width <= 0 || info.height <= 0) {
            return false;
        }
        g_displayInfo.orientation = info.orientation;
        g_displayInfo.width = info.width;
        g_displayInfo.height = info.height;

        native_window = android::ANativeWindowCreator::Create("ImGuiWnd", info.width, info.height, false, false);
        if (!native_window)
            return false;

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY)
            return false;
        if (!eglInitialize(display, 0, 0))
            return false;
        EGLint num_config;
        const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

        if (!eglChooseConfig(display, attribs, &config, 1, &num_config) || num_config <= 0)
            return false;
        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
            return false;
        ANativeWindow_setBuffersGeometry(native_window, 0, 0, format);
        const EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs);
        if (context == EGL_NO_CONTEXT)
            return false;
        surface = eglCreateWindowSurface(display, config, native_window, nullptr);
        if (surface == EGL_NO_SURFACE)
            return false;
        if (!eglMakeCurrent(display, surface, surface, context))
            return false;

        ImGui::CreateContext();
        if (!ImGui::GetCurrentContext())
            return false;
        ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    static ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    static ImFontConfig font_cfg2;
    font_cfg2.SizePixels = 22.0f;
    verdana = io.Fonts->AddFontFromMemoryTTF((void *)Verdana, sizeof Verdana, 15.0f, &font_cfg2, io.Fonts->GetGlyphRangesCyrillic());
    verdana_min = io.Fonts->AddFontFromMemoryTTF((void *)Verdana, sizeof Verdana, 13.0f, &font_cfg2, io.Fonts->GetGlyphRangesCyrillic());

    ImFontConfig cfg;

    font_pixel = io.Fonts->AddFontFromMemoryTTF((void *)SmallestPixel, sizeof SmallestPixel, 16.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    font_pixel_flags = io.Fonts->AddFontFromMemoryTTF((void *)SmallestPixel, sizeof SmallestPixel, 25.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    weapons = io.Fonts->AddFontFromMemoryTTF((void *)weaponfont, sizeof weaponfont, 15.f);
    arial_font = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ArialFont_compressed_data, Fonts::ArialFont_compressed_size, 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    icon_font = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::IconFont_compressed_data, Fonts::IconFont_compressed_size, 17.f, &cfg);
    NewFont = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont1_compressed_data, Fonts::NixFont1_compressed_size, 33.f, &cfg);
    settings = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont1_compressed_data, Fonts::NixFont1_compressed_size, 19.f, &cfg);
    VirgoFont = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::VirgoFont_compressed_data, Fonts::VirgoFont_compressed_size, 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    LatoFont = io.Fonts->AddFontFromMemoryTTF((void *)Fonts::LatoFont_compressed_data, sizeof Fonts::LatoFont_compressed_data, 28.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    Lapa = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont_compressed_data, Fonts::NixFont_compressed_size, 30.f, &cfg);
    rage = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont_compressed_data, Fonts::NixFont_compressed_size, 20.f, &cfg);
    player = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::IconFont_compressed_data, Fonts::IconFont_compressed_size, 25.f, &cfg);
    nixware = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont_compressed_data, Fonts::NixFont_compressed_size, 24.f, &cfg);
    ScinIcons = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::SkinsIcons_compressed_data, Fonts::SkinsIcons_compressed_size, 24.f, &cfg);
    IconsNew = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::IconNewFont_compressed_data, Fonts::IconNewFont_compressed_size, 20.f, &cfg);
    BolNix = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont_compressed_data, Fonts::NixFont_compressed_size, 24.f, &cfg);
    folder = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::ArialFont_compressed_data, Fonts::ArialFont_compressed_size, 16.f, &cfg);
    legit = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::NixFont1_compressed_data, Fonts::NixFont1_compressed_size, 15.f, &cfg);
    roboto_font = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::RobotoFont_compressed_data, Fonts::RobotoFont_compressed_size, 23.f, &cfg);
    Notif = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::RobotoFont_compressed_data, Fonts::RobotoFont_compressed_size, 14.f, &cfg);
    segoe_font = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::SegoeFont_compressed_data, Fonts::SegoeFont_compressed_size, 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    segoe_font1 = io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::SegoeFont_compressed_data, Fonts::SegoeFont_compressed_size, 25.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    sans = io.Fonts->AddFontFromMemoryTTF((void *)Sans, sizeof Sans, 50.0f, &font_cfg2, io.Fonts->GetGlyphRangesCyrillic());
    static const ImWchar hitmarker_ranges[] = { 0xE900, 0xE9FF, 0 };
	hitmarker = io.Fonts->AddFontFromMemoryTTF((void *)Hitmarker, sizeof Hitmarker, 50.0f, nullptr, hitmarker_ranges);

    io.Fonts->AddFontDefault(&font_cfg);

        ImGui::StyleColorsDark();

        g_BackendAndroidReady = ImGui_ImplAndroid_Init(native_window);
        g_BackendGlReady = ImGui_ImplOpenGL3_Init("#version 300 es");
        if (!g_BackendAndroidReady || !g_BackendGlReady) {
            return false;
        }

        // Disable raw input threads for stability on some Android 13 builds.
        // Menu can still render; this avoids crashes in detached /dev/input readers.
        // touch::init(info.width, info.height, info.orientation);
        g_Initialized = true;
        return true;
    }

    void processInput() {}

    void beginFrame() {
        g_FrameReady = false;
        if (!g_Initialized || !g_BackendAndroidReady || !g_BackendGlReady || !ImGui::GetCurrentContext() || display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
            return;
        ImGuiIO &io = ImGui::GetIO();
        // This Android backend doesn't always populate BackendPlatformUserData.
        if (!io.BackendRendererUserData)
            return;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame(g_displayInfo.width, g_displayInfo.height);
        ImGui::NewFrame();
        g_FrameReady = true;
    }

    bool isFrameReady() {
        return g_FrameReady;
    }

    void endFrame() {
        if (!g_FrameReady || !g_Initialized || !g_BackendAndroidReady || !g_BackendGlReady || !ImGui::GetCurrentContext() || display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
            return;
        ImGui::Render();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        eglSwapBuffers(display, surface);
        g_FrameReady = false;
    }

    void shutdown() {
        if (!g_Initialized)
            return;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplAndroid_Shutdown();
        g_BackendAndroidReady = false;
        g_BackendGlReady = false;
        ImGui::DestroyContext();
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT)
                eglDestroyContext(display, context);
            if (surface != EGL_NO_SURFACE)
                eglDestroySurface(display, surface);
            eglTerminate(display);
        }
        display = EGL_NO_DISPLAY;
        context = EGL_NO_CONTEXT;
        surface = EGL_NO_SURFACE;
        g_FrameReady = false;
        if (native_window) {
            android::ANativeWindowCreator::Destroy(native_window);
            native_window = nullptr;
        }
        // touch::shutdown();
        g_Initialized = false;
    }

}
