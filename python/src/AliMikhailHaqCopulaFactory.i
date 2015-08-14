// SWIG file AliMikhailHaqCopulaFactory.i

%{
#include "AliMikhailHaqCopulaFactory.hxx"
%}

%include AliMikhailHaqCopulaFactory_doc.i

%include AliMikhailHaqCopulaFactory.hxx
namespace OT { %extend AliMikhailHaqCopulaFactory { AliMikhailHaqCopulaFactory(const AliMikhailHaqCopulaFactory & other) { return new OT::AliMikhailHaqCopulaFactory(other); } } }
