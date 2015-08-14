// SWIG file ArchimedeanCopula.i

%{
#include "ArchimedeanCopula.hxx"
%}

%include ArchimedeanCopula_doc.i

%include ArchimedeanCopula.hxx
namespace OT { %extend ArchimedeanCopula { ArchimedeanCopula(const ArchimedeanCopula & other) { return new OT::ArchimedeanCopula(other); } } }
