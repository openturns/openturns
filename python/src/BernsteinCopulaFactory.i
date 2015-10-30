// SWIG file BernsteinCopulaFactory.i

%{
#include "BernsteinCopulaFactory.hxx"
%}

%include BernsteinCopulaFactory_doc.i

%include BernsteinCopulaFactory.hxx
namespace OT { %extend BernsteinCopulaFactory { BernsteinCopulaFactory(const BernsteinCopulaFactory & other) { return new OT::BernsteinCopulaFactory(other); } } }
