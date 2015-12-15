// SWIG file typ_module.i

%module(package="openturns", docstring="Internal data types.") typ
//%feature("autodoc","1");

%pythoncode %{
def getCorrelationMatrixFromMap(Vars, Map):
    """
    Convert dictionary to CorrelationMatrix.

    Transform a two-dimension dict whose keys are listed in Vars into a CorrelationMatrix.
    The values are numerical.
    No need to define all the elements in the map since CorrelationMatrix is symmetric.
    
    Examples
    --------
      Vars=['X','Y','Z']
      Map={}
      Map['X']={}
      Map['X']['Y']= 0.1
      Map['X']['Z']= 0.7
      Map['Y']={}
      Map['Y']['Z']= 0.8
      R = getCorrelationMatrixFromMap(Vars,Map)
    """
    corr = CorrelationMatrix(len(Vars))
    i = 0
    for first in Vars:
        j = 0
        for second in Vars:
            if first in Map and second in Map[first]:
                corr[i, j] = Map[first][second]
            j += 1
        i += 1
    return corr
%}

%{
#include "OTconfig.hxx"
#include "OTCommon.hxx"
#include "OTType.hxx"
#include "OTGeom.hxx"
#include "OTStat.hxx"
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
%include Indices.i
%include NumericalPoint.i
%include NumericalPointWithDescription.i
%include DomainImplementation.i
%include Interval.i
%include MatrixImplementation.i
%include Matrix.i
%include SquareMatrix.i
%include SymmetricMatrix.i
%include TensorImplementation.i
%include Tensor.i
%include SymmetricTensor.i
%include Cache.i
%include ComplexMatrixImplementation.i
%include ComplexMatrix.i
%include SquareComplexMatrix.i
%include HermitianMatrix.i
%include TriangularMatrix.i
%include TriangularComplexMatrix.i
%include ComplexTensorImplementation.i
%include ComplexTensor.i
%include CovarianceMatrix.i
%include CorrelationMatrix.i 
%include IdentityMatrix.i 
%include NumericalSample.i 
%include HistoryStrategyImplementation.i 
%include HistoryStrategy.i

/* At last we include template definitions */
%include BaseTypTemplateDefs.i

%pythoncode %{
class BoolCollection(UnsignedIntegerCollection):
    """Proxy of C++ OT::BoolCollection class"""
    pass
%}
