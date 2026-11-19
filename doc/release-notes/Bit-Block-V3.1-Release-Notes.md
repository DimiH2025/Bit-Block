# Bit-Block V3.1

V3.1 is a security and build-fix release on top of V3. Updating is recommended for everyone, and strongly for anyone who uses `-walletnotify`.

## Security

- **`-walletnotify` command injection fixed.** The `%w` placeholder was filled in by a function that treated the wallet name as a regex replacement pattern. A wallet name containing a sequence such as `$'` could pull extra text into the command line, which is then run by a shell. The replacement is now literal. Only people who use `-walletnotify` and open wallets with untrusted names were exposed.

## New in V3 (carried forward)

- Seven individually switchable anti-spam relay and mining rules, modelled on BIP-110, in Options -> Spam filtering. They are local policy only and do not change consensus. All are on by default.
- Optional DATUM Gateway preset (`-datum` or "Mine with DATUM" in Options) and a rate-limited `templatehint` ZMQ notification.
- Advisory, dismissible upgrade reminder instead of any shutdown behaviour.
- Chainstate compaction changes that stop LevelDB from rewriting large parts of the UTXO database every hour.
- Version shown as "Bit-Block V3.1" on the splash screen, About dialog and `--version`.

## Fixes

- **macOS:** builds with the newest Homebrew Boost (1.91) and packages correctly into `Bit-Block.zip`. The dependency-copy step now handles `@rpath` libraries.
- **Linux:** release build now includes the GUI (`bitcoin-qt`), and the archive name carries the version.
- **Windows:** installer shows "V3.1" in its version fields.

## Notes

- macOS builds are not code-signed or notarized; macOS may ask you to approve the app on first launch.
- The network user agent and the `bitcoin:` payment-link scheme are intentionally unchanged for compatibility.
- Berkeley DB (legacy wallet) support is not included in the macOS build, because Homebrew no longer ships the required version. Descriptor wallets are unaffected.
