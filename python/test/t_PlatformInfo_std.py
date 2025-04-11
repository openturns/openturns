#! /usr/bin/env python

import importlib
import openturns as ot


def try_import(name):
    try:
        importlib.import_module(name)
        return True
    except ImportError:
        return False


print(ot.PlatformInfo.Summary())

for module in ["openturns.viewer", "dill", "psutil"]:
    status = "ok" if try_import(module) else "n/a"
    print(f"{module:<20}{status}")
