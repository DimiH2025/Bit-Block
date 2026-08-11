// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <node/chainstatemanager_args.h>

#include <arith_uint256.h>
#include <common/args.h>
#include <common/system.h>
#include <logging.h>
#include <node/coins_view_args.h>
#include <node/database_args.h>
#include <tinyformat.h>
#include <uint256.h>
#include <util/result.h>
#include <util/strencodings.h>
#include <util/translation.h>
#include <validation.h>

#include <algorithm>
#include <chrono>
#include <string>

namespace node {
util::Result<void> ApplyArgsManOptions(const ArgsManager& args, ChainstateManager::Options& opts)
{
    if (auto value{args.GetIntArg("-checkblockindex")}) {
        // Interpret bare -checkblockindex argument as 1 instead of 0.
        opts.check_block_index = args.GetArg("-checkblockindex")->empty() ? 1 : *value;
    }

    if (auto value{args.GetBoolArg("-checkpoints")}) opts.checkpoints_enabled = *value;

    if (auto value{args.GetArg("-minimumchainwork")}) {
        if (auto min_work{uint256::FromUserHex(*value)}) {
            opts.minimum_chain_work = UintToArith256(*min_work);
        } else {
            return util::Error{Untranslated(strprintf("Invalid minimum work specified (%s), must be up to %d hex digits", *value, uint256::size() * 2))};
        }
    }

    if (auto value{args.GetArg("-assumevalid")}) {
        if (auto block_hash{uint256::FromUserHex(*value)}) {
            opts.assumed_valid_block = *block_hash;
        } else {
            return util::Error{Untranslated(strprintf("Invalid assumevalid block hash specified (%s), must be up to %d hex digits (or 0 to disable)", *value, uint256::size() * 2))};
        }
    }

    // -ibdsyncmode: an explicit, opt-in choice between "verify everything
    // from genesis" and a libbitcoin-milestone-style "trust the assumevalid
    // checkpoint" fast path. Leaving it unset preserves today's default
    // -assumevalid behavior unchanged.
    if (auto value{args.GetArg("-ibdsyncmode")}) {
        if (*value == "verify") {
            opts.assumed_valid_block = uint256{};
            LogInfo("ibdsyncmode=verify: full script verification from genesis is enabled; -assumevalid checkpoint disabled.");
        } else if (*value == "trust") {
            const bool has_checkpoint = opts.assumed_valid_block.has_value() && !opts.assumed_valid_block->IsNull();
            if (has_checkpoint) {
                LogWarning("ibdsyncmode=trust: skipping script verification for blocks at or below the "
                           "assumevalid checkpoint (%s). This is ONLY safe if you, or software you "
                           "trust, already independently verified the chain up to that block. If you "
                           "have not, use -ibdsyncmode=verify (or -assumevalid=0) to verify every "
                           "script back to genesis instead.", opts.assumed_valid_block->GetHex());
            } else {
                LogWarning("ibdsyncmode=trust was set, but there is no assumevalid checkpoint configured "
                           "for this chain, so full verification from genesis will occur anyway.");
            }
        } else {
            return util::Error{Untranslated(strprintf("Invalid -ibdsyncmode value (%s), must be 'verify' or 'trust'", *value))};
        }
    }

    if (auto value{args.GetBoolArg("-parpriority")}) opts.raise_validation_thread_priority = *value;

    if (auto value{args.GetIntArg("-maxtipage")}) opts.max_tip_age = std::chrono::seconds{*value};

    ReadDatabaseArgs(args, opts.coins_db);
    ReadCoinsViewArgs(args, opts.coins_view);

    int script_threads = args.GetIntArg("-par", DEFAULT_SCRIPTCHECK_THREADS);
    if (script_threads <= 0) {
        // -par=0 means autodetect (number of cores - 1 script threads)
        // -par=-n means "leave n cores free" (number of cores - n - 1 script threads)
        script_threads += GetNumCores();
    }
    // Subtract 1 because the main thread counts towards the par threads.
    opts.worker_threads_num = script_threads - 1;

    if (auto max_size = args.GetIntArg("-maxsigcachesize")) {
        // 1. When supplied with a max_size of 0, both the signature cache and
        //    script execution cache create the minimum possible cache (2
        //    elements). Therefore, we can use 0 as a floor here.
        // 2. Multiply first, divide after to avoid integer truncation.
        size_t clamped_size_each = std::max<int64_t>(*max_size, 0) * (1 << 20) / 2;
        opts.script_execution_cache_bytes = clamped_size_each;
        opts.signature_cache_bytes = clamped_size_each;
    }

    return {};
}
} // namespace node
