// SWIG file AbdoRackwitz.i

%{
#include "AbdoRackwitz.hxx"
%}

%include AbdoRackwitz_doc.i

%include AbdoRackwitz.hxx

namespace OT{ %extend AbdoRackwitz { AbdoRackwitz(const AbdoRackwitz & other) { return new OT::AbdoRackwitz(other); } } }
