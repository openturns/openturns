// SWIG file TestResult.i

%{
#include "openturns/TestResult.hxx"

namespace OT { 

  template <>
  struct traitsPythonType<OT::TestResult>
  {
    typedef _PySequence_ Type;
  };

  template <>
  inline
  OT::TestResult
  convert<_PySequence_, OT::TestResult>(PyObject * pyObj)
  {
    check<_PySequence_>(pyObj);

    if (PySequence_Length(pyObj) != 4)
      throw InvalidArgumentException(HERE) << "Sequence passed as argument is not convertible to a TestResult. Expected sequence (String, Bool, Scalar, Scalar)";

    ScopedPyObjectPointer item_0(PySequence_GetItem(pyObj, 0)); check<_PyString_>(item_0.get());
    ScopedPyObjectPointer item_1(PySequence_GetItem(pyObj, 1)); check<_PyBool_>(item_1.get());
    ScopedPyObjectPointer item_2(PySequence_GetItem(pyObj, 2)); check<_PyFloat_>(item_2.get());
    ScopedPyObjectPointer item_3(PySequence_GetItem(pyObj, 3)); check<_PyFloat_>(item_3.get());
    ScopedPyObjectPointer item_4(PySequence_GetItem(pyObj, 4)); check<_PyFloat_>(item_4.get());

    return OT::TestResult(convert<_PyString_,String>(item_0.get()),
                                              convert<_PyBool_,Bool>(item_1.get()),
                                              convert<_PyFloat_,Scalar>(item_2.get()),
                                              convert<_PyFloat_,Scalar>(item_3.get()),
                                              convert<_PyFloat_,Scalar>(item_4.get()));
  }


} /* namespace OT */

%}

%template(TestResultCollection) OT::Collection<OT::TestResult>;

%include TestResult_doc.i

%include openturns/TestResult.hxx
namespace OT{  

%extend TestResult {

TestResult(const TestResult & other) { return new OT::TestResult(other); }

TestResult(PyObject * pyObj)
{
 return new OT::TestResult( OT::convert<OT::_PySequence_,OT::TestResult>(pyObj) );
}

}
}
