%feature("docstring") OT::MemoizePointToFieldFunction
"Point to field function which caches its evaluations.

When this function is evaluated, it calls the
:class:`~openturns.PointToFieldFunction` passed as argument, and stores
the input :class:`~openturns.Point` with the resulting field values.
Repeated evaluations at the same input points reuse the stored fields
instead of calling the wrapped function again.
The cache mechanism is enabled by default and its size is initialized
by the value of the :class:`~openturns.ResourceMap` `Cache-MaxSize` entry.

Parameters
----------
function : :class:`~openturns.PointToFieldFunction`
    Delegate function

See also
--------
PointToFieldFunction, MemoizeFunction

Examples
--------

>>> import openturns as ot
>>> mesh = ot.RegularGrid(0.0, 1.0, 2)
>>> f = ot.PointToFieldFunction(ot.VertexValuePointToFieldFunction(ot.SymbolicFunction(['t'], ['t']), mesh))
>>> f = ot.MemoizePointToFieldFunction(f)
>>> print(f([]))
    [ y0 ]
0 : [ 0  ]
1 : [ 1  ]"

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::enableCache
"Activate the cache mechanism.

See Also
--------
disableCache, isCacheEnabled, clearCache"

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::disableCache
"Deactivate the cache mechanism.

See Also
--------
enableCache, isCacheEnabled, clearCache"

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::isCacheEnabled
"Test the cache mechanism activity.

Returns
-------
isCacheEnabled : bool
    Flag telling whether the cache mechanism is active."

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::getCacheHits
"Accessor to the number of cache hits.

Returns
-------
hits : int
    Number of evaluations served from the cache."

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::clearCache
"Empty the cache.

See Also
--------
enableCache, disableCache, isCacheEnabled"

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::getCacheInput
"Accessor to the input points stored in the cache.

Returns
-------
cacheInput : :class:`~openturns.Sample`
    The input points stored in the cache."

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::getCacheOutput
"Accessor to the output fields stored in the cache.

Returns
-------
cacheOutput : :class:`~openturns.ProcessSample`
    The output fields stored in the cache."

// ---------------------------------------------------------------------

%feature("docstring") OT::MemoizePointToFieldFunction::getFunction
"Accessor to the wrapped function.

Returns
-------
function : :class:`~openturns.PointToFieldFunction`
    The wrapped function."
