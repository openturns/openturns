// SWIG file OrthogonalDirection.i

%{
#include "OrthogonalDirection.hxx"
%}

%include OrthogonalDirection.hxx
namespace OT{ %extend OrthogonalDirection { OrthogonalDirection(const OrthogonalDirection & other) { return new OT::OrthogonalDirection(other); } } }
