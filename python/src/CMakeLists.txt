#                                               -*- cmake -*-

add_definitions (${OPENTURNS_SWIG_DEFINITIONS})

set (CMAKE_SWIG_FLAGS "" CACHE STRING "SWIG flags for generating wrapper code")

# allows one to pass compile flags like -O1 to reduce memory usage
set (SWIG_COMPILE_FLAGS "" CACHE STRING "C/C++ compiler flags used for wrapper code")
set (SWIG_CXX_COMPILE_FLAGS "" CACHE STRING "C++ specific compiler flags used for wrapper code")

# generate SWIG runtime header
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/openturns)
add_custom_command (OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/openturns/swigpyrun.h
                     COMMAND ${SWIG_EXECUTABLE}
                     ARGS -python -external-runtime ${CMAKE_CURRENT_BINARY_DIR}/openturns/swigpyrun.h
                     COMMENT "Swig runtime header")
add_custom_target (generate_swig_runtime DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/openturns/swigpyrun.h)

# Build Python wrapping classes
add_library (OTPythonWrapping OBJECT
                      PythonEvaluation.cxx
                      PythonGradient.cxx
                      PythonHessian.cxx
                      PythonExperiment.cxx
                      PythonRandomVector.cxx
                      PythonDistribution.cxx
                      PythonFieldFunction.cxx
                      PythonFieldToPointFunction.cxx
                      PythonPointToFieldFunction.cxx)
add_dependencies (OTPythonWrapping generate_swig_runtime)
target_include_directories (OTPythonWrapping PRIVATE ${Python_INCLUDE_DIRS})
target_include_directories (OTPythonWrapping PRIVATE ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} BEFORE)
set_target_properties (OTPythonWrapping PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS} ${SWIG_CXX_COMPILE_FLAGS}"
                                                   POSITION_INDEPENDENT_CODE ON)

add_custom_target (pylib)
set (OPENTURNS_PYTHON_MODULES)

macro (ot_add_python_module MODULENAME SOURCEFILE)
  set_source_files_properties (${SOURCEFILE} PROPERTIES CPLUSPLUS ON
                                                        SWIG_MODULE_NAME ${MODULENAME}
                                                        INCLUDE_DIRECTORIES "${INTERNAL_INCLUDE_DIRS};${CMAKE_CURRENT_SOURCE_DIR}")

  ot_install_swig_file (${SOURCEFILE})
  if (${ARGC} GREATER 2)
    set (SWIG_MODULE_${MODULENAME}_EXTRA_DEPS ${ARGN})
  endif ()
  list (APPEND SWIG_MODULE_${MODULENAME}_EXTRA_DEPS openturns/PythonWrappingFunctions.hxx)

  set (swig_other_sources)
  foreach (it ${ARGN})
    if (${it} MATCHES ".*\\.i$")
      ot_install_swig_file (${it})
    else ()
      list (APPEND swig_other_sources "${it}")
    endif ()
  endforeach ()

  # swig_add_module is deprecated
  swig_add_library (${MODULENAME} LANGUAGE python SOURCES ${SOURCEFILE} ${swig_other_sources})

  target_include_directories (${MODULENAME} PRIVATE ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} BEFORE)

  target_link_libraries (${MODULENAME} OT)
  target_link_libraries (${MODULENAME} Python::Module)

  # https://github.com/swig/swig/issues/2638
  target_compile_definitions(${MODULENAME} PRIVATE SWIG_PYTHON_SILENT_MEMLEAK)

  set_target_properties (${MODULENAME} PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS} ${SWIG_CXX_COMPILE_FLAGS}"
                                                  UNITY_BUILD OFF)

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULENAME}.py
           DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)
  install (TARGETS ${MODULENAME}
            LIBRARY DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)

  add_dependencies (pylib ${MODULENAME})
  list (APPEND OPENTURNS_PYTHON_MODULES ${MODULENAME})
endmacro ()

install (DIRECTORY openturns DESTINATION ${OPENTURNS_INCLUDE_PATH})

ot_add_python_module(common common_module.i
                      OTtypes.i
                      OTexceptions.i
                      TBB.i TBB_doc.i
                      Object.i Object_doc.i
                      PersistentObject.i PersistentObject_doc.i
                      Study.i Study_doc.i
                      StorageManager.i StorageManager_doc.i
                      XMLStorageManager.i XMLStorageManager_doc.i
                      XMLH5StorageManager.i XMLH5StorageManager_doc.i
                      Log.i Log_doc.i
                      TTY.i TTY_doc.i
                      Path.i Path_doc.i
                      Catalog.i
                      ResourceMap.i ResourceMap_doc.i
                      ComparisonOperatorImplementation.i
                      ComparisonOperator.i ComparisonOperator_doc.i
                      Equal.i Equal_doc.i
                      Greater.i Greater_doc.i
                      GreaterOrEqual.i GreaterOrEqual_doc.i
                      Less.i Less_doc.i
                      LessOrEqual.i LessOrEqual_doc.i
                      Cache.i
                      Pointer.i
                      InterfaceObject.i InterfaceObject_doc.i
                      TypedInterfaceObject.i
                      TypedCollectionInterfaceObject.i
                      BaseCommonTemplateDefs.i
                   )

if (LibXml2_FOUND)
  set_property (TARGET common APPEND PROPERTY SWIG_COMPILE_DEFINITIONS OPENTURNS_HAVE_LIBXML2=1)
endif ()

if (HDF5_FOUND)
  set_property (TARGET common APPEND PROPERTY SWIG_COMPILE_DEFINITIONS OPENTURNS_HAVE_HDF5=1)
endif ()

ot_add_python_module(typ typ_module.i
                      Collection.i Collection_doc.i
                      PersistentCollection.i
                      Description.i Description_doc.i
                      PlatformInfo.i PlatformInfo_doc.i
                      Point.i Point_doc.i
                      PointWithDescription.i PointWithDescription_doc.i
                      DomainImplementation.i DomainImplementation_doc.i
                      Interval.i Interval_doc.i
                      Indices.i Indices_doc.i
                      IndicesCollection.i IndicesCollection_doc.i
                      IndicesCollectionImplementation.i
                      MatrixImplementation.i
                      Matrix.i Matrix_doc.i
                      SquareMatrix.i SquareMatrix_doc.i
                      SymmetricMatrix.i SymmetricMatrix_doc.i
                      Tensor.i Tensor_doc.i
                      SymmetricTensor.i SymmetricTensor_doc.i
                      ComplexTensor.i ComplexTensor_doc.i
                      ComplexMatrix.i ComplexMatrix_doc.i
                      HermitianMatrix.i HermitianMatrix_doc.i
                      SquareComplexMatrix.i SquareComplexMatrix_doc.i
                      TriangularMatrix.i TriangularMatrix_doc.i
                      TriangularComplexMatrix.i TriangularComplexMatrix_doc.i
                      CovarianceMatrix.i CovarianceMatrix_doc.i
                      CorrelationMatrix.i CorrelationMatrix_doc.i
                      IdentityMatrix.i IdentityMatrix_doc.i
                      Sample.i Sample_doc.i
                      HistoryStrategyImplementation.i HistoryStrategyImplementation_doc.i
                      HistoryStrategy.i HistoryStrategy_doc.i
                      BaseTypCollection.i
                      BaseTypTemplateDefs.i
                   )

ot_add_python_module(statistics statistics_module.i
                      Compact.i Compact_doc.i
                      Full.i Full_doc.i
                      Last.i Last_doc.i
                      Null.i Null_doc.i
                      TestResult.i TestResult_doc.i
                      RandomGenerator.i RandomGenerator_doc.i
                      RandomGeneratorState.i RandomGeneratorState_doc.i
                      LowDiscrepancySequenceImplementation.i LowDiscrepancySequenceImplementation_doc.i
                      LowDiscrepancySequence.i LowDiscrepancySequence_doc.i
                      SobolSequence.i SobolSequence_doc.i
                      FaureSequence.i FaureSequence_doc.i
                      HaltonSequence.i HaltonSequence_doc.i
                      HaselgroveSequence.i HaselgroveSequence_doc.i
                      ReverseHaltonSequence.i ReverseHaltonSequence_doc.i
                      SphericalModel.i SphericalModel_doc.i
                      FilteringWindowsImplementation.i FilteringWindowsImplementation_doc.i
                      FilteringWindows.i FilteringWindows_doc.i
                      Hamming.i Hamming_doc.i
                      Hann.i Hann_doc.i
                      FFTImplementation.i FFTImplementation_doc.i
                      FFT.i FFT_doc.i
                      SpectralModelImplementation.i SpectralModelImplementation_doc.i
                      SpectralModel.i SpectralModel_doc.i
                      CovarianceModelImplementation.i CovarianceModelImplementation_doc.i
                      CovarianceModel.i CovarianceModel_doc.i
                      StationaryFunctionalCovarianceModel.i StationaryFunctionalCovarianceModel_doc.i
                      SpectralModelFactoryImplementation.i SpectralModelFactoryImplementation_doc.i
                      SpectralModelFactory.i SpectralModelFactory_doc.i
                      UserDefinedSpectralModel.i UserDefinedSpectralModel_doc.i
                      WelchFactory.i WelchFactory_doc.i
                      KroneckerCovarianceModel.i KroneckerCovarianceModel_doc.i
                      ExponentialModel.i ExponentialModel_doc.i
                      ExponentiallyDampedCosineModel.i ExponentiallyDampedCosineModel_doc.i
                      CauchyModel.i CauchyModel_doc.i
                      CovarianceModelFactoryImplementation.i CovarianceModelFactoryImplementation_doc.i
                      CovarianceModelFactory.i CovarianceModelFactory_doc.i
                      UserDefinedCovarianceModel.i UserDefinedCovarianceModel_doc.i
                      UserDefinedStationaryCovarianceModel.i UserDefinedStationaryCovarianceModel_doc.i
                      StationaryCovarianceModelFactory.i StationaryCovarianceModelFactory_doc.i
                      NonStationaryCovarianceModelFactory.i NonStationaryCovarianceModelFactory_doc.i
                      SquaredExponential.i SquaredExponential_doc.i
                      GeneralizedExponential.i GeneralizedExponential_doc.i
                      AbsoluteExponential.i AbsoluteExponential_doc.i
                      MaternModel.i MaternModel_doc.i
                      DiracCovarianceModel.i DiracCovarianceModel_doc.i
                      ProductCovarianceModel.i ProductCovarianceModel_doc.i
                      RankMCovarianceModel.i RankMCovarianceModel_doc.i
                      FractionalBrownianMotionModel.i FractionalBrownianMotionModel_doc.i
                      TensorizedCovarianceModel.i TensorizedCovarianceModel_doc.i
                      HMatrixParameters.i HMatrixParameters_doc.i
                      HMatrixFactory.i HMatrixFactory_doc.i
                      HMatrix.i HMatrix_doc.i
                      HMatrixImplementation.i HMatrixImplementation_doc.i
                      BaseStatisticsTemplateDefs.i
                      IsotropicCovarianceModel.i IsotropicCovarianceModel_doc.i
                      CorrelationAnalysis.i CorrelationAnalysis_doc.i
                      LatentVariableModel.i LatentVariableModel_doc.i
                      SamplePartition.i SamplePartition_doc.i
                   )

ot_add_python_module(iterative_statistics iterative_statistics_module.i
                      IterativeAlgorithm.i IterativeAlgorithm_doc.i
                      IterativeAlgorithmImplementation.i IterativeAlgorithmImplementation_doc.i
                      IterativeExtrema.i IterativeExtrema_doc.i
                      IterativeThresholdExceedance.i IterativeThresholdExceedance_doc.i
                      IterativeMoments.i IterativeMoments_doc.i
                      BaseIterativeStatisticsTemplateDefs.i
                   )

ot_add_python_module(geom geom_module.i
                      Domain.i Domain_doc.i
                      BipartiteGraph.i BipartiteGraph_doc.i
                      Mesh.i Mesh_doc.i
                      RegularGrid.i RegularGrid_doc.i
                      IntervalMesher.i IntervalMesher_doc.i
                      BoundaryMesher.i BoundaryMesher_doc.i
                      MeshDomain.i MeshDomain_doc.i
                      DomainComplement.i DomainComplement_doc.i
                      DomainIntersection.i DomainIntersection_doc.i
                      DomainUnion.i DomainUnion_doc.i
                      DomainDisjunctiveUnion.i DomainDisjunctiveUnion_doc.i
                      DomainDifference.i DomainDifference_doc.i
                      BaseGeomTemplateDefs.i
                    )

ot_add_python_module(graph graph_module.i
                      DrawableImplementation.i DrawableImplementation_doc.i
                      Drawable.i Drawable_doc.i
                      GraphImplementation.i GraphImplementation_doc.i
                      Graph.i Graph_doc.i
                      BarPlot.i BarPlot_doc.i
                      Cloud.i Cloud_doc.i
                      Contour.i Contour_doc.i
                      Curve.i Curve_doc.i
                      GridLayout.i GridLayout_doc.i
                      Pie.i Pie_doc.i
                      Polygon.i Polygon_doc.i
                      PolygonArray.i PolygonArray_doc.i
                      Staircase.i Staircase_doc.i
                      Text.i Text_doc.i
                      BaseGraphTemplateDefs.i
                   )

ot_add_python_module(func func_module.i
                      EnumerateFunction.i EnumerateFunction_doc.i
                      EnumerateFunctionImplementation.i EnumerateFunctionImplementation_doc.i
                      LinearEnumerateFunction.i LinearEnumerateFunction_doc.i
                      HyperbolicAnisotropicEnumerateFunction.i HyperbolicAnisotropicEnumerateFunction_doc.i
                      NormInfEnumerateFunction.i NormInfEnumerateFunction_doc.i
                      EvaluationImplementation.i EvaluationImplementation_doc.i
                      Evaluation.i Evaluation_doc.i
                      GradientImplementation.i GradientImplementation_doc.i
                      Gradient.i Gradient_doc.i
                      HessianImplementation.i HessianImplementation_doc.i
                      Hessian.i Hessian_doc.i
                      NoEvaluation.i
                      NoGradient.i
                      NoHessian.i
                      NullHessian.i
                      TranslationEvaluation.i TranslationEvaluation_doc.i
                      LinearEvaluation.i LinearEvaluation_doc.i
                      LinearGradient.i LinearGradient_doc.i
                      QuadraticEvaluation.i QuadraticEvaluation_doc.i
                      ConstantGradient.i ConstantGradient_doc.i
                      ConstantHessian.i ConstantHessian_doc.i
                      ComposedEvaluation.i
                      ComposedGradient.i
                      ComposedHessian.i
                      DualLinearCombinationEvaluation.i DualLinearCombinationEvaluation_doc.i
                      DualLinearCombinationGradient.i DualLinearCombinationGradient_doc.i
                      DualLinearCombinationHessian.i DualLinearCombinationHessian_doc.i
                      LinearCombinationFunction.i LinearCombinationFunction_doc.i
                      LinearCombinationEvaluation.i
                      LinearCombinationGradient.i
                      LinearCombinationHessian.i
                      ParametricFunction.i ParametricFunction_doc.i
                      ProductEvaluation.i
                      ProductGradient.i
                      ProductHessian.i
                      FunctionImplementation.i FunctionImplementation_doc.i
                      ProductFunction.i ProductFunction_doc.i
                      Function.i Function_doc.i
                      EvaluationProxy.i
                      MemoizeEvaluation.i MemoizeEvaluation_doc.i
                      MemoizeFunction.i MemoizeFunction_doc.i
                      TensorizedUniVariateFunctionFactory.i TensorizedUniVariateFunctionFactory_doc.i
                      UniVariateFunctionFactory.i UniVariateFunctionFactory_doc.i
                      MonomialFunctionFactory.i MonomialFunctionFactory_doc.i
                      UniVariateFunctionFamily.i UniVariateFunctionFamily_doc.i
                      AggregatedFunction.i AggregatedFunction_doc.i
                      SymbolicFunction.i SymbolicFunction_doc.i
                      ComposedFunction.i ComposedFunction_doc.i
                      IdentityFunction.i IdentityFunction_doc.i
                      DualLinearCombinationFunction.i DualLinearCombinationFunction_doc.i
                      BaseFuncCollection.i
                      FieldFunctionImplementation.i FieldFunctionImplementation_doc.i
                      FieldFunction.i FieldFunction_doc.i
                      FieldToPointFunctionImplementation.i FieldToPointFunctionImplementation_doc.i
                      FieldToPointFunction.i FieldToPointFunction_doc.i
                      PointToFieldFunctionImplementation.i PointToFieldFunctionImplementation_doc.i
                      PointToFieldFunction.i PointToFieldFunction_doc.i
                      ParametricPointToFieldFunction.i ParametricPointToFieldFunction_doc.i
                      VertexValuePointToFieldFunction.i VertexValuePointToFieldFunction_doc.i
                      PointToPointEvaluation.i PointToPointEvaluation_doc.i
                      PointToPointConnection.i PointToPointConnection_doc.i
                      FieldToPointConnection.i FieldToPointConnection_doc.i
                      PointToFieldConnection.i PointToFieldConnection_doc.i
                      FieldToFieldConnection.i FieldToFieldConnection_doc.i
                      KarhunenLoeveProjection.i KarhunenLoeveProjection_doc.i
                      KarhunenLoeveLifting.i KarhunenLoeveLifting_doc.i
                      IndicatorFunction.i IndicatorFunction_doc.i
                      DistanceToDomainFunction.i DistanceToDomainFunction_doc.i
                      ValueFunction.i ValueFunction_doc.i
                      VertexValueFunction.i VertexValueFunction_doc.i
                      TranslationFunction.i TranslationFunction_doc.i
                      LinearFunction.i LinearFunction_doc.i
                      QuadraticFunction.i QuadraticFunction_doc.i
                      UniVariatePolynomialImplementation.i UniVariatePolynomialImplementation_doc.i
                      UniVariatePolynomial.i UniVariatePolynomial_doc.i
                      AggregatedEvaluation.i AggregatedEvaluation_doc.i
                      SymbolicEvaluation.i
                      SymbolicGradient.i
                      SymbolicHessian.i
                      DatabaseFunction.i DatabaseFunction_doc.i
                      DatabaseEvaluation.i DatabaseEvaluation_doc.i
                      IndicatorEvaluation.i
                      DistanceToDomainEvaluation.i
                      PiecewiseLinearEvaluation.i PiecewiseLinearEvaluation_doc.i
                      PiecewiseHermiteEvaluation.i PiecewiseHermiteEvaluation_doc.i
                      ProductPolynomialEvaluation.i ProductPolynomialEvaluation_doc.i
                      ParametricEvaluation.i
                      ParametricGradient.i
                      ParametricHessian.i
                      P1LagrangeEvaluation.i P1LagrangeEvaluation_doc.i
                      P1LagrangeInterpolation.i P1LagrangeInterpolation_doc.i
                      BasisImplementation.i BasisImplementation_doc.i
                      Basis.i Basis_doc.i
                      BasisSequenceImplementation.i
                      BasisSequence.i BasisSequence_doc.i
                      BasisSequenceFactoryImplementation.i BasisSequenceFactoryImplementation_doc.i
                      BasisSequenceFactory.i BasisSequenceFactory_doc.i
                      LARS.i LARS_doc.i
                      BoxCoxEvaluation.i
                      BoxCoxTransform.i BoxCoxTransform_doc.i
                      InverseBoxCoxEvaluation.i
                      InverseBoxCoxTransform.i InverseBoxCoxTransform_doc.i
                      TrendEvaluation.i
                      TrendTransform.i TrendTransform_doc.i
                      InverseTrendEvaluation.i
                      InverseTrendTransform.i InverseTrendTransform_doc.i
                      BasisFactory.i BasisFactory_doc.i
                      ConstantBasisFactory.i ConstantBasisFactory_doc.i
                      LinearBasisFactory.i LinearBasisFactory_doc.i
                      QuadraticBasisFactory.i QuadraticBasisFactory_doc.i
                      FieldImplementation.i FieldImplementation_doc.i
                      Field.i Field_doc.i
                      TimeSeries.i TimeSeries_doc.i
                      ProcessSample.i ProcessSample_doc.i
                      ProcessSampleImplementation.i ProcessSampleImplementation_doc.i
                      MarginalEvaluation.i
                      MarginalGradient.i
                      MarginalHessian.i
                      BaseFuncTemplateDefs.i
                   )

ot_add_python_module(SpecFunc specfunc_module.i
                      SpecFunc.i SpecFunc_doc.i
                    )
ot_add_python_module(diff diff_module.i
                      FiniteDifferenceStepImplementation.i FiniteDifferenceStepImplementation_doc.i
                      FiniteDifferenceStep.i FiniteDifferenceStep_doc.i
                      ConstantStep.i ConstantStep_doc.i
                      BlendedStep.i BlendedStep_doc.i
                      FiniteDifferenceGradient.i FiniteDifferenceGradient_doc.i
                      FiniteDifferenceHessian.i FiniteDifferenceHessian_doc.i
                      CenteredFiniteDifferenceGradient.i CenteredFiniteDifferenceGradient_doc.i
                      CenteredFiniteDifferenceHessian.i CenteredFiniteDifferenceHessian_doc.i
                      NonCenteredFiniteDifferenceGradient.i NonCenteredFiniteDifferenceGradient_doc.i
                      BaseDiffTemplateDefs.i
                    )

ot_add_python_module(optim optim_module.i
                      OptimizationProblem.i OptimizationProblem_doc.i
                      OptimizationProblemImplementation.i OptimizationProblemImplementation_doc.i
                      NearestPointProblem.i NearestPointProblem_doc.i
                      LeastSquaresProblem.i LeastSquaresProblem_doc.i
                      OptimizationAlgorithm.i OptimizationAlgorithm_doc.i
                      OptimizationResult.i OptimizationResult_doc.i
                      OptimizationAlgorithmImplementation.i OptimizationAlgorithmImplementation_doc.i
                      AbdoRackwitz.i AbdoRackwitz_doc.i
                      Bonmin.i Bonmin_doc.i
                      Ceres.i Ceres_doc.i
                      CMinpack.i CMinpack_doc.i
                      Cobyla.i Cobyla_doc.i
                      Dlib.i Dlib_doc.i
                      Ipopt.i Ipopt_doc.i
                      TNC.i TNC_doc.i
                      SQP.i SQP_doc.i
                      MultiStart.i MultiStart_doc.i
		      Pagmo.i Pagmo_doc.i
                      NLopt.i NLopt_doc.i
                      LevelSet.i LevelSet_doc.i
                      LevelSetMesher.i LevelSetMesher_doc.i
                      BaseOptimTemplateDefs.i
                   )

ot_add_python_module(solver solver_module.i
                      SolverImplementation.i SolverImplementation_doc.i
                      Solver.i Solver_doc.i
                      Bisection.i Bisection_doc.i
                      Secant.i Secant_doc.i
                      Brent.i Brent_doc.i
                      ODESolverImplementation.i ODESolverImplementation_doc.i
                      ODESolver.i ODESolver_doc.i
                      RungeKutta.i RungeKutta_doc.i
                      Fehlberg.i Fehlberg_doc.i
                      BaseSolverTemplateDefs.i
                   )

ot_add_python_module(algo algo_module.i
                      ApproximationAlgorithmImplementation.i ApproximationAlgorithmImplementation_doc.i
                      ApproximationAlgorithmImplementationFactory.i ApproximationAlgorithmImplementationFactory_doc.i
                      ApproximationAlgorithm.i ApproximationAlgorithm_doc.i
                      DesignProxy.i DesignProxy_doc.i
                      LeastSquaresMethodImplementation.i LeastSquaresMethodImplementation_doc.i
                      LeastSquaresMethod.i LeastSquaresMethod_doc.i
                      CholeskyMethod.i CholeskyMethod_doc.i
                      QRMethod.i QRMethod_doc.i
                      SVDMethod.i SVDMethod_doc.i
                      SparseMethod.i SparseMethod_doc.i
                      LeastSquaresMetaModelSelection.i LeastSquaresMetaModelSelection_doc.i
                      LeastSquaresMetaModelSelectionFactory.i LeastSquaresMetaModelSelectionFactory_doc.i
                      ClassifierImplementation.i ClassifierImplementation_doc.i
                      Classifier.i Classifier_doc.i
                      FittingAlgorithmImplementation.i FittingAlgorithmImplementation_doc.i
                      FittingAlgorithm.i FittingAlgorithm_doc.i
                      CorrectedLeaveOneOut.i CorrectedLeaveOneOut_doc.i
                      NearestNeighbourAlgorithmImplementation.i NearestNeighbourAlgorithmImplementation_doc.i
                      NearestNeighbourAlgorithm.i NearestNeighbourAlgorithm_doc.i
                      KDTree.i KDTree_doc.i
                      RegularGridNearestNeighbour.i RegularGridNearestNeighbour_doc.i
                      NearestNeighbour1D.i NearestNeighbour1D_doc.i
                      NaiveNearestNeighbour.i NaiveNearestNeighbour_doc.i
                      EnclosingSimplexAlgorithmImplementation.i EnclosingSimplexAlgorithmImplementation_doc.i
                      EnclosingSimplexAlgorithm.i EnclosingSimplexAlgorithm_doc.i
                      NaiveEnclosingSimplex.i  NaiveEnclosingSimplex_doc.i
                      RegularGridEnclosingSimplex.i  RegularGridEnclosingSimplex_doc.i
                      EnclosingSimplexMonotonic1D.i  EnclosingSimplexMonotonic1D_doc.i
                      BoundingVolumeHierarchy.i BoundingVolumeHierarchy_doc.i
                      KFold.i KFold_doc.i
                      PenalizedLeastSquaresAlgorithm.i PenalizedLeastSquaresAlgorithm_doc.i
                      PenalizedLeastSquaresAlgorithmFactory.i PenalizedLeastSquaresAlgorithmFactory_doc.i
                      KissFFT.i KissFFT_doc.i
                      IntegrationAlgorithmImplementation.i IntegrationAlgorithmImplementation_doc.i
                      IntegrationAlgorithm.i IntegrationAlgorithm_doc.i
                      FilonQuadrature.i FilonQuadrature_doc.i
                      GaussKronrodRule.i GaussKronrodRule_doc.i
                      GaussKronrod.i GaussKronrod_doc.i
                      GaussLegendre.i GaussLegendre_doc.i
                      IteratedQuadrature.i IteratedQuadrature_doc.i
                      ExpertMixture.i ExpertMixture_doc.i
                      KarhunenLoeveReduction.i KarhunenLoeveReduction_doc.i
                      KarhunenLoeveResultImplementation.i KarhunenLoeveResultImplementation_doc.i
                      KarhunenLoeveResult.i KarhunenLoeveResult_doc.i
                      KarhunenLoeveAlgorithmImplementation.i KarhunenLoeveAlgorithmImplementation_doc.i
                      KarhunenLoeveP1Algorithm.i KarhunenLoeveP1Algorithm_doc.i
                      KarhunenLoeveAlgorithm.i KarhunenLoeveAlgorithm_doc.i
                      KarhunenLoeveValidation.i KarhunenLoeveValidation_doc.i
                      SimplicialCubature.i SimplicialCubature_doc.i
                      CubaIntegration.i CubaIntegration_doc.i
                      BaseAlgoTemplateDefs.i
                   )

ot_add_python_module(experiment experiment_module.i
                      ExperimentImplementation.i ExperimentImplementation_doc.i
                      StratifiedExperiment.i StratifiedExperiment_doc.i
                      Experiment.i Experiment_doc.i
                      Axial.i Axial_doc.i
                      Composite.i Composite_doc.i
                      Factorial.i Factorial_doc.i
                      Box.i Box_doc.i
                      CombinatorialGeneratorImplementation.i CombinatorialGeneratorImplementation_doc.i
                      CombinatorialGenerator.i CombinatorialGenerator_doc.i
                      Combinations.i Combinations_doc.i
                      KPermutations.i KPermutations_doc.i
                      Tuples.i Tuples_doc.i
                      BaseExperimentTemplateDefs.i
                   )

ot_add_python_module(base base_module.i
                      BaseTemplateDefs.i
                   )

ot_add_python_module(model_copula model_copula_module.i
                      DistributionImplementation.i DistributionImplementation_doc.i
                      DistributionFactoryImplementation.i DistributionFactoryImplementation_doc.i
                      Distribution.i Distribution_doc.i
                      DistributionFactoryResult.i DistributionFactoryResult_doc.i
                      DistributionFactory.i DistributionFactory_doc.i
                      DistributionParametersImplementation.i DistributionParametersImplementation_doc.i
                      DistributionParameters.i DistributionParameters_doc.i
                      EllipticalDistribution.i EllipticalDistribution_doc.i
                      ArchimedeanCopula.i ArchimedeanCopula_doc.i
                      SklarCopula.i SklarCopula_doc.i
                      IndependentCopula.i IndependentCopula_doc.i
                      IndependentCopulaFactory.i IndependentCopulaFactory_doc.i
                      MinCopula.i MinCopula_doc.i
                      NormalCopula.i NormalCopula_doc.i
                      NormalCopulaFactory.i NormalCopulaFactory_doc.i
                      OrdinalSumCopula.i OrdinalSumCopula_doc.i
                      FarlieGumbelMorgensternCopula.i FarlieGumbelMorgensternCopula_doc.i
                      FarlieGumbelMorgensternCopulaFactory.i FarlieGumbelMorgensternCopulaFactory_doc.i
                      MarshallOlkinCopula.i MarshallOlkinCopula_doc.i
                      AliMikhailHaqCopula.i AliMikhailHaqCopula_doc.i
                      AliMikhailHaqCopulaFactory.i AliMikhailHaqCopulaFactory_doc.i
                      FrankCopula.i FrankCopula_doc.i
                      FrankCopulaFactory.i FrankCopulaFactory_doc.i
                      ClaytonCopula.i ClaytonCopula_doc.i
                      ClaytonCopulaFactory.i ClaytonCopulaFactory_doc.i
                      GumbelCopula.i GumbelCopula_doc.i
                      GumbelCopulaFactory.i GumbelCopulaFactory_doc.i
                      BlockIndependentDistribution.i BlockIndependentDistribution_doc.i
                      JointDistribution.i JointDistribution_doc.i
                      BlockIndependentCopula.i BlockIndependentCopula_doc.i
                      CumulativeDistributionNetwork.i CumulativeDistributionNetwork_doc.i
                      PlackettCopula.i PlackettCopula_doc.i
                      PlackettCopulaFactory.i PlackettCopulaFactory_doc.i
                      UncertaintyModelCopulaCollection.i
                      UncertaintyModelCopulaTemplateDefs.i
                   )

ot_add_python_module(randomvector randomvector_module.i
                      RandomVectorImplementation.i RandomVectorImplementation_doc.i
                      ConstantRandomVector.i ConstantRandomVector_doc.i
                      UsualRandomVector.i UsualRandomVector_doc.i
                      UncertaintyRandomVectorTemplateDefs.i
                   )

ot_add_python_module(model_process model_process_module.i
                      ProcessImplementation.i ProcessImplementation_doc.i
                      Process.i Process_doc.i
                      WhiteNoise.i WhiteNoise_doc.i
                      RandomWalk.i RandomWalk_doc.i
                      DiscreteMarkovChain.i DiscreteMarkovChain_doc.i
                      ARMACoefficients.i ARMACoefficients_doc.i
                      ARMAState.i ARMAState_doc.i
                      ARMA.i ARMA_doc.i
                      SpectralGaussianProcess.i SpectralGaussianProcess_doc.i
                      GaussianProcess.i GaussianProcess_doc.i
                      AggregatedProcess.i AggregatedProcess_doc.i
                      CompositeProcess.i CompositeProcess_doc.i
                      FunctionalBasisProcess.i FunctionalBasisProcess_doc.i
                      ARMAFactoryImplementation.i ARMAFactoryImplementation_doc.i
                      WhittleFactory.i WhittleFactory_doc.i
                      WhittleFactoryState.i WhittleFactoryState_doc.i
                      ARMALikelihoodFactory.i  ARMALikelihoodFactory_doc.i
                      ARMAFactory.i ARMAFactory_doc.i
                      KarhunenLoeveQuadratureAlgorithm.i KarhunenLoeveQuadratureAlgorithm_doc.i
                      KarhunenLoeveSVDAlgorithm.i KarhunenLoeveSVDAlgorithm_doc.i
                      UncertaintyProcessTemplateDefs.i
                   )

ot_add_python_module(DistFunc distfunc_module.i
                      DistFunc.i DistFunc_doc.i
                    )
ot_add_python_module(dist_bundle1 dist_bundle1_module.i
                      Arcsine.i Arcsine_doc.i
                      ArcsineFactory.i ArcsineFactory_doc.i
                      ArcsineMuSigma.i ArcsineMuSigma_doc.i
                      JointByConditioningDistribution.i JointByConditioningDistribution_doc.i
                      Bernoulli.i Bernoulli_doc.i
                      BernoulliFactory.i BernoulliFactory_doc.i
                      EmpiricalBernsteinCopula.i EmpiricalBernsteinCopula_doc.i
                      BernsteinCopulaFactory.i BernsteinCopulaFactory_doc.i
                      Beta.i Beta_doc.i
                      BetaFactory.i BetaFactory_doc.i
                      BetaMuSigma.i BetaMuSigma_doc.i
                      Binomial.i Binomial_doc.i
                      BinomialFactory.i BinomialFactory_doc.i
                      Burr.i Burr_doc.i
                      BurrFactory.i BurrFactory_doc.i
                      ChiSquare.i ChiSquare_doc.i
                      ChiSquareFactory.i ChiSquareFactory_doc.i
                      Chi.i Chi_doc.i
                      ChiFactory.i ChiFactory_doc.i
                      CompositeDistribution.i CompositeDistribution_doc.i
                      Dirac.i Dirac_doc.i
                      DiracFactory.i DiracFactory_doc.i
                      Dirichlet.i Dirichlet_doc.i
                      DirichletFactory.i DirichletFactory_doc.i
                      DiscreteCompoundDistribution.i DiscreteCompoundDistribution_doc.i
                      Epanechnikov.i Epanechnikov_doc.i
                      Exponential.i Exponential_doc.i
                      ExponentialFactory.i ExponentialFactory_doc.i
                      ExtremeValueCopula.i ExtremeValueCopula_doc.i
                      JoeCopula.i JoeCopula_doc.i
                      FisherSnedecor.i FisherSnedecor_doc.i
                      FisherSnedecorFactory.i FisherSnedecorFactory_doc.i
                      Frechet.i Frechet_doc.i
                      FrechetFactory.i FrechetFactory_doc.i
                      GalambosCopula.i GalambosCopula_doc.i
                      Gamma.i Gamma_doc.i
                      GammaFactory.i GammaFactory_doc.i
                      GammaMuSigma.i GammaMuSigma_doc.i
                      WeibullMax.i WeibullMax_doc.i
                      WeibullMaxFactory.i WeibullMaxFactory_doc.i
                      WeibullMaxMuSigma.i WeibullMaxMuSigma_doc.i
                      GeneralizedExtremeValue.i GeneralizedExtremeValue_doc.i
                      GeneralizedExtremeValueFactory.i GeneralizedExtremeValueFactory_doc.i
		      GeneralizedExtremeValueValidation.i GeneralizedExtremeValueValidation_doc.i
                      DistributionFactoryLikelihoodResult.i DistributionFactoryLikelihoodResult_doc.i
                      ProfileLikelihoodResult.i ProfileLikelihoodResult_doc.i
		      CovariatesResult.i CovariatesResult_doc.i
		      TimeVaryingResult.i TimeVaryingResult_doc.i
                      Pareto.i Pareto_doc.i
                      ParetoFactory.i ParetoFactory_doc.i
                      GeneralizedPareto.i GeneralizedPareto_doc.i
                      GeneralizedParetoFactory.i GeneralizedParetoFactory_doc.i
		      GeneralizedParetoValidation.i GeneralizedParetoValidation_doc.i
                      Geometric.i Geometric_doc.i
                      GeometricFactory.i GeometricFactory_doc.i
                      Gumbel.i Gumbel_doc.i
                      GumbelFactory.i GumbelFactory_doc.i
                      GumbelLambdaGamma.i GumbelLambdaGamma_doc.i
                      GumbelMuSigma.i GumbelMuSigma_doc.i
                   )

ot_add_python_module(dist_bundle2 dist_bundle2_module.i
                      Histogram.i Histogram_doc.i
                      HistogramFactory.i HistogramFactory_doc.i
                      Hypergeometric.i Hypergeometric_doc.i
                      InverseChiSquare.i InverseChiSquare_doc.i
                      InverseGamma.i InverseGamma_doc.i
                      InverseNormal.i InverseNormal_doc.i
                      InverseNormalFactory.i InverseNormalFactory_doc.i
                      InverseWishart.i InverseWishart_doc.i
                      KPermutationsDistribution.i KPermutationsDistribution_doc.i
                      KernelMixture.i KernelMixture_doc.i
                      KernelSmoothing.i KernelSmoothing_doc.i
                      Laplace.i Laplace_doc.i
                      LaplaceFactory.i LaplaceFactory_doc.i
                      LeastSquaresDistributionFactory.i LeastSquaresDistributionFactory_doc.i
                      Logistic.i Logistic_doc.i
                      LogisticFactory.i LogisticFactory_doc.i
                      LogNormal.i LogNormal_doc.i
                      LogNormalFactory.i LogNormalFactory_doc.i
                      LogNormalMuErrorFactor.i LogNormalMuErrorFactor_doc.i
                      LogNormalMuSigma.i LogNormalMuSigma_doc.i
                      LogNormalMuSigmaOverMu.i LogNormalMuSigmaOverMu_doc.i
                      LogUniform.i LogUniform_doc.i
                      LogUniformFactory.i LogUniformFactory_doc.i
                      MarginalDistribution.i MarginalDistribution_doc.i
                      MaximumDistribution.i MaximumDistribution_doc.i
                      MaximumEntropyOrderStatisticsDistribution.i MaximumEntropyOrderStatisticsDistribution_doc.i
                      MaximumLikelihoodFactory.i MaximumLikelihoodFactory_doc.i
                      MeixnerDistribution.i MeixnerDistribution_doc.i
                      MeixnerDistributionFactory.i MeixnerDistributionFactory_doc.i
                      MethodOfMomentsFactory.i MethodOfMomentsFactory_doc.i
                      MixedHistogramUserDefined.i MixedHistogramUserDefined_doc.i
                      Mixture.i  Mixture_doc.i
                      CompoundDistribution.i CompoundDistribution_doc.i
                      Multinomial.i Multinomial_doc.i
                      MultinomialFactory.i MultinomialFactory_doc.i
                      Polya.i Polya_doc.i
                      PolyaFactory.i PolyaFactory_doc.i
                      PosteriorDistribution.i PosteriorDistribution_doc.i
                      NonCentralChiSquare.i NonCentralChiSquare_doc.i
                      NonCentralStudent.i NonCentralStudent_doc.i
                      Normal.i Normal_doc.i
                      NormalFactory.i NormalFactory_doc.i
                      NormalGamma.i NormalGamma_doc.i
                   )

ot_add_python_module(dist_bundle3 dist_bundle3_module.i
                      OrderStatisticsMarginalChecker.i OrderStatisticsMarginalChecker_doc.i
                      ParametrizedDistribution.i ParametrizedDistribution_doc.i
		      PointConditionalDistribution.i PointConditionalDistribution_doc.i
                      Poisson.i Poisson_doc.i
                      PoissonFactory.i PoissonFactory_doc.i
                      ProductDistribution.i ProductDistribution_doc.i
                      QuantileMatchingFactory.i QuantileMatchingFactory_doc.i
                      RandomMixture.i RandomMixture_doc.i
                      Rayleigh.i Rayleigh_doc.i
                      RayleighFactory.i RayleighFactory_doc.i
                      Rice.i Rice_doc.i
                      RiceFactory.i RiceFactory_doc.i
                      Skellam.i Skellam_doc.i
                      SkellamFactory.i SkellamFactory_doc.i
                      SmoothedUniform.i SmoothedUniform_doc.i
                      SmoothedUniformFactory.i SmoothedUniformFactory_doc.i
                      SquaredNormal.i SquaredNormal_doc.i
                      Student.i Student_doc.i
                      StudentFactory.i StudentFactory_doc.i
                      StudentCopula.i StudentCopula_doc.i
                      StudentCopulaFactory.i StudentCopulaFactory_doc.i
                      Trapezoidal.i Trapezoidal_doc.i
                      TrapezoidalFactory.i TrapezoidalFactory_doc.i
                      Triangular.i Triangular_doc.i
                      TriangularFactory.i TriangularFactory_doc.i
                      TruncatedDistribution.i TruncatedDistribution_doc.i
                      TruncatedNormal.i TruncatedNormal_doc.i
                      TruncatedNormalFactory.i TruncatedNormalFactory_doc.i
                      TruncatedOverMesh.i TruncatedOverMesh_doc.i
                      Uniform.i Uniform_doc.i
                      UniformFactory.i UniformFactory_doc.i
                      UniformMuSigma.i UniformMuSigma_doc.i
                      UniformOverMesh.i UniformOverMesh_doc.i
                      UserDefined.i UserDefined_doc.i
                      UserDefinedFactory.i UserDefinedFactory_doc.i
                      UniformOrderStatistics.i UniformOrderStatistics_doc.i
                      VonMises.i VonMises_doc.i
                      VonMisesFactory.i VonMisesFactory_doc.i
                      WeibullMin.i WeibullMin_doc.i
                      WeibullMinFactory.i WeibullMinFactory_doc.i
                      WeibullMinMuSigma.i WeibullMinMuSigma_doc.i
                      Wishart.i Wishart_doc.i
                      ZipfMandelbrot.i ZipfMandelbrot_doc.i
                      UncertaintyDistTemplateDefs.i
                   )

ot_add_python_module(bayesian bayesian_module.i
                      GaussianLinearCalibration.i GaussianLinearCalibration_doc.i
                      CalibrationAlgorithmImplementation.i CalibrationAlgorithmImplementation_doc.i
                      CalibrationAlgorithm.i CalibrationAlgorithm_doc.i
                      CalibrationResult.i CalibrationResult_doc.i
                      LinearLeastSquaresCalibration.i LinearLeastSquaresCalibration_doc.i
                      MetropolisHastingsImplementation.i MetropolisHastingsImplementation_doc.i
                      MetropolisHastings.i MetropolisHastings_doc.i
                      NonLinearLeastSquaresCalibration.i NonLinearLeastSquaresCalibration_doc.i
                      GaussianNonLinearCalibration.i GaussianNonLinearCalibration_doc.i
                      RandomVectorMetropolisHastings.i RandomVectorMetropolisHastings_doc.i
                      RandomWalkMetropolisHastings.i RandomWalkMetropolisHastings_doc.i
                      IndependentMetropolisHastings.i IndependentMetropolisHastings_doc.i
                      UserDefinedMetropolisHastings.i UserDefinedMetropolisHastings_doc.i
                      Gibbs.i Gibbs_doc.i
                      CompoundRandomVector.i CompoundRandomVector_doc.i
                      UncertaintyBayesianTemplateDefs.i
                    )

ot_add_python_module(classification classification_module.i
                      MixtureClassifier.i MixtureClassifier_doc.i
                      MinimumVolumeClassifier.i MinimumVolumeClassifier_doc.i
                      UncertaintyClassificationTemplateDefs.i
                   )

ot_add_python_module(weightedexperiment weightedexperiment_module.i
                      WeightedExperiment.i WeightedExperiment_doc.i
                      WeightedExperimentImplementation.i WeightedExperimentImplementation_doc.i
                      BootstrapExperiment.i BootstrapExperiment_doc.i
                      FixedExperiment.i FixedExperiment_doc.i
                      ImportanceSamplingExperiment.i ImportanceSamplingExperiment_doc.i
                      GaussProductExperiment.i GaussProductExperiment_doc.i
                      TensorProductExperiment.i TensorProductExperiment_doc.i
                      LHSExperiment.i LHSExperiment_doc.i
                      SimulatedAnnealingLHS.i SimulatedAnnealingLHS_doc.i
                      MonteCarloLHS.i MonteCarloLHS_doc.i
                      LHSResult.i LHSResult_doc.i
                      OptimalLHSExperiment.i OptimalLHSExperiment_doc.i
                      SpaceFillingPhiP.i SpaceFillingPhiP_doc.i
                      SpaceFillingMinDist.i SpaceFillingMinDist_doc.i
                      SpaceFillingC2.i SpaceFillingC2_doc.i
                      SpaceFillingImplementation.i SpaceFillingImplementation_doc.i
                      SpaceFilling.i SpaceFilling_doc.i
                      LinearProfile.i LinearProfile_doc.i
                      GeometricProfile.i GeometricProfile_doc.i
                      TemperatureProfile.i TemperatureProfile_doc.i
                      TemperatureProfileImplementation.i TemperatureProfileImplementation_doc.i
                      LowDiscrepancyExperiment.i LowDiscrepancyExperiment_doc.i
                      MonteCarloExperiment.i MonteCarloExperiment_doc.i
                      ExperimentIntegration.i ExperimentIntegration_doc.i
		      SmolyakExperiment.i SmolyakExperiment_doc.i
                      SplitterImplementation.i SplitterImplementation_doc.i
                      KFoldSplitter.i KFoldSplitter_doc.i
                      LeaveOneOutSplitter.i LeaveOneOutSplitter_doc.i
                      UncertaintyWeightedExperimentTemplateDefs.i
                   )

ot_add_python_module(orthogonalbasis orthogonalbasis_module.i
                      OrthogonalUniVariatePolynomial.i OrthogonalUniVariatePolynomial_doc.i
                      OrthogonalUniVariatePolynomialFactory.i OrthogonalUniVariatePolynomialFactory_doc.i
                      CharlierFactory.i CharlierFactory_doc.i
                      ChebychevFactory.i ChebychevFactory_doc.i
                      HermiteFactory.i HermiteFactory_doc.i
                      HistogramPolynomialFactory.i HistogramPolynomialFactory_doc.i
                      JacobiFactory.i JacobiFactory_doc.i
                      KrawtchoukFactory.i KrawtchoukFactory_doc.i
                      LaguerreFactory.i LaguerreFactory_doc.i
                      LegendreFactory.i LegendreFactory_doc.i
                      MeixnerFactory.i MeixnerFactory_doc.i
                      SoizeGhanemFactory.i SoizeGhanemFactory_doc.i
                      OrthogonalUniVariatePolynomialFamily.i OrthogonalUniVariatePolynomialFamily_doc.i
                      OrthonormalizationAlgorithmImplementation.i OrthonormalizationAlgorithmImplementation_doc.i
                      OrthonormalizationAlgorithm.i OrthonormalizationAlgorithm_doc.i
                      AdaptiveStieltjesAlgorithm.i AdaptiveStieltjesAlgorithm_doc.i
                      StandardDistributionPolynomialFactory.i StandardDistributionPolynomialFactory_doc.i
                      OrthogonalFunctionFactory.i OrthogonalFunctionFactory_doc.i
                      OrthogonalProductPolynomialFactory.i OrthogonalProductPolynomialFactory_doc.i
                      OrthogonalBasis.i OrthogonalBasis_doc.i
                      UniVariateFunctionImplementation.i UniVariateFunctionImplementation_doc.i
                      UniVariateFunction.i UniVariateFunction_doc.i
                      MonomialFunction.i MonomialFunction_doc.i
                      OrthogonalUniVariateFunctionFactory.i OrthogonalUniVariateFunctionFactory_doc.i
                      OrthogonalUniVariateFunctionFamily.i OrthogonalUniVariateFunctionFamily_doc.i
                      OrthogonalUniVariatePolynomialFunctionFactory.i OrthogonalUniVariatePolynomialFunctionFactory_doc.i
                      FourierSeriesFactory.i FourierSeriesFactory_doc.i
                      HaarWaveletFactory.i HaarWaveletFactory_doc.i
                      OrthogonalProductFunctionFactory.i OrthogonalProductFunctionFactory_doc.i
                      UncertaintyOrthogonalBasisTemplateDefs.i
                   )

ot_add_python_module(metamodel metamodel_module.i
                      MetaModelResult.i MetaModelResult_doc.i
                      MetaModelAlgorithm.i MetaModelAlgorithm_doc.i
                      KrigingResult.i KrigingResult_doc.i
                      LinearModelResult.i LinearModelResult_doc.i
                      MetaModelValidation.i MetaModelValidation_doc.i
                      GeneralLinearModelResult.i GeneralLinearModelResult_doc.i
                      GeneralLinearModelAlgorithm.i GeneralLinearModelAlgorithm_doc.i
                      LinearTaylor.i LinearTaylor_doc.i
                      QuadraticTaylor.i QuadraticTaylor_doc.i
                      LinearLeastSquares.i LinearLeastSquares_doc.i
                      QuadraticLeastSquares.i QuadraticLeastSquares_doc.i
                      AdaptiveStrategyImplementation.i AdaptiveStrategyImplementation_doc.i
                      FixedStrategy.i FixedStrategy_doc.i
                      CleaningStrategy.i CleaningStrategy_doc.i
                      AdaptiveStrategy.i AdaptiveStrategy_doc.i
                      ProjectionStrategyImplementation.i ProjectionStrategyImplementation_doc.i
                      LeastSquaresStrategy.i LeastSquaresStrategy_doc.i
                      IntegrationStrategy.i IntegrationStrategy_doc.i
                      ProjectionStrategy.i ProjectionStrategy_doc.i
                      FunctionalChaosResult.i FunctionalChaosResult_doc.i
                      FunctionalChaosAlgorithm.i FunctionalChaosAlgorithm_doc.i
                      KrigingAlgorithm.i KrigingAlgorithm_doc.i
                      GaussianProcessFitterResult.i GaussianProcessFitterResult_doc.i
                      GaussianProcessFitter.i GaussianProcessFitter_doc.i
                      GaussianProcessRegressionResult.i GaussianProcessRegressionResult_doc.i
                      GaussianProcessRegression.i GaussianProcessRegression_doc.i
                      GaussianProcessConditionalCovariance.i GaussianProcessConditionalCovariance_doc.i
                      LinearModelStepwiseAlgorithm.i LinearModelStepwiseAlgorithm_doc.i
                      LinearModelAlgorithm.i LinearModelAlgorithm_doc.i
                      LinearModelAnalysis.i LinearModelAnalysis_doc.i
                      LinearModelValidation.i LinearModelValidation_doc.i
                      RandomVector.i RandomVector_doc.i
                      CompositeRandomVector.i CompositeRandomVector_doc.i
                      ThresholdEvent.i ThresholdEvent_doc.i
                      DomainEvent.i DomainEvent_doc.i
                      ProcessEvent.i ProcessEvent_doc.i
                      StandardEvent.i StandardEvent_doc.i
                      IntersectionEvent.i IntersectionEvent_doc.i
                      UnionEvent.i UnionEvent_doc.i
                      FunctionalChaosRandomVector.i FunctionalChaosRandomVector_doc.i
                      FunctionalChaosSobolIndices.i FunctionalChaosSobolIndices_doc.i
                      FunctionalChaosValidation.i FunctionalChaosValidation_doc.i
                      KrigingRandomVector.i KrigingRandomVector_doc.i
                      LeastSquaresExpansion.i LeastSquaresExpansion_doc.i
                      IntegrationExpansion.i IntegrationExpansion_doc.i
                      FieldFunctionalChaosAlgorithm.i FieldFunctionalChaosAlgorithm_doc.i
                      FieldToPointFunctionalChaosAlgorithm.i FieldToPointFunctionalChaosAlgorithm_doc.i
                      PointToFieldFunctionalChaosAlgorithm.i PointToFieldFunctionalChaosAlgorithm_doc.i
                      FieldFunctionalChaosResult.i FieldFunctionalChaosResult_doc.i
                      FieldFunctionalChaosSobolIndices.i FieldFunctionalChaosSobolIndices_doc.i
                      UncertaintyMetaModelTemplateDefs.i
                   )

ot_add_python_module(transformation transformation_module.i
                      MarginalTransformationEvaluation.i MarginalTransformationEvaluation_doc.i
                      MarginalTransformationGradient.i
                      MarginalTransformationHessian.i
                      NatafEllipticalCopulaEvaluation.i
                      NatafEllipticalCopulaGradient.i
                      NatafEllipticalCopulaHessian.i
                      InverseNatafEllipticalCopulaEvaluation.i
                      InverseNatafEllipticalCopulaGradient.i
                      InverseNatafEllipticalCopulaHessian.i
                      NatafIndependentCopulaEvaluation.i
                      NatafIndependentCopulaGradient.i
                      NatafIndependentCopulaHessian.i
                      InverseNatafIndependentCopulaEvaluation.i
                      InverseNatafIndependentCopulaGradient.i
                      InverseNatafIndependentCopulaHessian.i
                      NatafEllipticalDistributionEvaluation.i
                      NatafEllipticalDistributionGradient.i
                      NatafEllipticalDistributionHessian.i
                      InverseNatafEllipticalDistributionEvaluation.i
                      InverseNatafEllipticalDistributionGradient.i
                      InverseNatafEllipticalDistributionHessian.i
                      RosenblattEvaluation.i
                      InverseRosenblattEvaluation.i
                      BoxCoxFactory.i BoxCoxFactory_doc.i
                      TrendFactory.i TrendFactory_doc.i
                      DistributionTransformation.i DistributionTransformation_doc.i
                      UncertaintyTransformationTemplateDefs.i
                   )

ot_add_python_module(analytical analytical_module.i
                      AnalyticalResult.i AnalyticalResult_doc.i
                      Analytical.i Analytical_doc.i
                      FORMResult.i FORMResult_doc.i
                      FORM.i FORM_doc.i
                      MultiFORMResult.i MultiFORMResult_doc.i
                      SORMResult.i SORMResult_doc.i
                      SORM.i SORM_doc.i
                      StrongMaximumTest.i StrongMaximumTest_doc.i
                      SystemFORM.i SystemFORM_doc.i
                      MultiFORM.i MultiFORM_doc.i
                      UncertaintyAnalyticalTemplateDefs.i
                   )

ot_add_python_module(simulation simulation_module.i
                      ANCOVA.i ANCOVA_doc.i
                      SobolIndicesAlgorithmImplementation.i SobolIndicesAlgorithmImplementation_doc.i
                      SobolIndicesAlgorithm.i SobolIndicesAlgorithm_doc.i
                      MartinezSensitivityAlgorithm.i MartinezSensitivityAlgorithm_doc.i
                      SaltelliSensitivityAlgorithm.i SaltelliSensitivityAlgorithm_doc.i
                      JansenSensitivityAlgorithm.i JansenSensitivityAlgorithm_doc.i
                      MauntzKucherenkoSensitivityAlgorithm.i MauntzKucherenkoSensitivityAlgorithm_doc.i
                      SobolIndicesExperiment.i SobolIndicesExperiment_doc.i
                      RankSobolSensitivityAlgorithm.i RankSobolSensitivityAlgorithm_doc.i
                      SimulationSensitivityAnalysis.i SimulationSensitivityAnalysis_doc.i
                      ProbabilitySimulationResult.i ProbabilitySimulationResult_doc.i
                      SimulationResult.i SimulationResult_doc.i
                      SimulationAlgorithm.i SimulationAlgorithm_doc.i
                      EventSimulation.i EventSimulation_doc.i
                      ProbabilitySimulationAlgorithm.i ProbabilitySimulationAlgorithm_doc.i
                      PostAnalyticalSimulation.i PostAnalyticalSimulation_doc.i
                      Wilks.i Wilks_doc.i
                      PostAnalyticalControlledImportanceSampling.i PostAnalyticalControlledImportanceSampling_doc.i
                      PostAnalyticalImportanceSampling.i PostAnalyticalImportanceSampling_doc.i
                      RootStrategyImplementation.i RootStrategyImplementation_doc.i
                      RootStrategy.i RootStrategy_doc.i
                      SamplingStrategyImplementation.i SamplingStrategyImplementation_doc.i
                      SamplingStrategy.i SamplingStrategy_doc.i
                      DirectionalSampling.i DirectionalSampling_doc.i
                      OrthogonalDirection.i OrthogonalDirection_doc.i
                      RandomDirection.i RandomDirection_doc.i
                      MediumSafe.i MediumSafe_doc.i
                      RiskyAndFast.i RiskyAndFast_doc.i
                      SafeAndSlow.i SafeAndSlow_doc.i
                      SubsetSamplingResult.i SubsetSamplingResult_doc.i
                      SubsetSampling.i SubsetSampling_doc.i
                      AdaptiveDirectionalStratification.i AdaptiveDirectionalStratification_doc.i
                      ExpectationSimulationAlgorithm.i ExpectationSimulationAlgorithm_doc.i
                      ExpectationSimulationResult.i ExpectationSimulationResult_doc.i
                      SobolSimulationAlgorithm.i SobolSimulationAlgorithm_doc.i
                      SobolSimulationResult.i SobolSimulationResult_doc.i
                      UncertaintySimulationTemplateDefs.i
                      NAISResult.i NAISResult_doc.i
                      NAIS.i NAIS_doc.i
                      CrossEntropyResult.i CrossEntropyResult_doc.i
                      CrossEntropyImportanceSampling.i CrossEntropyImportanceSampling_doc.i
                      StandardSpaceCrossEntropyImportanceSampling.i StandardSpaceCrossEntropyImportanceSampling_doc.i
                      PhysicalSpaceCrossEntropyImportanceSampling.i PhysicalSpaceCrossEntropyImportanceSampling_doc.i
                    )

ot_add_python_module(stattests stattests_module.i
                      DickeyFullerTest.i DickeyFullerTest_doc.i
                      UncertaintyStatTestsTemplateDefs.i
                    )
ot_add_python_module(FittingTest fittingtest_module.i
                      FittingTest.i FittingTest_doc.i
                    )
ot_add_python_module(HypothesisTest hypothesistest_module.i
                      HypothesisTest.i HypothesisTest_doc.i
                    )
ot_add_python_module(LinearModelTest linearmodeltest_module.i
                      LinearModelTest.i LinearModelTest_doc.i
                    )
ot_add_python_module(NormalityTest normalitytest_module.i
                      NormalityTest.i NormalityTest_doc.i
                    )
ot_add_python_module(VisualTest visualtest_module.i
                      VisualTest.i VisualTest_doc.i
                    )
ot_add_python_module(uncertainty uncertainty_module.i
                      FAST.i FAST_doc.i
                      FejerAlgorithm.i FejerAlgorithm_doc.i
                      TaylorExpansionMoments.i TaylorExpansionMoments_doc.i
                      HSICStatImplementation.i HSICStatImplementation_doc.i
                      HSICStat.i HSICStat_doc.i
                      HSICUStat.i HSICUStat_doc.i
                      HSICVStat.i HSICVStat_doc.i
                      HSICEstimator.i HSICEstimator_doc.i
                      HSICEstimatorImplementation.i HSICEstimatorImplementation_doc.i
                      HSICEstimatorConditionalSensitivity.i HSICEstimatorConditionalSensitivity_doc.i
                      HSICEstimatorGlobalSensitivity.i HSICEstimatorGlobalSensitivity_doc.i
                      HSICEstimatorTargetSensitivity.i HSICEstimatorTargetSensitivity_doc.i
                      UncertaintyTemplateDefs.i
                    )

ot_add_python_module(testing testing_module.i
                     OTtestcode_doc.i
                    )

ot_add_python_module(experimental experimental_module.i
                     CombinationsDistribution.i CombinationsDistribution_doc.i
                     SequentialSamplingAlgorithm.i SequentialSamplingAlgorithm_doc.i
                     SequentialSamplingAlgorithmImplementation.i SequentialSamplingAlgorithmImplementation_doc.i
                     LOLAVoronoi.i LOLAVoronoi_doc.i
                     MarginalUniformOrderStatistics.i MarginalUniformOrderStatistics_doc.i
                     ConstantEvaluation.i ConstantEvaluation_doc.i
                     ConstantFunction.i ConstantFunction_doc.i
                     VertexFieldToPointFunction.i VertexFieldToPointFunction_doc.i
                     FejerExperiment.i FejerExperiment_doc.i
                     ConditionedGaussianProcess.i ConditionedGaussianProcess_doc.i
                     EfficientGlobalOptimization.i EfficientGlobalOptimization_doc.i
                     HiGHS.i HiGHS_doc.i
                     LinearProblem.i LinearProblem_doc.i
                     LeastSquaresEquationsSolver.i LeastSquaresEquationsSolver_doc.i
                     RatioOfUniforms.i RatioOfUniforms_doc.i
                     QuantileConfidence.i QuantileConfidence_doc.i
                     LineSampling.i LineSampling_doc.i
                     GaussianProcessRandomVector.i GaussianProcessRandomVector_doc.i
                     FiniteOrthogonalFunctionFactory.i FiniteOrthogonalFunctionFactory_doc.i
                    )
set (OPENTURNS_PYTHON_MODULES ${OPENTURNS_PYTHON_MODULES} PARENT_SCOPE) # for the docstring test

# only a subset of the modules actually need wrapping classes
foreach (module algo experimental experiment func metamodel model_copula model_process)
  target_link_libraries (${module} OTPythonWrapping)
endforeach ()

# Build and install openturns.memoryview
add_library (memoryview MODULE PythonReadOnlyBuffer.c)
target_include_directories (memoryview PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries (memoryview Python::Module)
set_target_properties(memoryview PROPERTIES NO_SONAME ON PREFIX "" UNITY_BUILD OFF)
if (WIN32 AND NOT CYGWIN)
  set_target_properties(memoryview PROPERTIES SUFFIX ".pyd")
else ()
  set_target_properties(memoryview PROPERTIES SUFFIX ".so")
endif ()
set_target_properties(memoryview PROPERTIES COMPILE_FLAGS "${SWIG_COMPILE_FLAGS}")
install (TARGETS memoryview LIBRARY DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)

set (EXTRA_PYTHON_FILES __init__.py dist.py viewer.py coupling_tools.py)

install (FILES ${EXTRA_PYTHON_FILES}
          DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns
       )
install (DIRECTORY usecases DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)

install (FILES ${SWIGFILES} DESTINATION ${OPENTURNS_SWIG_INCLUDE_DIRS})

if (SPHINX_FOUND)

  # exclude examples according to available dependencies
  if (NOT Bonmin_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_bonmin)")
  endif ()
  if (NOT Ceres_FOUND AND NOT CMinpack_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(calibration_flooding)|(calibration_chaboche)|(calibration_deflection_tube)")
  endif ()
  if (NOT dlib_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_dlib)")
  endif ()
  if (NOT highs_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_highs)")
  endif ()
  if (NOT Pagmo_FOUND)
    set(SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_pagmo)")
  endif ()
  if (NOT PANDAS_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(sample_pandas)")
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(estimate_gpd_wooster)|(estimate_gpd_dowjones)")
  endif ()
  if (NOT HMAT_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(gpr_cantilever_beam_hmat)")
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(gaussian_process_covariance_hmat)")
  endif ()
  if (NOT HDF5_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(study_save_load)")
  endif ()
  if (NOT NLopt_FOUND)
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(optimization_nlopt)|(optimization_rosenbrock)")
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(advanced_mle_estimator)|(kriging_advanced)")
    set (SPHINX_GALLERY_IGNORE_PATTERN "${SPHINX_GALLERY_IGNORE_PATTERN}|(kriging_hyperparameters_optimization)")
  endif ()

  # create a build site-package from which sphinx can import
  set (OPENTURNS_PYTHON_BUILD_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/site-packages)
  execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns)
  execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/usecases)
  get_target_property (module_suffix memoryview SUFFIX)
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/memoryview${module_suffix} ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/memoryview${module_suffix})
  foreach (module ${OPENTURNS_PYTHON_MODULES})
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${module}.py ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/${module}.py)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/_${module}${module_suffix} ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/_${module}${module_suffix})
  endforeach ()
  foreach (file ${EXTRA_PYTHON_FILES})
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/${file})
  endforeach ()

  # copy usecases python files
  file(GLOB usecases "${CMAKE_CURRENT_SOURCE_DIR}/usecases/*.py" "${CMAKE_CURRENT_SOURCE_DIR}/usecases/*.csv")
  foreach(file ${usecases})
    get_filename_component(fname ${file} NAME)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/usecases/${fname} ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/usecases/${fname})
  endforeach ()

  # sphinx configuration
  set (doc_formats)
  list (APPEND doc_formats html)
  list (APPEND doc_formats htmlhelp)
  list (APPEND doc_formats latex)
  list (APPEND doc_formats doctest)
  list (APPEND doc_formats qthelp)
  list (APPEND doc_formats devhelp)
  list (APPEND doc_formats linkcheck)

  set (SPHINX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx_build)
  set (SPHINX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../doc)

  file (MAKE_DIRECTORY ${SPHINX_BINARY_DIR}/_static) # currently empty
  configure_file (${SPHINX_SOURCE_DIR}/conf.py.in ${SPHINX_BINARY_DIR}/conf.py @ONLY)

  # get sphinx sources
  file (GLOB_RECURSE sphinx_sources
    ${SPHINX_SOURCE_DIR}/*.rst
    ${SPHINX_SOURCE_DIR}/*.rst_t
    ${SPHINX_SOURCE_DIR}/*.css_t
    ${SPHINX_SOURCE_DIR}/*.conf
    ${SPHINX_SOURCE_DIR}/*.py
    ${SPHINX_SOURCE_DIR}/*.png
    ${SPHINX_SOURCE_DIR}/*.svg
    ${SPHINX_SOURCE_DIR}/*.html
    ${SPHINX_SOURCE_DIR}/*.css
    ${SPHINX_SOURCE_DIR}/*.jpg
    ${SPHINX_SOURCE_DIR}/*.ico
    ${SPHINX_SOURCE_DIR}/*.sty
    ${SPHINX_SOURCE_DIR}/*.txt
    ${SPHINX_SOURCE_DIR}/*.in
    ${SPHINX_SOURCE_DIR}/*.js
  )

  # copy sphinx sources
  set (sphinx_clones)
  foreach (file ${sphinx_sources})
    if (EXISTS ${file})
      file (RELATIVE_PATH rel_file "${SPHINX_SOURCE_DIR}" "${file}")
      get_filename_component (rel_path ${rel_file} PATH)
      file (MAKE_DIRECTORY ${SPHINX_BINARY_DIR}/${rel_path})
      set (sphinx_clone ${SPHINX_BINARY_DIR}/${rel_file})
      add_custom_command (OUTPUT ${sphinx_clone}
        COMMAND ${CMAKE_COMMAND} -E copy ${file} ${sphinx_clone}
        DEPENDS ${file}
     )
      list (APPEND sphinx_clones ${sphinx_clone})
    endif ()
  endforeach ()

  set (SPHINX_FLAGS "-W -T" CACHE STRING "sphinx flags")
  separate_arguments (SPHINX_FLAGS)

  # generate sphinx rule for each format
  foreach (format ${doc_formats})
    add_custom_target (sphinx_${format}
                        COMMAND ${SPHINX_EXECUTABLE}
                                  -b ${format}
                                  -d ${SPHINX_BINARY_DIR}/.doctrees
                                  ${SPHINX_FLAGS}
                                  ${SPHINX_BINARY_DIR}
                                  ${SPHINX_BINARY_DIR}/${format}
                        DEPENDS ${sphinx_clones}
                     )
    foreach (MODULENAME ${OPENTURNS_PYTHON_MODULES})
      add_dependencies (sphinx_${format} ${MODULENAME})
    endforeach ()
    add_dependencies (sphinx_${format} memoryview)
  endforeach ()

  # build html by default
  set_target_properties (sphinx_html PROPERTIES EXCLUDE_FROM_ALL OFF)

  add_custom_command (OUTPUT ${SPHINX_BINARY_DIR}/latex/OpenTURNS.pdf
                      WORKING_DIRECTORY ${SPHINX_BINARY_DIR}/latex
                      COMMAND make all-pdf
                   )
  add_custom_target (sphinx_pdf DEPENDS ${SPHINX_BINARY_DIR}/latex/OpenTURNS.pdf)
  add_dependencies (sphinx_pdf sphinx_latex)

  install (DIRECTORY ${SPHINX_BINARY_DIR}/html DESTINATION ${OPENTURNS_DOC_PATH})
endif ()

# wheel files
configure_file (METADATA.in METADATA @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/METADATA
	 DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns-${PACKAGE_VERSION}.dist-info)
