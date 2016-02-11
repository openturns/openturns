// SWIG file Contour.i

%{
#include "openturns/Contour.hxx"
%}

%include Contour_doc.i

%include openturns/Contour.hxx
namespace OT { %extend Contour { Contour(const Contour & other) { return new OT::Contour(other); } } }
