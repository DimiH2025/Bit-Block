// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_BIP39_BIP39_H
#define BITCOIN_BIP39_BIP39_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace bip39 {

//! Number of six-sided dice rolls SeedSigner requires for a 128-bit
//! (12-word) mnemonic. log2(6) * 50 ~= 129.3 bits, truncated to 128.
static constexpr size_t DICE_ROLLS_FOR_12_WORDS = 50;

//! Number of six-sided dice rolls SeedSigner requires for a 256-bit
//! (24-word) mnemonic. log2(6) * 99 ~= 255.9 bits.
static constexpr size_t DICE_ROLLS_FOR_24_WORDS = 99;

//! Human-readable reason a dice-roll string or mnemonic was rejected.
enum class Error {
    NONE,
    BAD_ROLL_LENGTH,
    BAD_ROLL_CHARACTER,
    BAD_WORD_COUNT,
    UNKNOWN_WORD,
    BAD_CHECKSUM,
};

const char* ErrorString(Error err);

std::optional<std::string> MnemonicFromDiceRolls(const std::string& rolls, Error& error);
bool CheckMnemonic(const std::string& mnemonic, Error& error);
std::vector<unsigned char> MnemonicToSeed(const std::string& mnemonic, const std::string& passphrase = "");

} // namespace bip39

#endif // BITCOIN_BIP39_BIP39_H
