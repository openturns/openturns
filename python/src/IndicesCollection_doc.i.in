%feature("docstring") OT::IndicesCollection
"Fixed size collection of Indices.

This class is a container for a fixed size list of Indices.
Lists cannot be shrunk or extended, but values can be modified.

Available constructors:
    IndicesCollection(*size, stride, indices*)

    IndicesCollection(*sequence*)

Parameters
----------
size : int
    Collection size.
stride : int
    Size of each element
    Default creates an empty sample with dimension 1.
values : :class:`~openturns.Indices` or flat (1d) array, list or tuple of int
    The point that will be repeated along the sample.
    Default creates a sample filled with zeros (null vectors).
sequence : :class:`~openturns.IndicesCollection`
    Copy values from a collection of Indices.

Examples
--------
Create a IndicesCollection

>>> import openturns as ot
>>> indices = list(range(12))
>>> listIndices = ot.IndicesCollection(3, 4, indices)
>>> print(listIndices)
[[0,1,2,3],[4,5,6,7],[8,9,10,11]]

Create a IndicesCollection from a (2d) array, list or tuple

>>> import numpy as np
>>> listIndices = ot.IndicesCollection(np.array([(1, 2), (3, 4), (5, 6)]))

and back

>>> indices = np.array(listIndices)

Get an Indices.  Note that a copy is returned, modifying it does not alter original list.

>>> first = listIndices[0]
>>> first[0] = 0
>>> print(first)
[0,2]
>>> print(listIndices)
[[1,2],[3,4],[5,6]]

To mdify values, one has to change the whole element.

>>> listIndices[0] = [7, 8]
>>> print(listIndices)
[[7,8],[3,4],[5,6]]"

// ---------------------------------------------------------------------

%feature("docstring") OT::IndicesCollection::getSize
"Accessor to the collection size.

Returns
-------
m : int
    The number of :class:`~openturns.Indices` in the collection."

// ---------------------------------------------------------------------

%feature("docstring") OT::IndicesCollection::getStride
"Accessor to the indices size.

Returns
-------
m : int
    The number of integers in each :class:`~openturns.Indices`."

