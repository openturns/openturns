// SWIG file NormalFactory.i

%{
#include "openturns/NormalFactory.hxx"
%}

%include NormalFactory_doc.i

%include openturns/NormalFactory.hxx
namespace OT { %extend NormalFactory { NormalFactory(const NormalFactory & other) { return new OT::NormalFactory(other); } } }
