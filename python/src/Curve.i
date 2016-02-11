// SWIG file Curve.i

%{
#include "openturns/Curve.hxx"
%}

%include Curve_doc.i

%include openturns/Curve.hxx
namespace OT { %extend Curve { Curve(const Curve & other) { return new OT::Curve(other); } } }
