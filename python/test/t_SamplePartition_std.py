import openturns as ot
import openturns.experimental as otexp

u = 0.5
r = 2
sample = ot.Sample.BuildFromPoint([1, 2, -4, -5, 3, 4, -2, 5, -1, -3, 6, -2, -2, 7, 8])
part = otexp.SamplePartition(sample)
peaks, clusters = part.getPeakOverThreshold(u, r)
print(peaks)
assert peaks.asPoint() == [2.0, 5.0, 6.0, 8.0]
indices = clusters.getIndicesCollection()
print(indices)
assert len(peaks) == len(indices)

graph = clusters.draw(u)
