# Overview

Bitcoin Knots is a variant of Bitcoin Core that provides enhanced features and capabilities while maintaining compatibility with the Bitcoin network. This repository contains the complete Bitcoin node implementation including the core daemon (bitcoind), command-line utilities, and GUI client (bitcoin-qt). The project is built using CMake and supports cross-platform development across Windows, macOS, and various Unix-like systems.

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