# Bit-Block for StartOS

[Bit-Block](https://github.com/DimiH2025/Bit-Block) is a Bitcoin full node with a GUI and local anti-spam relay policy. This StartOS package runs the node together with the [OCEAN DATUM gateway](https://github.com/OCEAN-xyz/datum_gateway), so a home miner can mine on block templates built by their own node.

This is a test release.

## What you need

- A server running **StartOS 0.4**: a normal PC or mini PC (x86), or a **Raspberry Pi** (ARM).
- A large SSD for the blockchain. The first sync downloads the whole chain and takes days.
- The `.s9pk` file for your hardware:

| Hardware | File |
|----------|------|
| PC or mini PC (Intel/AMD) | `bit-block_x86_64.s9pk` |
| Raspberry Pi | `bit-block_aarch64.s9pk` |

Get the files from the maintainer, or download them from the **Artifacts** section of the "Build StartOS package (.s9pk)" run in this repository's Actions tab (`bit-block-s9pk-x86` and `bit-block-s9pk-arm`). Each artifact is a zip file: unzip it first to get the `.s9pk`.

## Install

1. Open the StartOS web interface of your server in a browser.
2. Go to **Sideload** and upload the `.s9pk` file for your hardware.
3. Click **Install**, then **Start**.
4. Open the service and run the action **Show dashboard password**. Write the password down.
5. Open the **Dashboard** interface and sign in with that password.
6. Run the action **Set payout address** and enter your payout address.

## Raspberry Pi

- Install StartOS on the Pi first, following the instructions on [start9.com](https://start9.com). Use a Pi 4 or 5 with a good SSD, not an SD card, for the blockchain.
- Use the `bit-block_aarch64.s9pk` file.
- Syncing on a Pi is slow. Expect it to take longer than on a PC.

## Mining

Point your miner at:

`stratum+tcp://<your server address>:23334`

Use any worker name. Mining at home is a lottery: one rig has a very small chance of finding a block. With DATUM pooled mining, payouts come from the pool according to its rules.

## Good to know

- The node cannot run pruned, because DATUM needs the transaction index.
- Do not run it together with another Bitcoin node service that uses the same ports.
- If something does not work, send the service **Logs** from StartOS together with a description of what you did: https://github.com/DimiH2025/Bit-Block/issues

## Building the package yourself

Requirements: Docker, Node.js and `start-cli`.

```
npm install
make x86      # or: make arm
```

Or run the GitHub workflow **Build StartOS package (.s9pk)**. The package uses the container images `ghcr.io/dimih2025/bit-block-node` and `ghcr.io/dimih2025/datum-gateway`, which are built by `.github/workflows/docker-images.yml`. For every release, update the image tags in `startos/manifest/index.ts` and the version in `startos/versions/current.ts`.
