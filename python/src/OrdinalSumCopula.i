// SWIG file OrdinalSumCopula.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/OrdinalSumCopula.hxx"
%}


%include OrdinalSumCopula_doc.i


%include openturns/OrdinalSumCopula.hxx

namespace OT {

%extend OrdinalSumCopula {

     OrdinalSumCopula(PyObject * pyObj, const Point & bounds)
{
  OT::Pointer<OT::Collection<OT::Copula> > p_coll =  OT::buildCollectionFromPySequence<OT::Copula>( pyObj );
  return new OT::OrdinalSumCopula( *p_coll, bounds );
}

OrdinalSumCopula(const OrdinalSumCopula & other)
{
  return new OT::OrdinalSumCopula(other);
}

} // OrdinalSumCopula
 
} // OT
