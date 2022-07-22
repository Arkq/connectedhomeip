#!/usr/bin/env bash

#
#    Copyright (c) 2021 Project CHIP Authors
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

set -e

#Options
TIZEN_SDK_ROOT=/opt/tizen-sdk
TIZEN_SDK_DATA_PATH=~/tizen-sdk-data
TIZEN_VERSION=6.0

SCRIPT_NAME=$(basename "$(readlink -f "$0")")
SCRIPT_DIR=$(dirname -- "$(readlink -f "${BASH_SOURCE:?}")")

DEPENDENCIES="cpio obs-build openjdk-8-jre-headless zip wget rpm"
TMP_DIR=$(mktemp -d)

SECRET_TOOL=0

if which tput >/dev/null 2>&1 && [[ $(tput -T $TERM colors) -ge 8 ]]; then
    COLOR_NONE="$(tput sgr0)"
    COLOR_RED="$(tput setaf 1)"
    COLOR_GREEN="$(tput setaf 2)"
    COLOR_YELLOW="$(tput setaf 3)"
    COLOR_BLUE="$(tput setaf 4)"
fi

# ------------------------------------------------------------------------------
# Help display function
function show_help() {
    echo "Installation Tizen Help"
    echo "----------------------"
    echo "Usage: $SCRIPT_NAME [ options .. ]"
    echo "Example usage: $SCRIPT_NAME --tizen-sdk-path ~/tizen-sdk --install-dependencies --tizen-version 6.0"
    echo
    echo "Options:
    --help                     Display this information
    --tizen-sdk-path           Set directory where Tizen will be installed. Default is $TIZEN_SDK_ROOT
    --tizen-sdk-data-path      Set directory where Tizen have data. Default is $TIZEN_SDK_DATA_PATH
    --install-dependencies     This options install all dependencies.
    --tizen-version            Select Tizen version. Default is $TIZEN_VERSION
    --override-secret-tool     Without password manager circumvents the requirement of having functional D-Bus Secrets service"
    echo "NOTE:
    The script should run fully with ubuntu 20.0.4 LTS. For ubuntu 22.04 LTS you have to manually
    install all needed dependencies. Use the script specifying --tizen-sdk-path with or
    without --tizen-version. The script will only install the tizen platform for Matter.
    "
}

# ------------------------------------------------------------------------------
# Error print function
function error() {
    echo "${COLOR_RED}[ERROR]: $1${COLOR_NONE}"
}

# ------------------------------------------------------------------------------
# Info print function
function info() {
    echo "${COLOR_GREEN}$1${COLOR_NONE}"
}

# ------------------------------------------------------------------------------
# Warning print function
function warning() {
    echo "${COLOR_YELLOW}[WARNING]: $1${COLOR_NONE}"
}

# ------------------------------------------------------------------------------
# Show dependencies
function show_dependencies() {
    warning "Need dependencies for use this script installation SDK:
        cpio
        unzip
        wget
        unrpm
    "
    warning "Need dependencies for Tizen SDK:
        JAVA JRE >=8.0
    "
}

# ------------------------------------------------------------------------------
# Function helper massive download
# Usage: download "--options_wget url_dir_package" ${package_array[@]}
function download() {
    echo "$COLOR_BLUE"

    for package in "${@:2}" arr; do
        if [[ "$1" =~ .*"--no-parent".* ]]; then
            wget $1 --progress=dot:mega -A $package
        else
            wget --progress=dot:giga $1$package
        fi
    done

    echo -n "$COLOR_NONE"
}

# ------------------------------------------------------------------------------
# Function install all dependencies.
function install_dependencies() {
    if ! command -v apt &>/dev/null; then
        show_dependencies
        error "Cannot install dependencies script need apt package manager. Install dependencies manually"
        return 1
    fi

    info "Installation dependencies"
    apt-get update
    DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends $DEPENDENCIES || return

}

# ------------------------------------------------------------------------------
# Function install tizen sdk.
function install_tizen_sdk() {
    if [[ ! -d $TIZEN_SDK_ROOT ]]; then
        mkdir -p "$TIZEN_SDK_ROOT" || return
    fi

    echo "Installation Tizen SDK directory: $TIZEN_SDK_ROOT"
    TIZEN_SDK_SYSROOT="$TIZEN_SDK_ROOT/platforms/tizen-$TIZEN_VERSION/mobile/rootstraps/mobile-$TIZEN_VERSION-device.core"

    # Get tizen studio CLI
    info "Get tizen studio CLI [...] create tmp directory $TMP_DIR"
    cd "$TMP_DIR" || return

    # Download
    url="http://download.tizen.org/sdk/tizenstudio/official/binary/"
    pkg_arr=(
        'certificate-encryptor_1.0.7_ubuntu-64.zip'
        'certificate-generator_0.1.3_ubuntu-64.zip'
        'new-common-cli_2.5.7_ubuntu-64.zip'
        'new-native-cli_2.5.7_ubuntu-64.zip'
        'sdb_4.2.23_ubuntu-64.zip')
    download "$url" "${pkg_arr[@]}"

    # Install tizen studio CLI
    unzip -o '*.zip'
    cp -rf data/* $TIZEN_SDK_ROOT

    echo "TIZEN_SDK_INSTALLED_PATH=$TIZEN_SDK_ROOT" >$TIZEN_SDK_ROOT/sdk.info
    echo "TIZEN_SDK_DATA_PATH=$TIZEN_SDK_DATA_PATH" >>$TIZEN_SDK_ROOT/sdk.info
    ln -sf $TIZEN_SDK_DATA_PATH/.tizen-cli-config $TIZEN_SDK_ROOT/tools/.tizen-cli-config

    # Cleanup
    rm -rf "${TMP_DIR:?}/"*

    # Install secret tool or not
    if [[ $SECRET_TOOL == 1 ]]; then
        cp "$SCRIPT_DIR/secret-tool.py" "$TIZEN_SDK_ROOT/tools/certificate-encryptor/secret-tool"
        chmod 0755 $TIZEN_SDK_ROOT/tools/certificate-encryptor/secret-tool
    fi

    # Get toolchain
    info "Get toolchain"

    # Download
    url="http://download.tizen.org/sdk/tizenstudio/official/binary/"
    pkg_arr=(
        "cross-arm-gcc-9.2_0.1.9_ubuntu-64.zip"
        "sbi-toolchain-gcc-9.2.cpp.app_2.2.16_ubuntu-64.zip")
    download "$url" "${pkg_arr[@]}" || return

    # Install toolchain
    unzip -o '*.zip'
    cp -rf data/* $TIZEN_SDK_ROOT

    # Cleanup
    rm -rf "${TMP_DIR:?}"/*

    # Get tizen sysroot
    info "Get tizen sysroot"

    # Base sysroot
    url="http://download.tizen.org/sdk/tizenstudio/official/binary/"
    pkg_arr=(
        "mobile-$TIZEN_VERSION-core-add-ons_0.0.262_ubuntu-64.zip"
        "mobile-$TIZEN_VERSION-rs-device.core_0.0.123_ubuntu-64.zip")
    download "$url" "${pkg_arr[@]}"

    # Base packages
    url="http://download.tizen.org/releases/milestone/tizen/base/latest/repos/standard/packages/armv7l/"
    pkg_arr=(
        'iniparser-*.armv7l.rpm'
        'libblkid-devel-*.armv7l.rpm'
        'libcap-*.armv7l.rpm'
        'libffi-devel-*.armv7l.rpm'
        'liblzma-*.armv7l.rpm'
        'libmount-devel-*.armv7l.rpm'
        'libncurses6-*.armv7l.rpm'
        'libreadline-*.armv7l.rpm'
        'libuuid-*.armv7l.rpm'
        'pcre-devel-*.armv7l.rpm'
        'readline-devel-*.armv7l.rpm'
        'xdgmime-*.armv7l.rpm')
    download "-r -nd --no-parent $url" "${pkg_arr[@]}"

    # Unified packages
    url="  http://download.tizen.org/releases/milestone/tizen/unified/latest/repos/standard/packages/armv7l/"
    pkg_arr=(
        'aul-0*.armv7l.rpm'
        'aul-devel-*.armv7l.rpm'
        'bundle-0*.armv7l.rpm'
        'bundle-devel-*.armv7l.rpm'
        'buxton2-*.armv7l.rpm'
        'cynara-devel-*.armv7l.rpm'
        'dbus-1*.armv7l.rpm'
        'dbus-devel-*.armv7l.rpm'
        'dbus-libs-1*.armv7l.rpm'
        'glib2-devel-2*.armv7l.rpm'
        'json-glib-devel-*.armv7l.rpm'
        'libcynara-client-*.armv7l.rpm'
        'libcynara-commons-*.armv7l.rpm'
        'libdns_sd-*.armv7l.rpm'
        'libjson-glib-*.armv7l.rpm'
        'libsessiond-0*.armv7l.rpm'
        'libsystemd-*.armv7l.rpm'
        'libtzplatform-config-*.armv7l.rpm'
        'parcel-0*.armv7l.rpm'
        'parcel-devel-*.armv7l.rpm'
        'pkgmgr-info-*.armv7l.rpm'
        'vconf-compat-*.armv7l.rpm'
        'vconf-internal-keys-devel-*.armv7l.rpm')
    download "-r -nd --no-parent $url" "${pkg_arr[@]}"

    # Unified packages (snapshots)
    url="http://download.tizen.org/snapshots/tizen/unified/latest/repos/standard/packages/armv7l/"
    pkg_arr=(
        'capi-network-nsd-*.armv7l.rpm'
        'capi-network-thread-*.armv7l.rpm'
        'libnsd-dns-sd-*.armv7l.rpm')
    download "-r -nd --no-parent $url" "${pkg_arr[@]}"

    # Install base sysroot
    unzip -o '*.zip'
    cp -rf data/* $TIZEN_SDK_ROOT

    # Install packages
    unrpm *.rpm
    cp -rf lib usr $TIZEN_SDK_SYSROOT

    # Make symbolic links relative TODO:Test if it's working
    find "$TIZEN_SDK_SYSROOT"/usr/lib -maxdepth 1 -type l | while IFS= read -r -d '' pkg; do
        ln -sf "$(basename "$(readlink "$pkg")")" "$pkg"
    done
    ln -sf ../../lib/libcap.so.2 $TIZEN_SDK_SYSROOT/usr/lib/libcap.so
    ln -sf openssl1.1.pc $TIZEN_SDK_SYSROOT/usr/lib/pkgconfig/openssl.pc

    # Cleanup remove tmp directory
    rm -rf "${TMP_DIR:?}"

    warning "You must add the appropriate environment variables before proceeding with matter."
    echo "${COLOR_YELLOW}"
    echo "export TIZEN_VESRSION=\"$TIZEN_VERSION\""
    echo "export TIZEN_SDK_ROOT=\"$(realpath $TIZEN_SDK_ROOT)\""
    echo "export TIZEN_SDK_TOOLCHAIN=\$TIZEN_SDK_ROOT/tools/arm-linux-gnueabi-gcc-9.2\""
    echo "export TIZEN_SDK_SYSROOT=\"\$TIZEN_SDK_ROOT/platforms/tizen-6.0/mobile/rootstraps/mobile-6.0-device.core\""
    echo "export PATH=\"\$TIZEN_SDK_ROOT/tools/ide/bin:\$TIZEN_SDK_ROOT/tools:\$PATH\""
    echo -n "${COLOR_NONE}"

}

# Check if the script is run with elevated privileges
if [[ "$(id -u)" -eq 0 ]]; then
    if [[ -n "$SUDO_USER" ]]; then
        warning "Script running as sudo user"
    else
        warning "Script running as root"
    fi
fi

while (($#)); do
    case $1 in
    --help)
        show_help
        exit 0
        ;;
    --tizen-sdk-path)
        TIZEN_SDK_ROOT="$2"
        shift
        ;;
    --tizen-sdk-data-path)
        TIZEN_SDK_DATA_PATH="$2"
        shift
        ;;
    --tizen-version)
        TIZEN_VERSION=$2
        shift
        ;;
    --install-dependencies)
        dependencies='true'
        ;;
    --override-secret-tool)
        SECRET_TOOL=1
        ;;
    *)
        error "Wrong options usage!"
        exit 1
        ;;
    esac
    shift
done

# ------------------------------------------------------------------------------
# Checks if the selected version is available.
url="http://download.tizen.org/sdk/tizenstudio/official/binary/mobile-$TIZEN_VERSION-core-add-ons_0.0.262_ubuntu-64.zip"
if ! wget --quiet --spider "$url"; then
    error "Tizen version: $TIZEN_VERSION not exist"
    exit 1
fi
echo "Tizen version: $TIZEN_VERSION"

# ------------------------------------------------------------------------------
# Checks if the user need install dependencies
if [[ $dependencies == 'true' ]]; then
    if ! install_dependencies; then
        error "Cannot install dependencies, please use this script as sudo user or root. Use --help"
        show_dependencies
        exit 1
    fi
else
    show_dependencies
fi

# ------------------------------------------------------------------------------
# Checking dependencies needed to install the tizen platform
for pkg in 'unzip' 'wget' 'unrpm'; do
    if ! command -v $pkg &>/dev/null; then
        warning "Not found $pkg"
        dep_lost=1
    fi
done
if [[ $dep_lost ]]; then
    echo "[HINT]: On Ubuntu-like distro run: sudo apt install $DEPENDENCIES"
    error "You need install dependencies before"
    exit 1
fi

# ------------------------------------------------------------------------------
# Installation Tizen SDK
if ! install_tizen_sdk; then
    rm -rf "${TMP_DIR:?}"
    exit 1
fi
