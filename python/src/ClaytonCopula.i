// SWIG file ClaytonCopula.i

%{
#include "openturns/ClaytonCopula.hxx"
%}

%include ClaytonCopula_doc.i

%include openturns/ClaytonCopula.hxx
namespace OT { %extend ClaytonCopula { ClaytonCopula(const ClaytonCopula & other) { return new OT::ClaytonCopula(other); } } }
