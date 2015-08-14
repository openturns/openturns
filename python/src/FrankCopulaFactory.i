// SWIG file FrankCopulaFactory.i

%{
#include "FrankCopulaFactory.hxx"
%}

%include FrankCopulaFactory_doc.i

%include FrankCopulaFactory.hxx
namespace OT { %extend FrankCopulaFactory { FrankCopulaFactory(const FrankCopulaFactory & other) { return new OT::FrankCopulaFactory(other); } } }
