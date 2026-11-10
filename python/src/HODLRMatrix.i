// SWIG file HODLRMatrix.i

%{
#include "openturns/HODLRMatrix.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

// The assembly of a Python callable always runs serially on the calling
// thread, which holds the GIL: parallel assembly is disabled for such
// evaluators (see HODLREntryEvaluator::isParallel, same rationale as
// PythonEvaluation::isParallel), so no GIL acquire/release logic is needed.
static OT::Scalar callPythonEvaluator(PyObject * pyObj, OT::UnsignedInteger i, OT::UnsignedInteger j)
{
    OT::ScopedPyObjectPointer index1(OT::convert< OT::UnsignedInteger, OT::_PyLong_ >(i));
    OT::ScopedPyObjectPointer index2(OT::convert< OT::UnsignedInteger, OT::_PyLong_ >(j));
    OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, index1.get(), index2.get(), NULL));
    OT::handleException();
    return OT::convert<OT::_PyFloat_, OT::Scalar>(result.get());
}

// Keeps the callable alive as long as an evaluator referring to it exists
struct SafePyObjectRef {
    PyObject* ptr;
    SafePyObjectRef(PyObject* p) : ptr(p) { Py_XINCREF(ptr); }
    SafePyObjectRef(const SafePyObjectRef& o) : ptr(o.ptr) { Py_XINCREF(ptr); }
    SafePyObjectRef& operator=(const SafePyObjectRef& o) { if (ptr != o.ptr) { Py_XDECREF(ptr); ptr = o.ptr; Py_XINCREF(ptr); } return *this; }
    ~SafePyObjectRef() { Py_XDECREF(ptr); }
};

%}

%include HODLRMatrix_doc.i

%template(_HODLRMatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::HODLRMatrixImplementation>;
%template() std::pair< size_t, size_t >;

%copyctor OT::HODLRMatrix;

%include openturns/HODLRMatrix.hxx

namespace OT {

%extend HODLRMatrix {

  void assembleReal(PyObject * callable, char symmetry) {
    if (!PyCallable_Check(callable)) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
    }
    OT::UnsignedInteger n = self->getNbRows();
    SafePyObjectRef ref(callable);
    // A Python callable cannot run on TBB worker threads: disable parallel
    // assembly for it (free-threaded builds have no GIL, so it stays allowed)
#ifdef Py_GIL_DISABLED
    const OT::Bool parallel = true;
#else
    const OT::Bool parallel = false;
#endif
    OT::HODLRFunctionEvaluator evaluator(
      [ref](OT::UnsignedInteger i, OT::UnsignedInteger j) -> OT::Scalar {
        return callPythonEvaluator(ref.ptr, i, j);
      }, n, parallel);
    self->assemble(evaluator, symmetry);
  }

} // HODLRMatrix
} // OT
