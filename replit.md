# Overview

Bit-Block is a variant of Bitcoin Core that provides enhanced features and capabilities while maintaining compatibility with the Bitcoin network. This repository contains the complete Bitcoin node implementation including the core daemon (bitcoind), command-line utilities, and GUI client (bitcoin-qt). The project is built using CMake and supports cross-platform development across Windows, macOS, and various Unix-like systems.

# User Preferences

Preferred communication style: Simple, everyday language.

# System Architecture

## Build System
- **CMake-based build system** with support for multiple presets and configurations
- **vcpkg package manager integration** for Windows builds with both dynamic and static linking options
- **Cross-platform compilation support** using depends system for deterministic builds across different architectures
- **Multiple build configurations** including debug, release, and specialized fuzzing builds with sanitizers

## Core Components
- **Bitcoin daemon (bitcoind)** - Headless node that validates blocks and transactions
- **GUI client (bitcoin-qt)** - Qt-based graphical user interface with wallet functionality
- **Command-line utilities** - Tools for interacting with the Bitcoin network and managing wallets
- **Networking layer** - P2P protocol implementation with support for IPv4/IPv6, Tor, I2P, and CJDNS

## Wallet Architecture
- **Descriptor-based wallets** - Modern wallet format using output descriptors for improved flexibility
- **Legacy wallet support** - Backward compatibility with Berkeley DB-based wallets
- **SQLite database backend** - Default storage for descriptor wallets
- **HD wallet implementation** - Hierarchical deterministic key generation following BIP32/44/49/84/86
- **Multisig capabilities** - Support for multi-signature transactions and wallet setups
- **PSBT support** - Partially Signed Bitcoin Transactions for offline signing workflows

## Testing Framework
- **Comprehensive test suite** including unit tests, functional tests, and fuzz testing
- **Continuous Integration** - Automated testing across multiple platforms and configurations
- **Benchmarking tools** - Performance testing for cryptographic operations and system components
- **libFuzzer integration** - Automated fuzz testing for security-critical components

## Security Features
- **Wallet encryption** - AES encryption for private keys with user-defined passphrases
- **External signing support** - Hardware wallet integration through external signer interface
- **Address validation** - Built-in address verification and display capabilities
- **Sandboxed architecture** - Process isolation for enhanced security (multiprocess mode)

# External Dependencies

## Core Dependencies
- **Boost libraries** - C++ utility libraries for multi-index containers, signals, and system operations
- **libevent** - Event notification library for asynchronous I/O operations
- **CMake 3.22+** - Build system generator and configuration management

## Optional Dependencies
- **Qt 5.11+** - Cross-platform GUI framework for the bitcoin-qt client
- **Berkeley DB 4.8** - Database backend for legacy wallet support
- **SQLite 3.7+** - Default database backend for descriptor wallets
- **ZeroMQ** - Message queue library for blockchain notifications
- **MiniUPnPc** - Universal Plug and Play client for automatic port forwarding
- **libqrencode** - QR code generation library for payment addresses

## Development and Testing
- **Python 3.10+** - Required for functional test suite and utility scripts
- **Doxygen** - Documentation generation tool for API documentation
- **Clang/GCC** - Modern C++ compilers with C++17 support
- **systemtap** - Dynamic tracing framework for system analysis

## Network and Privacy
- **Tor proxy support** - Anonymous networking through The Onion Router
- **I2P integration** - Privacy network support through Invisible Internet Project
- **CJDNS compatibility** - Encrypted IPv6 mesh network support

# Building from Source (Replit Environment)

## Compilation Instructions

To compile Bit-Block executables from source in the Replit environment:

### 1. Install Dependencies
```bash
# Dependencies are managed via Nix and are already installed:
# - CMake 3.31.6
# - GCC 14.2.1
# - pkg-config, libevent, boost, sqlite
```

### 2. Configure Build
```bash
# Clean and configure CMake with proper compiler and library paths
rm -rf build
cmake -B build \
  -DCMAKE_C_COMPILER=/nix/store/a0d7m3zn9p2dfa1h7ag9h2wzzr2w25sn-gcc-wrapper-14.2.1.20250322/bin/gcc \
  -DCMAKE_CXX_COMPILER=/nix/store/a0d7m3zn9p2dfa1h7ag9h2wzzr2w25sn-gcc-wrapper-14.2.1.20250322/bin/g++ \
  -DCMAKE_EXE_LINKER_FLAGS="-L/nix/store/bmi5znnqk4kg2grkrhk6py0irc8phf6l-gcc-14.2.1.20250322-lib/lib -latomic" \
  -DBoost_ROOT=/nix/store/hp8fsx1sg3dadia1i092188ll9d216sg-boost-1.87.0-dev \
  -DENABLE_WALLET=OFF \
  -DBUILD_GUI=OFF \
  -DBUILD_TESTS=OFF \
  -DBUILD_BENCH=OFF \
  -DCMAKE_BUILD_TYPE=Release
```

### 3. Compile
```bash
# Build with git operations disabled (required in Replit environment)
export BITCOIN_GENBUILD_NO_GIT=1
cmake --build build -j6
```

### 4. Verify Build
```bash
# Test the compiled executables
./build/bin/bitcoind --version
./build/bin/bitcoin-cli --version
```

## Build Output

Compiled executables are located in `build/bin/`:
- **bitcoind** (~13MB) - Bit-Block daemon
- **bitcoin-cli** (~1.3MB) - Command-line RPC client

Both executables display proper Bit-Block branding:
```
Bit-Block daemon version v29.1.0.knots20250903
Copyright (C) 2009-2025 The Bit-Block developers
```

## Important Notes

- **BITCOIN_GENBUILD_NO_GIT=1** is required to bypass git operations that are blocked by Replit's security measures
- Build time: approximately 30-60 minutes on a 6-core system
- The build disables wallet, GUI, tests, and benchmarks for faster compilation
- Nix store paths may vary; adjust compiler and library paths as needed