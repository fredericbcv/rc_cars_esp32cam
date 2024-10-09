#!/usr/bin/sh

# Download & Install Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source ~/.cargo/env

# Clone repo
git clone --depth 1 https://github.com/G2-Games/sixaxis-pair_rs.git
cd sixaxis-pair_rs

# Build
cargo build
