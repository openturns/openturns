// SWIG file Greater.i

%{
#include "openturns/Greater.hxx"
%}

%include Greater_doc.i

%include openturns/Greater.hxx

namespace OT { %extend Greater { Greater(const Greater & other) { return new OT::Greater(other); } } }
