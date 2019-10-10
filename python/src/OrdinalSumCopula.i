// SWIG file OrdinalSumCopula.i

%{
#include "openturns/OrdinalSumCopula.hxx"
%}


%include OrdinalSumCopula_doc.i


%include openturns/OrdinalSumCopula.hxx

namespace OT {

%extend OrdinalSumCopula {

OrdinalSumCopula(const OrdinalSumCopula & other)
{
  return new OT::OrdinalSumCopula(other);
}

} // OrdinalSumCopula
 
} // OT
