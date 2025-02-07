#!/bin/bash

# Essential packages to keep
ESSENTIAL_PACKAGES=(
    apt
    systemd
    systemd-sysv
    systemd-timesyncd
    udev
    net-tools
    iproute2
    iputils-ping
    dnsutils
    curl
    wget
    openssh-server
    nano
    bash
    coreutils
    findutils
    grep
    sed
    awk
    tar
    gzip
)

# Convert array to space-separated string for grep
ESSENTIAL_REGEX=$(printf "|%s" "${ESSENTIAL_PACKAGES[@]}")
ESSENTIAL_REGEX=${ESSENTIAL_REGEX:1} # Remove leading |

echo "Identifying installed packages..."
INSTALLED_PACKAGES=$(dpkg --get-selections | awk '{print $1}')

echo "Filtering out essential packages..."
TO_REMOVE=()
for pkg in $INSTALLED_PACKAGES; do
    if ! echo "$pkg" | grep -qE "$ESSENTIAL_REGEX"; then
        TO_REMOVE+=("$pkg")
    fi
done

# Confirm before removal
echo "Packages to be removed:"
printf "%s\n" "${TO_REMOVE[@]}"
echo "⚠️ WARNING: This will remove a LOT of packages, including GUI and non-essential tools."
read -p "Proceed? (y/N): " confirm
if [[ "$confirm" != "y" ]]; then
    echo "Aborted."
    exit 1
fi

# Remove unwanted packages
echo "Removing unnecessary packages..."
sudo apt purge --auto-remove -y "${TO_REMOVE[@]}"

# Clean up
echo "Cleaning up..."
sudo apt autoremove -y
sudo apt clean

echo "✅ System cleanup complete! Reboot recommended."

