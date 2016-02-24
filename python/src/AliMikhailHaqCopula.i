// SWIG file AliMikhailHaqCopula.i

%{
#include "openturns/AliMikhailHaqCopula.hxx"
%}

%include AliMikhailHaqCopula_doc.i

%include openturns/AliMikhailHaqCopula.hxx
namespace OT { %extend AliMikhailHaqCopula { AliMikhailHaqCopula(const AliMikhailHaqCopula & other) { return new OT::AliMikhailHaqCopula(other); } } }
