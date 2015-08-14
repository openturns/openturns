// SWIG file OrthogonalBasis.i

%{
#include "OrthogonalBasis.hxx"
%}

%include OrthogonalBasis_doc.i

OTTypedInterfaceObjectImplementationHelper(OrthogonalBasis, OrthogonalFunctionFactory)

%include OrthogonalBasis.hxx

namespace OT{ %extend OrthogonalBasis { OrthogonalBasis(const OrthogonalBasis & other) { return new OT::OrthogonalBasis(other); } } }
