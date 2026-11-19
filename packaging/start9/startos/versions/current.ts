import { IMPOSSIBLE, VersionInfo } from '@start9labs/start-sdk'

export const current = VersionInfo.of({
  version: '4.0.0:0',
  releaseNotes: {
    en_US: 'First StartOS release of Bit-Block, based on Bit-Block V4, with the DATUM gateway.',
  },
  migrations: {
    up: async ({ effects }) => {},
    down: IMPOSSIBLE,
  },
})
