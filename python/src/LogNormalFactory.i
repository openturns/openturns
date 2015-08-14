// SWIG file LogNormalFactory.i

%{
#include "LogNormalFactory.hxx"
%}

%include LogNormalFactory_doc.i

%include LogNormalFactory.hxx
namespace OT { %extend LogNormalFactory { LogNormalFactory(const LogNormalFactory & other) { return new OT::LogNormalFactory(other); } } }
