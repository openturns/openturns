// SWIG file RiskyAndFast.i

%{
#include "RiskyAndFast.hxx"
%}

%include RiskyAndFast_doc.i

%include RiskyAndFast.hxx
namespace OT{ %extend RiskyAndFast { RiskyAndFast(const RiskyAndFast & other) { return new OT::RiskyAndFast(other); } } }
