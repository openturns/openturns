// SWIG file MaximumEntropyOrderStatisticsCopula.i

%{
#include "openturns/MaximumEntropyOrderStatisticsCopula.hxx"
%}

%include MaximumEntropyOrderStatisticsCopula_doc.i

%include openturns/MaximumEntropyOrderStatisticsCopula.hxx
namespace OT { %extend MaximumEntropyOrderStatisticsCopula { MaximumEntropyOrderStatisticsCopula(const MaximumEntropyOrderStatisticsCopula & other) { return new OT::MaximumEntropyOrderStatisticsCopula(other); } } }
