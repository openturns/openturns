// SWIG file GammaFactory.i

%{
#include "openturns/GammaFactory.hxx"
%}

%include GammaFactory_doc.i

%include openturns/GammaFactory.hxx
namespace OT { %extend GammaFactory { GammaFactory(const GammaFactory & other) { return new OT::GammaFactory(other); } } }
