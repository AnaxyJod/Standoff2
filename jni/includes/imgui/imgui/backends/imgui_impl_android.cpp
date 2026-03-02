





















#include "../imgui.h"
#include "imgui_impl_android.h"
#include <ctime>
#include <map>
#include <queue>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>

#define LOG_TAG "Chitoge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


static double g_Time = 0.0;
static ANativeWindow *g_Window;
static char g_LogTag[] = "INTERNAL";
static std::map<int32_t, std::queue<int32_t>> g_KeyEventQueues; 

int32_t ImGui_ImplAndroid_HandleInputEvent(AInputEvent *input_event, ImVec2 screen_scale) {
    ImGuiIO &io = ImGui::GetIO();
    int32_t event_type = AInputEvent_getType(input_event);
    switch (event_type) {
        case AINPUT_EVENT_TYPE_KEY: {
            int32_t event_key_code = AKeyEvent_getKeyCode(input_event);
            int32_t event_action = AKeyEvent_getAction(input_event);
            int32_t event_meta_state = AKeyEvent_getMetaState(input_event);

            io.KeyCtrl = ((event_meta_state & AMETA_CTRL_ON) != 0);
            io.KeyShift = ((event_meta_state & AMETA_SHIFT_ON) != 0);
            io.KeyAlt = ((event_meta_state & AMETA_ALT_ON) != 0);

            switch (event_action) {
                
                
                
                case AKEY_EVENT_ACTION_DOWN:
                case AKEY_EVENT_ACTION_UP:
                    g_KeyEventQueues[event_key_code].push(event_action);
                    break;
                default:
                    break;
            }
            break;
        }
        case AINPUT_EVENT_TYPE_MOTION: {
            int32_t event_action = AMotionEvent_getAction(input_event);
            int32_t event_pointer_index = (event_action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            event_action &= AMOTION_EVENT_ACTION_MASK;
            switch (event_action) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_UP:
                    
                    
                    
                    if ((AMotionEvent_getToolType(input_event, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_FINGER) || (AMotionEvent_getToolType(input_event, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)) {
                        io.MouseDown[0] = (event_action == AMOTION_EVENT_ACTION_DOWN);
                        ImVec2 pos(AMotionEvent_getRawX(input_event, event_pointer_index), AMotionEvent_getRawY(input_event, event_pointer_index));
                        io.MousePos = ImVec2(screen_scale.x > 0 ? pos.x / screen_scale.x : pos.x, screen_scale.y > 0 ? pos.y / screen_scale.y : pos.y);
                    }
                    break;
                case AMOTION_EVENT_ACTION_BUTTON_PRESS:
                case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
                    int32_t button_state = AMotionEvent_getButtonState(input_event);
                    io.MouseDown[0] = ((button_state & AMOTION_EVENT_BUTTON_PRIMARY) != 0);
                    io.MouseDown[1] = ((button_state & AMOTION_EVENT_BUTTON_SECONDARY) != 0);
                    io.MouseDown[2] = ((button_state & AMOTION_EVENT_BUTTON_TERTIARY) != 0);
                }
                    break;
                case AMOTION_EVENT_ACTION_HOVER_MOVE: 
                case AMOTION_EVENT_ACTION_MOVE: {       
                    ImVec2 pos(AMotionEvent_getRawX(input_event, event_pointer_index), AMotionEvent_getRawY(input_event, event_pointer_index));
                    io.MousePos = ImVec2(screen_scale.x > 0 ? pos.x / screen_scale.x : pos.x, screen_scale.y > 0 ? pos.y / screen_scale.y : pos.y);
                    break;
                }
                case AMOTION_EVENT_ACTION_SCROLL:
                    io.MouseWheel = AMotionEvent_getAxisValue(input_event,AMOTION_EVENT_AXIS_VSCROLL,event_pointer_index);
                    io.MouseWheelH = AMotionEvent_getAxisValue(input_event,AMOTION_EVENT_AXIS_HSCROLL,event_pointer_index);
                    break;
                default:
                    break;
            }
        }
            return 1;
        default:
            break;
    }

    return 0;
}

bool ImGui_ImplAndroid_Init(ANativeWindow *window) {
    g_Window = window;
    g_Time = 0.0;

    
    ImGuiIO &io = ImGui::GetIO();
    io.BackendPlatformName = "ofxImGui";

    
    io.KeyMap[ImGuiKey_Tab] = AKEYCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = AKEYCODE_DPAD_LEFT;   
    io.KeyMap[ImGuiKey_RightArrow] = AKEYCODE_DPAD_RIGHT; 
    io.KeyMap[ImGuiKey_UpArrow] = AKEYCODE_DPAD_UP;       
    io.KeyMap[ImGuiKey_DownArrow] = AKEYCODE_DPAD_DOWN;   
    io.KeyMap[ImGuiKey_PageUp] = AKEYCODE_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = AKEYCODE_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = AKEYCODE_MOVE_HOME;
    io.KeyMap[ImGuiKey_End] = AKEYCODE_MOVE_END;
    io.KeyMap[ImGuiKey_Insert] = AKEYCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = AKEYCODE_FORWARD_DEL;
    io.KeyMap[ImGuiKey_Backspace] = AKEYCODE_DEL;
    io.KeyMap[ImGuiKey_Space] = AKEYCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = AKEYCODE_ENTER;
    io.KeyMap[ImGuiKey_Escape] = AKEYCODE_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = AKEYCODE_NUMPAD_ENTER;
    io.KeyMap[ImGuiKey_A] = AKEYCODE_A;
    io.KeyMap[ImGuiKey_C] = AKEYCODE_C;
    io.KeyMap[ImGuiKey_V] = AKEYCODE_V;
    io.KeyMap[ImGuiKey_X] = AKEYCODE_X;
    io.KeyMap[ImGuiKey_Y] = AKEYCODE_Y;
    io.KeyMap[ImGuiKey_Z] = AKEYCODE_Z;

    return true;
}

void ImGui_ImplAndroid_Shutdown() {
}

void ImGui_ImplAndroid_NewFrame(int screen_width, int screen_height)
{
    ImGuiIO& io = ImGui::GetIO();

    
    
    for (auto& key_queue : g_KeyEventQueues)
    {
        if (key_queue.second.empty())
            continue;
        io.KeysDown[key_queue.first] = (key_queue.second.front() == AKEY_EVENT_ACTION_DOWN);
        key_queue.second.pop();
    }

    
    int32_t window_width = g_Window ? ANativeWindow_getWidth(g_Window) : screen_width;
    int32_t window_height = g_Window ? ANativeWindow_getHeight(g_Window) : screen_height;
    int display_width = window_width;
    int display_height = window_height;

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);
    if (window_width > 0 && window_height > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_width / window_width, (float)display_height / window_height);

    
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;
}
