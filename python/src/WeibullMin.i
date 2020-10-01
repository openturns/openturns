// SWIG file WeibullMin.i

%{
#include "openturns/WeibullMin.hxx"
%}

%include WeibullMin_doc.i

%include openturns/WeibullMin.hxx
namespace OT { %extend WeibullMin { WeibullMin(const WeibullMin & other) { return new OT::WeibullMin(other); } } }

