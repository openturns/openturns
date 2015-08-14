// SWIG file LogisticFactory.i

%{
#include "LogisticFactory.hxx"
%}

%include LogisticFactory_doc.i

%include LogisticFactory.hxx
namespace OT { %extend LogisticFactory { LogisticFactory(const LogisticFactory & other) { return new OT::LogisticFactory(other); } } }
