// SWIG file UniVariatePolynomial.i

%{
#include "openturns/UniVariatePolynomial.hxx"

namespace OT { 

  template <>
  struct traitsPythonType<OT::UniVariatePolynomial>
  {
    typedef _PySequence_ Type;
  };


  template <>
  inline
  OT::UniVariatePolynomial
  convert<_PySequence_,OT::UniVariatePolynomial>(PyObject * pyObj)
  {
    return OT::convert<OT::_PySequence_,OT::Point>(pyObj);
  }

} /* namespace OT */

%}

%include UniVariatePolynomial_doc.i

OTTypedInterfaceObjectHelper(UniVariatePolynomial)

%include openturns/UniVariatePolynomial.hxx
namespace OT {  

%extend UniVariatePolynomial {

UniVariatePolynomial(const UniVariatePolynomial & other) { return new OT::UniVariatePolynomial(other); }

UniVariatePolynomial(PyObject * pyObj)
{
 return new OT::UniVariatePolynomial( OT::convert<OT::_PySequence_,OT::Point>(pyObj) );
}

}

}

%pythoncode %{
def __UniVariatePolynomial_repr_html(self):
    """Get HTML representation."""
    return self.__repr_html__()

UniVariatePolynomial._repr_html_ = __UniVariatePolynomial_repr_html
%}
