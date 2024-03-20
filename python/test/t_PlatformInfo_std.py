#! /usr/bin/env python

import importlib
import openturns as ot


def try_import(name):
    try:
        importlib.import_module(name)
        return True
    except ImportError:
        return False


print(f"version              {ot.PlatformInfo.GetVersion()}")
print(f"revision             {ot.PlatformInfo.GetRevision()}")
print(f"date                 {ot.PlatformInfo.GetDate()}")
print(f"compiler id          {ot.PlatformInfo.GetCompilerId()}")
print(f"compiler version     {ot.PlatformInfo.GetCompilerVersion()}")

for feature in ot.PlatformInfo.GetFeatures():
    status = "ok" if ot.PlatformInfo.HasFeature(feature) else "n/a"
    print(f"{feature:<20} {status}")

for module in ["openturns.viewer", "dill", "psutil"]:
    status = "ok" if try_import(module) else "n/a"
    print(f"{module:<20} {status}")
