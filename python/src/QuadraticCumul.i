// SWIG file QuadraticCumul.i

%{
#include "QuadraticCumul.hxx"
%}

%include QuadraticCumul_doc.i

%include QuadraticCumul.hxx
namespace OT{ %extend QuadraticCumul { QuadraticCumul(const QuadraticCumul & other) { return new OT::QuadraticCumul(other); } } }
