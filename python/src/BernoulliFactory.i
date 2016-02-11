// SWIG file BernoulliFactory.i

%{
#include "openturns/BernoulliFactory.hxx"
%}

%include BernoulliFactory_doc.i

%include openturns/BernoulliFactory.hxx
namespace OT { %extend BernoulliFactory { BernoulliFactory(const BernoulliFactory & other) { return new OT::BernoulliFactory(other); } } }
