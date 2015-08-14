// SWIG file MaximumEntropyOrderStatisticsCopula.i

%{
#include "MaximumEntropyOrderStatisticsCopula.hxx"
%}

%include MaximumEntropyOrderStatisticsCopula_doc.i

%include MaximumEntropyOrderStatisticsCopula.hxx
namespace OT { %extend MaximumEntropyOrderStatisticsCopula { MaximumEntropyOrderStatisticsCopula(const MaximumEntropyOrderStatisticsCopula & other) { return new OT::MaximumEntropyOrderStatisticsCopula(other); } } }
