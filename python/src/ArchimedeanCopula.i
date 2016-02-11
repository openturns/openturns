// SWIG file ArchimedeanCopula.i

%{
#include "openturns/ArchimedeanCopula.hxx"
%}

%include ArchimedeanCopula_doc.i

%include openturns/ArchimedeanCopula.hxx
namespace OT { %extend ArchimedeanCopula { ArchimedeanCopula(const ArchimedeanCopula & other) { return new OT::ArchimedeanCopula(other); } } }
