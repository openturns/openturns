%define OT_CombinatorialGenerator_doc
"Combinatorial generator base class.

See also
--------
Combinations, KPermutations, Tuples"
%enddef
%feature("docstring") OT::CombinatorialGeneratorImplementation
OT_CombinatorialGenerator_doc

// ---------------------------------------------------------------------

%define OT_CombinatorialGenerator_generate_doc
"Generate the combinatorial sequence.

Returns
-------
indicesCol : collection  of :class:`~openturns.Indices`
    The collection of all the possible values of the combinatorial generator as
    a set of non-negative integer values stored into an
    :class:`~openturns.Indices`."
%enddef
%feature("docstring") OT::CombinatorialGeneratorImplementation::generate
OT_CombinatorialGenerator_generate_doc
