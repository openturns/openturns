// SWIG file Basis.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/Basis.hxx"
%}

%include BaseFuncCollection.i

%typemap(in) const BasisCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  }
  else {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::Basis >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Basis");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const BasisCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OT::Basis >( $input );
}


%apply const BasisCollection & { const OT::Collection<OT::Basis> & };

%template(BasisCollection) OT::Collection<OT::Basis>;
%template(BasisPersistentCollection) OT::PersistentCollection<OT::Basis>;

%include Basis_doc.i

OTTypedInterfaceObjectHelper(Basis)
%ignore OT::Basis::operator[];

%include openturns/Basis.hxx

%extend OT::Collection<OT::NumericalMathFunction>
{

OT::Collection<OT::NumericalMathFunction> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::NumericalMathFunction>( pyObj );
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
  try
  {
    OT::Pointer<OT::Collection<OT::NumericalMathFunction> > p_coll =  OT::buildCollectionFromPySequence<OT::NumericalMathFunction>(pyObj);
    return new OT::Basis(*p_coll);
  }
  catch (OT::InvalidArgumentException & ex)
  {
    return new OT::Basis(OT::convert<OT::_PyObject_, OT::Basis >(pyObj));
  }
}



} // Basis
} // OT
