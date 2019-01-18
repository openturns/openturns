// SWIG file OPTpp.i

%{
#include "openturns/OPTpp.hxx"
%}

%include OPTpp_doc.i

%include openturns/OPTpp.hxx

namespace OT { %extend OPTpp { OPTpp(const OPTpp & other) { return new OT::OPTpp(other); } } }

