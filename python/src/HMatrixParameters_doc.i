%feature("docstring") OT::HMatrixParameters
"Parameters for HMatrix class.

Notes
-----
This class regroups the parameters used by :class:`~openturns.HMatrix`
Default values are provided in the :class:`~openturns.ResourceMap` and can be modified by
the user.

For example, for the admissibility factor, the default value is given by
the key 'HMatrix-AdmissibilityFactor' in the :class:`~openturns.ResourceMap` and applied
only if the 'HMatrix-Admissibility' key is set to 'standard'.
The default clustering algorithm is given by the key 'HMatrix-ClusteringAlgorithm' in the :class:`~openturns.ResourceMap`.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getAssemblyEpsilon
"Return the epsilon used for the assembly.

Returns
-------
epsilon : float
    assembly epsilon for HMatrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setAssemblyEpsilon
"Set the epsilon used for the assembly.

Parameters
----------
epsilon : float
    assembly epsilon for HMatrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getRecompressionEpsilon
"Return the epsilon used for the recompression.

Returns
-------
epsilon : float
    recompression epsilon for HMatrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setRecompressionEpsilon
"Set the epsilon used for the recompression.

Parameters
----------
epsilon : float
    recompression epsilon for HMatrix."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getAdmissibility
"Return the admissibility used for cluster blocks.

Returns
-------
admissibility : string
    Used admissibility for HMatrix.
    Might be 'standard' or 'hodlr'."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setAdmissibility
"Set the admissibility used for cluster blocks.

Parameters
----------
admissibility : string
    Used admissibility for HMatrix.
    Should be 'standard' or 'hodlr'."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getAdmissibilityFactor
"Return the admissibility factor used for cluster blocks.

Returns
-------
epsilon : float
    admissibility factor for HMatrix.

Note
----
Factor is used only in case of standard admissibility."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setAdmissibilityFactor
"Set the admissibility factor used for cluster blocks.

Parameters
----------
epsilon : float
    admissibility factor for HMatrix.
    
Note
----
Factor is used only in case of standard admissibility."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getClusteringAlgorithm
"Return the clustering algorithm.

Returns
-------
algorithm : str
    clustering algorithm (either 'median', 'geometric' or 'hybrid')."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setClusteringAlgorithm
"Set the clustering algorithm.

Parameters
----------
algorithm : str
    clustering algorithm (either 'median', 'geometric' or 'hybrid')."

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getCompressionMethod
"Return the compression method.

Returns
-------
method : str
    compression method (either 'Svd', 'AcaFull', 'AcaPartial', 'AcaPlus' or 'AcaRandom')"

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setCompressionMethod
"Set the compression method.

Parameters
----------
method : str
    compression method (either 'Svd', 'AcaFull', 'AcaPartial', 'AcaPlus' or 'AcaRandom')"

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::getFactorizationMethod
"Return the factorization method.

Returns
-------
method : str
    Factorization method (either 'LU', 'LDLt', or 'LLt', 'hodlr' or 'hodlrsym')"

// ---------------------------------------------------------------------

%feature("docstring") OT::HMatrixParameters::setFactorizationMethod
"Set the factorization method.

Parameters
----------
method : str
    Factorization method (either 'LU', 'LDLt', or 'LLt', 'hodlr' or 'hodlrsym')"

// ---------------------------------------------------------------------

