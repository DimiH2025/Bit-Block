// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <coins.h>
#include <kernel/mempool_options.h>
#include <policy/antispam.h>
#include <policy/policy.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/solver.h>
#include <test/util/setup_common.h>
#include <uint256.h>

#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

namespace {

// BasicTestingSetup switches the anti-spam rules off; these tests switch them on
// (all seven, as in a default node) and restore the "off" baseline afterwards.
struct AntiSpamOn {
    AntiSpamOn()
    {
        Set(true);
    }
    ~AntiSpamOn()
    {
        Set(false);
    }
    static void Set(bool on)
    {
        g_antispam_limit_scriptpubkey_size = on;
        g_antispam_limit_pushdata_size = on;
        g_antispam_reject_undefined_witness_version = on;
        g_antispam_reject_taproot_annex = on;
        g_antispam_limit_control_block_size = on;
        g_antispam_reject_op_success = on;
        g_antispam_reject_tapscript_if = on;
    }
};

// Builds a one-input transaction that spends `spk` with the given witness stack
// and runs IsWitnessStandard on it.
bool WitnessStandard(const CScript& spk, std::vector<std::vector<unsigned char>> stack, std::string& reason)
{
    CCoinsView dummy;
    CCoinsViewCache coins(&dummy);
    const COutPoint prevout{Txid::FromUint256(uint256{1}), 0};
    coins.AddCoin(prevout, Coin(CTxOut(10000, spk), 1, false), /*possible_overwrite=*/false);

    CMutableTransaction mtx;
    mtx.vin.emplace_back(prevout);
    mtx.vin[0].scriptWitness.stack = std::move(stack);
    mtx.vout.emplace_back(5000, CScript() << OP_TRUE);
    return IsWitnessStandard(CTransaction{mtx}, coins, "", reason);
}

CScript P2WSH()
{
    return CScript() << OP_0 << std::vector<unsigned char>(32, 0x11);
}

CScript P2TR()
{
    return CScript() << OP_1 << std::vector<unsigned char>(32, 0x22);
}

} // namespace

BOOST_FIXTURE_TEST_SUITE(antispam_tests, BasicTestingSetup)

// Rule 1: output scripts over 34 bytes are non-standard (OP_RETURN excepted).
BOOST_AUTO_TEST_CASE(rule1_scriptpubkey_size)
{
    // Bare 1-of-1 multisig with a compressed key is 37 bytes.
    std::vector<unsigned char> pubkey(33, 0x02);
    CScript multisig;
    multisig << OP_1 << pubkey << OP_1 << OP_CHECKMULTISIG;
    BOOST_REQUIRE_EQUAL(multisig.size(), 37U);

    CMutableTransaction mtx;
    mtx.vin.emplace_back(COutPoint{Txid::FromUint256(uint256{1}), 0});
    mtx.vin[0].scriptSig = CScript() << OP_0;
    mtx.vout.emplace_back(100000, multisig);

    kernel::MemPoolOptions opts;
    opts.permit_bare_multisig = true;
    opts.max_datacarrier_bytes = 83;

    std::string reason;
    BOOST_CHECK_MESSAGE(IsStandardTx(CTransaction{mtx}, opts, reason), reason);

    AntiSpamOn on;
    BOOST_CHECK(!IsStandardTx(CTransaction{mtx}, opts, reason));
    BOOST_CHECK_EQUAL(reason, "antispam-scriptpubkey-size");

    // A 34-byte P2WSH output is still fine.
    mtx.vout[0].scriptPubKey = P2WSH();
    BOOST_CHECK(IsStandardTx(CTransaction{mtx}, opts, reason));

    // OP_RETURN is governed by the datacarrier limit, not by rule 1.
    mtx.vout.emplace_back(0, CScript() << OP_RETURN << std::vector<unsigned char>(60, 0x33));
    BOOST_CHECK_MESSAGE(IsStandardTx(CTransaction{mtx}, opts, reason), reason);
}

// Rule 2 (witness half): a large P2WSH witness *script* is exempt, a large
// script *argument* is not.
BOOST_AUTO_TEST_CASE(rule2_p2wsh_large_script_is_exempt)
{
    AntiSpamOn on;
    std::string reason;

    // 300-byte witness script (over the 256-byte limit), small arguments.
    const std::vector<unsigned char> big_script(300, OP_NOP);
    BOOST_CHECK_MESSAGE(WitnessStandard(P2WSH(), {std::vector<unsigned char>(71, 0x01), big_script}, reason), reason);

    // A 300-byte argument in front of a small script is rejected.
    const std::vector<unsigned char> small_script(20, OP_NOP);
    BOOST_CHECK(!WitnessStandard(P2WSH(), {std::vector<unsigned char>(300, 0x01), small_script}, reason));
}

// Rule 2 (witness half) and rule 5 for Taproot script-path spends.
BOOST_AUTO_TEST_CASE(rule2_taproot_script_and_control_block_exempt)
{
    AntiSpamOn on;
    std::string reason;

    // Control block: leaf version 0xc0 + 32-byte internal key + 7 * 32-byte path = 257 bytes (the limit).
    std::vector<unsigned char> control_ok(33 + 7 * 32, 0x44);
    control_ok[0] = 0xc0;
    const std::vector<unsigned char> big_tapscript(400, OP_NOP);
    BOOST_CHECK_MESSAGE(WitnessStandard(P2TR(), {std::vector<unsigned char>(10, 0x01), big_tapscript, control_ok}, reason), reason);

    // One path element more makes the control block 289 bytes: rejected by rule 5.
    std::vector<unsigned char> control_big(33 + 8 * 32, 0x44);
    control_big[0] = 0xc0;
    BOOST_CHECK(!WitnessStandard(P2TR(), {std::vector<unsigned char>(10, 0x01), big_tapscript, control_big}, reason));

    // Rule 4: an annex (last item, starts with 0x50) is rejected.
    std::vector<unsigned char> annex{0x50, 0x01};
    BOOST_CHECK(!WitnessStandard(P2TR(), {std::vector<unsigned char>(10, 0x01), big_tapscript, control_ok, annex}, reason));
}

// Rules 6 (OP_SUCCESS) and 7 (OP_IF/OP_NOTIF) are enforced as script verification
// flags in validation.cpp, not in IsWitnessStandard, so they are not covered here.

BOOST_AUTO_TEST_SUITE_END()
