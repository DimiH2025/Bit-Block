#!/bin/bash
set -euo pipefail

# Bit-block Security-Hardened Startup Script
# This script provides secure deployment configuration

# RPC user setting
RPC_USER="${BITCOIN_RPC_USER:-bitblock}"

log() {
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $*" >&2
}

generate_secure_password() {
    # Generate a cryptographically secure 32-character password
    if command -v openssl >/dev/null 2>&1; then
        openssl rand -base64 32 | tr -d "=+/" | cut -c1-32
    elif command -v head >/dev/null 2>&1 && [ -c /dev/urandom ]; then
        head -c 24 /dev/urandom | base64 | tr -d "=+/" | cut -c1-32
    else
        # Fallback to pseudo-random (less secure but better than hardcoded)
        date +%s | sha256sum | base64 | head -c 32
    fi
}

setup_secure_config() {
    log "Setting up secure Bit-block configuration..."
    
    # Ensure data directory exists with proper permissions
    mkdir -p "$DATADIR"
    chmod 700 "$DATADIR"
    
    # Generate secure RPC password if config doesn't exist
    if [ ! -f "$CONFIG_FILE" ]; then
        RPC_PASSWORD=$(generate_secure_password)
        
        log "Creating secure bitcoin.conf configuration..."
        
        cat > "$CONFIG_FILE" << EOF
# Bit-block Security-Hardened Configuration
# Generated on $(date)

# Network settings
regtest=1
server=1
listen=0
dnsseed=0
upnp=0
natpmp=0

# RPC Security
rpcuser=$RPC_USER
rpcpassword=$RPC_PASSWORD
rpcbind=127.0.0.1
rpcallowip=127.0.0.1
rpcserialversion=1

# Network restrictions
bind=127.0.0.1
whitelist=127.0.0.1

# Performance and limits
maxconnections=8
maxuploadtarget=100

# Logging
printtoconsole=1
shrinkdebugfile=1

# Fees
fallbackfee=0.001

# Transaction policy
datacarriersize=0

# Security
disablewallet=0
EOF
        
        chmod 600 "$CONFIG_FILE"
        
        log "✓ Secure configuration created"
        log "RPC User: $RPC_USER"
        log "RPC Password: [REDACTED - check $CONFIG_FILE for password]"
        
        # Save credentials to secure file for reference
        echo "RPC_USER=$RPC_USER" > "$DATADIR/.rpc_credentials"
        echo "RPC_PASSWORD=$RPC_PASSWORD" >> "$DATADIR/.rpc_credentials"
        chmod 600 "$DATADIR/.rpc_credentials"
        
    else
        log "Using existing configuration at $CONFIG_FILE"
        # Read existing credentials
        if [ -f "$DATADIR/.rpc_credentials" ]; then
            source "$DATADIR/.rpc_credentials"
            log "RPC User: $RPC_USER"
            log "RPC Password: [REDACTED]"
        fi
        
        # Ensure datacarriersize=0 is present in existing config
        if ! grep -q "^datacarriersize=0$" "$CONFIG_FILE"; then
            log "Adding datacarriersize=0 to existing configuration..."
            echo "" >> "$CONFIG_FILE"
            echo "# Transaction policy (added by secure startup)" >> "$CONFIG_FILE"
            echo "datacarriersize=0" >> "$CONFIG_FILE"
        fi
    fi
}

verify_security() {
    log "Verifying security configuration..."
    
    # Check file permissions
    if [ "$(stat -c %a "$DATADIR")" != "700" ]; then
        log "WARNING: Data directory permissions not secure"
    fi
    
    if [ -f "$CONFIG_FILE" ] && [ "$(stat -c %a "$CONFIG_FILE")" != "600" ]; then
        log "WARNING: Config file permissions not secure"
    fi
    
    # Verify RPC binding is restricted
    if grep -q "rpcbind=127.0.0.1" "$CONFIG_FILE" && grep -q "rpcallowip=127.0.0.1" "$CONFIG_FILE"; then
        log "✓ RPC properly restricted to localhost"
    else
        log "WARNING: RPC binding may not be secure"
    fi
    
    # Verify datacarriersize=0 is present
    if grep -q "^datacarriersize=0$" "$CONFIG_FILE"; then
        log "✓ datacarriersize=0 policy is active"
    else
        log "ERROR: datacarriersize=0 policy missing from configuration"
        exit 1
    fi
    
    log "✓ Security verification completed"
}

start_bitcoind() {
    local BITCOIND_PATH="$(pwd)/bin/bit-block/bin/bitcoind"
    
    if [ ! -x "$BITCOIND_PATH" ]; then
        log "ERROR: bitcoind not found or not executable at $BITCOIND_PATH"
        exit 1
    fi
    
    log "Starting Bit-block daemon with secure configuration..."
    
    # Start with configuration file instead of command line arguments
    exec "$BITCOIND_PATH" -conf="$CONFIG_FILE" -datadir="$DATADIR"
}

main() {
    log "=== Bit-block Security-Hardened Startup ==="
    
    # Ensure we're in the correct directory
    ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
    cd "$ROOT_DIR"
    
    # Set data directory paths after establishing correct working directory
    DATADIR="${BITCOIN_DATADIR:-$ROOT_DIR/.bitcoin-regtest}"
    CONFIG_FILE="$DATADIR/bitcoin.conf"
    log "Configuration will be created at: $DATADIR"
    
    # Run the smoke test first to ensure binaries are available  
    SCRIPT_DIR="$ROOT_DIR/scripts"
    if ! "$SCRIPT_DIR/smoke_bit-block.sh"; then
        log "ERROR: Bit-block smoke test failed"
        exit 1
    fi
    
    # Setup secure configuration
    setup_secure_config
    
    # Verify security settings
    verify_security
    
    # Start the daemon
    start_bitcoind
}

# Execute main function
main "$@"