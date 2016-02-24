// SWIG file LogisticFactory.i

%{
#include "openturns/LogisticFactory.hxx"
%}

%include LogisticFactory_doc.i

%include openturns/LogisticFactory.hxx
namespace OT { %extend LogisticFactory { LogisticFactory(const LogisticFactory & other) { return new OT::LogisticFactory(other); } } }
