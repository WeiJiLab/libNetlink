#-------------------------------------------------------------------------------------------------------------
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See https://go.microsoft.com/fwlink/?linkid=2090316 for license information.
#-------------------------------------------------------------------------------------------------------------

# To fully customize the contents of this image, use the following Dockerfile as a base and add the RUN statement from this file:
# https://github.com/microsoft/vscode-dev-containers/blob/v0.112.0/containers/debian-10-git/.devcontainer/Dockerfile
FROM zionlab/clang-gn-docker

# This Dockerfile's base image has a non-root user with sudo access. Use the "remoteUser"
# property in devcontainer.json to use it. On Linux, the container user's GID/UIDs
# will be updated to match your local UID/GID (when using the dockerFile property).
# See https://aka.ms/vscode-remote/containers/non-root-user for details.
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

ARG INSTALL_ZSH="true"

ARG COMMON_SCRIPT_SOURCE="https://raw.githubusercontent.com/microsoft/vscode-dev-containers/v0.112.0/script-library/common-debian.sh"
ARG COMMON_SCRIPT_SHA="28e3d552a08e0d82935ad7335837f354809bec9856a3e0c2855f17bfe3a19523"
# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils dialog wget cppcheck valgrind git wget tar openssl libssl-dev vim ca-certificates iptables telnet iputils-ping 2>&1 \
    #
    # Verify git, common tools / libs installed, add/modify non-root user, optionally install zsh
    && wget -q -O /tmp/common-setup.sh $COMMON_SCRIPT_SOURCE \
    && if [ "$COMMON_SCRIPT_SHA" != "dev-mode" ]; then echo "$COMMON_SCRIPT_SHA /tmp/common-setup.sh" | sha256sum -c - ; fi \
    && /bin/bash /tmp/common-setup.sh "$INSTALL_ZSH" "$USERNAME" "$USER_UID" "$USER_GID" \
    && rm /tmp/common-setup.sh \
    #
    # Clean up
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/linux-test-project/lcov.git       \
    && cd lcov  \
    && make install

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog
