// SWIG file PlackettCopulaFactory.i

%{
#include "openturns/PlackettCopulaFactory.hxx"
%}

%include PlackettCopulaFactory_doc.i

%include openturns/PlackettCopulaFactory.hxx
namespace OT { %extend PlackettCopulaFactory { PlackettCopulaFactory(const PlackettCopulaFactory & other) { return new OT::PlackettCopulaFactory(other); } } }
