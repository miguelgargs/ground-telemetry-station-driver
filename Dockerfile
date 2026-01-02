# Build stage
FROM debian:bookworm AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    clang \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    libgrpc-dev \
    protobuf-compiler-grpc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN ./build.sh

# Runtime stage
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libprotobuf32 \
    libgrpc++1.51 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/mavlink_driver .

EXPOSE 50051

CMD ["./mavlink_driver"]
