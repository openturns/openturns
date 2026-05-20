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

  OT::Scalar operator() (const OT::UnsignedInteger i, const OT::UnsignedInteger j) const override
  {
    OT::ScopedPyObjectPointer index1(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(i));
    OT::ScopedPyObjectPointer index2(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(j));
    OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj_, index1.get(), index2.get(), NULL));
    OT::handleException();
    const OT::Scalar value = OT::convert<OT::_PyFloat_, OT::Scalar>(result.get());
    return value;
  }
private:
  PyObject * pyObj_ = NULL;
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

  void compute(const OT::UnsignedInteger i, const OT::UnsignedInteger j, OT::Matrix* localValues) const override
  {
    OT::ScopedPyObjectPointer index1(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(i));
    OT::ScopedPyObjectPointer index2(OT::convert< OT::UnsignedInteger, OT::_PyInt_ >(j));
    OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj_, index1.get(), index2.get(), NULL));
    OT::handleException();
    void * ptr = nullptr;
    if (SWIG_IsOK(SWIG_ConvertPtr(result.get(), &ptr, SWIG_TypeQuery("OT::Matrix *"), SWIG_POINTER_NO_NULL))) {
      OT::Matrix *p_matrix = reinterpret_cast< OT::Matrix * >(ptr);
      *localValues = *p_matrix;
    }
    else
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Matrix";
  }
private:
  PyObject * pyObj_ = NULL;
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
