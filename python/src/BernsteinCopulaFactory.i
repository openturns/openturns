// SWIG file BernsteinCopulaFactory.i

%{
#include "openturns/BernsteinCopulaFactory.hxx"
%}

%include BernsteinCopulaFactory_doc.i

%include openturns/BernsteinCopulaFactory.hxx
namespace OT { %extend BernsteinCopulaFactory { BernsteinCopulaFactory(const BernsteinCopulaFactory & other) { return new OT::BernsteinCopulaFactory(other); } } }
