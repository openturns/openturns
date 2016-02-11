// SWIG file ComposedCopula.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/ComposedCopula.hxx"
%}

%include ComposedCopula_doc.i

%template(CopulaCollection) OT::Collection<OT::Copula>;

%include openturns/ComposedCopula.hxx

%extend OT::Collection<OT::Copula>
{

OT::Collection<OT::Copula> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::Copula>( pyObj );
}

}

namespace OT {

%extend ComposedCopula {

ComposedCopula(PyObject * pyObj)
{
  OT::Pointer<OT::Collection<OT::Copula> > p_coll =  OT::buildCollectionFromPySequence<OT::Copula>( pyObj );
  return new OT::ComposedCopula( *p_coll );
}

ComposedCopula(const ComposedCopula & other)
{
  return new OT::ComposedCopula(other);
}

} // ComposedCopula
 
} // OT
