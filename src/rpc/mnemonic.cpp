// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bip39/bip39.h>
#include <crypto/hex_base.h>
#include <key.h>
#include <key_io.h>
#include <rpc/protocol.h>
#include <rpc/request.h>
#include <rpc/server.h>
#include <rpc/util.h>
#include <span.h>
#include <univalue.h>

#include <string>

static RPCHelpMan mnemonicfromdice()
{
    return RPCHelpMan{"mnemonicfromdice",
        "\nGenerate a BIP-39 mnemonic seed phrase from physical six-sided dice rolls,\n"
        "using the same roll-string -> SHA256 -> entropy -> mnemonic method as\n"
        "SeedSigner, Coldcard, and Krux, so the result is cross-verifiable against\n"
        "those tools or against https://iancoleman.io/bip39.\n"
        "\nSECURITY WARNING: the security of the resulting seed depends entirely on\n"
        "the physical randomness of the dice rolls you supply. This node cannot\n"
        "verify that your rolls came from fair dice, that they weren't observed by\n"
        "anyone else, or that this machine is free of malware capturing this RPC\n"
        "call. Prefer running this offline/air-gapped for real funds, the same way\n"
        "you would with a dedicated signing device.\n",
        {
            {"rolls", RPCArg::Type::STR, RPCArg::Optional::NO, "A string of dice pip digits ('1'-'6', no spaces or separators), one\n"
                "character per roll. Must be exactly 50 characters for a 12-word/128-bit\n"
                "mnemonic, or 99 characters for a 24-word/256-bit mnemonic."},
        },
        RPCResult{
            RPCResult::Type::STR, "mnemonic", "The generated BIP-39 mnemonic seed phrase"
        },
        RPCExamples{
            HelpExampleCli("mnemonicfromdice", "\"65515223131652132161133154444123616466443112153441\"")
            + HelpExampleRpc("mnemonicfromdice", "\"65515223131652132161133154444123616466443112153441\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            const std::string rolls = self.Arg<std::string>("rolls");

            bip39::Error error;
            auto mnemonic = bip39::MnemonicFromDiceRolls(rolls, error);
            if (!mnemonic) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, bip39::ErrorString(error));
            }
            return *mnemonic;
        },
    };
}

static RPCHelpMan mnemonictoseed()
{
    return RPCHelpMan{"mnemonictoseed",
        "\nDerive the 64-byte BIP-39 seed (and its BIP-32 master extended private\n"
        "key) from a mnemonic sentence and optional passphrase. The seed is\n"
        "standard and interoperable with any other BIP-39/BIP-32 wallet given the\n"
        "same mnemonic and passphrase.\n"
        "\nThis RPC does not create or modify any wallet. Use importdescriptors\n"
        "with the returned master extended private key (at whatever derivation\n"
        "path you want, e.g. m/84'/0'/0' for single-sig native segwit) to actually\n"
        "load funds-controlling keys into a wallet.\n"
        "\nSECURITY WARNING: this call passes your mnemonic (and derives your\n"
        "private keys) through this node's RPC interface and logs. Treat the\n"
        "output as highly sensitive -- anyone who obtains the seed or master key\n"
        "can spend any funds derived from it.\n",
        {
            {"mnemonic", RPCArg::Type::STR, RPCArg::Optional::NO, "The BIP-39 mnemonic sentence (space-separated words)."},
            {"passphrase", RPCArg::Type::STR, RPCArg::Default{""}, "Optional BIP-39 passphrase (the so-called \"25th word\"). Using a\n"
                "passphrase produces a completely different wallet than the same mnemonic\n"
                "with no passphrase -- there is no way to recover funds from the wrong\n"
                "passphrase."},
            {"require_valid", RPCArg::Type::BOOL, RPCArg::Default{true}, "Reject the mnemonic if its word count or checksum is invalid, rather than\n"
                "silently deriving a seed from it anyway. Leave this on unless you have a\n"
                "specific reason to derive a seed from a non-standard word list."},
        },
        RPCResult{
            RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::STR_HEX, "seed", "The 64-byte BIP-39 seed, hex-encoded"},
                {RPCResult::Type::STR, "master_key", "The BIP-32 master extended private key (xprv), base58-encoded"},
            }
        },
        RPCExamples{
            HelpExampleCli("mnemonictoseed", "\"hole luggage safe present express tragic orbit shed switch metal identify path\"")
            + HelpExampleRpc("mnemonictoseed", "\"hole luggage safe present express tragic orbit shed switch metal identify path\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            const std::string mnemonic = self.Arg<std::string>("mnemonic");
            const std::string passphrase = self.Arg<std::string>("passphrase");
            const bool require_valid = self.Arg<bool>("require_valid");

            if (require_valid) {
                bip39::Error error;
                if (!bip39::CheckMnemonic(mnemonic, error)) {
                    throw JSONRPCError(RPC_INVALID_PARAMETER, bip39::ErrorString(error));
                }
            }

            const std::vector<unsigned char> seed = bip39::MnemonicToSeed(mnemonic, passphrase);

            CExtKey master_key;
            master_key.SetSeed(MakeByteSpan(seed));

            UniValue result(UniValue::VOBJ);
            result.pushKV("seed", HexStr(seed));
            result.pushKV("master_key", EncodeExtKey(master_key));
            return result;
        },
    };
}

void RegisterMnemonicRPCCommands(CRPCTable& t)
{
    static const CRPCCommand commands[]{
        {"util", &mnemonicfromdice},
        {"util", &mnemonictoseed},
    };
    for (const auto& c : commands) {
        t.appendCommand(c.name, &c);
    }
}
