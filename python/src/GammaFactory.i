// SWIG file GammaFactory.i

%{
#include "GammaFactory.hxx"
%}

%include GammaFactory_doc.i

%include GammaFactory.hxx
namespace OT { %extend GammaFactory { GammaFactory(const GammaFactory & other) { return new OT::GammaFactory(other); } } }
