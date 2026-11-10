# Upstream tracking

Bit-Block is a fork of **Bitcoin Knots v29.1.knots20250903** (commit `271fd206893a`). It follows the main Bitcoin chain. Knots 29.2 and later enforce BIP-110 as consensus rules, so Bit-Block does **not** merge or rebase onto them. Upstream fixes are ported one at a time, preferably from Bitcoin Core's `29.x` branch.

Bit-Block's anti-spam rules are local policy only and never change consensus.

## Ported fixes (V4)

| Upstream | What | Files |
|---|---|---|
| Core #35209 / #35212 (CVE-2024-52911) | Use-after-free in `ConnectBlock`: `txsdata` must outlive the script-check queue | `src/validation.cpp` |
| Core #33788 (CVE-2025-46598) | Crafted-transaction DoS: sighash midstate cache, no duplicate script re-verification, no peer bans for tx script failures | `src/script/interpreter.*`, `src/policy/policy.*`, `src/validation.cpp`, `src/net_processing.cpp` |
| Core #33344 | Witness-mutation check in `FillBlock`, repeated `blocktxn` handling, no whitelist permissions for Tor inbound | `src/blockencodings.*`, `src/net.*`, `src/net_processing.cpp` |
| Core #34222, #34370 | Failed wallet migration/restore/dump no longer removes whole wallet directories | `src/wallet/*` |
| Core #33474 | `getblockheader` target for the requested block | `src/rpc/blockchain.cpp` |
| Core #34268, #35234 | `sendall` unsolvable descriptors, input-size estimate, `IsFromMe` | `src/wallet/rpc/spend.cpp`, `src/wallet/spend.cpp`, `src/wallet/wallet.cpp` |
| Core #34680 | Netlink build fix | `src/common/netif.cpp` |
| Own, along the lines of Core #34544 | Reject wallet names with `.` / `..` path elements | `src/wallet/wallet.cpp` |

Upstream tests were ported where they applied (not `src/test/transaction_tests.cpp`).

## Deliberately not ported

- Knots BIP-110 / RDTS consensus enforcement (29.3+).
- Knots commit to the block's transaction count (CVE-2017-12842 change).
- TRUC check skipped on reorg (Core #33474 part): Knots 29.1 has no `bypass_limits` plumbing.
- Miner block-weight reserve change (Core #34268 part): Bit-Block's miner code does not use `block_reserved_weight`.
- Doc and seed-list changes.

## Still to check (open)

- Core notes for 32.1: #35384 (settings.json), #35227, #35228, #35316 (MuSig), #35410 (private broadcast), not yet reviewed against Bit-Block.
- The three medium/high issues fixed in Core 30.0 that Core said it would disclose after 32.1. Check the Core security advisories page and port the 29.x backports, if any.

## Routine for each new upstream release

1. Read the Core release notes on bitcoincore.org.
2. Diff the Core `29.x` branch since the last item above.
3. Port only what applies, keep Bit-Block's custom code, update this file and the release notes.
