// SWIG file Basis.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/Basis.hxx"
%}

%include BaseFuncCollection.i

%typemap(in) const BasisCollection & (OT::Pointer<OT::Collection<OT::Basis> > temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  }
  else {
    try {
      temp = OT::buildCollectionFromPySequence< OT::Basis >($input);
      $1 = temp.get();
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Basis");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const BasisCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< OT::Basis >($input);
}


%apply const BasisCollection & { const OT::Collection<OT::Basis> & };

%template(BasisCollection) OT::Collection<OT::Basis>;
%template(BasisPersistentCollection) OT::PersistentCollection<OT::Basis>;

%include Basis_doc.i

OTTypedInterfaceObjectHelper(Basis)
%ignore OT::Basis::operator[];

%include openturns/Basis.hxx

%extend OT::Collection<OT::Function>
{

OT::Collection<OT::Function> (PyObject * pyObj)
{
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  return OT::buildCollectionFromPySequence<OT::Function>(pyObj);
}


}




namespace OT {

%extend Basis {

Basis(const Basis & other)
{
  return new OT::Basis(other);
}

UnsignedInteger __len__() const
{
  return self->getSize();
}



Basis(PyObject * pyObj)
{
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  try
  {
    OT::Pointer<OT::Collection<OT::Function> > p_coll =  OT::buildCollectionFromPySequence<OT::Function>( pyObj );
  return new OT::Basis( *p_coll );
  }
  catch (OT::InvalidArgumentException &)
  {
    return new OT::Basis(OT::convert<OT::_PyObject_, OT::Basis >(pyObj));
  }
}

} // Basis
} // OT
