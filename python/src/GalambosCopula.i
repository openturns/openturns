// SWIG file GalambosCopula.i

%{
#include "openturns/GalambosCopula.hxx"
%}

%include GalambosCopula_doc.i

%include openturns/GalambosCopula.hxx
namespace OT { %extend GalambosCopula { GalambosCopula(const GalambosCopula & other) { return new OT::GalambosCopula(other); } } }
