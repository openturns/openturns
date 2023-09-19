// SWIG file typ_module.i

%module(package="openturns", docstring="Internal data types.") typ
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTType.hxx"
#include "openturns/OTGeom.hxx"
#include "openturns/OTStat.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i

/* Base/Type */
%include Collection.i
%template (UnsignedIntegerCollection) OT::Collection<OT::UnsignedInteger>;
%include PersistentCollection.i
%include Description.i
%include PlatformInfo.i
%include Indices.i
%include IndicesCollectionImplementation.i
%include IndicesCollection.i
%include Point.i
%include PointWithDescription.i
%include MatrixImplementation.i
%include Matrix.i
%include SquareMatrix.i
%include SymmetricMatrix.i
%include TensorImplementation.i
%include Tensor.i
%include SymmetricTensor.i
%include Cache.i
%include ComplexMatrix.i
%include SquareComplexMatrix.i
%include HermitianMatrix.i
%include TriangularMatrix.i
%include TriangularComplexMatrix.i
%include ComplexTensor.i
%include CovarianceMatrix.i
%include CorrelationMatrix.i 
%include IdentityMatrix.i 
%include Sample.i
%include DomainImplementation.i
%include Interval.i
%include HistoryStrategyImplementation.i 
%include HistoryStrategy.i

/* At last we include template definitions */
%include BaseTypTemplateDefs.i

%pythoncode %{
class BoolCollection(UnsignedIntegerCollection):
    """Proxy of C++ OT::BoolCollection class."""
    pass
%}
