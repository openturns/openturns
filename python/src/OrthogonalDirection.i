// SWIG file OrthogonalDirection.i

%{
#include "openturns/OrthogonalDirection.hxx"
%}

%include OrthogonalDirection_doc.i

%include openturns/OrthogonalDirection.hxx
namespace OT{ %extend OrthogonalDirection { OrthogonalDirection(const OrthogonalDirection & other) { return new OT::OrthogonalDirection(other); } } }
