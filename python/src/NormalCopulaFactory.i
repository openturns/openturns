// SWIG file NormalCopulaFactory.i

%{
#include "NormalCopulaFactory.hxx"
%}

%include NormalCopulaFactory_doc.i

%include NormalCopulaFactory.hxx
namespace OT { %extend NormalCopulaFactory { NormalCopulaFactory(const NormalCopulaFactory & other) { return new OT::NormalCopulaFactory(other); } } }
