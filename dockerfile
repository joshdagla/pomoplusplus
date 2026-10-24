FROM debian:bookworm AS build

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
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

# Copy application source
COPY CMakeLists.txt .
COPY src/ src/
COPY include/ include/
COPY lib/ lib/
COPY templates/ templates/
COPY static/ static/
COPY config.json .

# Configure and compile
RUN cmake -S . -B build/Release \
    -DCMAKE_TOOLCHAIN_FILE=/build/Release/generators/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

RUN cmake --build build/Release -j2

FROM debian:bookworm AS runtime

RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libssl3 \
    libstdc++6 \
    libgcc-s1 \
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
