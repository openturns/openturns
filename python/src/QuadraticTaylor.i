// SWIG file QuadraticTaylor.i

%{
#include "openturns/QuadraticTaylor.hxx"
%}

%include QuadraticTaylor_doc.i

%include openturns/QuadraticTaylor.hxx
namespace OT { %extend QuadraticTaylor { QuadraticTaylor(const QuadraticTaylor & other) { return new OT::QuadraticTaylor(other); } } }
