// SWIG file Curve.i

%{
#include "Curve.hxx"
%}

%include Curve_doc.i

%include Curve.hxx
namespace OT { %extend Curve { Curve(const Curve & other) { return new OT::Curve(other); } } }
