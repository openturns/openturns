// SWIG file MetaModelResult.i

%{
#include "MetaModelResult.hxx"
%}

%include MetaModelResult_doc.i

%include MetaModelResult.hxx
namespace OT{ %extend MetaModelResult { MetaModelResult(const MetaModelResult & other) { return new OT::MetaModelResult(other); } } }
