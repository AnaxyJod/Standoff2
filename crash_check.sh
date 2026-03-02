#!/usr/bin/env sh
set -eu

# Usage:
#   ./crash_check.sh [binary_path] [output_dir]
# Example:
#   ./crash_check.sh /data/local/tmp/q.sh

TARGET_BIN="${1:-/data/local/tmp/q.sh}"
TS="$(date +%Y%m%d_%H%M%S)"
OUTDIR="${2:-./crash_reports/${TS}}"

LOGCAT_FULL="${OUTDIR}/logcat_full.txt"
LOGCAT_FILTERED="${OUTDIR}/logcat_filtered.txt"
TOMB_FULL="${OUTDIR}/tombstone_full.txt"
TOMB_FILTERED="${OUTDIR}/tombstone_filtered.txt"
SUMMARY="${OUTDIR}/summary.txt"

mkdir -p "${OUTDIR}"

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "Missing required command: $1" >&2
    exit 1
  }
}

need_cmd su
need_cmd logcat
need_cmd grep

latest_tombstone() {
  su -c 'ls -1t /data/tombstones 2>/dev/null | head -n 1' || true
}

echo "[1/5] Preparing capture..."
BEFORE_TOMB="$(latest_tombstone)"

su -c 'logcat -c' || true

echo "[2/5] Running target: ${TARGET_BIN}"
set +e
su -c "${TARGET_BIN}"
RUN_RC=$?
set -e

echo "[3/5] Capturing logcat..."
su -c 'logcat -d' > "${LOGCAT_FULL}" 2>/dev/null || true

grep -E "SIGSEGV|backtrace|F DEBUG|F libc|F DEBUG   :|Fatal signal|q\.sh|${TARGET_BIN}" "${LOGCAT_FULL}" > "${LOGCAT_FILTERED}" || true

echo "[4/5] Capturing tombstone..."
AFTER_TOMB="$(latest_tombstone)"
SELECTED_TOMB=""

if [ -n "${AFTER_TOMB}" ]; then
  SELECTED_TOMB="${AFTER_TOMB}"
  su -c "cat /data/tombstones/${SELECTED_TOMB}" > "${TOMB_FULL}" 2>/dev/null || true
  grep -E "signal|Abort message|backtrace|#00|#01|#02|#03|#04|#05|q\.sh|${TARGET_BIN}" "${TOMB_FULL}" > "${TOMB_FILTERED}" || true
fi

echo "[5/5] Writing summary..."
{
  echo "target_bin=${TARGET_BIN}"
  echo "run_exit_code=${RUN_RC}"
  echo "tombstone_before=${BEFORE_TOMB}"
  echo "tombstone_after=${AFTER_TOMB}"
  echo "selected_tombstone=${SELECTED_TOMB}"
  echo "logcat_full=${LOGCAT_FULL}"
  echo "logcat_filtered=${LOGCAT_FILTERED}"
  echo "tombstone_full=${TOMB_FULL}"
  echo "tombstone_filtered=${TOMB_FILTERED}"
} > "${SUMMARY}"

echo ""
echo "Done. Files saved in: ${OUTDIR}"
echo "- ${SUMMARY}"
echo "- ${LOGCAT_FILTERED}"
if [ -n "${SELECTED_TOMB}" ]; then
  echo "- ${TOMB_FILTERED}"
else
  echo "- No tombstone found or readable"
fi

echo ""
echo "Share these files for analysis:"
echo "1) ${SUMMARY}"
echo "2) ${LOGCAT_FILTERED}"
echo "3) ${TOMB_FILTERED} (if present)"
