// SWIG file StudentFactory.i

%{
#include "openturns/StudentFactory.hxx"
%}

%include StudentFactory_doc.i

%include openturns/StudentFactory.hxx
namespace OT { %extend StudentFactory { StudentFactory(const StudentFactory & other) { return new OT::StudentFactory(other); } } }
