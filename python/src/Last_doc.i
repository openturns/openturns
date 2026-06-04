%feature("docstring") OT::Last
"Last history storage strategy.

Parameters
----------
N : int
    Number of points to store.

See also
--------
HistoryStrategy, Compact, Null, Full

Notes
-----
The last strategy stores only the *N* last points."

// ---------------------------------------------------------------------

%feature("docstring") OT::Last::getHasWrapped
"Accessor to a wrapped flag.

Returns
-------
wrapped : bool
    Return *True* if the storage has been wrapped."

// ---------------------------------------------------------------------

%feature("docstring") OT::Last::getIndex
"Accessor to the index.

Returns
-------
index : int
    The index of the storage position."

// ---------------------------------------------------------------------

%feature("docstring") OT::Last::getMaximumSize
"Accessor to the maximum size of the storage strategy.

Returns
-------
N : int
    The maximum number of points to store."
