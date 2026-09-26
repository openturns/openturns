// SWIG file HODLRMatrix.i

%{
#include "openturns/HODLRMatrix.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

// Holds the GIL for the duration of the scope and releases it even if the
// scope is left through an exception. The assembly is parallelized
// (HODLRMatrix-ParallelAssembly), so the evaluator and its stored callback may
// be used from a worker thread, which does not hold the GIL.
// PyGILState_Ensure is reentrant and almost free when the calling thread
// already holds it.
class ScopedGIL {
public:
  ScopedGIL() : state_(PyGILState_Ensure()) {}
  ~ScopedGIL() { PyGILState_Release(state_); }
private:
  PyGILState_STATE state_;
};

// Releases the GIL while the library works, so that a worker thread evaluating
// the Python callback can acquire it. Without this the main thread would hold
// the GIL while waiting for the workers, which wait for the GIL.
class ScopedPyThreadRelease {
public:
  ScopedPyThreadRelease() : state_(PyEval_SaveThread()) {}
  ~ScopedPyThreadRelease() { PyEval_RestoreThread(state_); }
private:
  PyThreadState * state_;
};

static OT::Scalar callPythonEvaluator(PyObject * pyObj, OT::UnsignedInteger i, OT::UnsignedInteger j)
{
    ScopedGIL gil;
    OT::ScopedPyObjectPointer index1(OT::convert< OT::UnsignedInteger, OT::_PyLong_ >(i));
    OT::ScopedPyObjectPointer index2(OT::convert< OT::UnsignedInteger, OT::_PyLong_ >(j));
    OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, index1.get(), index2.get(), NULL));
    OT::handleException();
    return OT::convert<OT::_PyFloat_, OT::Scalar>(result.get());
}

// Keeps the callback alive as long as an evaluator referring to it exists. The
// evaluator is cloned by the assembly, possibly from a worker thread, so every
// reference count update takes the GIL.
struct SafePyObjectRef {
    PyObject* ptr;
    SafePyObjectRef(PyObject* p) : ptr(p) { ScopedGIL gil; Py_XINCREF(ptr); }
    SafePyObjectRef(const SafePyObjectRef& o) : ptr(o.ptr) { ScopedGIL gil; Py_XINCREF(ptr); }
    SafePyObjectRef& operator=(const SafePyObjectRef& o) { ScopedGIL gil; if (ptr != o.ptr) { Py_XDECREF(ptr); ptr = o.ptr; Py_XINCREF(ptr); } return *this; }
    ~SafePyObjectRef() { ScopedGIL gil; Py_XDECREF(ptr); }
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
    OT::HODLRFunctionEvaluator evaluator(
      [ref](OT::UnsignedInteger i, OT::UnsignedInteger j) -> OT::Scalar {
        return callPythonEvaluator(ref.ptr, i, j);
      }, n);
    // The GIL is released here: a parallel assembly evaluates the callback from
    // worker threads, and the callback cannot run without the GIL. The
    // evaluation of a Python callback is then serialized, which is the price
    // of the interpreter.
    {
      ScopedPyThreadRelease unlock;
      self->assemble(evaluator, symmetry);
    }
  }

} // HODLRMatrix
} // OT
