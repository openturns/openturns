// SWIG file Trapezoidal.i

%{
#include "openturns/Trapezoidal.hxx"
%}

%include Trapezoidal_doc.i

%include openturns/Trapezoidal.hxx
namespace OT { %extend Trapezoidal { Trapezoidal(const Trapezoidal & other) { return new OT::Trapezoidal(other); } } }
