import { setupManifest } from '@start9labs/start-sdk'
import { long, short } from './i18n'

export const manifest = setupManifest({
  id: 'bit-block',
  title: 'Bit-Block',
  license: 'MIT',
  packageRepo: 'https://github.com/DimiH2025/bit-block-startos',
  upstreamRepo: 'https://github.com/DimiH2025/Bit-Block',
  marketingUrl: 'https://github.com/DimiH2025/Bit-Block',
  donationUrl: null,
  description: { short, long },
  volumes: ['main'],
  images: {
    node: {
      source: { dockerTag: 'ghcr.io/dimih2025/bit-block-node:V4' },
      arch: ['x86_64', 'aarch64'],
    },
    datum: {
      source: { dockerTag: 'ghcr.io/dimih2025/datum-gateway:V4' },
      arch: ['x86_64', 'aarch64'],
    },
  },
})
