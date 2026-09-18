#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Default graph bounds are automatic
graph = ot.Normal().drawPDF()
bbox = graph.getBoundingBox()
assert graph.getAutomaticBoundingBox(), "automatic bounding box expected"

# setXMin / setXMax / setYMin / setYMax
graph.setXMin(-2.0)
graph.setXMax(3.0)
graph.setYMin(-0.1)
graph.setYMax(1.5)
bbox = graph.getBoundingBox()
ott.assert_almost_equal(bbox.getLowerBound()[0], -2.0)
ott.assert_almost_equal(bbox.getUpperBound()[0], 3.0)
ott.assert_almost_equal(bbox.getLowerBound()[1], -0.1)
ott.assert_almost_equal(bbox.getUpperBound()[1], 1.5)
assert not graph.getAutomaticBoundingBox(), "auto bounding box expected off"
# setting one bound keeps the others
graph.setYMin(-0.5)
bbox = graph.getBoundingBox()
ott.assert_almost_equal(bbox.getLowerBound()[0], -2.0)
ott.assert_almost_equal(bbox.getLowerBound()[1], -0.5)

# example from issue #1583
n = ot.Normal()
graph = n.drawPDF()
graph.setXMin(-1.0)
graph.setXMax(2.0)
graph.setYMin(-3.0)
graph.setYMax(1.0)
bbox = graph.getBoundingBox()
ott.assert_almost_equal(bbox.getLowerBound()[0], -1.0)
ott.assert_almost_equal(bbox.getUpperBound()[0], 2.0)
ott.assert_almost_equal(bbox.getLowerBound()[1], -3.0)
ott.assert_almost_equal(bbox.getUpperBound()[1], 1.0)
