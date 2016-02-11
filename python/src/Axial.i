// SWIG file Axial.i

%{
#include "openturns/Axial.hxx"
%}

%include Axial_doc.i

%include openturns/Axial.hxx
namespace OT { %extend Axial { Axial(const Axial & other) { return new OT::Axial(other); } } }
