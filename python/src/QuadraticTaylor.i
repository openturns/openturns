// SWIG file QuadraticTaylor.i

%{
#include "QuadraticTaylor.hxx"
%}

%include QuadraticTaylor_doc.i

%include QuadraticTaylor.hxx
namespace OT { %extend QuadraticTaylor { QuadraticTaylor(const QuadraticTaylor & other) { return new OT::QuadraticTaylor(other); } } }
