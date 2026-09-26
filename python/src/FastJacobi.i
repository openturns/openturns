// SWIG file FastJacobi.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/FastJacobi.hxx"
#include "openturns/Point.hxx"
%}

%include FastJacobi_doc.i

%inline %{
  OT::Point FastJacobi(const OT::UnsignedInteger n,
                       const OT::Scalar alpha,
                       const OT::Scalar beta,
                       OT::Point & weightsOut)
  {
    if (n == 0) throw OT::InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (alpha <= -1.0) throw OT::InvalidArgumentException(HERE) << "Error: alpha must be > -1";
    if (beta <= -1.0) throw OT::InvalidArgumentException(HERE) << "Error: beta must be > -1";
    OT::Point nodes(n);
    weightsOut = OT::Point(n);
    OT::FastJacobi::ComputeNodesAndWeights(n, alpha, beta, &nodes[0], &weightsOut[0]);
    return nodes;
  }
%}