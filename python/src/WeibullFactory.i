// SWIG file WeibullFactory.i

%{
#include "WeibullFactory.hxx"
%}

%include WeibullFactory_doc.i

%include WeibullFactory.hxx
namespace OT { %extend WeibullFactory { WeibullFactory(const WeibullFactory & other) { return new OT::WeibullFactory(other); } } }
