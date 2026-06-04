%feature("docstring") OT::RandomGeneratorState
"Random generator state.

Data structure used to save the internal state of the generator.

Parameters
----------
buffer : sequence of int
    The history buffer
index : int
    The index into buffer"

// ---------------------------------------------------------------------

%feature("docstring") OT::RandomGeneratorState::getBuffer
"Random generator state buffer accessor.

Returns
-------
buffer : sequence of int
    The history buffer"

// ---------------------------------------------------------------------

%feature("docstring") OT::RandomGeneratorState::getIndex
"Random generator state index accessor.

Returns
-------
index : int
    The index into buffer"
