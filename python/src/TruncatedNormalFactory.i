// SWIG file TruncatedNormalFactory.i

%{
#include "TruncatedNormalFactory.hxx"
%}

%include TruncatedNormalFactory_doc.i

%include TruncatedNormalFactory.hxx
namespace OT { %extend TruncatedNormalFactory { TruncatedNormalFactory(const TruncatedNormalFactory & other) { return new OT::TruncatedNormalFactory(other); } } }
