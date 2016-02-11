// SWIG file Polygon.i

%{
#include "openturns/Polygon.hxx"
%}

%include Polygon_doc.i

%include openturns/Polygon.hxx
namespace OT { %extend Polygon { Polygon(const Polygon & other) { return new OT::Polygon(other); } } }
