
#include "openturns/IterativeAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(IterativeAlgorithm)

/* finalize */
void IterativeAlgorithm::finalize()
{
  // Nothing to do
}

/* String converter */
String IterativeAlgorithm::__repr__() const
{
  return OSS(true) << "class=" << getClassName();
}

/* Pretty string converter */
String IterativeAlgorithm::__str__(const String & ) const
{
  return __repr__();
}

END_NAMESPACE_OPENTURNS
