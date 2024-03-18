// SWIG file BlockIndependentCopula.i

%{
#include "openturns/BlockIndependentCopula.hxx"
%}

%include BlockIndependentCopula_doc.i

%include openturns/BlockIndependentCopula.hxx

namespace OT {

%extend BlockIndependentCopula {

BlockIndependentCopula(const BlockIndependentCopula & other)
{
  return new OT::BlockIndependentCopula(other);
}

} // BlockIndependentCopula
 
} // OT
