// SWIG file GreaterOrEqual.i

%{
#include "openturns/GreaterOrEqual.hxx"
%}

%include GreaterOrEqual_doc.i

%include openturns/GreaterOrEqual.hxx

namespace OT { %extend GreaterOrEqual { GreaterOrEqual(const GreaterOrEqual & other) { return new OT::GreaterOrEqual(other); } } }
