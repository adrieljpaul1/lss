#!/bin/bash

echo "Starting safe cleanup of non-essential packages..."

# Ensure package lists are updated
sudo apt update

# Define essential packages for system and networking
ESSENTIALS=(
    "bash" "sudo" "coreutils" "systemd" "raspberrypi-bootloader"
    "raspberrypi-kernel" "raspberrypi-sys-mods" "dhcpcd5"
    "net-tools" "openssh-server" "ca-certificates" "iptables"
    "nano" "vim" "wget" "curl" "tar" "gzip" "wpa_supplicant"
    "rfkill" "libc6" "libstdc++6" "apt" "iproute2" "openssh-client"
)

# List installed packages
INSTALLED_PACKAGES=$(dpkg --get-selections | awk '{print $1}')

# Filter and remove non-essential packages
for pkg in $INSTALLED_PACKAGES; do
    if [[ ! " ${ESSENTIALS[@]} " =~ " $pkg " ]]; then
        echo "Removing $pkg..."
        sudo apt remove --purge -y "$pkg"
    fi
done

# Auto-remove dependencies that are no longer needed
sudo apt autoremove -y
sudo apt clean

echo "Safe cleanup complete! Reboot recommended."