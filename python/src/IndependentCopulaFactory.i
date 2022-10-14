// SWIG file IndependentCopulaFactory.i

%{
#include "openturns/IndependentCopulaFactory.hxx"
%}

%include IndependentCopulaFactory_doc.i

%include openturns/IndependentCopulaFactory.hxx
namespace OT { %extend IndependentCopulaFactory { IndependentCopulaFactory(const IndependentCopulaFactory & other) { return new OT::IndependentCopulaFactory(other); } } }
