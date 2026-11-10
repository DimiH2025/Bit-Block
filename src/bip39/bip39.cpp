// Copyright (c) 2026 The Bit-Block developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bip39/bip39.h>
#include <bip39/wordlist_english.h>

#include <crypto/hmac_sha512.h>
#include <crypto/sha256.h>
#include <support/cleanse.h>

#include <array>
#include <map>

namespace bip39 {

namespace {

constexpr size_t ENTROPY_BITS_12_WORD = 128;
constexpr size_t ENTROPY_BITS_24_WORD = 256;
constexpr size_t CHECKSUM_BITS_12_WORD = ENTROPY_BITS_12_WORD / 32;
constexpr size_t CHECKSUM_BITS_24_WORD = ENTROPY_BITS_24_WORD / 32;

std::vector<unsigned char> Sha256(const std::string& data)
{
    std::vector<unsigned char> hash(CSHA256::OUTPUT_SIZE);
    CSHA256().Write(reinterpret_cast<const unsigned char*>(data.data()), data.size()).Finalize(hash.data());
    return hash;
}

const std::map<std::string, uint16_t>& WordIndexMap()
{
    static const std::map<std::string, uint16_t> map = [] {
        std::map<std::string, uint16_t> m;
        for (uint16_t i = 0; i < 2048; ++i) m.emplace(WORDLIST_ENGLISH[i], i);
        return m;
    }();
    return map;
}

//! Split on any run of ASCII whitespace (space, tab, CR, LF).
std::vector<std::string> SplitWords(const std::string& s)
{
    std::vector<std::string> words;
    std::string word;
    for (char c : s) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!word.empty()) { words.push_back(word); word.clear(); }
        } else {
            word += c;
        }
    }
    if (!word.empty()) words.push_back(word);
    return words;
}

void AppendBits(std::vector<bool>& bits, uint32_t value, size_t num_bits)
{
    for (size_t i = num_bits; i-- > 0;) {
        bits.push_back((value >> i) & 1);
    }
}

std::string EntropyToMnemonic(const std::vector<unsigned char>& entropy, size_t checksum_bits)
{
    std::vector<unsigned char> hash(CSHA256::OUTPUT_SIZE);
    CSHA256().Write(entropy.data(), entropy.size()).Finalize(hash.data());

    std::vector<bool> bits;
    bits.reserve(entropy.size() * 8 + checksum_bits);
    for (unsigned char byte : entropy) AppendBits(bits, byte, 8);
    // The checksum is the FIRST checksum_bits bits of SHA256(entropy) (the high
    // bits of hash[0]), matching what CheckMnemonic() verifies. Appending the
    // low bits instead yields an invalid checksum for 12-word mnemonics.
    AppendBits(bits, hash[0] >> (8 - checksum_bits), checksum_bits);

    std::string mnemonic;
    for (size_t i = 0; i < bits.size(); i += 11) {
        uint32_t idx = 0;
        for (size_t j = 0; j < 11; ++j) idx = (idx << 1) | (bits[i + j] ? 1 : 0);
        if (!mnemonic.empty()) mnemonic += ' ';
        mnemonic += WORDLIST_ENGLISH[idx];
    }
    return mnemonic;
}

} // namespace

const char* ErrorString(Error err)
{
    switch (err) {
    case Error::NONE: return "no error";
    case Error::BAD_ROLL_LENGTH: return "dice roll string must be exactly 50 characters (12-word) or 99 characters (24-word)";
    case Error::BAD_ROLL_CHARACTER: return "dice roll string must contain only digits 1-6";
    case Error::BAD_WORD_COUNT: return "mnemonic must contain exactly 12 or 24 words";
    case Error::UNKNOWN_WORD: return "mnemonic contains a word that is not in the BIP-39 English wordlist";
    case Error::BAD_CHECKSUM: return "mnemonic checksum does not match its entropy (mistyped or corrupted mnemonic)";
    }
    return "unknown error";
}

std::optional<std::string> MnemonicFromDiceRolls(const std::string& rolls, Error& error)
{
    error = Error::NONE;

    size_t checksum_bits;
    size_t entropy_bytes_len;
    if (rolls.size() == DICE_ROLLS_FOR_12_WORDS) {
        checksum_bits = CHECKSUM_BITS_12_WORD;
        entropy_bytes_len = ENTROPY_BITS_12_WORD / 8;
    } else if (rolls.size() == DICE_ROLLS_FOR_24_WORDS) {
        checksum_bits = CHECKSUM_BITS_24_WORD;
        entropy_bytes_len = ENTROPY_BITS_24_WORD / 8;
    } else {
        error = Error::BAD_ROLL_LENGTH;
        return std::nullopt;
    }

    for (char c : rolls) {
        if (c < '1' || c > '6') {
            error = Error::BAD_ROLL_CHARACTER;
            return std::nullopt;
        }
    }

    std::vector<unsigned char> entropy = Sha256(rolls);
    entropy.resize(entropy_bytes_len);

    return EntropyToMnemonic(entropy, checksum_bits);
}

bool CheckMnemonic(const std::string& mnemonic, Error& error)
{
    error = Error::NONE;

    const std::vector<std::string> words = SplitWords(mnemonic);

    if (words.size() != 12 && words.size() != 24) {
        error = Error::BAD_WORD_COUNT;
        return false;
    }

    const auto& index_map = WordIndexMap();
    std::vector<bool> bits;
    bits.reserve(words.size() * 11);
    for (const auto& w : words) {
        auto it = index_map.find(w);
        if (it == index_map.end()) {
            error = Error::UNKNOWN_WORD;
            return false;
        }
        AppendBits(bits, it->second, 11);
    }

    const size_t checksum_bits = words.size() == 12 ? CHECKSUM_BITS_12_WORD : CHECKSUM_BITS_24_WORD;
    const size_t entropy_bits = bits.size() - checksum_bits;

    std::vector<unsigned char> entropy(entropy_bits / 8, 0);
    for (size_t i = 0; i < entropy_bits; ++i) {
        if (bits[i]) entropy[i / 8] |= (1 << (7 - (i % 8)));
    }

    std::vector<unsigned char> hash(CSHA256::OUTPUT_SIZE);
    CSHA256().Write(entropy.data(), entropy.size()).Finalize(hash.data());

    for (size_t i = 0; i < checksum_bits; ++i) {
        bool expected = (hash[0] >> (7 - i)) & 1;
        if (bits[entropy_bits + i] != expected) {
            error = Error::BAD_CHECKSUM;
            return false;
        }
    }

    return true;
}

std::vector<unsigned char> MnemonicToSeed(const std::string& mnemonic_in, const std::string& passphrase)
{
    // Canonicalise the sentence the way other BIP-39 wallets do: words joined
    // by exactly one space, no leading/trailing whitespace. Without this, a
    // stray double or trailing space would still pass CheckMnemonic() but
    // derive a different seed than every other wallet for the same words.
    std::string mnemonic;
    for (const auto& w : SplitWords(mnemonic_in)) {
        if (!mnemonic.empty()) mnemonic += ' ';
        mnemonic += w;
    }

    const std::string salt = "mnemonic" + passphrase;

    std::array<unsigned char, CHMAC_SHA512::OUTPUT_SIZE> u{};
    {
        std::vector<unsigned char> data(salt.begin(), salt.end());
        data.push_back(0x00);
        data.push_back(0x00);
        data.push_back(0x00);
        data.push_back(0x01);
        CHMAC_SHA512(reinterpret_cast<const unsigned char*>(mnemonic.data()), mnemonic.size())
            .Write(data.data(), data.size())
            .Finalize(u.data());
    }

    std::array<unsigned char, CHMAC_SHA512::OUTPUT_SIZE> t = u;
    for (int i = 1; i < 2048; ++i) {
        std::array<unsigned char, CHMAC_SHA512::OUTPUT_SIZE> next{};
        CHMAC_SHA512(reinterpret_cast<const unsigned char*>(mnemonic.data()), mnemonic.size())
            .Write(u.data(), u.size())
            .Finalize(next.data());
        u = next;
        for (size_t j = 0; j < t.size(); ++j) t[j] ^= u[j];
    }

    std::vector<unsigned char> seed(t.begin(), t.end());
    memory_cleanse(u.data(), u.size());
    memory_cleanse(t.data(), t.size());
    memory_cleanse(mnemonic.data(), mnemonic.size());
    return seed;
}

} // namespace bip39
