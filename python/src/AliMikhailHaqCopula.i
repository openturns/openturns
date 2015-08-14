// SWIG file AliMikhailHaqCopula.i

%{
#include "AliMikhailHaqCopula.hxx"
%}

%include AliMikhailHaqCopula_doc.i

%include AliMikhailHaqCopula.hxx
namespace OT { %extend AliMikhailHaqCopula { AliMikhailHaqCopula(const AliMikhailHaqCopula & other) { return new OT::AliMikhailHaqCopula(other); } } }
