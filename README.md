<p align="center">
  <img src="docs/assets/logo.png" alt="dfrog logo" width="280">
</p>

<h1 align="center">dfrog</h1>

<p align="center"><em>Deterministic Fault-Resilient Operations Guardian</em></p>

<p align="center">
  <a href="https://github.com/yoyold/dfrog/actions/workflows/ci.yml"><img alt="CI" src="https://github.com/yoyold/dfrog/actions/workflows/ci.yml/badge.svg"></a>
  <a href="LICENSE"><img alt="License: Apache 2.0" src="https://img.shields.io/badge/License-Apache_2.0-blue.svg"></a>
  <a href="https://en.cppreference.com/w/cpp/20"><img alt="C++20" src="https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&amp;logoColor=white"></a>
  <a href="https://cmake.org/"><img alt="CMake 3.25+" src="https://img.shields.io/badge/CMake-3.25%2B-064F8C?logo=cmake&amp;logoColor=white"></a>
  <a href="#"><img alt="Platform: Linux" src="https://img.shields.io/badge/Platform-Linux-FCC624?logo=linux&amp;logoColor=black"></a>
  <a href="https://github.com/GoogleContainerTools/distroless"><img alt="Container: distroless" src="https://img.shields.io/badge/Container-distroless-2496ED?logo=docker&amp;logoColor=white"></a>
  <a href=".clang-format"><img alt="Code style: clang-format" src="https://img.shields.io/badge/code_style-clang--format-262D3A?logo=llvm&amp;logoColor=white"></a>
  <a href="https://github.com/microsoft/vcpkg"><img alt="Deps: vcpkg" src="https://img.shields.io/badge/deps-vcpkg-0078D4?logo=microsoft&amp;logoColor=white"></a>
  <a href="#"><img alt="Status: pre-alpha" src="https://img.shields.io/badge/status-pre--alpha-red"></a>
</p>

---

A daemonized Linux service that watches system resources and services, and
performs **deterministic, sandboxed recovery actions** when things go wrong.
Think "watchdog plus", with a safety-first design.

> **Status:** pre-alpha. Scaffolding only — no functionality yet.

## Quickstart

```bash
# Configure & build (Debug + sanitizers)
cmake --preset dev
cmake --build --preset dev

# Run smoke tests
ctest --preset dev

# Container image
docker build -t dfrog:dev .
docker run --rm dfrog:dev --version
```

## Repository layout

```
.
├── src/         # Daemon sources
├── include/     # Public headers (plugin ABI lives here)
├── tests/       # GoogleTest unit + integration tests
├── plugins/     # Example out-of-tree plugin builds
├── cmake/       # Reusable CMake helpers (Warnings, Sanitizers)
├── deploy/      # Dockerfile target manifests, compose, k8s
└── docs/        # Architecture & user docs
```

## License

Apache-2.0 — see [LICENSE](LICENSE).
