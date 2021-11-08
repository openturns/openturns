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
  convert<_PySequence_,OT::TestResult>(PyObject * pyObj)
  {
    check<_PySequence_>( pyObj );
    ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));

    if (PySequence_Fast_GET_SIZE( newPyObj.get() ) != 4) 
      throw InvalidArgumentException(HERE) << "Sequence passed as argument is not convertible to a TestResult. Expected sequence (String, Bool, Scalar, Scalar)";

    PyObject * item_0 = PySequence_Fast_GET_ITEM( newPyObj.get(), 0 ); check<_PyString_>( item_0 );
    PyObject * item_1 = PySequence_Fast_GET_ITEM( newPyObj.get(), 1 ); check<_PyBool_>( item_1 );
    PyObject * item_2 = PySequence_Fast_GET_ITEM( newPyObj.get(), 2 ); check<_PyFloat_>( item_2 );
    PyObject * item_3 = PySequence_Fast_GET_ITEM( newPyObj.get(), 3 ); check<_PyFloat_>( item_3 );
    PyObject * item_4 = PySequence_Fast_GET_ITEM( newPyObj.get(), 4 ); check<_PyFloat_>( item_4 );

    return OT::TestResult( convert<_PyString_,String>( item_0 ),
                                              convert<_PyBool_,Bool>( item_1 ),
                                              convert<_PyFloat_,Scalar>( item_2 ),
                                              convert<_PyFloat_,Scalar>( item_3 ),
                                              convert<_PyFloat_,Scalar>( item_4 ) );
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
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  return new OT::TestResult(OT::convert<OT::_PySequence_, OT::TestResult>(pyObj));
}

}
}
