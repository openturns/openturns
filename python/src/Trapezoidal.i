// SWIG file Trapezoidal.i

%{
#include "Trapezoidal.hxx"
%}

%include Trapezoidal_doc.i

%include Trapezoidal.hxx
namespace OT { %extend Trapezoidal { Trapezoidal(const Trapezoidal & other) { return new OT::Trapezoidal(other); } } }
