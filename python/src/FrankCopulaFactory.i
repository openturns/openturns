// SWIG file FrankCopulaFactory.i

%{
#include "openturns/FrankCopulaFactory.hxx"
%}

%include FrankCopulaFactory_doc.i

%include openturns/FrankCopulaFactory.hxx
namespace OT { %extend FrankCopulaFactory { FrankCopulaFactory(const FrankCopulaFactory & other) { return new OT::FrankCopulaFactory(other); } } }
