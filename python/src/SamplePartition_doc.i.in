%feature("docstring") OT::SamplePartition
"Select partition of samples.

Class to define subset of points, and select new ones, see :meth:`getPeakOverThreshold`.

Parameters
----------
sample : 2-d sequence of float
    A sample of dimension 1.
indices : sequence of int 2-tuple or sequence of int, optional
    Flat indices of population in the partition or
    list of (start,end) ranges.
    If not provided the default is to consider one single group with all the sample values.

Examples
--------
>>> import openturns as ot
>>> ref = ot.Uniform().getSample(10)
>>> sp1 = ot.SamplePartition(ref, [[0, 3], [5, 9]])
>>> sp2 = ot.SamplePartition(ref, [0, 1, 2, 3])
"

// ----------------------------------------------------------------------------

%feature("docstring") OT::SamplePartition::getPeakOverThreshold
"Compute extreme values using Peaks Over Threshold (POT) method.

Parameters
----------
sample : 2-d sequence of float
    A sample of dimension 1.
threshold : float
    The threshold value
r : int
    Minimum number of consecutive values below the threshold between clusters

Returns
-------
peaks : :class:`~openturns.Sample`
    The peaks for each cluster
clusters : :class:`openturns.SamplePartition`
    The clusters partition
"

// ----------------------------------------------------------------------------

%feature("docstring") OT::SamplePartition::draw
"Draw clusters and peaks.

Parameters
----------
threshold : float
    The threshold value

Returns
-------
graph : :class:`~openturns.Graph`
    Graph of clusters, peaks.
"

// ----------------------------------------------------------------------------

%feature("docstring") OT::SamplePartition::getSample
"Sample accessor.

Returns
-------
sample : :class:`~openturns.Sample`
    The internal sample
"

// ----------------------------------------------------------------------------

%feature("docstring") OT::SamplePartition::getIndicesCollection
"Partition indices accessor.

Returns
-------
indicesCollection : :class:`~openturns.IndicesCollection`
    List of partitions ranges as (start,end) indices.
"
