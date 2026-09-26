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
- The `utils/build_locally.sh` script launches the container with scripts located in `.ci_support/` or `.circleci/`.
- The CI scripts can be modified temporarily to exercise only the tests that need to be run.
- The source code is mounted into the container from the host current source dir.
- The MAKEFLAGS build parallelization level is copied from the host to the container.

## Worked example: Sphinx doc

```sh
./utils/build_locally.sh sphinx
```

This variant builds the HTML documentation and copies it to the host, and runs Python tests.
The documentation archive is also copied to the host for inspection.

## Pointers
- Jobs and steps: `.github/workflows/build.yml`
- Launch script from host: `utils/build_locally.sh`
- CI scripts inside container: `.ci_support/` or `.circleci/`.
