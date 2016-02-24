// SWIG file WeibullFactory.i

%{
#include "openturns/WeibullFactory.hxx"
%}

%include WeibullFactory_doc.i

%include openturns/WeibullFactory.hxx
namespace OT { %extend WeibullFactory { WeibullFactory(const WeibullFactory & other) { return new OT::WeibullFactory(other); } } }
