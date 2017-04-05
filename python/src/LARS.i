// SWIG file LARS.i

%{
#include "openturns/LARS.hxx"
%}

%include LARS_doc.i

%include openturns/LARS.hxx
namespace OT { %extend LARS { LARS(const LARS & other) { return new OT::LARS(other); } } }

