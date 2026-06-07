FROM devkitpro/devkita64:20260219

ARG PREMAKE_URL=https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-linux.tar.gz

ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITA64=/opt/devkitpro/devkitA64
ENV PATH=${PATH}:/opt/devkitpro/tools/bin:/opt/devkitpro/devkitA64/bin

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        curl \
        git \
        make \
        python3 \
        tar && \
    rm -rf /var/lib/apt/lists/* && \
    dkp-pacman -Sy --noconfirm switch-dev && \
    curl -L "$PREMAKE_URL" -o /tmp/premake5.tar.gz && \
    tar -xzf /tmp/premake5.tar.gz -C /usr/local/bin premake5 && \
    rm -f /tmp/premake5.tar.gz

WORKDIR /src

CMD ["/bin/bash"]