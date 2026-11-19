# Bit-Block V4

V4 is a bug-fix and hardening release. It combines a full review of the code that is specific to Bit-Block with fixes that the Bitcoin Core and Bitcoin Knots 29.x maintenance releases made after Bit-Block's Knots 29.1 base. Updating is recommended for everyone. Anyone who used `mnemonicfromdice` with 12 words should read the first item.

## Important: 12-word dice mnemonics

- **`mnemonicfromdice` produced an invalid checksum for 12-word phrases.** The wrong bits of the hash were used as the checksum, so roughly 15 out of 16 generated 12-word phrases failed validation in other wallets and in `mnemonictoseed`. 24-word phrases were not affected. If you generated a 12-word phrase with an earlier version, do not use or fund it: generate a new one with V4. A phrase that other tools reject cannot be trusted as a backup.

## Fixes

- **BIP-39 phrases with extra spaces.** A phrase with a double or trailing space passed validation but derived a different wallet than every other tool. Phrases are now normalised before the seed is derived.
- **Non-ASCII passphrases are refused.** Unicode (NFKD) normalisation is not implemented, so such a passphrase would silently give a wallet that other software cannot recover. `mnemonictoseed` now returns a clear error instead.
- **Anti-spam rule 2 was too strict.** The 256-byte limit also rejected large witness scripts, Taproot scripts and control blocks, which BIP-110 exempts. Large multisig and complex Taproot spends can be relayed again. The Options tooltip now says so.
- **Mine with DATUM on a pruned node.** The preset forces the transaction index, which cannot be combined with pruning, so startup failed on every launch once the option was saved. On a pruned node the preset now skips the transaction index and writes a log message.
- **Restart notice.** The anti-spam switches and the DATUM option now show "restart required", because they take effect at startup.
- **DATUM tooltip.** Enabling the transaction index does not require a reindex; the node builds it in the background.
- **Template-hint notifications** no longer go silent if the system clock moves backwards.
- **`mnemonictoseed`** wipes seed material from memory after use.
- **Smoke-test script** uses a private temporary directory instead of a fixed `/tmp` path.

## Fixes inherited from Bitcoin Core / Knots 29.x

Bit-Block shares most of its node code with Knots 29.1. On 10 November 2026 Bitcoin Core disclosed CVE-2025-62859, CVE-2025-62860 and CVE-2025-62861; all three are fixed in V4. These upstream fixes were ported by hand (only the targeted changes, not the whole Knots 29.4 delta):

- **Use-after-free in block validation (CVE-2024-52911).** `ConnectBlock` could free per-transaction data before the script-check threads finished with it. Objects are now declared in the safe order.
- **Crafted-transaction DoS (CVE-2025-46598, Core 29.x backport #33788).** A transaction could make a node spend far more CPU than its size justified. Signature-hash midstates are now cached, the duplicate script re-verification used to classify failures is removed, and peers are no longer banned for transaction script failures.
- **TRUC sibling eviction during a reorg (CVE-2025-62861, high; Core #33504).** When blocks were disconnected in a reorganisation, re-adding their transactions could evict valid transactions from the mempool through TRUC sibling eviction. Sibling eviction is now disabled for such re-adds.
- **Compact-block hardening (CVE-2025-62859, Core #32646/#33344).** Witness-mutation is checked inside `FillBlock`, a repeated `blocktxn` message no longer reaches an empty header, and the unused `READ_STATUS_CHECKBLOCK_FAILED` path is gone.
- **Tor inbound whitelist (CVE-2025-62860, #33344).** Address-based whitelist permissions are no longer applied to onion inbound connections, whose source address is meaningless.
- **Wallet deletion safety (#34222, #34370).** A failed wallet migration or restore no longer runs a recursive delete on a wallet directory; only the files the operation created are removed, which protects the default (unnamed) wallet and other files in the wallets folder. A failed `restorewallet` and a failed `createfromdump` follow the same rule. Migrated wallets of the unnamed default wallet get proper `default_wallet_*` names.
- **Wallet names with `.` or `..` path elements are refused** (hardening along the lines of Core #34544).
- **Wallet fixes:** `sendall` returns a clear error instead of an internal assertion when a wallet holds unsolvable descriptors; input-size estimation for such outputs is corrected; `IsFromMe` is evaluated per input.
- **`getblockheader`** now reports the target of the requested block, not the chain tip.
- **Build:** Linux gateway-query code compiles on more libc versions (#34680).

Not ported on purpose: Knots' BIP-110 consensus enforcement and the 29.3+ commit-to-transaction-count change. Bit-Block keeps following the main Bitcoin chain and applies the anti-spam rules as local policy only.

## What this release does not do

- It is not a rebase onto Knots 29.4.x. Core 29.x is end-of-life, so future upstream fixes will keep needing to be ported by hand. A rebase onto a current Knots line is the proper next step and is planned as a separate, larger change.
- One small upstream change was left out because it depends on code that differs in Knots 29.1: a block-weight reserve in the miner that Bit-Block does not have. See `UPSTREAM.md` for the full list of what was and was not ported.

## Unchanged

- The `-walletnotify` security fix and the anti-spam rules from V3.1 stay as they are; the anti-spam rules remain local policy only and never change consensus.
- Network user agent and the `bitcoin:` payment-link scheme are intentionally unchanged.

## Known limits

- macOS builds are not code-signed or notarized.
- Berkeley DB (legacy wallet) is not included in the macOS build.
- Passing a mnemonic on the command line can leave it in shell history; use `bitcoin-cli -stdin` for real funds, and keep dice-based seed generation on an offline machine.
