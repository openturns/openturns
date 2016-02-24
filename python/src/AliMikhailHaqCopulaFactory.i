// SWIG file AliMikhailHaqCopulaFactory.i

%{
#include "openturns/AliMikhailHaqCopulaFactory.hxx"
%}

%include AliMikhailHaqCopulaFactory_doc.i

%include openturns/AliMikhailHaqCopulaFactory.hxx
namespace OT { %extend AliMikhailHaqCopulaFactory { AliMikhailHaqCopulaFactory(const AliMikhailHaqCopulaFactory & other) { return new OT::AliMikhailHaqCopulaFactory(other); } } }
