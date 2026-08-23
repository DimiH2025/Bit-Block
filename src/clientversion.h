// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CLIENTVERSION_H
#define BITCOIN_CLIENTVERSION_H

#include <util/macros.h>

#include <bitcoin-build-config.h> // IWYU pragma: keep

// Check that required client information is defined
#if !defined(CLIENT_VERSION_MAJOR) || !defined(CLIENT_VERSION_MINOR) || !defined(CLIENT_VERSION_BUILD) || !defined(CLIENT_VERSION_IS_RELEASE) || !defined(COPYRIGHT_YEAR)
#error Client version information missing: version is not defined by bitcoin-build-config.h or in any other way
#endif

//! Copyright string used in Windows .rc files
#define COPYRIGHT_STR "2009-" STRINGIZE(COPYRIGHT_YEAR) " " COPYRIGHT_HOLDERS_FINAL

/**
 * bitcoind-res.rc includes this file, but it cannot cope with real c++ code.
 * WINDRES_PREPROC is defined to indicate that its pre-processor is running.
 * Anything other than a define should be guarded below.
 */

#if !defined(WINDRES_PREPROC)

#include <cstdint>
#include <string>
#include <vector>

static const int CLIENT_VERSION =
                             10000 * CLIENT_VERSION_MAJOR
                         +     100 * CLIENT_VERSION_MINOR
                         +       1 * CLIENT_VERSION_BUILD;

extern const std::string UA_NAME;


std::string FormatFullVersion();
std::string FormatSubVersion(const std::string& name, int nClientVersion, const std::vector<std::string>& comments, bool base_name_only = false);

std::string CopyrightHolders(const std::string& strPrefix);

/** Returns licensing information (for -version) */
std::string LicenseInfo();

static constexpr int64_t SECONDS_PER_YEAR = 31558060;
static constexpr int POSIX_EPOCH_YEAR = 1970;

// Two years after COPYRIGHT_YEAR, Bit-Block shows a one-time, dismissible
// reminder suggesting the user consider upgrading to a newer build.
//
// This is intentionally advisory-only. An earlier version of this mechanism
// (inherited from Bitcoin Knots, itself based on a Bitcoin Core PR -- #10282
// -- that was proposed and ultimately closed/rejected upstream) additionally
// refused to start, and refused to accept new blocks, once past this date.
// Bit-Block does neither: reaching this date changes nothing about how the
// node validates or runs, it only shows a message. This follows the
// suggestion of a reviewer on that original, rejected PR: "I'd be much
// happier if this just alarmed and warned the user rather than shutdown
// the node."
static constexpr int64_t DEFAULT_UPGRADE_REMINDER_TIME = ((COPYRIGHT_YEAR - POSIX_EPOCH_YEAR) * SECONDS_PER_YEAR) + (SECONDS_PER_YEAR * 2);
extern int64_t g_upgrade_reminder_time;

#endif // WINDRES_PREPROC

#endif // BITCOIN_CLIENTVERSION_H
