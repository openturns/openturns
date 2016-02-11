// SWIG file TrapezoidalFactory.i

%{
#include "openturns/TrapezoidalFactory.hxx"
%}

%include TrapezoidalFactory_doc.i

%include openturns/TrapezoidalFactory.hxx
namespace OT { %extend TrapezoidalFactory { TrapezoidalFactory(const TrapezoidalFactory & other) { return new OT::TrapezoidalFactory(other); } } }
