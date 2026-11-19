# Bit-Block for StartOS

StartOS package for [Bit-Block](https://github.com/DimiH2025/Bit-Block): the node plus the OCEAN DATUM gateway.

It uses two container images built by the Bit-Block repository (`.github/workflows/docker-images.yml`):
`ghcr.io/dimih2025/bit-block-node` and `ghcr.io/dimih2025/datum-gateway`.

## Build

Requirements: Docker, Node.js, `start-cli`.

```
npm install
make x86      # or: make arm
```

Install the resulting `.s9pk` through **Sideload** in the StartOS web interface.

Bump the image tags in `startos/manifest/index.ts` and the version in `startos/versions/current.ts` for every release.
