// SWIG file WeibullMaxFactory.i

%{
#include "openturns/WeibullMaxFactory.hxx"
%}

%include WeibullMaxFactory_doc.i

%include openturns/WeibullMaxFactory.hxx
namespace OT { %extend WeibullMaxFactory { WeibullMaxFactory(const WeibullMaxFactory & other) { return new OT::WeibullMaxFactory(other); } } }

