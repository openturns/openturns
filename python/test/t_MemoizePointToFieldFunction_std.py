#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()


class CountingPointToFieldFunction(ot.OpenTURNSPythonPointToFieldFunction):
    calls = 0

    def __init__(self):
        mesh = ot.RegularGrid(0.0, 0.1, 11)
        super().__init__(2, mesh, 2)

    def _exec(self, X):
        CountingPointToFieldFunction.calls += 1
        size = self.getOutputMesh().getVerticesNumber()
        return [ot.Point(X) * i for i in range(size)]


# Memoized point to field function, see issue #2802
counting = CountingPointToFieldFunction()
func = ot.PointToFieldFunction(counting)
memo = ot.MemoizePointToFieldFunction(func)
assert memo.getInputDimension() == 2
assert memo.getOutputDimension() == 2
assert memo.isCacheEnabled()
assert memo.getCacheHits() == 0

# Repeated evaluations reuse the cache: 3 calls, 1 underlying evaluation
first = memo([1.0, 2.0])
second = memo([1.0, 2.0])
third = memo([3.0, 4.0])
ott.assert_almost_equal(first, second)
assert CountingPointToFieldFunction.calls == 2
assert memo.getCacheHits() == 1
assert memo.getInputCache().getSize() == 2
assert memo.getOutputCache().getSize() == 2
ott.assert_almost_equal(memo.getOutputCache().getField(0).getValues(), first)

# Batch evaluation deduplicates its inputs
batch = memo([[1.0, 2.0], [1.0, 2.0], [5.0, 6.0]])
assert batch.getSize() == 3
ott.assert_almost_equal(batch.getField(0).getValues(), first)
assert CountingPointToFieldFunction.calls == 3
# Only the two cached inputs count as hits, the freshly computed one does not
assert memo.getCacheHits() == 3

# Disabled cache evaluates through every time
memo.disableCache()
assert not memo.isCacheEnabled()
memo([1.0, 2.0])
assert CountingPointToFieldFunction.calls == 4
memo.enableCache()
memo.clearCache()
assert memo.getInputCache().getSize() == 0
memo([1.0, 2.0])
assert CountingPointToFieldFunction.calls == 5

# Persistence round-trip keeps the wrapped function and the cache content
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("memoize_ptf.xml"))
memo.setName("memo")
study.add("memo", memo)
study.save()
loadedStudy = ot.Study("memoize_ptf.xml")
loadedStudy.load()
loaded = ot.MemoizePointToFieldFunction()
loadedStudy.fillObject("memo", loaded)
assert loaded.isCacheEnabled()
assert loaded.getInputCache().getSize() == memo.getInputCache().getSize()
ott.assert_almost_equal(loaded.getInputCache(), memo.getInputCache())
calls = CountingPointToFieldFunction.calls
ott.assert_almost_equal(loaded([1.0, 2.0]), memo([1.0, 2.0]))
assert CountingPointToFieldFunction.calls == calls
os.remove("memoize_ptf.xml")

# The cache content is saved even when the cache is disabled
memo.disableCache()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("memoize_ptf_disabled.xml"))
study.add("memo", memo)
study.save()
loadedStudy = ot.Study("memoize_ptf_disabled.xml")
loadedStudy.load()
loadedDisabled = ot.MemoizePointToFieldFunction()
loadedStudy.fillObject("memo", loadedDisabled)
assert not loadedDisabled.isCacheEnabled()
assert loadedDisabled.getInputCache().getSize() == memo.getInputCache().getSize()
ott.assert_almost_equal(loadedDisabled.getInputCache(), memo.getInputCache())
os.remove("memoize_ptf_disabled.xml")
