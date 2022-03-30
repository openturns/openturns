// SWIG file HSICUStat.i

%{
#include "openturns/HSICUStat.hxx"
%}

%include HSICUStat_doc.i

%include openturns/HSICUStat.hxx
namespace OT{ %extend HSICUStat { HSICUStat(const HSICUStat & other) { return new OT::HSICUStat(other); } } }
