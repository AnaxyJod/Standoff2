@echo off

set NDK=C:\ndk\ndk-build.cmd
set PROJECT=C:\Users\kai\Desktop\newxyeta

call %NDK% NDK_PROJECT_PATH=%PROJECT% APP_BUILD_SCRIPT=%PROJECT%\jni\Android.mk NDK_APPLICATION_MK=%PROJECT%\jni\Application.mk
pause
