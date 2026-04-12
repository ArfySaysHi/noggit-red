# syntax=docker/dockerfile:1
# Use Ubuntu 24.04 as base
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build pkg-config git curl unzip \
    gcc-13 \
    g++-13 \
    python3 \
    libgl1-mesa-dev libglu1-mesa-dev libglew-dev \
    libx11-dev libxi-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libxxf86vm-dev \
    libwayland-dev libxkbcommon-dev wayland-protocols \
    libglm-dev \
    libfreetype6-dev \
    libbz2-dev zlib1g-dev \
    liblua5.3-dev \
    libboost-all-dev \
    qtbase5-dev qtbase5-dev-tools qtbase5-private-dev \
    qttools5-dev qttools5-dev-tools \
    qtdeclarative5-dev qtquickcontrols2-5-dev \
    qtmultimedia5-dev \
    libqt5svg5-dev libqt5x11extras5-dev \
    libqt5opengl5-dev \
    && rm -rf /var/lib/apt/lists/*

# Fix Qt5 headers: Ubuntu installs them without .h extension, source uses <QtWidgets/QFoo.h>
RUN find /usr/include/x86_64-linux-gnu/qt5 -maxdepth 2 -name 'Q*' ! -name '*.*' \
    | while read f; do ln -sf "$f" "${f}.h"; done

WORKDIR /tmp/noggit-red

RUN git clone https://github.com/ladislav-zezula/StormLib.git StormLib && \
    cmake -S StormLib -B StormLib/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON && \
    cmake --build StormLib/build -j$(nproc) && \
    cmake --install StormLib/build && \
    mkdir /tmp/bz2obj && cd /tmp/bz2obj && \
    ar x /usr/lib/x86_64-linux-gnu/libbz2.a && \
    ar qf /usr/local/lib/libstorm.a /tmp/bz2obj/*.o && \
    rm -rf /tmp/bz2obj

RUN git clone https://github.com/ladislav-zezula/CascLib.git CascLib && \
    cmake -S CascLib -B CascLib/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON && \
    cmake --build CascLib/build -j$(nproc) && \
    cmake --install CascLib/build

RUN git clone https://github.com/Auburn/FastNoise2.git FastNoise2 && \
    cmake -S FastNoise2 -B FastNoise2/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DFASTNOISE2_NOISETOOL=OFF && \
    cmake --build FastNoise2/build -j$(nproc) && \
    cmake --install FastNoise2/build

COPY . .

# RUN git submodule update --init --recursive 2>/dev/null || true

RUN cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=gcc-13 \
        -DCMAKE_CXX_COMPILER=g++-13 \
        -DCMAKE_CXX_FLAGS="-Wno-error -Wno-deprecated -Wno-deprecated-enum-enum-conversion -Wno-unknown-warning-option" \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DNOGGIT_OPENGL_ERROR_CHECK=OFF \
        -DNOGGIT_ENABLE_TRACY_PROFILER=OFF \
        -DFASTNOISE2_NOISETOOL=OFF \
        -DCMAKE_PREFIX_PATH="/usr/local;/usr/lib/x86_64-linux-gnu/cmake/Qt5" \
        -DBoost_NO_BOOST_CMAKE=ON && \
    bash -c 'cmake --build build -j$(nproc) 2>&1 | tee /tmp/build.log; \
             ret=${PIPESTATUS[0]}; \
             grep -E " error:" /tmp/build.log | grep -v "warning:" | head -40; \
             exit $ret'
