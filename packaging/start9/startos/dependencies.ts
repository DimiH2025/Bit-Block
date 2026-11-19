import { sdk } from './sdk'

// Bit-Block bundles its own node, so it depends on no other service.
export const dependencies = sdk.Dependencies.of()
