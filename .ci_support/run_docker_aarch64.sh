#!/bin/sh

set -e

cd /tmp
mkdir build && cd build
aarch64-cmake -DCMAKE_INSTALL_PREFIX=~/.local \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DPython_EXECUTABLE=/usr/bin/aarch64-python -DSWIG_COMPILE_FLAGS="-O1" /io
make install
OPENTURNS_NUM_THREADS=1 OMP_NUM_THREADS=1 ctest -R pyinstall --output-on-failure --timeout 1000 ${MAKEFLAGS} --schedule-random -E "FunctionalChaosSobolIndices_std|LeastSquaresExpansion_std|FieldToPointFunctionalChaosAlgorithm_std|FieldFunctionalChaosSobolIndices_std|HSICEstimatorTargetSensitivity_std"
