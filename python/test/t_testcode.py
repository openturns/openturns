#!/usr/bin/env python

import openturns.testing as ott

# check assert_almost_equal
ott.assert_almost_equal(42.0, 42.0)
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(42.0, 42.5)
ott.assert_almost_equal(0.0, 1e-12, 0.0, 1e-12)
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(0.0, 1e-10, 0.0, 1e-12)
ott.assert_almost_equal(1.0, 1.0 + 1e-12, 1e-10, 0.0)
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(1.0, 1e-10, 1e-12, 0.0)

ott.assert_almost_equal(float("inf"), float("inf"))
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(float("-inf"), float("inf"))
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(float("inf"), float("-inf"))
ott.assert_almost_equal(float("-inf"), float("-inf"))

ott.assert_almost_equal(float("nan"), float("nan"))
ott.assert_almost_equal(float("-nan"), float("nan"))
ott.assert_almost_equal(float("nan"), float("-nan"))
ott.assert_almost_equal(float("-nan"), float("-nan"))

ott.assert_almost_equal(complex(1.0, 2.0), complex(1.0, 2.0))
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(complex(1.0, 2.0), complex(1.0, 3.0))
ott.assert_almost_equal(complex(1.0, float("inf")), complex(1.0, float("inf")))
ott.assert_almost_equal(complex(1.0, float("nan")), complex(1.0, float("nan")))
ott.assert_almost_equal(complex(1.0, float("inf")), complex(1.0, float("inf")))
ott.assert_almost_equal(complex(1.0, float("nan")), complex(1.0, float("nan")))
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(complex(float("inf"), 0.0), complex(float("-inf"), 0.0))
with ott.assert_raises(RuntimeError):
    ott.assert_almost_equal(complex(1.0, float("-inf")), complex(1.0, float("inf")))
