// SWIG file BernoulliFactory.i

%{
#include "BernoulliFactory.hxx"
%}

%include BernoulliFactory_doc.i

%include BernoulliFactory.hxx
namespace OT { %extend BernoulliFactory { BernoulliFactory(const BernoulliFactory & other) { return new OT::BernoulliFactory(other); } } }
