// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_THREADPRIORITY_H
#define BITCOIN_UTIL_THREADPRIORITY_H

namespace util {

/**
 * Raise the scheduling priority of the calling thread so it is favored by
 * the OS scheduler, similar in spirit to libbitcoin-blockchain's
 * `blockchain.priority` setting for its validation thread pool.
 *
 * This is best-effort: platforms or permission models that don't allow a
 * process to raise its own thread priority are simply left at the default
 * priority. Callers should not depend on this succeeding.
 *
 * Governed by the `-parpriority` startup option (default: true), wired in
 * via ChainstateManager::Options::raise_validation_thread_priority.
 */
void RaiseValidationThreadPriority();

} // namespace util

#endif // BITCOIN_UTIL_THREADPRIORITY_H
