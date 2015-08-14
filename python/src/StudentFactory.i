// SWIG file StudentFactory.i

%{
#include "StudentFactory.hxx"
%}

%include StudentFactory_doc.i

%include StudentFactory.hxx
namespace OT { %extend StudentFactory { StudentFactory(const StudentFactory & other) { return new OT::StudentFactory(other); } } }
