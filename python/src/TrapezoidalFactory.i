// SWIG file TrapezoidalFactory.i

%{
#include "TrapezoidalFactory.hxx"
%}

%include TrapezoidalFactory_doc.i

%include TrapezoidalFactory.hxx
namespace OT { %extend TrapezoidalFactory { TrapezoidalFactory(const TrapezoidalFactory & other) { return new OT::TrapezoidalFactory(other); } } }
