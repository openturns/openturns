// SWIG file StudentFactory.i

%{
#include "openturns/StudentFactory.hxx"
%}

%include StudentFactory_doc.i

%copyctor OT::StudentFactory;

%include openturns/StudentFactory.hxx
