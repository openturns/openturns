// SWIG file Analytical.i

%{
#include "openturns/Analytical.hxx"
%}

%include Analytical_doc.i

%include openturns/Analytical.hxx
namespace OT{ %extend Analytical { Analytical(const Analytical & other) { return new OT::Analytical(other); } } }
