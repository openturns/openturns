// SWIG file TruncatedNormalFactory.i

%{
#include "openturns/TruncatedNormalFactory.hxx"
%}

%include TruncatedNormalFactory_doc.i

%include openturns/TruncatedNormalFactory.hxx
namespace OT { %extend TruncatedNormalFactory { TruncatedNormalFactory(const TruncatedNormalFactory & other) { return new OT::TruncatedNormalFactory(other); } } }
