// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <policy/antispam.h>

// All seven default to enabled -- see antispam.h for what each governs and
// -init.cpp for the corresponding -antispam* startup options.
bool g_antispam_limit_scriptpubkey_size{true};
bool g_antispam_limit_pushdata_size{true};
bool g_antispam_reject_undefined_witness_version{true};
bool g_antispam_reject_taproot_annex{true};
bool g_antispam_limit_control_block_size{true};
bool g_antispam_reject_op_success{true};
bool g_antispam_reject_tapscript_if{true};
