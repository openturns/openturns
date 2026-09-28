#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

generators = [(ot.Tuples([4, 6, 9]), 216),
              (ot.KPermutations(4, 6), 360),
              (ot.Combinations(4, 6), 15)]

for generator, expectedSize in generators:
    print("generator:", generator)
    assert generator.getSize() == expectedSize, "wrong size"
    assert generator.getDimension() == len(generator.generateNext())
    generator.restart()
    # collect through generateNext
    generator.restart()
    first_pass = [generator.generateNext()
                  for _ in range(expectedSize)]
    with ott.assert_raises(Exception):
        generator.generateNext()
    # collect through the python iterator protocol
    second_pass = [indices for indices in generator]
    assert first_pass == second_pass, "iterator mismatch"
    print("size:", len(first_pass), "first:", first_pass[0],
          "last:", first_pass[-1])
    # restart restarts the sequence
    generator.restart()
    assert generator.generateNext() == first_pass[0]
    assert generator.generateNext() == first_pass[1]

# large sequence from #2885: getSize without building the collection
big = ot.Combinations(24, 34)
print("big size:", big.getSize())
assert big.getSize() == 131128140, "wrong big size"
big.restart()
first = big.generateNext()
print("big first:", first)
assert list(first) == list(range(24)), "wrong big first element"

# wrapper forwards the stateful API
wrapped = ot.CombinatorialGenerator(ot.Combinations(2, 5))
assert wrapped.getSize() == 10, "wrong wrapped size"
wrapped.restart()
assert list(wrapped.generateNext()) == [0, 1], "wrong wrapped element"
count = 0
for indices in wrapped:
    count += 1
assert count == wrapped.getSize(), "wrong wrapped iteration size"
