#! /usr/bin/env python

import importlib
import openturns as ot

width = 40

count = 1


def print_feature(feat, condition):
    global count
    print(
        (str(count).rjust(2) + ": " + feat).ljust(width), "ok" if condition else "n/a"
    )
    count += 1


def try_import(name):
    try:
        importlib.import_module(name)
        return True
    except ImportError:
        return False


print_feature("plotting (matplotlib)", try_import("openturns.viewer"))

print_feature("serialization (LibXML2)", ot.PlatformInfo.HasFeature("libxml2"))
print_feature("serialization (HDF5)", ot.PlatformInfo.HasFeature("hdf5"))
print_feature("serialization (dill)", try_import("dill"))
print_feature(
    "legacy symbolic function (muParser)", ot.PlatformInfo.HasFeature("muparser")
)
print_feature("h-matrix (hmat-oss)", ot.PlatformInfo.HasFeature("hmat"))
print_feature("iterative SVD (Spectra)", ot.PlatformInfo.HasFeature("spectra"))
print_feature("multithreading (TBB)", ot.PlatformInfo.HasFeature("tbb"))
print_feature("process control (psutil)", try_import("psutil"))
print_feature("optimization (CMinpack)", ot.PlatformInfo.HasFeature("cminpack"))
print_feature("optimization (Ceres Solver)", ot.PlatformInfo.HasFeature("ceres"))
print_feature("optimization (Dlib)", ot.PlatformInfo.HasFeature("dlib"))
print_feature("optimization (Bonmin)", ot.PlatformInfo.HasFeature("bonmin"))
print_feature("optimization (Ipopt)", ot.PlatformInfo.HasFeature("ipopt"))
print_feature("optimization (NLopt)", ot.PlatformInfo.HasFeature("nlopt"))
print_feature("optimization (Pagmo)", ot.PlatformInfo.HasFeature("pagmo"))
print_feature(
    "optimization (Pagmo with eigen)",
    ot.PlatformInfo.HasFeature("pagmo") and "cmaes" in ot.Pagmo.GetAlgorithmNames(),
)
print_feature("CSV parser (bison/flex)", ot.PlatformInfo.HasFeature("bison"))
print_feature("special functions (Boost)", ot.PlatformInfo.HasFeature("boost"))
print_feature("special functions (MPFR)", ot.PlatformInfo.HasFeature("mpfr"))
print_feature("special functions (MPC)", ot.PlatformInfo.HasFeature("mpc"))
print_feature(
    "fast prime numbers (Primesieve)", ot.PlatformInfo.HasFeature("primesieve")
)
