---
name: openturns-ci-repro
description: 'Reproduce a GitHub Actions Linux CI failure locally when it does not happen on your machine. Covers the helper utils/build_locally.sh.'
---

# Reproducing GitHub Actions Linux CI failures locally

## When to use
- A test or build passes locally but fails only on the GHA Linux runner.
- The failure depends on a build target you do not have installed
- You need an interactive shell in a CI-equivalent environment to run gdb or valgrind.

Linux jobs only. macOS and Windows runners cannot be containerised - for those, re-run the job with
a debug session, use a self hosted runner, or a local VM.

## How GHA Linux jobs are defined
- The jobs are defined in `.github/workflows/build.yml`.
- Each entry you can reproduce locally uses `utils/build_locally.sh` with a different tag.

## Primary: the docker images
- Each job uses either a default debian image or an openturns-specific prebuilt image
- Prebuilt images are defined in a separate repository: https://github.com/openturns/docker-images

## Secondary: the ci scripts
- `utils/build_locally.sh` maps each tag to a container script (source mounted at `/io`, `MAKEFLAGS` forwarded from the host):
  * `sphinx` -> `.circleci/run_docker_linux.sh`
  * `mingw` -> `.circleci/run_docker_mingw.sh` (Windows cross-build, not a Linux GHA job)
  * `coverage` -> `.ci_support/run_docker_coverage.sh`
  * `aarch64` -> `.ci_support/run_docker_aarch64.sh` (or `.ci_support/run_docker_debian_native.sh` on aarch64 hosts)
  * `mindeps` -> `.ci_support/run_docker_mindeps.sh`
  * `abi3` -> `.ci_support/run_docker_abi3.sh`
- `.github/workflows/build.yml` currently invokes the `aarch64`/`coverage`/`mindeps`/`abi3` tags; `sphinx`/`mingw` exist in the script without a direct GHA equivalent.
- The CI scripts can be modified temporarily to exercise only the tests that need to be run.
- The source code is mounted into the container from the host current source dir.

## Worked example: Sphinx doc

```sh
./utils/build_locally.sh sphinx
```

This variant builds the HTML documentation and copies it to the host, and runs Python tests.
The documentation archive is also copied to the host for inspection.

## Pointers
- Jobs and steps: `.github/workflows/build.yml`
- Launch script from host: `utils/build_locally.sh`
- CI scripts inside container: `.ci_support/run_docker_*.sh` or `.circleci/run_docker_*.sh` per the tag table above.
