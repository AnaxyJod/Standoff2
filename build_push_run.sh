#!/usr/bin/env sh
set -eu

# Usage:
#   ./build_push_run.sh
# Builds arm64-v8a binary only (no adb/push/run).
PROJECT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
BINARY_NAME="q.sh"

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "Missing required command: $1" >&2
    exit 1
  }
}

if [ -n "${NDK_BUILD:-}" ] && [ -x "${NDK_BUILD}" ]; then
  NDK_BUILD_CMD="${NDK_BUILD}"
elif command -v ndk-build >/dev/null 2>&1; then
  NDK_BUILD_CMD="$(command -v ndk-build)"
elif [ -n "${ANDROID_NDK_HOME:-}" ] && [ -x "${ANDROID_NDK_HOME}/ndk-build" ]; then
  NDK_BUILD_CMD="${ANDROID_NDK_HOME}/ndk-build"
else
  echo "Could not find ndk-build. Set NDK_BUILD or add ndk-build to PATH." >&2
  exit 1
fi

echo "[1/5] Building arm64-v8a binary..."
"${NDK_BUILD_CMD}" \
  NDK_PROJECT_PATH="${PROJECT_DIR}" \
  APP_BUILD_SCRIPT="${PROJECT_DIR}/jni/Android.mk" \
  NDK_APPLICATION_MK="${PROJECT_DIR}/jni/Application.mk"

LOCAL_BIN="${PROJECT_DIR}/libs/arm64-v8a/${BINARY_NAME}"
if [ ! -f "${LOCAL_BIN}" ]; then
  echo "Build finished but binary not found: ${LOCAL_BIN}" >&2
  exit 1
fi

echo "Build complete."
echo "Binary: ${LOCAL_BIN}"
