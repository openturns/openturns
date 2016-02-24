// SWIG file LaguerreFactory.i

%{
#include "openturns/LaguerreFactory.hxx"
%}

%include LaguerreFactory_doc.i

%include openturns/LaguerreFactory.hxx
namespace OT { %extend LaguerreFactory { LaguerreFactory(const LaguerreFactory & other) { return new OT::LaguerreFactory(other); } } }
