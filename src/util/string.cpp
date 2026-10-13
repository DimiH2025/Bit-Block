// Copyright (c) 2019-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/string.h>

#include <string>

namespace util {
void ReplaceAll(std::string& in_out, const std::string& search, const std::string& substitute)
{
    // SECURITY: this performs a literal, non-recursive substring replacement.
    // It must never interpret `substitute` as anything other than plain
    // text. An earlier implementation used std::regex_replace() with
    // `substitute` as the replacement-format argument -- std::regex_replace
    // treats sequences like $' and $& in that argument as special escapes
    // (copying in surrounding context from `in_out`/the match), not as
    // literal characters. Since ReplaceAll() is used with externally
    // controlled text as the substitute (e.g. a user-chosen wallet name
    // inserted via -walletnotify's %w placeholder, in wallet.cpp), that
    // allowed a wallet name such as one containing $' to inject attacker
    // -controlled content into the resulting command string, which is
    // ultimately passed to a shell. See upstream Bitcoin Core PR #36048,
    // which restores this exact literal/non-recursive contract.
    //
    // "Non-recursive" matters too: after inserting `substitute`, scanning
    // resumes after the inserted text, not from the start of it -- so if
    // `substitute` itself happens to contain `search` as a substring, it is
    // not replaced again.
    if (search.empty()) return;
    std::string::size_type pos = 0;
    while ((pos = in_out.find(search, pos)) != std::string::npos) {
        in_out.replace(pos, search.length(), substitute);
        pos += substitute.length();
    }
}
} // namespace util
