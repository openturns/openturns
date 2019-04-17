// SWIG file LessOrEqual.i

%{
#include "openturns/LessOrEqual.hxx"
%}

%include LessOrEqual_doc.i

%include openturns/LessOrEqual.hxx

namespace OT { %extend LessOrEqual { LessOrEqual(const LessOrEqual & other) { return new OT::LessOrEqual(other); } } }
