// SWIG file NLopt.i

%{
#include "openturns/NLopt.hxx"
%}

%include NLopt_doc.i

%include openturns/NLopt.hxx

namespace OT { %extend NLopt { NLopt(const NLopt & other) { return new OT::NLopt(other); } } }

