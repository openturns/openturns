from concurrent.futures import ThreadPoolExecutor
import importlib
import sysconfig
import sys

# check the GIL setting is not changed
gil0 = sys._is_gil_enabled() if hasattr(sys, "_is_gil_enabled") else True
ot = importlib.import_module("openturns")
gil1 = sys._is_gil_enabled() if hasattr(sys, "_is_gil_enabled") else True
assert gil0 == gil1
assert not gil0 == bool(sysconfig.get_config_var("Py_GIL_DISABLED"))

dist = ot.Normal(0.0, 1.0)


def worker():
    sample = dist.getSample(10000)

    mean = sample.computeMean()[0]
    var = sample.computeVariance()[0]

    # perform additional operations to stress the library
    cov = sample.computeCovariance()

    return (mean, var, cov[0, 0])


with ThreadPoolExecutor(max_workers=8) as pool:
    futures = []

    for i in range(200):
        futures.append(pool.submit(worker))

    results = [f.result() for f in futures]

print("Completed", len(results), "tasks")
