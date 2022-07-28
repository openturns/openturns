// SWIG file OTexceptions.i

%{
#include "openturns/Exception.hxx"
%}

namespace std {
  class exception
  {
  public:
    exception() throw() { }
    virtual ~exception() throw();
    /** Returns a C-style character string describing the general cause
     *  of the current error.  */
    virtual const char* what() const throw();
  };
}

%include exception.i

%exception {
  std::signal(SIGINT, OT::SignalHandler);
  try
  {
    $action
  }
  catch (const OT::InvalidArgumentException & ex) {
    SWIG_exception(SWIG_TypeError,ex.__repr__().c_str());
  }
  catch (const OT::OutOfBoundException & ex) {
    SWIG_exception(SWIG_IndexError, ex.__repr__().c_str());
  }
  catch (const OT::InterruptionException & ex) {
    SWIG_exception(SWIG_RuntimeError, "Interruption in $decl");
  }
  catch (const OT::Exception & ex) {
    SWIG_exception(SWIG_RuntimeError, ex.__repr__().c_str());
  }
  catch (const std::range_error & ex) {
    SWIG_exception(SWIG_IndexError, ex.what());
  }
  catch (const std::out_of_range & ex) {
    SWIG_exception(SWIG_IndexError, ex.what());
  }
  catch (const std::logic_error & ex) {
    SWIG_exception(SWIG_IndexError, ex.what());
  }
  catch (const std::exception & ex) {
    SWIG_exception(SWIG_RuntimeError, ex.what());
  }
}
