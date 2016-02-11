// SWIG file MetaModelResult.i

%{
#include "openturns/MetaModelResult.hxx"
%}

%include MetaModelResult_doc.i

%include openturns/MetaModelResult.hxx
namespace OT{ %extend MetaModelResult { MetaModelResult(const MetaModelResult & other) { return new OT::MetaModelResult(other); } } }
