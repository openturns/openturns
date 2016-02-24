// SWIG file RiskyAndFast.i

%{
#include "openturns/RiskyAndFast.hxx"
%}

%include RiskyAndFast_doc.i

%include openturns/RiskyAndFast.hxx
namespace OT{ %extend RiskyAndFast { RiskyAndFast(const RiskyAndFast & other) { return new OT::RiskyAndFast(other); } } }
