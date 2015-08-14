// SWIG file Axial.i

%{
#include "Axial.hxx"
%}

%include Axial_doc.i

%include Axial.hxx
namespace OT { %extend Axial { Axial(const Axial & other) { return new OT::Axial(other); } } }
