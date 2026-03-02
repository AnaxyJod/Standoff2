LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := q.sh

LOCAL_CFLAGS := -std=c++17 -fvisibility=hidden -DUSE_OPENGL
LOCAL_CPPFLAGS := -std=c++17 -fvisibility=hidden -DUSE_OPENGL -Wno-error=format-security -fexceptions

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/core \
    $(LOCAL_PATH)/includes \
    $(LOCAL_PATH)/includes/imgui/imgui \
    $(LOCAL_PATH)/includes/imgui/imgui/backends

LOCAL_SRC_FILES := main.cpp \
	menu/ui.cpp \
	Functions/weapon_mods.cpp \
	Functions/economy.cpp \
	Functions/esp.cpp \
	Functions/aim.cpp \
	Functions/rcs.cpp \
	Functions/silent.cpp \
	Functions/simpleskin.cpp \
	Functions/hostindicator.cpp \
	Functions/rainbow.cpp \
	Functions/hitlog.cpp \
	Functions/hitmarker.cpp \
	Functions/skinchanger.cpp \
	cfg_manager.cpp \
	includes/tracer.cpp \
	includes/memory/helper.cpp \
	includes/imgui/imgui/imgui.cpp \
    includes/imgui/draw/draw.cpp \
    includes/imgui/touch/touch_manager.cpp \
    includes/imgui/imgui/imgui_draw.cpp \
    includes/imgui/imgui/imgui_tables.cpp \
    includes/imgui/imgui/imgui_widgets.cpp \
    includes/imgui/imgui/backends/imgui_impl_android.cpp \
    includes/imgui/imgui/backends/imgui_impl_opengl3.cpp

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3

include $(BUILD_EXECUTABLE)
