// SWIG file WeibullMax.i

%{
#include "openturns/WeibullMax.hxx"
%}

%include WeibullMax_doc.i

%include openturns/WeibullMax.hxx
namespace OT { %extend WeibullMax { WeibullMax(const WeibullMax & other) { return new OT::WeibullMax(other); } } }

