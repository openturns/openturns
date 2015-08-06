// SWIG file TNCObsolete.i

%{
#include "TNCObsolete.hxx"
%}

%include TNCObsolete.hxx
namespace OT{ %extend TNCObsolete { TNCObsolete(const TNCObsolete & other) { return new OT::TNCObsolete(other); } } }
