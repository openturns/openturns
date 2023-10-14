// SWIG file HMatrix.i

%{
#include "openturns/HMatrix.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

class PythonHMatrixRealAssemblyFunction : public OT::HMatrixRealAssemblyFunction
{
public:
  PythonHMatrixRealAssemblyFunction(PyObject * pyObj)
  : pyObj_(pyObj)
  {
    if (!PyCallable_Check(pyObj)) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
    }
  }

  virtual OT::Scalar operator() (OT::UnsignedInteger i, OT::UnsignedInteger j) const
  {
    OT::ScopedPyObjectPointer index1(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(i));
    OT::ScopedPyObjectPointer index2(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(j));
    OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj_, index1.get(), index2.get(), NULL));
    OT::Scalar value = OT::convert<OT::_PyFloat_, OT::Scalar>(result.get());
    return value;
  }
private:
  PyObject * pyObj_;
};

class PythonHMatrixTensorRealAssemblyFunction : public OT::HMatrixTensorRealAssemblyFunction
{
public:
  PythonHMatrixTensorRealAssemblyFunction(PyObject * pyObj, const OT::UnsignedInteger outputDimension)
  : HMatrixTensorRealAssemblyFunction(outputDimension), pyObj_(pyObj)
  {
    if (!PyCallable_Check(pyObj)) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
    }
  }

  virtual void compute(OT::UnsignedInteger i, OT::UnsignedInteger j, OT::Matrix* localValues) const
  {
    OT::ScopedPyObjectPointer index1(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(i));
    OT::ScopedPyObjectPointer index2(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(j));
    OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj_, index1.get(), index2.get(), NULL));
    OT::Matrix value(OT::convert<OT::_PySequence_, OT::Matrix>(result.get()));
    *localValues = value;
  }
private:
  PyObject * pyObj_;
};

%}

%include HMatrix_doc.i

%template(_HMatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::HMatrixImplementation>;
%template() std::pair< size_t, size_t >;

%include openturns/HMatrix.hxx

namespace OT {

%extend HMatrix {

  HMatrix(const HMatrix & other) { return new OT::HMatrix(other); }

  void assembleReal(PyObject * callable, char symmetry) {
    PythonHMatrixRealAssemblyFunction f(callable);
    self->assemble(f, symmetry);
  }

  void assembleTensor(PyObject * callable, const UnsignedInteger outputDimension, char symmetry) {
    PythonHMatrixTensorRealAssemblyFunction f(callable, outputDimension);
    self->assemble(f, symmetry);
  }

} // HMatrix
} // OT
