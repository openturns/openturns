// SWIG file Polygon.i

%{
#include "Polygon.hxx"
%}

%include Polygon_doc.i

%include Polygon.hxx
namespace OT { %extend Polygon { Polygon(const Polygon & other) { return new OT::Polygon(other); } } }
