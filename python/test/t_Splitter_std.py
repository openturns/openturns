#!/usr/bin/env python

import openturns as ot

N = 10

splitters = [ot.LeaveOneOutSplitter(N)]
splitter = ot.KFoldSplitter(N, 5)
splitters.append(splitter)
splitter = ot.KFoldSplitter(N, 5)
splitter.setRandomize(True)
splitters.append(splitter)

for splitter in splitters:
    print("splitter=", splitter)
    size = 0
    for indices1, indices2 in splitter:
        print("indices=", indices1, indices2)

        assert len(indices1) + len(indices2) == N, "not enough indices"

        assert indices1.check(N), "dup or too large indices1"
        assert indices2.check(N), "dup or too large indices2"

        for i1 in indices1:
            assert i1 not in indices2, "indices1 in indices2"
        for i2 in indices2:
            assert i2 not in indices1, "indices2 in indices1"
        size += 1
    assert size == splitter.getSize(), "wrong size"

# try selection
indices = [8, 0]
sample = ot.Normal().getSample(N)
sample_i = sample[indices]
print("sample=", sample, "indices=", indices, "sample_i=", sample_i)
assert len(sample_i) == len(indices), "wrong selection len"
assert sample_i[0] == sample[indices[0]], "wrong first item in selection"
