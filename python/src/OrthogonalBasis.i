// SWIG file OrthogonalBasis.i

%{
#include "openturns/OrthogonalBasis.hxx"
%}

%include OrthogonalBasis_doc.i

OTTypedInterfaceObjectImplementationHelper(OrthogonalBasis, OrthogonalFunctionFactory)

%copyctor OT::OrthogonalBasis;

%include openturns/OrthogonalBasis.hxx
