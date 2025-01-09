%feature("docstring") OT::HMatrixFactory
"Empty HMatrix builder.

See also
--------
HMatrix"

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixFactory::build
"Create the compressed skeleton representation.

Parameters
----------
sample : 2-d sequence of float
    Vertex locations
outputDimension : int
    Number of degrees of freedom by vertex location
symmetric : bool
    Whether matrix is symmetric.
parameters : :class:`~openturns.HMatrixParameters`
    User-defined settings for hmatrix

Returns
-------
mat : :class:`~openturns.HMatrix`
    The compressed representation.

Notes
-----
The HMatrix is built from the sample locations and the output dimension.
The parameters are used to set the admissibility, the clustering algorithm and other parameters (validation, maxLeafSize,...).
Default values are provided in the :class:`~openturns.ResourceMap`, and can be modified by the user before calling this method."
