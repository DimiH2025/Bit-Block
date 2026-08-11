// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/threadpriority.h>

#include <logging.h>

#if defined(WIN32)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <sys/resource.h>
#include <sys/time.h>
#if defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#endif
#endif

namespace util {

void RaiseValidationThreadPriority()
{
#if defined(WIN32)
    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
        LogDebug(BCLog::VALIDATION, "Could not raise validation thread priority on Windows\n");
    }
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    // Lower the niceness value (more negative = higher priority). This is
    // best-effort: unprivileged processes are typically only permitted to
    // request a small increase in priority (e.g. down to nice -0 from the
    // default of 0, or further if the OS/ulimits allow it).
#if defined(__linux__)
    // On Linux (NPTL), threads are schedulable entities with their own id;
    // target this specific thread rather than the whole process.
    const pid_t target_id = static_cast<pid_t>(syscall(SYS_gettid));
#else
    // Other POSIX platforms generally only support process-wide niceness.
    const pid_t target_id = 0;
#endif
    errno = 0;
    const int current = getpriority(PRIO_PROCESS, target_id);
    if (current == -1 && errno != 0) {
        return; // Could not read current priority; leave it alone.
    }
    const int target = current - 2;
    if (setpriority(PRIO_PROCESS, target_id, target) != 0) {
        LogDebug(BCLog::VALIDATION, "Could not raise validation thread priority (insufficient permissions)\n");
    }
#else
    // Unsupported platform: no-op.
#endif
}

} // namespace util
