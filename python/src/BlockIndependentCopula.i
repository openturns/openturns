// SWIG file ComposedCopula.i

%{
#include "openturns/ComposedCopula.hxx"
%}

%include ComposedCopula_doc.i

%include openturns/ComposedCopula.hxx

namespace OT {

%extend ComposedCopula {

ComposedCopula(const ComposedCopula & other)
{
  return new OT::ComposedCopula(other);
}

} // ComposedCopula
 
} // OT
