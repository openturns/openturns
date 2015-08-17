// SWIG file CobylaObsolete.i

%{
#include "CobylaObsolete.hxx"
%}

%include CobylaObsolete.hxx
namespace OT{ %extend CobylaObsolete { CobylaObsolete(const CobylaObsolete & other) { return new OT::CobylaObsolete(other); } } }
