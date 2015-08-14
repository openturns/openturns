// SWIG file Contour.i

%{
#include "Contour.hxx"
%}

%include Contour_doc.i

%include Contour.hxx
namespace OT { %extend Contour { Contour(const Contour & other) { return new OT::Contour(other); } } }
