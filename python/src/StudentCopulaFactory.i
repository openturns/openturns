// SWIG file StudentCopulaFactory.i

%{
#include "openturns/StudentCopulaFactory.hxx"
%}

%include StudentCopulaFactory_doc.i

%include openturns/StudentCopulaFactory.hxx
namespace OT { %extend StudentCopulaFactory { StudentCopulaFactory(const StudentCopulaFactory & other) { return new OT::StudentCopulaFactory(other); } } }
