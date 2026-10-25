
# Stage 1: Build
FROM debian:trixie AS build

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    make \
    python3 \
    python3-pip \
    libssl-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --break-system-packages conan

WORKDIR /app

# Install Conan dependencies
COPY conanfile.txt .

RUN conan profile detect --force && \
    conan install . \
      --output-folder=build \
      --build=missing \
      -s build_type=Release

# Copy application files
COPY CMakeLists.txt .
COPY src/ src/
COPY include/ include/
COPY lib/ lib/
COPY templates/ templates/
COPY static/ static/
COPY config.json .

# Configure CMake
RUN find /app/build -name conan_toolchain.cmake -print

RUN cmake -S . -B build/Release \
    -DCMAKE_TOOLCHAIN_FILE=/app/build/build/Release/generators/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

# Compile
RUN cmake --build build/Release -j2


# Stage 2: Runtime
FROM debian:trixie AS runtime

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libgcc-s1 \
    libsqlite3-0 \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /app/build/Release/PomoPlusPlus .
COPY --from=build /app/templates/ templates/
COPY --from=build /app/static/ static/
COPY --from=build /app/config.json .
COPY --from=build /app/lib/ lib/

ENV LD_LIBRARY_PATH=/app/lib

RUN mkdir -p /data

EXPOSE 18080

CMD ["/app/PomoPlusPlus"]
