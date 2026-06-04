%feature("docstring") OT::NearestNeighbour1D
"Partition tree data structure for 1D points.

Allows one to store and search 1D points fast, by using
dichotomy on sorted points.

Parameters
----------
sample : :class:`~openturns.Sample`
    1D points

See also
--------
NearestNeighbourAlgorithm

Examples
--------
>>> import openturns as ot
>>> myRegularGrid = ot.RegularGrid(0.0, 0.1, 100)
>>> tree = ot.NearestNeighbour1D([[x] for x in myRegularGrid.getValues()])
>>> neighbour = tree.queryScalar(0.1)"


// ---------------------------------------------------------------------

%feature("docstring") OT::NearestNeighbour1D::queryScalar
"Accessor to the nearest neighbour index.

Available usages:
    queryScalar(*x*)

    queryScalar(*point*)


Parameters
----------
x : float
    Given 1D point.
point : sequence of float
    Sequence of 1D points.

Returns
-------
index : int
    Index of the nearest neighbour.
indices : :class:`~openturns.Indices`
    Index of the nearest neighbour of the given points."

// ---------------------------------------------------------------------

%feature("docstring") OT::NearestNeighbour1D::queryScalarK
"Accessor to the nearest neighbours indices.

Parameters
----------
x : float
    Given 1D point.
k : int
    Number of indices to return.
sorted : bool
    Boolean to tell whether returned indices are sorted according to
    the distance to the given point.

Returns
-------
indices : :class:`~openturns.Indices`
    Indices of the k nearest neighbours."

