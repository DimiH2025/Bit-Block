// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POLICY_ANTISPAM_H
#define BITCOIN_POLICY_ANTISPAM_H

// Seven independently toggleable relay/mining policy rules mirroring the
// technical content of BIP-110 ("Reduced Data Temporary Softfork"),
// implemented here as *local node policy only* -- never as a consensus
// rule. When one of these is enabled, this node simply declines to relay
// or mine transactions matching that pattern; it never rejects a block
// containing them. There is no fork risk, and nothing to coordinate with
// the rest of the network -- each of these is exactly as safe, and exactly
// as unilateral, as any other existing relay policy setting (e.g.
// -datacarriersize).
//
// Reference (rule content only -- NOT the activation/signaling mechanism,
// which this node does not implement):
// https://github.com/bitcoin/bips/blob/master/bip-0110.mediawiki
//
// Three of these seven rules were already enforced unconditionally in this
// codebase prior to adding these switches (inherited from Bitcoin Knots):
// rule 3 (undefined witness versions), rule 4 (Taproot annex), and rule 6
// (OP_SUCCESS). Turning those three switches off restores the ability to
// relay/mine such transactions; all seven behave identically as ON/OFF
// switches regardless of that history.

// Rule 1: new non-OP_RETURN output scripts over ANTISPAM_MAX_SCRIPTPUBKEY_SIZE
// bytes are non-standard. (OP_RETURN outputs are governed separately by the
// existing -datacarriersize setting, already stricter than BIP-110's own
// 83-byte OP_RETURN allowance by default in this codebase.)
extern bool g_antispam_limit_scriptpubkey_size;

// Rule 2: scriptSig push-data and witness stack items over
// ANTISPAM_MAX_PUSHDATA_SIZE bytes are non-standard, except the redeemScript
// push itself in a BIP16 (P2SH) scriptSig.
extern bool g_antispam_limit_pushdata_size;

// Rule 3: spending an output with an undefined witness (or Tapleaf) version
// -- i.e. anything other than Witness v0, Taproot (BIP341), or Pay-to-Anchor
// -- is non-standard. (Creating such an output is a separate, pre-existing
// setting: -acceptunknownwitness / opts.acceptunknownwitness.)
extern bool g_antispam_reject_undefined_witness_version;

// Rule 4: a witness stack containing a Taproot annex is non-standard.
extern bool g_antispam_reject_taproot_annex;

// Rule 5: a Taproot control block over ANTISPAM_MAX_CONTROL_BLOCK_SIZE bytes
// is non-standard.
extern bool g_antispam_limit_control_block_size;

// Rule 6: a tapscript containing any OP_SUCCESS* opcode, anywhere in the
// script (even in an untaken branch), is non-standard.
extern bool g_antispam_reject_op_success;

// Rule 7: a tapscript that executes OP_IF or OP_NOTIF (regardless of the
// branch taken) is non-standard. This is the rule that actually prevents
// the OP_FALSE OP_IF ... OP_ENDIF envelope pattern used by Ordinals-style
// inscriptions.
extern bool g_antispam_reject_tapscript_if;

static constexpr unsigned int ANTISPAM_MAX_SCRIPTPUBKEY_SIZE{34};
static constexpr unsigned int ANTISPAM_MAX_PUSHDATA_SIZE{256};
static constexpr unsigned int ANTISPAM_MAX_CONTROL_BLOCK_SIZE{257};

#endif // BITCOIN_POLICY_ANTISPAM_H
