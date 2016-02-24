// SWIG file NormalCopulaFactory.i

%{
#include "openturns/NormalCopulaFactory.hxx"
%}

%include NormalCopulaFactory_doc.i

%include openturns/NormalCopulaFactory.hxx
namespace OT { %extend NormalCopulaFactory { NormalCopulaFactory(const NormalCopulaFactory & other) { return new OT::NormalCopulaFactory(other); } } }
