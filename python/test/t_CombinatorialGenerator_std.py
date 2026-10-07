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

# default constructors
print("default:", repr(ot.Tuples()))
print("default:", repr(ot.Combinations()))
print("default:", repr(ot.KPermutations()))

# single-argument KPermutations: full permutations
full = ot.KPermutations(3)
assert full.getSize() == 6, "wrong full size"
assert list(full.generateNext()) == [0, 1, 2], "wrong full first"

# k > n: empty sequence
for void in [ot.KPermutations(5, 3), ot.Combinations(5, 3)]:
    assert void.getSize() == 0, "wrong void size"
    with ott.assert_raises(IndexError):
        void.generateNext()

# k == 0: single empty element
for trivial in [ot.KPermutations(0, 3), ot.Combinations(0, 3)]:
    assert trivial.getSize() == 1, "wrong trivial size"
    assert list(trivial.generateNext()) == [], "wrong trivial element"

# k == n: single combination
full = ot.Combinations(3, 3)
assert full.getSize() == 1, "wrong k==n size"
assert list(full.generateNext()) == [0, 1, 2], "wrong k==n element"

# bounds with a zero entry: empty tuple sequence
empty = ot.Tuples([2, 0])
assert empty.getSize() == 0, "wrong empty size"
with ott.assert_raises(IndexError):
    empty.generateNext()

# empty bounds: empty sequence of dimension 0
void = ot.Tuples([])
assert void.getSize() == 0, "wrong void size"
assert void.getDimension() == 0, "wrong void dimension"

# accessors restart the sequence
generator = ot.Combinations(2, 4)
generator.setK(1)
generator.setN(3)
assert (generator.getK(), generator.getN()) == (1, 3), "wrong accessors"
assert generator.getSize() == 3, "wrong resized size"
assert list(generator.generateNext()) == [0], "wrong restarted element"
generator = ot.KPermutations(2, 4)
generator.setK(1)
generator.setN(3)
assert (generator.getK(), generator.getN()) == (1, 3), "wrong accessors"
assert generator.getSize() == 3, "wrong resized size"
assert list(generator.generateNext()) == [0], "wrong restarted element"
generator = ot.Tuples([3, 2])
generator.setBounds([2, 2])
assert list(generator.getBounds()) == [2, 2], "wrong bounds"
assert generator.getSize() == 4, "wrong resized size"
assert list(generator.generateNext()) == [0, 0], "wrong restarted element"
