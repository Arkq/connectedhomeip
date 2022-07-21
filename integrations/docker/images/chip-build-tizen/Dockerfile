ARG VERSION=latest
FROM connectedhomeip/chip-build:${VERSION}

# ------------------------------------------------------------------------------
# Add group/user for tizen
ARG USER_NAME=tizen
ARG USER_UID=1000
ARG USER_GID=$USER_UID
ENV USER_HOME /home/$USER_NAME

RUN set -x \
    && groupadd -g $USER_GID $USER_NAME \
    && useradd -m $USER_NAME -s /bin/bash -u $USER_UID -g $USER_GID -G sudo -l \
    && : # last line

# ------------------------------------------------------------------------------
# Install dependencies
RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends \
    cpio \
    obs-build \
    openjdk-8-jre-headless \
    zip \
    # Cleanup
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && : # last line

# ------------------------------------------------------------------------------
# Set environment
ENV TIZEN_SDK_ROOT /opt/tizen-sdk
ENV TIZEN_VERSION 6.0
ENV PATH="$TIZEN_SDK_ROOT/tools/ide/bin:$TIZEN_SDK_ROOT/tools:$PATH"
ENV TIZEN_SDK_TOOLCHAIN $TIZEN_SDK_ROOT/tools/arm-linux-gnueabi-gcc-9.2
ENV PATH="$TIZEN_SDK_TOOLCHAIN/bin:$PATH"
ENV TIZEN_SDK_SYSROOT \
    $TIZEN_SDK_ROOT/platforms/tizen-$TIZEN_VERSION/mobile/rootstraps/mobile-$TIZEN_VERSION-device.core

# ------------------------------------------------------------------------------
# Build tizen
COPY install_tizen_sdk.sh /user/bin/
RUN  install_tizen_sdk.sh --tizen-sdk-path $TIZEN_SDK_ROOT --tizen-sdk-data-path $USER_HOME/tizen-sdk-data
COPY secret-tool.py $TIZEN_SDK_ROOT/tools/certificate-encryptor/secret-tool

# ------------------------------------------------------------------------------
# Switch to the non-root user
USER $USER_NAME
WORKDIR $USER_HOME
