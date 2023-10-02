// SWIG file StudentCopula.i

%{
#include "openturns/StudentCopula.hxx"
%}

%include StudentCopula_doc.i

%include openturns/StudentCopula.hxx
namespace OT { %extend StudentCopula { StudentCopula(const StudentCopula & other) { return new OT::StudentCopula(other); } } }
