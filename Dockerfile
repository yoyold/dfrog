# syntax=docker/dockerfile:1.7
# ---- Stage 1: builder ----
# Builder image deliberately matches the runtime glibc (Debian 12 / glibc 2.36).
# Building on a newer glibc (e.g. ubuntu:24.04) would link against symbols that
# the distroless/cc-debian12 runtime cannot resolve.
FROM debian:12-slim AS builder

ENV DEBIAN_FRONTEND=noninteractive \
    VCPKG_ROOT=/opt/vcpkg \
    VCPKG_DEFAULT_BINARY_CACHE=/var/cache/vcpkg

# apt lists/cache are kept in BuildKit cache mounts (tmpfs-style, not persisted
# into the image layer), so an apt-get clean step would be a no-op.
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        git \
        ninja-build \
        pkg-config \
        tar \
        unzip \
        zip

# vcpkg pin is the single source of truth in .vcpkg-commit at repo root —
# the same file is consumed by CI to keep container and CI builds in lockstep.
COPY .vcpkg-commit /tmp/.vcpkg-commit
RUN VCPKG_COMMIT="$(tr -d '[:space:]' < /tmp/.vcpkg-commit)" \
 && git clone https://github.com/microsoft/vcpkg "${VCPKG_ROOT}" \
 && git -C "${VCPKG_ROOT}" checkout "${VCPKG_COMMIT}" \
 && "${VCPKG_ROOT}/bootstrap-vcpkg.sh" -disableMetrics

WORKDIR /src
COPY vcpkg.json CMakeLists.txt CMakePresets.json VERSION ./
COPY cmake ./cmake
COPY include ./include
COPY src ./src
COPY tests ./tests

RUN --mount=type=cache,target=/var/cache/vcpkg \
    cmake --preset release -DDFROG_BUILD_TESTS=OFF \
 && cmake --build --preset release --parallel \
 && cmake --install build/release --prefix /out --strip

# ---- Stage 2: runtime (distroless, nonroot) ----
FROM gcr.io/distroless/cc-debian12:nonroot AS runtime

COPY --from=builder /out/bin/dfrogd /usr/local/bin/dfrogd

ENV DFROG_CONFIG=/etc/dfrog/config.yaml

USER nonroot:nonroot
ENTRYPOINT ["/usr/local/bin/dfrogd"]
CMD ["--version"]
