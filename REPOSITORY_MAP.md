# Bit-Block Repository Map

This document explains what each part of this repository does, in plain
language. It exists so that anyone — contributor, auditor, or curious user —
can understand what a given file or folder is for without having to read
the whole codebase first.

Bit-Block is a fork of [Bitcoin Knots](https://bitcoinknots.org), which is
itself a fork of [Bitcoin Core](https://bitcoincore.org). The large majority
of this repository is therefore inherited, well-audited code shared with
those projects. This document calls out clearly, in its own section below,
exactly what is specific to Bit-Block on top of that inherited base.

---

## ⚠️ Known issue worth flagging here

`bin/bit-block/` (~220 MB) contains **precompiled, stripped Linux binaries
checked directly into git** — actual `bitcoind`, `bitcoin-qt`, `bitcoin-cli`,
etc., not source code. This is inconsistent with the transparency this
document is meant to support: nobody can verify a committed binary actually
corresponds to the visible source without independently rebuilding it and
diffing the result, and there's no record of what compiler, flags, or
dependency versions produced these particular files. It also bloats every
clone of this repository by 220 MB for no functional benefit, now that
reproducible builds run in CI (see `.github/workflows/`). **Recommended:**
remove this directory from version control and add `bin/` to `.gitignore`.

---

## Top-level layout

| Path | What it is |
|---|---|
| `src/` | All C++ source code — the actual node, wallet, and GUI software. See detailed breakdown below. |
| `doc/` | Human-readable documentation: build instructions per OS, the JSON-RPC and REST API references, release notes, developer notes, BIP compliance list, tutorials. |
| `test/` | Test suites that run against compiled binaries: functional tests (Python scripts that start real nodes and script their behavior), fuzz test harnesses, and lint checks. Unit tests that compile *into* the binaries live under `src/test/` instead. |
| `contrib/` | Auxiliary scripts and tooling that isn't part of the node itself — release signing, reproducible-build tooling (Guix), Debian packaging, init scripts (systemd/upstart), block explorer linearization tools, developer utilities. |
| `depends/` | A self-contained build system that compiles this project's dependencies (Boost, Qt, libevent, SQLite, etc.) from source for a specific target platform. This is what makes cross-compiling to Windows or reproducible builds possible — it doesn't rely on whatever library versions happen to be installed on the machine. |
| `cmake/` | CMake build-system modules: dependency detection, compiler flag configuration, the Windows installer (NSIS) generation logic, macOS `.app` packaging logic. |
| `ci/` | Configuration for this project's continuous integration test matrix (separate from `.github/workflows/`, which handles *release builds* rather than *test runs*). |
| `.github/workflows/` | **Bit-Block-specific.** GitHub Actions definitions that compile installable builds for Windows, Linux, and macOS automatically. See the dedicated section below. |
| `share/` | Files installed alongside the binaries at runtime: example config files, the Windows installer script template, application icons/pixmaps, RPC auth helper scripts. |
| `bin/` | ⚠️ See flagged issue above — should not contain committed binaries. |
| `attached_assets/` | Miscellaneous uploaded assets (currently a logo image) not wired into the build. |
| `scripts/` | **Bit-Block-specific.** Plain shell entry points for local setup/smoke-testing — see below. |
| `run.sh` | **Bit-Block-specific.** Root-level convenience entry point; hands off to `scripts/`. |
| `README.md`, `CONTRIBUTING.md`, `SECURITY.md`, `INSTALL.md`, `COPYING` | Standard project documentation: what this is, how to contribute, how to report vulnerabilities, how to install, and the MIT license text. |
| `CMakeLists.txt`, `CMakePresets.json`, `configure.ac`, `Makefile.am`, `autogen.sh` | Build system entry points. **CMake is the build system actually used** (see `cmake/` and every `CMakeLists.txt` under `src/`). The `configure.ac`/`Makefile.am`/`autogen.sh` autotools files are legacy leftovers not used to build this project — see the note in the FAQ below. |
| `vcpkg.json` | Dependency manifest used only by the Windows MSVC (Visual Studio) build path — an alternative to `depends/` for that specific toolchain. |

---

## Inside `src/` — the actual software

| Path | What it does |
|---|---|
| `validation.cpp` / `validation.h` | The consensus engine. Verifies every block and transaction against Bitcoin's rules, manages the active chain, and decides what counts as valid. This is the most security-critical file in the entire codebase. |
| `net.cpp` / `net_processing.cpp` | The peer-to-peer networking layer: connecting to other nodes, exchanging blocks/transactions, handling the Bitcoin wire protocol. |
| `kernel/` | The "libbitcoinkernel" — an effort to isolate the core consensus/validation logic into a standalone library, separate from networking, wallet, and RPC concerns. |
| `wallet/` | Wallet functionality: key management, transaction creation and signing, coin selection, HD (hierarchical deterministic) key derivation. |
| `bip39/` | **Bit-Block-specific.** SeedSigner-compatible dice-roll entropy and BIP-39 mnemonic seed phrase support — not present in upstream Bitcoin Core/Knots, which use their own native HD seed format instead of BIP-39 mnemonics. |
| `rpc/` | JSON-RPC command implementations — every `bitcoin-cli` command is defined here. `rpc/mnemonic.cpp` (the `mnemonicfromdice` / `mnemonictoseed` commands) is Bit-Block-specific. |
| `qt/` | The Qt-based graphical interface (`bitcoin-qt`). Icons, splash screen, wallet dialogs, the send/receive UI, the installer's visual assets under `qt/res/`. |
| `script/` | Bitcoin Script interpreter and related validation (signature checking, script templates like P2WPKH/P2TR, Miniscript). |
| `consensus/` | Core consensus rules: block/transaction validity checks, proof-of-work, merkle trees. Deliberately kept minimal and separate so consensus-critical code is easy to audit in isolation. |
| `policy/` | Non-consensus mempool/relay policy — rules about what this node *chooses* to relay or mine, as opposed to what's actually valid on the network (e.g. minimum fees, dust limits, replace-by-fee rules). |
| `index/` | Optional secondary indexes (transaction index, block filter index, coinstats index) built on top of the base chain data for faster lookups. |
| `node/` | Glue code coordinating chainstate, mempool, and block storage — the "node" as distinct from wallet or GUI concerns. |
| `interfaces/` | Abstract interfaces used to decouple major components (e.g. so the GUI doesn't need to know internal wallet implementation details). |
| `ipc/` | Inter-process communication support for the experimental multiprocess build mode (separate `bitcoin-node`, `bitcoin-gui`, `bitcoin-wallet` processes talking over Cap'n Proto). |
| `crypto/` | Cryptographic primitives: SHA-256/512, RIPEMD-160, ChaCha20, Poly1305, HMAC. Used to build `bip39.cpp`'s PBKDF2 seed derivation. |
| `secp256k1/` | Vendored copy of the `libsecp256k1` elliptic-curve cryptography library (Bitcoin's actual signing/verification math). Includes a MuSig2 module in source form that Bit-Block does **not** enable (`SECP256K1_ENABLE_MODULE_MUSIG OFF` in `src/CMakeLists.txt`) and has no wallet-layer integration for. |
| `leveldb/` | Vendored copy of Google's LevelDB, used for the chainstate (UTXO set) and block index databases. `leveldb/db/version_set.cc` carries a Bit-Block-specific fix disabling an overly aggressive automatic-compaction heuristic (see the fixes section below). |
| `univalue/` | Vendored JSON library used throughout RPC. |
| `minisketch/` | Vendored set-reconciliation library, used by erlay-style transaction relay. |
| `common/`, `util/`, `support/` | Shared utility code: string/number formatting, filesystem helpers, threading helpers, memory locking. `util/threadpriority.{h,cpp}` is Bit-Block-specific (see below). |
| `compat/` | Platform-compatibility shims for differences between Linux/macOS/Windows. |
| `test/` | Unit tests that compile into the `test_bitcoin` binary (distinct from `test/` at the repo root, which holds *external* test scripts). |
| `bench/` | Microbenchmarks for performance-sensitive code paths. |
| `zmq/` | Optional ZeroMQ-based publish/subscribe notifications (new blocks, transactions) for external programs. |

---

## Bit-Block-specific additions

Everything below is new relative to the upstream Knots/Core codebase this
was forked from. Everything *not* listed here is inherited, shared code.

| File(s) | Purpose |
|---|---|
| `.github/workflows/build-windows.yml` | Cross-compiles a Windows installer (`bit-block-win64-setup.exe`) via mingw-w64 + NSIS, runs on GitHub's servers. |
| `.github/workflows/build-linux.yml` | Native Linux build (no cross-compile toolchain needed), packaged as a `.tar.gz` of binaries. |
| `.github/workflows/build-macos.yml` | Native macOS builds for both Apple Silicon and Intel, packaged as `Bit-Block.zip` containing `Bitcoin-Qt.app`. Not code-signed/notarized. |
| `run.sh`, `scripts/secure_startup.sh`, `scripts/smoke_bit-block.sh` | Plain shell entry points for local build/run/smoke-testing, replacing an earlier Replit-specific workspace configuration. |
| `src/bip39/bip39.h`, `bip39.cpp`, `wordlist_english.h` | SeedSigner-compatible dice-roll-to-mnemonic entropy derivation and standard BIP-39 seed derivation (PBKDF2-HMAC-SHA512). Verified against all 24 official BIP-39 test vectors and SeedSigner's own published known-answer test vectors. |
| `src/rpc/mnemonic.cpp` | Exposes the above as two RPC commands: `mnemonicfromdice` and `mnemonictoseed`. |
| `src/util/threadpriority.h`, `threadpriority.cpp` | Best-effort OS-level thread priority elevation for script-verification worker threads, controlled by the `-parpriority` startup option (inspired by libbitcoin's `blockchain.priority` setting). |
| `-ibdsyncmode` option (in `src/node/chainstatemanager_args.cpp`, `src/init.cpp`) | Lets an operator explicitly choose between full script verification from genesis (`verify`) or trusting the built-in `-assumevalid` checkpoint (`trust`, with an explicit warning logged) — inspired by libbitcoin's milestone-sync concept, layered on top of Core's existing `-assumevalid` mechanism rather than replacing it. |
| Chainstate compaction fix (`src/dbwrapper.{h,cpp}`, `src/txdb.{h,cpp}`, `src/validation.cpp`, `src/leveldb/db/version_set.cc` and its tests) | Disables LevelDB's automatic seek-triggered compaction (which was causing large parts of the chainstate database to be rewritten roughly every hour) and replaces it with a randomized, backgrounded full compaction roughly every two weeks. Adapted from Bitcoin Knots' own backport of upstream Bitcoin Core PR #35465 and leveldb-subtree PR #61 to the 29.x line. |
| `src/qt/splashscreen.cpp` fix | The splash screen's title-rendering code assumed every product name is exactly two space-separated words (e.g. "Bitcoin Core"). Made it fall back to splitting on a hyphen for names like "Bit-Block", instead of crashing with a failed assertion. |
| `src/qt/res/src/bitcoin.svg`, `bitcoinknots-logo.svg` | Rebranded application icon and installer/splash-screen artwork. Also fixes two real, pre-existing bugs uncovered while doing this: (1) the SVGs' declared physical size didn't match what the build's DPI-based icon-scaling math expected, breaking `.ico` generation entirely; (2) the installer header referenced a `#logo` element ID that didn't exist in the logo file, silently rendering nothing. |
| `cmake/module/GenerateSetupNsi.cmake`, `Maintenance.cmake` | Installer output renamed from `bitcoin-win64-setup.exe` to `bit-block-win64-setup.exe`. (The `bitcoin:` URI protocol registration was deliberately left unchanged — it's the BIP-21 payment-link protocol identifier, not branding, and renaming it would break `bitcoin:` link handling.) |
| `BITBLOCK_RELEASE_VERSION` (`CMakeLists.txt`, `cmake/bitcoin-build-config.h.in`, `src/clientversion.cpp`) | Bit-Block's own simple release counter ("V2", "V3", ...), shown everywhere a human-facing version string appears (splash screen, About dialog, `--version` output on every binary) in place of the confusing raw upstream string (e.g. `v29.1.0.knots20250903`). Bumped by hand at each release; independent of `CLIENT_VERSION_MAJOR`/`MINOR`/`BUILD`, which still track the real upstream base internally and are unaffected — the real technical version remains visible via the `subversion` field in the `getnetworkinfo` RPC for support/debugging purposes. |
| `COPYRIGHT_YEAR` fix (`CMakeLists.txt`) | Was hardcoded to `2025`, shown incorrectly on the splash screen's copyright lines. This value isn't purely cosmetic in this codebase — see the next row. |
| Advisory upgrade reminder (`src/clientversion.h`, `clientversion.cpp`, `src/init.cpp`, `src/kernel/warning.h`) | Bitcoin Knots inherited a "software expiry" mechanism from a Bitcoin Core PR (`#10282`) that was proposed and ultimately **closed/rejected upstream** — it refused to start the node, and refused to accept new blocks, roughly two years after `COPYRIGHT_YEAR`. Bit-Block replaced this with a purely advisory version: two years after `COPYRIGHT_YEAR`, a one-time, dismissible message suggests the user consider upgrading (shown as a friendly GUI popup, and as a warning visible via `getnetworkinfo`/`-getinfo` for headless `bitcoind` users). It does **not** affect startup, block validation, or mining in any way — following the suggestion of a reviewer on the original, rejected upstream PR: *"I'd be much happier if this just alarmed and warned the user rather than shutdown the node."* A `-upgradereminder=<timestamp>` debug flag exists for testing this without waiting two real years. |

---

## A note on the build system

This repository contains **two parallel build systems**: CMake (used, and
what every workflow in this repo actually invokes) and a legacy GNU
Autotools setup (`configure.ac`, `Makefile.am`, `autogen.sh`) inherited from
an older version of upstream Bitcoin Core, before it migrated to CMake. The
Autotools files are not wired up to build anything in this fork — if you're
building Bit-Block, `cmake -B build` is the correct starting point, covered
in `doc/build-unix.md`, `doc/build-windows.md`, and `doc/build-osx.md`.

---

*This document is maintained by hand and describes the repository as of the
changes discussed in project chat history. If the structure changes
significantly, this file should be updated to match.*
