// SWIG file ClaytonCopula.i

%{
#include "ClaytonCopula.hxx"
%}

%include ClaytonCopula_doc.i

%include ClaytonCopula.hxx
namespace OT { %extend ClaytonCopula { ClaytonCopula(const ClaytonCopula & other) { return new OT::ClaytonCopula(other); } } }
