// SWIG file Analytical.i

%{
#include "Analytical.hxx"
%}

%include Analytical_doc.i

%include Analytical.hxx
namespace OT{ %extend Analytical { Analytical(const Analytical & other) { return new OT::Analytical(other); } } }
