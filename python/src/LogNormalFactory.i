// SWIG file LogNormalFactory.i

%{
#include "openturns/LogNormalFactory.hxx"
%}

%include LogNormalFactory_doc.i

%include openturns/LogNormalFactory.hxx
namespace OT { %extend LogNormalFactory { LogNormalFactory(const LogNormalFactory & other) { return new OT::LogNormalFactory(other); } } }
