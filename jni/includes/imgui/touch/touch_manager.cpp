#include <cstdio>
#include <cstring>
#include <thread>
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>     
#include <sys/ioctl.h>  
#include <cmath>     
#include <vector>     
#include "../imgui/imgui.h"
#include "touch_manager.h"

#define TOUCH_MODE 2

namespace touch {
    struct input {
        int fd;
        bool isTouchDevice;
        bool isKeyboardDevice;
        input_absinfo absX{}, absY{};
        float absXMultiplier{0}, absYMultiplier{0};
#if TOUCH_MODE == 2
        bool trackingIDPresent{false};
#endif
        explicit input(int _fd, int32_t _screen_w, int32_t _screen_h) : fd(_fd), isKeyboardDevice(false) {
            isTouchDevice = ioctl(_fd, EVIOCGABS(ABS_MT_POSITION_X), &absX) == 0 && ioctl(_fd, EVIOCGABS(ABS_MT_POSITION_Y), &absY) == 0 && absX.maximum > 0 && absY.maximum > 0;
            if (!isTouchDevice) {
                
                unsigned long evbit = 0;
                if (ioctl(_fd, EVIOCGBIT(0, sizeof(evbit)), &evbit) >= 0) {
                    isKeyboardDevice = (evbit & (1 << EV_KEY)) != 0;
                }
                return;
            }
#if TOUCH_MODE == 2
            input_absinfo absTrackingID{};
            trackingIDPresent = ioctl(_fd, EVIOCGABS(ABS_MT_TRACKING_ID), &absTrackingID) == 0 && absTrackingID.minimum <= 0 && absTrackingID.maximum > 0;
#endif
            calculateMultipliers(_screen_w, _screen_h);
        }
        void calculateMultipliers(int32_t _screen_w, int32_t _screen_h) {
            float max = fmax((float)absX.maximum, (float)absY.maximum);
            float min = fmin((float)absX.maximum, (float)absY.maximum);
            absXMultiplier = (float)_screen_w / (_screen_w > _screen_h ? max : min);
            absYMultiplier = (float)_screen_h / (_screen_w > _screen_h ? min : max);
        }
    };
    uint8_t orientation;
    ImGuiIO *imgui_io;
    std::vector<input> inputs;
    
    
    ImGuiKey LinuxKeyToImGuiKey(int keycode) {
        switch (keycode) {
            case KEY_INSERT: return ImGuiKey_Insert;
            case 82: return ImGuiKey_Insert;  
            case KEY_DELETE: return ImGuiKey_Delete;
            case KEY_HOME: return ImGuiKey_Home;
            case KEY_END: return ImGuiKey_End;
            case KEY_PAGEUP: return ImGuiKey_PageUp;
            case KEY_PAGEDOWN: return ImGuiKey_PageDown;
            case KEY_LEFT: return ImGuiKey_LeftArrow;
            case KEY_RIGHT: return ImGuiKey_RightArrow;
            case KEY_UP: return ImGuiKey_UpArrow;
            case KEY_DOWN: return ImGuiKey_DownArrow;
            case KEY_ESC: return ImGuiKey_Escape;
            case KEY_ENTER: return ImGuiKey_Enter;
            case KEY_TAB: return ImGuiKey_Tab;
            case KEY_BACKSPACE: return ImGuiKey_Backspace;
            case KEY_SPACE: return ImGuiKey_Space;
            
            case KEY_A: return ImGuiKey_A;
            case KEY_B: return ImGuiKey_B;
            case KEY_C: return ImGuiKey_C;
            case KEY_D: return ImGuiKey_D;
            case KEY_E: return ImGuiKey_E;
            case KEY_F: return ImGuiKey_F;
            case KEY_G: return ImGuiKey_G;
            case KEY_H: return ImGuiKey_H;
            case KEY_I: return ImGuiKey_I;
            case KEY_J: return ImGuiKey_J;
            case KEY_K: return ImGuiKey_K;
            case KEY_L: return ImGuiKey_L;
            case KEY_M: return ImGuiKey_M;
            case KEY_N: return ImGuiKey_N;
            case KEY_O: return ImGuiKey_O;
            case KEY_P: return ImGuiKey_P;
            case KEY_Q: return ImGuiKey_Q;
            case KEY_R: return ImGuiKey_R;
            case KEY_S: return ImGuiKey_S;
            case KEY_T: return ImGuiKey_T;
            case KEY_U: return ImGuiKey_U;
            case KEY_V: return ImGuiKey_V;
            case KEY_W: return ImGuiKey_W;
            case KEY_X: return ImGuiKey_X;
            case KEY_Y: return ImGuiKey_Y;
            case KEY_Z: return ImGuiKey_Z;
            
            case KEY_0: return ImGuiKey_0;
            case KEY_1: return ImGuiKey_1;
            case KEY_2: return ImGuiKey_2;
            case KEY_3: return ImGuiKey_3;
            case KEY_4: return ImGuiKey_4;
            case KEY_5: return ImGuiKey_5;
            case KEY_6: return ImGuiKey_6;
            case KEY_7: return ImGuiKey_7;
            case KEY_8: return ImGuiKey_8;
            case KEY_9: return ImGuiKey_9;
            
            case KEY_MINUS: return ImGuiKey_Minus;
            case KEY_EQUAL: return ImGuiKey_Equal;
            case KEY_LEFTBRACE: return ImGuiKey_LeftBracket;
            case KEY_RIGHTBRACE: return ImGuiKey_RightBracket;
            case KEY_SEMICOLON: return ImGuiKey_Semicolon;
            case KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
            case KEY_GRAVE: return ImGuiKey_GraveAccent;
            case KEY_BACKSLASH: return ImGuiKey_Backslash;
            case KEY_COMMA: return ImGuiKey_Comma;
            case KEY_DOT: return ImGuiKey_Period;
            case KEY_SLASH: return ImGuiKey_Slash;
            default: return ImGuiKey_None;
        }
    }
    
    void keyboard_thread(input _input) {
        sleep(1);
        if (!imgui_io) imgui_io = &ImGui::GetIO();
        input_event events[64]{};
        while (_input.fd > 0 && imgui_io && imgui_io->BackendRendererUserData) {
            long event_readed_count = (long)(read(_input.fd, events, sizeof(events)) / sizeof(input_event));
            if (event_readed_count >= 64) continue;
            for (long j = 0; j < event_readed_count; j++) {
                auto &event = events[j];
                if (event.type == EV_KEY) {
                    ImGuiKey key = LinuxKeyToImGuiKey(event.code);
                    if (key != ImGuiKey_None) {
                        imgui_io->AddKeyEvent(key, event.value != 0);
                    }
                }
            }
        }
    }
    
    void input_thread(input _input) {
        sleep(1);
        if (!imgui_io) imgui_io = &ImGui::GetIO();
        input_event events[64]{};
        while (_input.fd > 0 && imgui_io && imgui_io->BackendRendererUserData) {
            long event_readed_count = (long)(read(_input.fd, events, sizeof(events)) / sizeof(input_event));
            if (event_readed_count >= 64) continue;
            static bool isDown = false;
            static float x = 0.0f, y = 0.0f;
            for (long j = 0; j < event_readed_count; j++) {
                auto &event = events[j];
                if (event.type == EV_ABS) {
#if TOUCH_MODE == 2
                    if (_input.trackingIDPresent && event.code == ABS_MT_TRACKING_ID) {
                        isDown = event.value != -1;
                        continue;
                    }
#elif TOUCH_MODE == 0
                    if (event.code == ABS_MT_TRACKING_ID) {
                        isDown = event.value != -1;
                        continue;
                    }
#endif
                    if (event.code == ABS_MT_POSITION_X) {
                        x = (float)event.value;
                        continue;
                    }
                    if (event.code == ABS_MT_POSITION_Y) {
                        y = (float)event.value;
                        continue;
                    }
                }
                if (event.code == SYN_REPORT) {
#if TOUCH_MODE == 2
                    imgui_io->MouseDown[0] = _input.trackingIDPresent ? isDown : event_readed_count > 2;
#elif TOUCH_MODE == 0
                    imgui_io->MouseDown[0] = isDown;
#else
                    imgui_io->MouseDown[0] = event_readed_count > 2;
#endif
                    if (imgui_io->MouseDown[0]) {
                        switch (orientation) {
                            case 1:
                                imgui_io->MousePos.x = y;
                                imgui_io->MousePos.y = _input.absX.maximum - x;
                                break;
                            case 2:
                                imgui_io->MousePos.x = _input.absX.maximum - x;
                                imgui_io->MousePos.y = _input.absY.maximum - y;
                                break;
                            case 3:
                                imgui_io->MousePos.x = _input.absY.maximum - y;
                                imgui_io->MousePos.y = x;
                                break;
                            default:
                                imgui_io->MousePos.x = x;
                                imgui_io->MousePos.y = y;
                                break;
                        }
                        imgui_io->MousePos.x *= _input.absXMultiplier;
                        imgui_io->MousePos.y *= _input.absYMultiplier;
                    }
                }
            }
        }
    }
    bool init(int32_t _screen_w, int32_t _screen_h, uint8_t _orientation) {
        orientation = _orientation;
        bool result = false;
        const char *path = "/dev/input/";
        auto dir = opendir(path);
        dirent *ptr;
        while ((ptr = readdir(dir)) != nullptr) {
            if (strstr(ptr->d_name, "event")) {
                char event_path[128];
                sprintf(event_path, "%s%s", path, ptr->d_name);
                auto event_fd = open(event_path, O_RDWR);
                if (event_fd <= 0) continue;
                input _input(event_fd, _screen_w, _screen_h);
                if (_input.isTouchDevice) {
                    result = true;
                    inputs.emplace_back(_input);
                    std::thread(input_thread, _input).detach();
                } else if (_input.isKeyboardDevice) {
                    inputs.emplace_back(_input);
                    std::thread(keyboard_thread, _input).detach();
                }
            }
        }
        closedir(dir);
        return result;
    }
    void update(int32_t _screen_w, int32_t _screen_h, uint8_t _orientation) {
        orientation = _orientation;
        for (auto &_input : inputs)
            _input.calculateMultipliers(_screen_w, _screen_h);
    }
    void updateOrientation(uint8_t _orientation) {
        orientation = _orientation;
    }
    void shutdown() {
        for (auto &_input : inputs)
            if (_input.fd > 0)
                close(_input.fd);
        inputs.clear();
        imgui_io = nullptr;
    }
}