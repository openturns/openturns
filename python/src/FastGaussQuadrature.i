// SWIG file FastGaussQuadrature.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/FastGaussQuadrature.hxx"
#include "openturns/Point.hxx"
%}

%include FastGaussQuadrature_doc.i

%inline %{
  OT::Point FastGaussQuadrature(const OT::Point & gamma,
                                const OT::Point & b,
                                OT::Point & weightsOut)
  {
    const OT::UnsignedInteger n = gamma.getDimension();
    if (n == 0) throw OT::InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (b.getDimension() != n) throw OT::InvalidArgumentException(HERE) << "Error: b must be of dimension n";
    OT::Point nodes(n);
    weightsOut = OT::Point(n);
    OT::FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], n, &nodes[0], &weightsOut[0]);
    return nodes;
  }
%}