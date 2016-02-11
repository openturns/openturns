// SWIG file OrthogonalBasis.i

%{
#include "openturns/OrthogonalBasis.hxx"
%}

%include OrthogonalBasis_doc.i

OTTypedInterfaceObjectImplementationHelper(OrthogonalBasis, OrthogonalFunctionFactory)

%include openturns/OrthogonalBasis.hxx

namespace OT{ %extend OrthogonalBasis { OrthogonalBasis(const OrthogonalBasis & other) { return new OT::OrthogonalBasis(other); } } }
