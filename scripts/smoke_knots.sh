#!/bin/bash
set -e

VERSION="29.1.knots20250903"
TARBALL="bitcoin-${VERSION}-x86_64-linux-gnu.tar.gz"
URL="https://bitcoinknots.org/files/29.x/${VERSION}/${TARBALL}"
BIN_DIR="./bin/knots/bin"
CACHE_FILE=".knots_downloaded"

echo "=== Bitcoin Knots Smoke Test ==="

# Check if already downloaded and extracted
if [ -f "$CACHE_FILE" ] && [ -f "$BIN_DIR/bitcoind" ]; then
    echo "Bitcoin Knots binaries already available"
else
    echo "Downloading Bitcoin Knots ${VERSION}..."
    
    # Download the release tarball
    curl -L -o "/tmp/${TARBALL}" "$URL"
    
    # Extract to bin directory
    mkdir -p "$BIN_DIR"
    tar -xzf "/tmp/${TARBALL}" -C "$BIN_DIR" --strip-components=1
    
    # Mark as downloaded
    touch "$CACHE_FILE"
    
    echo "Download and extraction complete"
fi

echo ""
echo "=== Testing Bitcoin Knots Functionality ==="

# Test 1: Help command
echo "1. Testing bitcoind help:"
$BIN_DIR/bitcoind -h | head -10
echo ""

# Test 2: Version info
echo "2. Testing bitcoind version:"
$BIN_DIR/bitcoind -version
echo ""

# Test 3: Invalid argument (should fail gracefully)
echo "3. Testing error handling with invalid argument:"
ERROR_OUTPUT=$($BIN_DIR/bitcoind -fakearg 2>&1 || true)
if echo "$ERROR_OUTPUT" | grep -q "Error parsing command line arguments"; then
    echo "✓ Correctly handled invalid argument"
else
    echo "ERROR: Should have shown error for invalid argument"
    exit 1
fi

echo ""
echo "=== Bitcoin CLI Test ==="
echo "4. Testing bitcoin-cli version:"
$BIN_DIR/bitcoin-cli -version
echo ""

echo "✓ All Bitcoin Knots smoke tests passed!"
echo "Bitcoin Knots is working correctly in Replit environment"