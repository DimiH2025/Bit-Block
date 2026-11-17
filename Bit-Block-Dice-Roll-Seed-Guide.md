# Bit-Block: Creating a Seed Phrase From Dice Rolls

Bit-Block includes an RPC called `mnemonicfromdice`. It turns physical dice rolls into a
BIP-39 seed phrase (12 or 24 words), using the same method as SeedSigner, Coldcard and
Krux: your roll string is hashed with SHA-256 and the result becomes the seed entropy.
A second RPC, `mnemonictoseed`, turns a seed phrase into the BIP-32 master key.

Neither command creates or changes a wallet. They only calculate and print.

---

## 1. Read this first (security)

- The safety of your seed depends entirely on your dice. Bit-Block cannot check that
  they are fair, or that nobody saw the rolls.
- The rolls and the resulting words pass through the node's RPC interface and may be
  recorded in logs and in the console history. If this computer has malware, the seed
  is not safe.
- For real funds, run this on an **offline (air-gapped) computer**, or better, use a
  hardware signing device. Treat this feature as a convenience for people who understand
  those risks.
- **Never** type a seed phrase that already holds funds into any RPC unless the computer
  is offline and trusted.
- Write the words on paper or metal. Do not photograph them, email them or store them in
  cloud notes.

## 2. What you need

- One or more normal six-sided dice (casino-grade dice are best).
- Pen and paper to record each roll as you go.
- Bit-Block V4 (Qt app, or `bitcoind` with `bitcoin-cli`).
- Time. 50 rolls for 12 words, 99 rolls for 24 words.

| Words | Strength | Rolls needed |
|-------|----------|--------------|
| 12    | 128-bit  | exactly 50   |
| 24    | 256-bit  | exactly 99   |

Use 24 words if you want maximum strength. 12 words is considered secure as well.

## 3. Roll the dice

1. Roll one die at a time. Shake it well, then roll it onto a flat surface.
2. After each roll, write down the number on top (1 to 6).
3. Do not skip, re-roll or "fix" a roll because it looks unlucky (for example many 6s in
   a row). That reduces randomness.
4. Count carefully. Wrong length is rejected by the RPC (see section 7).

You will end with a string of digits, for example (shortened here):
`3 1 6 2 4 ...` written without spaces as `31624...`

**Do not use the example strings in this guide for real funds.** Anyone can see them.

## 4. Run the command

### In the Bit-Block app

1. Open **Window → Console**.
2. Type the command with your digits inside quotes, then press Enter:

```
mnemonicfromdice "31624..."
```

The console prints your seed phrase as a line of words.

### From the command line

With the node running:

```
bitcoin-cli mnemonicfromdice "31624..."
```

On Windows run this from the folder that contains `bitcoin-cli.exe`.

Rules for the roll string:

- Digits 1 to 6 only.
- No spaces, commas or other separators.
- Exactly 50 characters (12 words) or exactly 99 characters (24 words).

## 5. Check the result before you trust it

Do this with a test string first, not your real rolls.

Example test (this is public, never use it for funds):

```
mnemonicfromdice "11111111111111111111111111111111111111111111111111"
```

(50 ones) gives:

```
diet glad hat rural panther lawsuit act drop gallery urge where fit
```

And 50 digits of `123456` repeated (`12345612345612345612345612345612345612345612345612`) gives:

```
unveil nice picture region tragic fault cream strike tourist control recipe tourist
```

You can compare a test result with another tool that supports the dice-roll method (a
SeedSigner, or https://iancoleman.io/bip39 saved and opened **offline**). Same rolls
should give the same words. If they differ, do not use the output; tell the Bit-Block
maintainer.

For your real rolls:

1. Run the command twice, to make sure you typed it the same way both times.
2. Write down the words in order, numbered 1, 2, 3, and so on.
3. Check the written copy against the screen word by word.

## 6. Turn the seed phrase into keys (optional)

If you want the BIP-32 master key from a phrase:

```
mnemonictoseed "word1 word2 ... word12"
```

It returns:

- `seed`: the 64-byte BIP-39 seed in hex.
- `master_key`: the extended private key. It starts with `xprv` on mainnet (`tprv` on
  test networks).

Optional arguments:

- A second argument adds a BIP-39 passphrase (the "25th word"). Different passphrase,
  completely different wallet. If you use one, store it separately from the words.
- A third argument `false` accepts phrases with an invalid checksum. Leave it at the
  default (checks on) unless you know why you need it.

This command does **not** put keys into a wallet. To use the key you would import it
yourself with `importdescriptors`, choosing the derivation path (for example
`m/84'/0'/0'` for single-signature native SegWit). If you are not sure how to do that,
load the seed into a hardware wallet or a wallet app that accepts BIP-39 phrases instead.

## 7. Error messages

| Message | Cause |
|---------|-------|
| `dice roll string must be exactly 50 characters (12-word) or 99 characters (24-word)` | Wrong number of rolls |
| `dice roll string must contain only digits 1-6` | A 0, 7, 8, 9, space or letter slipped in |

## 8. After you finish

- Clear the console history and close the console (restart Bit-Block if unsure).
- Destroy the paper with the dice rolls, or store it as securely as the seed itself.
  The rolls are enough to rebuild the seed.
- Send a **small** test amount to the first address of the new wallet, and practise
  restoring from your written words before moving larger amounts.
- Keep at least two copies of the seed phrase in separate secure places.
