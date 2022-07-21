// SWIG file WeibullMinFactory.i

%{
#include "openturns/WeibullMinFactory.hxx"
%}

%include WeibullMinFactory_doc.i

%include openturns/WeibullMinFactory.hxx
namespace OT { %extend WeibullMinFactory { WeibullMinFactory(const WeibullMinFactory & other) { return new OT::WeibullMinFactory(other); } } }

