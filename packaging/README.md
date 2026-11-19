# Bit-Block appliance packages (Umbrel and StartOS)

Bit-Block node plus the OCEAN DATUM gateway, packaged for home servers.

```
packaging/
  docker/bit-block-node/   node image (headless bitcoind, DATUM preset, generated RPC credentials)
  docker/datum-gateway/    gateway image (pinned upstream tag, config rendered at start)
  umbrel/                  Umbrel community app store (goes in its own repository)
  start9/                  StartOS package (goes in its own repository)
.github/workflows/docker-images.yml   builds and publishes both images for amd64 and arm64
```

## How the pieces fit

- The node container writes its RPC login to a small shared folder. The gateway container reads it from there. No password is typed or stored in a command line.
- The gateway runs with its dashboard editing enabled, so the payout address can be entered in the dashboard (Umbrel) or through an action (StartOS).
- Only the stratum port (23334) and the peer port (8333) are published. RPC stays inside the app.

## Release steps

1. Publish the Bit-Block release (for example `V4`).
2. Run the workflow **Container images (Umbrel / StartOS)** with the same tag. In the repository settings, make the two packages public (Packages, then Package settings, then Change visibility), or the home servers cannot pull them.
3. **Umbrel:** create a repository named for example `bitblock-umbrel-store`. Put the contents of `packaging/umbrel/` at its root. In umbrelOS, open App Store, Community App Stores, and add the repository URL.
4. **StartOS:** create a repository `bit-block-startos` from `packaging/start9/`, run `npm install` and `make x86` (or `make arm`), then sideload the `.s9pk`.
5. For each new Bit-Block release, change the image tag in the Umbrel `docker-compose.yml` and the StartOS `manifest`, and bump the app versions.

## Status

Written and checked as far as the sandbox allowed:

- Shell scripts: syntax checked; the gateway config rendering was run and keeps a saved payout address across restarts.
- DATUM gateway `v0.5.1beta` built from source and started with the rendered config: it serves its dashboard with the placeholder address.
- StartOS package: type-checked against `@start9labs/start-sdk` 4.0.0.
- Not tested: building the Docker images, the multi-arch workflow, installing on a real Umbrel or StartOS, and an actual mining session. Expect small fixes on the first real install.

Points to verify on a real Umbrel: that `deterministicPassword` provides `APP_PASSWORD` as assumed, that the shipped `data/` folders end up writable by user 1000, and that publishing port 8333 does not clash with other apps.
