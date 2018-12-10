#                                               -*- cmake -*-

add_definitions (${OPENTURNS_SWIG_DEFINITIONS})

# generate SWIG runtime header
set (SWIG_PYTHON_OPTIONS "-python")
execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/openturns)
add_custom_command (OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/openturns/swig_runtime.hxx
                     COMMAND ${SWIG_EXECUTABLE}
                     ARGS ${SWIG_PYTHON_OPTIONS} ${CMAKE_SWIG_FLAGS} ${OPENTURNS_SWIG_DEFINITIONS} -external-runtime ${CMAKE_CURRENT_BINARY_DIR}/openturns/swig_runtime.hxx
                     COMMENT "Swig header")
add_custom_target (generate_swig_runtime DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/openturns/swig_runtime.hxx)
add_custom_target (generate_docstrings)

include_directories (BEFORE ${CMAKE_CURRENT_BINARY_DIR})
include_directories (BEFORE ${INTERNAL_INCLUDE_DIRS})
include_directories (BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

set (OPENTURNS_PYTHON_MODULES)
macro (ot_add_python_module MODULENAME SOURCEFILE)
  set_source_files_properties (${SOURCEFILE} PROPERTIES CPLUSPLUS ON)
  set_source_files_properties (${SOURCEFILE} PROPERTIES SWIG_MODULE_NAME ${MODULENAME})
  ot_install_swig_file (${SOURCEFILE})
  if (${ARGC} GREATER 2)
    set (SWIG_MODULE_${MODULENAME}_EXTRA_DEPS ${ARGN})
  endif ()
  list (APPEND SWIG_MODULE_${MODULENAME}_EXTRA_DEPS generate_docstrings openturns/PythonWrappingFunctions.hxx)

  set (swig_other_sources)
  foreach (it ${ARGN})
    if (${it} MATCHES ".*\\.i$")
      ot_install_swig_file (${it})
    elseif(${it} MATCHES ".*_doc\\.i\\.in$")
     # handle docstring generation
      set (DOCSTRING_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${it})
      get_filename_component (DOCSTRING_FILE_BASENAME ${DOCSTRING_IN_FILE} NAME_WE)
      # https://github.com/swig/swig/issues/1273
      option (SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH "Force double escaping of backslashes" OFF)
      mark_as_advanced (SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH)
      set (TIMES x1)
      if ((SWIG_VERSION VERSION_LESS 4) OR SWIG_FORCE_DOUBLE_ESCAPE_BACKSLASH)
        set (TIMES x2)
      endif ()
      add_custom_target (generate_${DOCSTRING_FILE_BASENAME}
                          COMMAND ${CMAKE_COMMAND} -DDOCSTRING_IN_FILE=${DOCSTRING_IN_FILE} -DDOCSTRING_FILE=${CMAKE_CURRENT_BINARY_DIR}/${DOCSTRING_FILE_BASENAME}.i -DTIMES=${TIMES} -P ${CMAKE_SOURCE_DIR}/cmake/escape_backslash.cmake
                          DEPENDS ${DOCSTRING_IN_FILE})
      add_dependencies (generate_docstrings generate_${DOCSTRING_FILE_BASENAME})
      list (APPEND SWIGFILES ${CMAKE_CURRENT_BINARY_DIR}/${DOCSTRING_FILE_BASENAME}.i)
    else ()
      list (APPEND swig_other_sources "${it}")
    endif ()
  endforeach ()

  # swig_add_module is deprecated
  if (CMAKE_VERSION VERSION_LESS 3.8)
    swig_add_module (${MODULENAME} python ${SOURCEFILE} ${swig_other_sources})
  else ()
    swig_add_library (${MODULENAME} LANGUAGE python SOURCES ${SOURCEFILE} ${swig_other_sources})
  endif ()

  # UseSWIG generates now standard target names
  if (CMAKE_VERSION VERSION_LESS 3.13)
    set (module_target ${SWIG_MODULE_${MODULENAME}_REAL_NAME})
  else ()
    set (module_target ${MODULENAME})
  endif ()

  add_dependencies (${module_target} generate_swig_runtime)
  swig_link_libraries (${MODULENAME} OT)

  target_link_libraries_with_dynamic_lookup (${module_target} ${PYTHON_LIBRARIES})

  if (CMAKE_VERSION VERSION_LESS 3.1)
    set_target_properties (${module_target} PROPERTIES NO_SONAME ON)
  endif ()

  # allows to pass compile flags like -O1 to reduce memory usage
  if (DEFINED SWIG_COMPILE_FLAGS)
    set_target_properties (${module_target} PROPERTIES COMPILE_FLAGS ${SWIG_COMPILE_FLAGS})
  endif ()

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULENAME}.py
           DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)
  install (TARGETS ${module_target}
            LIBRARY DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)

  list (APPEND OPENTURNS_PYTHON_MODULES ${MODULENAME})
endmacro (ot_add_python_module)

install (DIRECTORY openturns DESTINATION ${OPENTURNS_INCLUDE_PATH})

if (LIBXML2_FOUND AND CMAKE_VERSION VERSION_LESS 3.12)
  list (APPEND SWIG_MODULE_common_EXTRA_FLAGS -DOPENTURNS_HAVE_LIBXML2=1)
endif ()

ot_add_python_module(common common_module.i
                      OTtypes.i
                      OTexceptions.i
                      Memory.i
                      TBB.i TBB_doc.i.in
                      Object.i Object_doc.i.in
                      PersistentObject.i PersistentObject_doc.i.in
                      Study.i Study_doc.i.in
                      StorageManager.i StorageManager_doc.i.in
                      XMLStorageManager.i XMLStorageManager_doc.i.in
                      Log.i Log_doc.i.in
                      TTY.i TTY_doc.i.in
                      Path.i Path_doc.i.in
                      Catalog.i
                      ResourceMap.i ResourceMap_doc.i.in
                      ComparisonOperatorImplementation.i
                      ComparisonOperator.i ComparisonOperator_doc.i.in
                      Equal.i Equal_doc.i.in
                      Greater.i Greater_doc.i.in
                      GreaterOrEqual.i GreaterOrEqual_doc.i.in
                      Less.i Less_doc.i.in
                      LessOrEqual.i LessOrEqual_doc.i.in
                      Cache.i
                      PlatformInfo.i PlatformInfo_doc.i.in
                      SharedPointer.i
                      Pointer.i
                      InterfaceObject.i InterfaceObject_doc.i.in
                      TypedInterfaceObject.i
                      TypedCollectionInterfaceObject.i
                      BaseCommonTemplateDefs.i
                   )

if (LIBXML2_FOUND AND NOT CMAKE_VERSION VERSION_LESS 3.12)
  set_property (TARGET ${SWIG_MODULE_common_REAL_NAME} PROPERTY SWIG_COMPILE_DEFINITIONS OPENTURNS_HAVE_LIBXML2=1)
endif ()

ot_add_python_module(typ typ_module.i
                      Collection.i Collection_doc.i.in
                      PersistentCollection.i
                      Description.i Description_doc.i.in
                      Point.i Point_doc.i.in
                      PointWithDescription.i PointWithDescription_doc.i.in
                      DomainImplementation.i DomainImplementation_doc.i.in
                      Interval.i Interval_doc.i.in
                      Indices.i Indices_doc.i.in
                      IndicesCollection.i IndicesCollection_doc.i.in
                      IndicesCollectionImplementation.i
                      MatrixImplementation.i
                      Matrix.i Matrix_doc.i.in
                      SquareMatrix.i SquareMatrix_doc.i.in
                      SymmetricMatrix.i SymmetricMatrix_doc.i.in
                      TensorImplementation.i TensorImplementation_doc.i.in
                      Tensor.i Tensor_doc.i.in
                      SymmetricTensor.i SymmetricTensor_doc.i.in
                      ComplexTensorImplementation.i
                      ComplexTensor.i ComplexTensor_doc.i.in
                      ComplexMatrixImplementation.i ComplexMatrixImplementation_doc.i.in
                      ComplexMatrix.i ComplexMatrix_doc.i.in
                      HermitianMatrix.i HermitianMatrix_doc.i.in
                      SquareComplexMatrix.i SquareComplexMatrix_doc.i.in
                      TriangularMatrix.i TriangularMatrix_doc.i.in
                      TriangularComplexMatrix.i TriangularComplexMatrix_doc.i.in
                      CovarianceMatrix.i CovarianceMatrix_doc.i.in
                      CorrelationMatrix.i CorrelationMatrix_doc.i.in
                      IdentityMatrix.i IdentityMatrix_doc.i.in
                      Sample.i Sample_doc.i.in
                      HistoryStrategyImplementation.i HistoryStrategyImplementation_doc.i.in
                      HistoryStrategy.i HistoryStrategy_doc.i.in
                      BaseTypCollection.i
                      BaseTypTemplateDefs.i
                   )

ot_add_python_module(statistics statistics_module.i
                      Compact.i Compact_doc.i.in
                      Full.i Full_doc.i.in
                      Last.i Last_doc.i.in
                      Null.i Null_doc.i.in
                      LinearModel.i LinearModel_doc.i.in
                      LinearModelFactory.i LinearModelFactory_doc.i.in
                      TestResult.i TestResult_doc.i.in
                      RandomGenerator.i RandomGenerator_doc.i.in
                      RandomGeneratorState.i RandomGeneratorState_doc.i.in
                      CorrelationAnalysis.i CorrelationAnalysis_doc.i.in
                      LowDiscrepancySequenceImplementation.i LowDiscrepancySequenceImplementation_doc.i.in
                      LowDiscrepancySequence.i LowDiscrepancySequence_doc.i.in
                      SobolSequence.i SobolSequence_doc.i.in
                      FaureSequence.i FaureSequence_doc.i.in
                      HaltonSequence.i HaltonSequence_doc.i.in
                      HaselgroveSequence.i HaselgroveSequence_doc.i.in
                      ReverseHaltonSequence.i ReverseHaltonSequence_doc.i.in
                      SphericalModel.i SphericalModel_doc.i.in
                      FilteringWindowsImplementation.i FilteringWindowsImplementation_doc.i.in
                      FilteringWindows.i FilteringWindows_doc.i.in
                      Hamming.i Hamming_doc.i.in
                      Hanning.i Hanning_doc.i.in
                      FFTImplementation.i FFTImplementation_doc.i.in
                      FFT.i FFT_doc.i.in
                      SpectralModelImplementation.i SpectralModelImplementation_doc.i.in
                      SpectralModel.i SpectralModel_doc.i.in
                      CovarianceModelImplementation.i CovarianceModelImplementation_doc.i.in
                      CovarianceModel.i CovarianceModel_doc.i.in
                      StationaryCovarianceModel.i 
                      SpectralModelFactoryImplementation.i SpectralModelFactoryImplementation_doc.i.in
                      SpectralModelFactory.i SpectralModelFactory_doc.i.in
                      UserDefinedSpectralModel.i UserDefinedSpectralModel_doc.i.in
                      WelchFactory.i WelchFactory_doc.i.in
                      ExponentialModel.i ExponentialModel_doc.i.in
                      ExponentiallyDampedCosineModel.i ExponentiallyDampedCosineModel_doc.i.in
                      CauchyModel.i CauchyModel_doc.i.in
                      CovarianceModelFactoryImplementation.i CovarianceModelFactoryImplementation_doc.i.in
                      CovarianceModelFactory.i CovarianceModelFactory_doc.i.in
                      UserDefinedCovarianceModel.i UserDefinedCovarianceModel_doc.i.in
                      UserDefinedStationaryCovarianceModel.i UserDefinedStationaryCovarianceModel_doc.i.in
                      StationaryCovarianceModelFactory.i StationaryCovarianceModelFactory_doc.i.in
                      NonStationaryCovarianceModelFactory.i NonStationaryCovarianceModelFactory_doc.i.in
                      SquaredExponential.i SquaredExponential_doc.i.in
                      GeneralizedExponential.i GeneralizedExponential_doc.i.in
                      AbsoluteExponential.i AbsoluteExponential_doc.i.in
                      MaternModel.i MaternModel_doc.i.in
                      DiracCovarianceModel.i DiracCovarianceModel_doc.i.in
                      ProductCovarianceModel.i ProductCovarianceModel_doc.i.in
                      RankMCovarianceModel.i RankMCovarianceModel_doc.i.in
                      FractionalBrownianMotionModel.i FractionalBrownianMotionModel_doc.i.in
                      TensorizedCovarianceModel.i TensorizedCovarianceModel_doc.i.in
                      HMatrixParameters.i HMatrixParameters_doc.i.in
                      HMatrixFactory.i HMatrixFactory_doc.i.in
                      HMatrix.i HMatrix_doc.i.in
                      HMatrixImplementation.i HMatrixImplementation_doc.i.in
                      BaseStatisticsTemplateDefs.i
                   )

ot_add_python_module(geom geom_module.i
                      Domain.i Domain_doc.i.in
                      BipartiteGraph.i BipartiteGraph_doc.i.in
                      Mesh.i Mesh_doc.i.in
                      RegularGrid.i RegularGrid_doc.i.in
                      IntervalMesher.i IntervalMesher_doc.i.in
                      MeshDomain.i MeshDomain_doc.i.in
                      DomainComplement.i DomainComplement_doc.i.in
                      DomainIntersection.i DomainIntersection_doc.i.in
                      DomainUnion.i DomainUnion_doc.i.in
                      DomainDisjunctiveUnion.i DomainDisjunctiveUnion_doc.i.in
                      DomainDifference.i DomainDifference_doc.i.in
                      BaseGeomTemplateDefs.i
                    )

ot_add_python_module(graph graph_module.i
                      DrawableImplementation.i DrawableImplementation_doc.i.in
                      Drawable.i Drawable_doc.i.in
                      GraphImplementation.i GraphImplementation_doc.i.in
                      Graph.i Graph_doc.i.in
                      BarPlot.i BarPlot_doc.i.in
                      Cloud.i Cloud_doc.i.in
                      Contour.i Contour_doc.i.in
                      Curve.i Curve_doc.i.in
                      Pairs.i Pairs_doc.i.in
                      Pie.i Pie_doc.i.in
                      Polygon.i Polygon_doc.i.in
                      PolygonArray.i PolygonArray_doc.i.in
                      Staircase.i Staircase_doc.i.in
                      Text.i Text_doc.i.in
                      BaseGraphTemplateDefs.i
                   )

ot_add_python_module(func func_module.i
                      EnumerateFunction.i EnumerateFunction_doc.i.in
                      EnumerateFunctionImplementation.i EnumerateFunctionImplementation_doc.i.in
                      LinearEnumerateFunction.i LinearEnumerateFunction_doc.i.in
                      HyperbolicAnisotropicEnumerateFunction.i HyperbolicAnisotropicEnumerateFunction_doc.i.in
                      NormInfEnumerateFunction.i NormInfEnumerateFunction_doc.i.in
                      SpecFunc.i SpecFunc_doc.i.in
                      EvaluationImplementation.i EvaluationImplementation_doc.i.in
                      Evaluation.i Evaluation_doc.i.in
                      GradientImplementation.i GradientImplementation_doc.i.in
                      Gradient.i Gradient_doc.i.in
                      HessianImplementation.i HessianImplementation_doc.i.in
                      Hessian.i Hessian_doc.i.in
                      NoEvaluation.i
                      NoGradient.i
                      NoHessian.i
                      TranslationEvaluation.i TranslationEvaluation_doc.i.in
                      LinearEvaluation.i LinearEvaluation_doc.i.in
                      LinearGradient.i LinearGradient_doc.i.in
                      QuadraticEvaluation.i
                      ConstantGradient.i
                      ConstantHessian.i
                      ComposedEvaluation.i
                      ComposedGradient.i
                      ComposedHessian.i
                      DualLinearCombinationEvaluation.i DualLinearCombinationEvaluation_doc.i.in
                      DualLinearCombinationGradient.i DualLinearCombinationGradient_doc.i.in
                      DualLinearCombinationHessian.i DualLinearCombinationHessian_doc.i.in
                      LinearCombinationFunction.i LinearCombinationFunction_doc.i.in
                      LinearCombinationEvaluation.i
                      LinearCombinationGradient.i
                      LinearCombinationHessian.i
                      ParametricFunction.i ParametricFunction_doc.i.in
                      ProductEvaluation.i
                      ProductGradient.i
                      ProductHessian.i
                      FunctionImplementation.i FunctionImplementation_doc.i.in
                      ProductFunction.i ProductFunction_doc.i.in
                      Function.i Function_doc.i.in
                      EvaluationProxy.i
                      MemoizeEvaluation.i
                      MemoizeFunction.i MemoizeFunction_doc.i.in
                      TensorizedUniVariateFunctionFactory.i TensorizedUniVariateFunctionFactory_doc.i.in
                      UniVariateFunctionFactory.i UniVariateFunctionFactory_doc.i.in
                      MonomialFunctionFactory.i MonomialFunctionFactory_doc.i.in
                      UniVariateFunctionFamily.i UniVariateFunctionFamily_doc.i.in
                      AggregatedFunction.i AggregatedFunction_doc.i.in
                      SymbolicFunction.i SymbolicFunction_doc.i.in
                      ComposedFunction.i ComposedFunction_doc.i.in
                      DualLinearCombinationFunction.i DualLinearCombinationFunction_doc.i.in
                      BaseFuncCollection.i
                      FieldFunctionImplementation.i FieldFunctionImplementation_doc.i.in
                      FieldFunction.i FieldFunction_doc.i.in
                      FieldToPointFunctionImplementation.i FieldToPointFunctionImplementation_doc.i.in
                      FieldToPointFunction.i FieldToPointFunction_doc.i.in
                      PointToFieldFunctionImplementation.i PointToFieldFunctionImplementation_doc.i.in
                      PointToFieldFunction.i PointToFieldFunction_doc.i.in
		      ParametricPointToFieldFunction.i ParametricPointToFieldFunction_doc.i.in
		      PointToPointEvaluation.i PointToPointEvaluation_doc.i.in
		      PointToPointConnection.i PointToPointConnection_doc.i.in
		      FieldToPointConnection.i FieldToPointConnection_doc.i.in
		      PointToFieldConnection.i PointToFieldConnection_doc.i.in
		      FieldToFieldConnection.i FieldToFieldConnection_doc.i.in
		      KarhunenLoeveProjection.i KarhunenLoeveProjection_doc.i.in
		      KarhunenLoeveLifting.i KarhunenLoeveLifting_doc.i.in
                      IndicatorFunction.i IndicatorFunction_doc.i.in
                      ValueFunction.i ValueFunction_doc.i.in
                      VertexValueFunction.i VertexValueFunction_doc.i.in
                      TranslationFunction.i TranslationFunction_doc.i.in
                      LinearFunction.i LinearFunction_doc.i.in
                      QuadraticFunction.i QuadraticFunction_doc.i.in
                      UniVariatePolynomialImplementation.i UniVariatePolynomialImplementation_doc.i.in
                      UniVariatePolynomial.i UniVariatePolynomial_doc.i.in
                      AggregatedEvaluation.i AggregatedEvaluation_doc.i.in
                      SymbolicEvaluation.i
                      SymbolicGradient.i
                      SymbolicHessian.i
                      DatabaseFunction.i DatabaseFunction_doc.i.in
                      DatabaseEvaluation.i DatabaseEvaluation_doc.i.in
                      IndicatorEvaluation.i
                      PiecewiseLinearEvaluation.i PiecewiseLinearEvaluation_doc.i.in
                      PiecewiseHermiteEvaluation.i PiecewiseHermiteEvaluation_doc.i.in
                      ProductPolynomialEvaluation.i ProductPolynomialEvaluation_doc.i.in
                      ParametricEvaluation.i
                      ParametricGradient.i
                      ParametricHessian.i
                      P1LagrangeEvaluation.i P1LagrangeEvaluation_doc.i.in
                      P1LagrangeInterpolation.i P1LagrangeInterpolation_doc.i.in
                      BasisImplementation.i BasisImplementation_doc.i.in
                      Basis.i Basis_doc.i.in
                      BasisSequenceImplementation.i BasisSequenceImplementation_doc.i.in
                      BasisSequence.i BasisSequence_doc.i.in
                      BasisSequenceFactoryImplementation.i BasisSequenceFactoryImplementation_doc.i.in
                      BasisSequenceFactory.i BasisSequenceFactory_doc.i.in
                      LARS.i LARS_doc.i.in
                      BoxCoxEvaluation.i
                      BoxCoxTransform.i BoxCoxTransform_doc.i.in
                      InverseBoxCoxEvaluation.i
                      PythonEvaluation.cxx
                      PythonGradient.cxx
                      PythonHessian.cxx
                      PythonFieldFunction.cxx
                      PythonFieldToPointFunction.cxx
                      PythonPointToFieldFunction.cxx
                      InverseBoxCoxTransform.i InverseBoxCoxTransform_doc.i.in
                      TrendEvaluation.i
                      TrendTransform.i TrendTransform_doc.i.in
                      InverseTrendEvaluation.i
                      InverseTrendTransform.i InverseTrendTransform_doc.i.in
                      BasisFactory.i BasisFactory_doc.i.in
                      ConstantBasisFactory.i ConstantBasisFactory_doc.i.in
                      LinearBasisFactory.i LinearBasisFactory_doc.i.in
                      QuadraticBasisFactory.i QuadraticBasisFactory_doc.i.in
                      FieldImplementation.i FieldImplementation_doc.i.in
                      Field.i Field_doc.i.in
                      TimeSeries.i TimeSeries_doc.i.in
                      ProcessSample.i ProcessSample_doc.i.in
                      ProcessSampleImplementation.i ProcessSampleImplementation_doc.i.in
                      BaseFuncTemplateDefs.i
                   )

ot_add_python_module(diff diff_module.i
                      FiniteDifferenceStepImplementation.i FiniteDifferenceStepImplementation_doc.i.in
                      FiniteDifferenceStep.i FiniteDifferenceStep_doc.i.in
                      ConstantStep.i ConstantStep_doc.i.in
                      BlendedStep.i BlendedStep_doc.i.in
                      FiniteDifferenceGradient.i FiniteDifferenceGradient_doc.i.in
                      FiniteDifferenceHessian.i FiniteDifferenceHessian_doc.i.in
                      CenteredFiniteDifferenceGradient.i CenteredFiniteDifferenceGradient_doc.i.in
                      CenteredFiniteDifferenceHessian.i CenteredFiniteDifferenceHessian_doc.i.in
                      NonCenteredFiniteDifferenceGradient.i NonCenteredFiniteDifferenceGradient_doc.i.in
                      BaseDiffTemplateDefs.i
                    )

ot_add_python_module(optim optim_module.i
                      OptimizationProblem.i OptimizationProblem_doc.i.in
                      OptimizationProblemImplementation.i OptimizationProblemImplementation_doc.i.in
                      OptimizationAlgorithm.i OptimizationAlgorithm_doc.i.in
                      OptimizationResult.i OptimizationResult_doc.i.in
                      OptimizationAlgorithmImplementation.i OptimizationAlgorithmImplementation_doc.i.in
                      NearestPointCheckerResult.i
                      NearestPointChecker.i
                      AbdoRackwitz.i AbdoRackwitz_doc.i.in
                      Cobyla.i Cobyla_doc.i.in
                      TNC.i TNC_doc.i.in
                      SQP.i SQP_doc.i.in
                      MultiStart.i MultiStart_doc.i.in
                      NLopt.i NLopt_doc.i.in
                      OPTpp.i OPTpp_doc.i.in
                      LevelSet.i LevelSet_doc.i.in
                      LevelSetMesher.i LevelSetMesher_doc.i.in
                      BaseOptimTemplateDefs.i
                   )

ot_add_python_module(solver solver_module.i
                      SolverImplementation.i SolverImplementation_doc.i.in
                      Solver.i Solver_doc.i.in
                      Bisection.i Bisection_doc.i.in
                      Secant.i Secant_doc.i.in
                      Brent.i Brent_doc.i.in
                      ODESolverImplementation.i ODESolverImplementation_doc.i.in
                      ODESolver.i ODESolver_doc.i.in
                      RungeKutta.i RungeKutta_doc.i.in
                      Fehlberg.i Fehlberg_doc.i.in
                      BaseSolverTemplateDefs.i
                   )

ot_add_python_module(algo algo_module.i
                      ApproximationAlgorithmImplementation.i ApproximationAlgorithmImplementation_doc.i.in
                      ApproximationAlgorithmImplementationFactory.i ApproximationAlgorithmImplementationFactory_doc.i.in
                      ApproximationAlgorithm.i ApproximationAlgorithm_doc.i.in
                      DesignProxy.i DesignProxy_doc.i.in
                      LeastSquaresMethodImplementation.i LeastSquaresMethodImplementation_doc.i.in
                      LeastSquaresMethod.i LeastSquaresMethod_doc.i.in
                      CholeskyMethod.i CholeskyMethod_doc.i.in
                      QRMethod.i QRMethod_doc.i.in
                      SVDMethod.i SVDMethod_doc.i.in
                      SparseMethod.i SparseMethod_doc.i.in
                      LeastSquaresMetaModelSelection.i LeastSquaresMetaModelSelection_doc.i.in
                      LeastSquaresMetaModelSelectionFactory.i LeastSquaresMetaModelSelectionFactory_doc.i.in
                      ClassifierImplementation.i ClassifierImplementation_doc.i.in
                      Classifier.i Classifier_doc.i.in
                      FittingAlgorithmImplementation.i FittingAlgorithmImplementation_doc.i.in
                      FittingAlgorithm.i FittingAlgorithm_doc.i.in
                      CorrectedLeaveOneOut.i CorrectedLeaveOneOut_doc.i.in
                      NearestNeighbourAlgorithmImplementation.i NearestNeighbourAlgorithmImplementation_doc.i.in
                      NearestNeighbourAlgorithm.i NearestNeighbourAlgorithm_doc.i.in
                      KDTree.i KDTree_doc.i.in
                      RegularGridNearestNeighbour.i RegularGridNearestNeighbour_doc.i.in
                      NearestNeighbour1D.i NearestNeighbour1D_doc.i.in
                      NaiveNearestNeighbour.i NaiveNearestNeighbour_doc.i.in
                      EnclosingSimplexAlgorithmImplementation.i EnclosingSimplexAlgorithmImplementation_doc.i.in
                      EnclosingSimplexAlgorithm.i EnclosingSimplexAlgorithm_doc.i.in
                      NaiveEnclosingSimplex.i  NaiveEnclosingSimplex_doc.i.in
                      RegularGridEnclosingSimplex.i  RegularGridEnclosingSimplex_doc.i.in
                      EnclosingSimplexMonotonic1D.i  EnclosingSimplexMonotonic1D_doc.i.in
                      BoundingVolumeHierarchy.i BoundingVolumeHierarchy_doc.i.in
                      KFold.i KFold_doc.i.in
                      PenalizedLeastSquaresAlgorithm.i PenalizedLeastSquaresAlgorithm_doc.i.in
                      PenalizedLeastSquaresAlgorithmFactory.i PenalizedLeastSquaresAlgorithmFactory_doc.i.in
                      PythonEvaluation.cxx
                      PythonGradient.cxx
                      PythonHessian.cxx
                      KissFFT.i KissFFT_doc.i.in
                      IntegrationAlgorithmImplementation.i IntegrationAlgorithmImplementation_doc.i.in
                      IntegrationAlgorithm.i IntegrationAlgorithm_doc.i.in
                      FilonQuadrature.i FilonQuadrature_doc.i.in
                      GaussKronrodRule.i GaussKronrodRule_doc.i.in
                      GaussKronrod.i GaussKronrod_doc.i.in
                      GaussLegendre.i GaussLegendre_doc.i.in
                      IteratedQuadrature.i IteratedQuadrature_doc.i.in
                      ExpertMixture.i ExpertMixture_doc.i.in
		      KarhunenLoeveResultImplementation.i KarhunenLoeveResultImplementation_doc.i.in
		      KarhunenLoeveResult.i KarhunenLoeveResult_doc.i.in
		      KarhunenLoeveAlgorithmImplementation.i KarhunenLoeveAlgorithmImplementation_doc.i.in
		      KarhunenLoeveP1Algorithm.i KarhunenLoeveP1Algorithm_doc.i.in
		      KarhunenLoeveAlgorithm.i KarhunenLoeveAlgorithm_doc.i.in
                      BaseAlgoTemplateDefs.i
                   )

ot_add_python_module(experiment experiment_module.i
                      ExperimentImplementation.i ExperimentImplementation_doc.i.in
                      StratifiedExperiment.i StratifiedExperiment_doc.i.in
                      Experiment.i Experiment_doc.i.in
                      Axial.i Axial_doc.i.in
                      Composite.i Composite_doc.i.in
                      Factorial.i Factorial_doc.i.in
                      Box.i Box_doc.i.in
                      CombinatorialGeneratorImplementation.i CombinatorialGeneratorImplementation_doc.i.in
                      CombinatorialGenerator.i CombinatorialGenerator_doc.i.in
                      Combinations.i Combinations_doc.i.in
                      KPermutations.i KPermutations_doc.i.in
                      Tuples.i Tuples_doc.i.in
                      PythonExperiment.cxx
                      BaseExperimentTemplateDefs.i
                   )

ot_add_python_module(base base_module.i
                      BaseTemplateDefs.i
                   )

ot_add_python_module(model_copula model_copula_module.i
                      DistributionImplementation.i DistributionImplementation_doc.i.in
                      DistributionFactoryImplementation.i DistributionFactoryImplementation_doc.i.in
                      Distribution.i Distribution_doc.i.in
                      DistributionFactoryResult.i DistributionFactoryResult_doc.i.in
                      DistributionFactory.i DistributionFactory_doc.i.in
                      DistributionParametersImplementation.i DistributionParametersImplementation_doc.i.in
                      DistributionParameters.i DistributionParameters_doc.i.in
                      ContinuousDistribution.i
                      EllipticalDistribution.i
                      DiscreteDistribution.i
                      CopulaImplementation.i CopulaImplementation_doc.i.in
                      Copula.i Copula_doc.i.in
                      ArchimedeanCopula.i ArchimedeanCopula_doc.i.in
                      SklarCopula.i SklarCopula_doc.i.in
                      IndependentCopula.i IndependentCopula_doc.i.in
                      MinCopula.i MinCopula_doc.i.in
                      NormalCopula.i NormalCopula_doc.i.in
                      NormalCopulaFactory.i NormalCopulaFactory_doc.i.in
                      OrdinalSumCopula.i OrdinalSumCopula_doc.i.in
                      FarlieGumbelMorgensternCopula.i FarlieGumbelMorgensternCopula_doc.i.in
                      FarlieGumbelMorgensternCopulaFactory.i FarlieGumbelMorgensternCopulaFactory_doc.i.in
                      AliMikhailHaqCopula.i AliMikhailHaqCopula_doc.i.in
                      AliMikhailHaqCopulaFactory.i AliMikhailHaqCopulaFactory_doc.i.in
                      FrankCopula.i FrankCopula_doc.i.in
                      FrankCopulaFactory.i FrankCopulaFactory_doc.i.in
                      ClaytonCopula.i ClaytonCopula_doc.i.in
                      ClaytonCopulaFactory.i ClaytonCopulaFactory_doc.i.in
                      GumbelCopula.i GumbelCopula_doc.i.in
                      GumbelCopulaFactory.i GumbelCopulaFactory_doc.i.in
                      ComposedDistribution.i ComposedDistribution_doc.i.in
                      ComposedCopula.i ComposedCopula_doc.i.in
                      CumulativeDistributionNetwork.i CumulativeDistributionNetwork_doc.i.in
                      PythonDistribution.cxx
                      UncertaintyModelCopulaCollection.i
                      UncertaintyModelCopulaTemplateDefs.i
                   )

ot_add_python_module(randomvector randomvector_module.i
                      RandomVectorImplementation.i RandomVectorImplementation_doc.i.in
                      ConstantRandomVector.i
                      UsualRandomVector.i
                      UncertaintyRandomVectorTemplateDefs.i
                   )

ot_add_python_module(model_process model_process_module.i
                      ProcessImplementation.i ProcessImplementation_doc.i.in
                      Process.i Process_doc.i.in
                      WhiteNoise.i WhiteNoise_doc.i.in
                      RandomWalk.i RandomWalk_doc.i.in
                      ARMACoefficients.i ARMACoefficients_doc.i.in
                      ARMAState.i ARMAState_doc.i.in
                      ARMA.i ARMA_doc.i.in
                      SpectralGaussianProcess.i SpectralGaussianProcess_doc.i.in
                      GaussianProcess.i GaussianProcess_doc.i.in
                      AggregatedProcess.i AggregatedProcess_doc.i.in
                      CompositeProcess.i CompositeProcess_doc.i.in
                      FunctionalBasisProcess.i FunctionalBasisProcess_doc.i.in
                      ARMAFactoryImplementation.i ARMAFactoryImplementation_doc.i.in
                      WhittleFactory.i WhittleFactory_doc.i.in
                      WhittleFactoryState.i WhittleFactoryState_doc.i.in
                      ARMALikelihoodFactory.i  ARMALikelihoodFactory_doc.i.in  
                      ARMAFactory.i ARMAFactory_doc.i.in
		      ConditionedGaussianProcess.i ConditionedGaussianProcess_doc.i.in
		      KarhunenLoeveQuadratureAlgorithm.i KarhunenLoeveQuadratureAlgorithm_doc.i.in
		      KarhunenLoeveSVDAlgorithm.i KarhunenLoeveSVDAlgorithm_doc.i.in
                      PythonEvaluation.cxx
                      PythonGradient.cxx
                      PythonHessian.cxx
                      UncertaintyProcessTemplateDefs.i
                   )

ot_add_python_module(dist_bundle1 dist_bundle1_module.i
                      DistFunc.i DistFunc_doc.i.in
                      Arcsine.i Arcsine_doc.i.in
                      ArcsineFactory.i ArcsineFactory_doc.i.in
                      ArcsineMuSigma.i ArcsineMuSigma_doc.i.in
                      BayesDistribution.i BayesDistribution_doc.i.in
                      Bernoulli.i Bernoulli_doc.i.in
                      BernoulliFactory.i BernoulliFactory_doc.i.in
                      EmpiricalBernsteinCopula.i EmpiricalBernsteinCopula_doc.i.in
                      BernsteinCopulaFactory.i BernsteinCopulaFactory_doc.i.in
                      Beta.i Beta_doc.i.in
                      BetaFactory.i BetaFactory_doc.i.in
                      BetaMuSigma.i BetaMuSigma_doc.i.in
                      Binomial.i Binomial_doc.i.in
                      BinomialFactory.i BinomialFactory_doc.i.in
                      Burr.i Burr_doc.i.in
                      BurrFactory.i BurrFactory_doc.i.in
                      ChiSquare.i ChiSquare_doc.i.in
                      ChiSquareFactory.i ChiSquareFactory_doc.i.in
                      Chi.i Chi_doc.i.in
                      ChiFactory.i ChiFactory_doc.i.in
                      CompositeDistribution.i CompositeDistribution_doc.i.in
                      Dirac.i Dirac_doc.i.in
                      DiracFactory.i DiracFactory_doc.i.in
                      Dirichlet.i Dirichlet_doc.i.in
                      DirichletFactory.i DirichletFactory_doc.i.in
                      Epanechnikov.i Epanechnikov_doc.i.in
                      Exponential.i Exponential_doc.i.in
                      ExponentialFactory.i ExponentialFactory_doc.i.in
                      ExtremeValueCopula.i ExtremeValueCopula_doc.i.in
                      FisherSnedecor.i FisherSnedecor_doc.i.in
                      FisherSnedecorFactory.i FisherSnedecorFactory_doc.i.in
                      Frechet.i Frechet_doc.i.in
                      FrechetFactory.i FrechetFactory_doc.i.in
                      Gamma.i Gamma_doc.i.in
                      GammaFactory.i GammaFactory_doc.i.in
                      GammaMuSigma.i GammaMuSigma_doc.i.in
                      GeneralizedExtremeValue.i GeneralizedExtremeValue_doc.i.in
                      GeneralizedPareto.i GeneralizedPareto_doc.i.in
                      GeneralizedParetoFactory.i GeneralizedParetoFactory_doc.i.in
                      Geometric.i Geometric_doc.i.in
                      GeometricFactory.i GeometricFactory_doc.i.in
                      Gumbel.i Gumbel_doc.i.in
                      GumbelFactory.i GumbelFactory_doc.i.in
                      GumbelAB.i GumbelAB_doc.i.in
                      GumbelMuSigma.i GumbelMuSigma_doc.i.in
                   )

ot_add_python_module(dist_bundle2 dist_bundle2_module.i
                      Histogram.i Histogram_doc.i.in
                      HistogramFactory.i HistogramFactory_doc.i.in
                      InverseChiSquare.i InverseChiSquare_doc.i.in
                      InverseGamma.i InverseGamma_doc.i.in
                      InverseNormal.i InverseNormal_doc.i.in
                      InverseNormalFactory.i InverseNormalFactory_doc.i.in
                      InverseWishart.i InverseWishart_doc.i.in
                      KPermutationsDistribution.i KPermutationsDistribution_doc.i.in
                      KernelMixture.i KernelMixture_doc.i.in
                      KernelSmoothing.i KernelSmoothing_doc.i.in
                      Laplace.i Laplace_doc.i.in
                      LaplaceFactory.i LaplaceFactory_doc.i.in
                      Logistic.i Logistic_doc.i.in
                      LogisticFactory.i LogisticFactory_doc.i.in
                      LogNormal.i LogNormal_doc.i.in
                      LogNormalFactory.i LogNormalFactory_doc.i.in
                      LogNormalMuSigma.i LogNormalMuSigma_doc.i.in
                      LogNormalMuSigmaOverMu.i LogNormalMuSigmaOverMu_doc.i.in
                      LogUniform.i LogUniform_doc.i.in
                      LogUniformFactory.i LogUniformFactory_doc.i.in
		      MarginalDistribution.i
		      MaximumDistribution.i MaximumDistribution_doc.i.in
                      MaximumEntropyOrderStatisticsDistribution.i MaximumEntropyOrderStatisticsDistribution_doc.i.in
                      MaximumEntropyOrderStatisticsCopula.i MaximumEntropyOrderStatisticsCopula_doc.i.in
                      MaximumLikelihoodFactory.i MaximumLikelihoodFactory_doc.i.in
                      MeixnerDistribution.i MeixnerDistribution_doc.i.in
                      MeixnerDistributionFactory.i MeixnerDistributionFactory_doc.i.in
                      MethodOfMomentsFactory.i MethodOfMomentsFactory_doc.i.in
                      Mixture.i  Mixture_doc.i.in
                      ConditionalDistribution.i ConditionalDistribution_doc.i.in
                      PosteriorDistribution.i PosteriorDistribution_doc.i.in
                      Multinomial.i Multinomial_doc.i.in
                      MultinomialFactory.i MultinomialFactory_doc.i.in
                      NegativeBinomial.i NegativeBinomial_doc.i.in
                      NegativeBinomialFactory.i NegativeBinomialFactory_doc.i.in
                      NonCentralChiSquare.i NonCentralChiSquare_doc.i.in
                      NonCentralStudent.i NonCentralStudent_doc.i.in
                      Normal.i Normal_doc.i.in
                      NormalFactory.i NormalFactory_doc.i.in
                      NormalGamma.i NormalGamma_doc.i.in
                   )

ot_add_python_module(dist_bundle3 dist_bundle3_module.i
                      OrderStatisticsMarginalChecker.i OrderStatisticsMarginalChecker_doc.i.in
                      ParametrizedDistribution.i ParametrizedDistribution_doc.i.in
                      Poisson.i Poisson_doc.i.in
                      PoissonFactory.i PoissonFactory_doc.i.in
                      ProductDistribution.i ProductDistribution_doc.i.in
                      RandomMixture.i RandomMixture_doc.i.in
                      Rayleigh.i Rayleigh_doc.i.in
                      RayleighFactory.i RayleighFactory_doc.i.in
                      Rice.i Rice_doc.i.in
                      RiceFactory.i RiceFactory_doc.i.in
                      Skellam.i Skellam_doc.i.in
                      SkellamFactory.i SkellamFactory_doc.i.in
                      SmoothedUniform.i SmoothedUniform_doc.i.in
                      Student.i Student_doc.i.in
                      StudentFactory.i StudentFactory_doc.i.in
                      Trapezoidal.i Trapezoidal_doc.i.in
                      TrapezoidalFactory.i TrapezoidalFactory_doc.i.in
                      Triangular.i Triangular_doc.i.in
                      TriangularFactory.i TriangularFactory_doc.i.in
                      TruncatedDistribution.i TruncatedDistribution_doc.i.in
                      TruncatedNormal.i TruncatedNormal_doc.i.in
                      TruncatedNormalFactory.i TruncatedNormalFactory_doc.i.in
                      Uniform.i Uniform_doc.i.in
                      UniformFactory.i UniformFactory_doc.i.in
                      UserDefined.i UserDefined_doc.i.in
                      UserDefinedFactory.i UserDefinedFactory_doc.i.in
                      VonMises.i VonMises_doc.i.in
                      Weibull.i Weibull_doc.i.in
                      WeibullFactory.i WeibullFactory_doc.i.in
                      WeibullMuSigma.i WeibullMuSigma_doc.i.in
                      Wishart.i Wishart_doc.i.in
                      ZipfMandelbrot.i ZipfMandelbrot_doc.i.in
                      UncertaintyDistTemplateDefs.i
                   )

ot_add_python_module(bayesian bayesian_module.i
                      CalibrationStrategyImplementation.i CalibrationStrategyImplementation_doc.i.in
                      CalibrationStrategy.i CalibrationStrategy_doc.i.in
                      MCMC.i MCMC_doc.i.in
                      RandomWalkMetropolisHastings.i RandomWalkMetropolisHastings_doc.i.in
                      ConditionalRandomVector.i ConditionalRandomVector_doc.i.in
                      UncertaintyBayesianTemplateDefs.i
                    )

ot_add_python_module(classification classification_module.i
                      MixtureClassifier.i MixtureClassifier_doc.i.in
                      UncertaintyClassificationTemplateDefs.i
                   )

ot_add_python_module(weightedexperiment weightedexperiment_module.i
                      WeightedExperiment.i WeightedExperiment_doc.i.in
                      WeightedExperimentImplementation.i WeightedExperimentImplementation_doc.i.in
                      BootstrapExperiment.i BootstrapExperiment_doc.i.in
                      FixedExperiment.i FixedExperiment_doc.i.in
                      ImportanceSamplingExperiment.i ImportanceSamplingExperiment_doc.i.in
                      GaussProductExperiment.i GaussProductExperiment_doc.i.in
                      LHSExperiment.i LHSExperiment_doc.i.in
SimulatedAnnealingLHS.i SimulatedAnnealingLHS_doc.i.in
MonteCarloLHS.i MonteCarloLHS_doc.i.in
LHSResult.i LHSResult_doc.i.in
OptimalLHSExperiment.i OptimalLHSExperiment_doc.i.in
SpaceFillingPhiP.i SpaceFillingPhiP_doc.i.in
SpaceFillingMinDist.i SpaceFillingMinDist_doc.i.in
SpaceFillingC2.i SpaceFillingC2_doc.i.in
SpaceFillingImplementation.i SpaceFillingImplementation_doc.i.in
SpaceFilling.i SpaceFilling_doc.i.in
LinearProfile.i LinearProfile_doc.i.in
GeometricProfile.i GeometricProfile_doc.i.in
TemperatureProfile.i TemperatureProfile_doc.i.in
TemperatureProfileImplementation.i TemperatureProfileImplementation_doc.i.in
                      LowDiscrepancyExperiment.i LowDiscrepancyExperiment_doc.i.in
                      MonteCarloExperiment.i MonteCarloExperiment_doc.i.in
                      UncertaintyWeightedExperimentTemplateDefs.i
                   )

ot_add_python_module(orthogonalbasis orthogonalbasis_module.i
                      OrthogonalUniVariatePolynomial.i OrthogonalUniVariatePolynomial_doc.i.in
                      OrthogonalUniVariatePolynomialFactory.i OrthogonalUniVariatePolynomialFactory_doc.i.in
                      CharlierFactory.i CharlierFactory_doc.i.in
                      ChebychevFactory.i ChebychevFactory_doc.i.in
                      HermiteFactory.i HermiteFactory_doc.i.in
                      HistogramPolynomialFactory.i HistogramPolynomialFactory_doc.i.in
                      JacobiFactory.i JacobiFactory_doc.i.in
                      KrawtchoukFactory.i KrawtchoukFactory_doc.i.in
                      LaguerreFactory.i LaguerreFactory_doc.i.in
                      LegendreFactory.i LegendreFactory_doc.i.in
                      MeixnerFactory.i MeixnerFactory_doc.i.in
                      SoizeGhanemFactory.i SoizeGhanemFactory_doc.i.in
                      OrthogonalUniVariatePolynomialFamily.i OrthogonalUniVariatePolynomialFamily_doc.i.in
                      OrthonormalizationAlgorithmImplementation.i OrthonormalizationAlgorithmImplementation_doc.i.in 
                      OrthonormalizationAlgorithm.i OrthonormalizationAlgorithm_doc.i.in
                      AdaptiveStieltjesAlgorithm.i AdaptiveStieltjesAlgorithm_doc.i.in
                      StandardDistributionPolynomialFactory.i StandardDistributionPolynomialFactory_doc.i.in
                      OrthogonalFunctionFactory.i OrthogonalFunctionFactory_doc.i.in
                      OrthogonalProductPolynomialFactory.i OrthogonalProductPolynomialFactory_doc.i.in
                      OrthogonalBasis.i OrthogonalBasis_doc.i.in
                      UniVariateFunctionImplementation.i UniVariateFunctionImplementation_doc.i.in
                      UniVariateFunction.i UniVariateFunction_doc.i.in
                      MonomialFunction.i MonomialFunction_doc.i.in
                      OrthogonalUniVariateFunctionFactory.i OrthogonalUniVariateFunctionFactory_doc.i.in
                      OrthogonalUniVariateFunctionFamily.i OrthogonalUniVariateFunctionFamily_doc.i.in
                      OrthogonalUniVariatePolynomialFunctionFactory.i OrthogonalUniVariatePolynomialFunctionFactory_doc.i.in
                      FourierSeriesFactory.i FourierSeriesFactory_doc.i.in
                      HaarWaveletFactory.i HaarWaveletFactory_doc.i.in
                      OrthogonalProductFunctionFactory.i OrthogonalProductFunctionFactory_doc.i.in
                      CanonicalTensorEvaluation.i CanonicalTensorEvaluation_doc.i.in
                      CanonicalTensorGradient.i CanonicalTensorGradient_doc.i.in
                      UncertaintyOrthogonalBasisTemplateDefs.i
                   )

ot_add_python_module(metamodel metamodel_module.i
                      MetaModelResult.i MetaModelResult_doc.i.in
                      MetaModelAlgorithm.i MetaModelAlgorithm_doc.i.in
                      KrigingResult.i KrigingResult_doc.i.in
                      MetaModelValidation.i MetaModelValidation_doc.i.in
		      GeneralLinearModelResult.i GeneralLinearModelResult_doc.i.in
		      GeneralLinearModelAlgorithm.i GeneralLinearModelAlgorithm_doc.i.in
                      LinearTaylor.i LinearTaylor_doc.i.in
                      QuadraticTaylor.i QuadraticTaylor_doc.i.in
                      LinearLeastSquares.i LinearLeastSquares_doc.i.in
                      QuadraticLeastSquares.i QuadraticLeastSquares_doc.i.in
                      AdaptiveStrategyImplementation.i AdaptiveStrategyImplementation_doc.i.in
                      FixedStrategy.i FixedStrategy_doc.i.in
                      SequentialStrategy.i SequentialStrategy_doc.i.in
                      CleaningStrategy.i CleaningStrategy_doc.i.in
                      AdaptiveStrategy.i AdaptiveStrategy_doc.i.in
                      ProjectionStrategyImplementation.i ProjectionStrategyImplementation_doc.i.in
                      LeastSquaresStrategy.i LeastSquaresStrategy_doc.i.in
                      IntegrationStrategy.i IntegrationStrategy_doc.i.in
                      ProjectionStrategy.i ProjectionStrategy_doc.i.in
                      FunctionalChaosResult.i FunctionalChaosResult_doc.i.in
                      FunctionalChaosAlgorithm.i FunctionalChaosAlgorithm_doc.i.in
                      KrigingAlgorithm.i KrigingAlgorithm_doc.i.in
                      RandomVector.i RandomVector_doc.i.in
                      PythonEvaluation.cxx
                      PythonGradient.cxx
                      PythonHessian.cxx
                      PythonRandomVectorImplementation.cxx
                      CompositeRandomVector.i
                      EventRandomVector.i
                      EventDomain.i
                      EventProcess.i
                      Event.i Event_doc.i.in
                      StandardEvent.i StandardEvent_doc.i.in
                      FunctionalChaosRandomVector.i FunctionalChaosRandomVector_doc.i.in
                      FunctionalChaosSobolIndices.i FunctionalChaosSobolIndices_doc.i.in
                      KrigingRandomVector.i KrigingRandomVector_doc.i.in
                      TensorApproximationResult.i TensorApproximationResult_doc.i.in
                      TensorApproximationAlgorithm.i TensorApproximationAlgorithm_doc.i.in
                      UncertaintyMetaModelTemplateDefs.i
                   )

ot_add_python_module(transformation transformation_module.i
                      MarginalTransformationEvaluation.i MarginalTransformationEvaluation_doc.i.in
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
                      BoxCoxFactory.i BoxCoxFactory_doc.i.in
                      TrendFactory.i TrendFactory_doc.i.in
                      DistributionTransformation.i DistributionTransformation_doc.i.in
                      UncertaintyTransformationTemplateDefs.i
                   )

ot_add_python_module(analytical analytical_module.i
                      AnalyticalResult.i AnalyticalResult_doc.i.in
                      Analytical.i Analytical_doc.i.in
                      FORMResult.i FORMResult_doc.i.in
                      FORM.i FORM_doc.i.in
                      SORMResult.i SORMResult_doc.i.in
                      SORM.i SORM_doc.i.in
                      StrongMaximumTest.i StrongMaximumTest_doc.i.in
                      UncertaintyAnalyticalTemplateDefs.i
                   )

ot_add_python_module(simulation simulation_module.i
                      SobolIndicesAlgorithmImplementation.i SobolIndicesAlgorithmImplementation_doc.i.in
                      SobolIndicesAlgorithm.i SobolIndicesAlgorithm_doc.i.in
                      MartinezSensitivityAlgorithm.i MartinezSensitivityAlgorithm_doc.i.in
                      SaltelliSensitivityAlgorithm.i SaltelliSensitivityAlgorithm_doc.i.in
                      JansenSensitivityAlgorithm.i JansenSensitivityAlgorithm_doc.i.in
                      MauntzKucherenkoSensitivityAlgorithm.i MauntzKucherenkoSensitivityAlgorithm_doc.i.in
                      SobolIndicesExperiment.i SobolIndicesExperiment_doc.i.in
                      SimulationSensitivityAnalysis.i SimulationSensitivityAnalysis_doc.i.in
                      ProbabilitySimulationResult.i ProbabilitySimulationResult_doc.i.in
                      SimulationResult.i SimulationResult_doc.i.in
                      SimulationAlgorithm.i SimulationAlgorithm_doc.i.in
                      EventSimulation.i EventSimulation_doc.i.in
                      ProbabilitySimulationAlgorithm.i ProbabilitySimulationAlgorithm_doc.i.in
                      PostAnalyticalSimulation.i PostAnalyticalSimulation_doc.i.in
                      Wilks.i Wilks_doc.i.in
                      LHS.i LHS_doc.i.in
                      PostAnalyticalControlledImportanceSampling.i PostAnalyticalControlledImportanceSampling_doc.i.in
                      PostAnalyticalImportanceSampling.i PostAnalyticalImportanceSampling_doc.i.in
                      RootStrategyImplementation.i RootStrategyImplementation_doc.i.in
                      RootStrategy.i RootStrategy_doc.i.in
                      SamplingStrategyImplementation.i SamplingStrategyImplementation_doc.i.in
                      SamplingStrategy.i SamplingStrategy_doc.i.in
                      DirectionalSampling.i DirectionalSampling_doc.i.in
                      OrthogonalDirection.i OrthogonalDirection_doc.i.in
                      RandomDirection.i RandomDirection_doc.i.in
                      MediumSafe.i MediumSafe_doc.i.in
                      RiskyAndFast.i RiskyAndFast_doc.i.in
                      SafeAndSlow.i SafeAndSlow_doc.i.in
                      SubsetSamplingResult.i SubsetSamplingResult_doc.i.in
                      SubsetSampling.i SubsetSampling_doc.i.in
                      AdaptiveDirectionalSampling.i AdaptiveDirectionalSampling_doc.i.in
                      ExpectationSimulationAlgorithm.i ExpectationSimulationAlgorithm_doc.i.in
                      ExpectationSimulationResult.i ExpectationSimulationResult_doc.i.in
                      SobolSimulationAlgorithm.i SobolSimulationAlgorithm_doc.i.in
                      SobolSimulationResult.i SobolSimulationResult_doc.i.in
                      UncertaintySimulationTemplateDefs.i
                    )

ot_add_python_module(stattests stattests_module.i
                      VisualTest.i VisualTest_doc.i.in
                      FittingTest.i FittingTest_doc.i.in
                      HypothesisTest.i HypothesisTest_doc.i.in
                      LinearModelTest.i LinearModelTest_doc.i.in
                      NormalityTest.i NormalityTest_doc.i.in
                      DickeyFullerTest.i DickeyFullerTest_doc.i.in
                      UncertaintyStatTestsTemplateDefs.i
                    )

ot_add_python_module(uncertainty uncertainty_module.i
                      ANCOVA.i ANCOVA_doc.i.in
                      FAST.i FAST_doc.i.in
                      EfficientGlobalOptimization.i EfficientGlobalOptimization_doc.i.in
                      TaylorExpansionMoments.i TaylorExpansionMoments_doc.i.in
                      UncertaintyTemplateDefs.i
                    )

ot_add_python_module(testing testing_module.i
                    )

set (OPENTURNS_PYTHON_MODULES ${OPENTURNS_PYTHON_MODULES} PARENT_SCOPE) # for the docstring test


# Build and install openturns.memoryview
add_library (memoryview MODULE PythonReadOnlyBuffer.c)
target_link_libraries_with_dynamic_lookup (memoryview ${PYTHON_LIBRARIES})
set_target_properties(memoryview PROPERTIES NO_SONAME ON PREFIX "")
if (WIN32 AND NOT CYGWIN)
  set_target_properties(memoryview PROPERTIES SUFFIX ".pyd")
endif ()
install (TARGETS memoryview LIBRARY DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns)


set (EXTRA_PYTHON_FILES __init__.py dist.py viewer.py coupling_tools.py)
install (FILES ${EXTRA_PYTHON_FILES}
          DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns
       )

install (FILES ${SWIGFILES} DESTINATION ${OPENTURNS_SWIG_INCLUDE_DIRS})

if (SPHINX_FOUND)

  # create a build site-package from which sphinx can import
  set (OPENTURNS_PYTHON_BUILD_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/site-packages)
  execute_process (COMMAND ${CMAKE_COMMAND} -E make_directory ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns)
  foreach (module ${OPENTURNS_PYTHON_MODULES})
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/${module}.py ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/${module}.py)
    # FIXME: get_target_property (suffix ${SWIG_MODULE_${module}_REAL_NAME} SUFFIX)
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/_${module}.so ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/_${module}.so)
  endforeach ()
  # install memoryview.so for sphinx
  execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/memoryview.so ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/memoryview.so)
  foreach (file ${EXTRA_PYTHON_FILES})
    execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${OPENTURNS_PYTHON_BUILD_MODULE_PATH}/openturns/${file})
  endforeach ()

  # sphinx configuration
  set (doc_formats)
  list (APPEND doc_formats html)
  list (APPEND doc_formats htmlhelp)
  list (APPEND doc_formats latex)
  list (APPEND doc_formats doctest)
  list (APPEND doc_formats qthelp)
  list (APPEND doc_formats devhelp)

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
    ${SPHINX_SOURCE_DIR}/*.ipynb
  )

  # copy sphinx sources
  set (sphinx_clones)
  foreach (file ${sphinx_sources})
    if (EXISTS ${file})
      file (RELATIVE_PATH rel_file ${SPHINX_SOURCE_DIR} ${file})
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

  set (SPHINX_FLAGS "-W" CACHE STRING "sphinx flags")
  if (NOT NBSPHINX_FOUND)
    # without notebook examples there are invalid links
    string (REPLACE "-W" "" SPHINX_FLAGS "${SPHINX_FLAGS}")
  endif ()
  string (REPLACE " " ";" SPHINX_FLAGS "${SPHINX_FLAGS}")

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
      if (CMAKE_VERSION VERSION_LESS 3.13)
        set (module_target ${SWIG_MODULE_${MODULENAME}_REAL_NAME})
      else ()
        set (module_target ${MODULENAME})
      endif ()
      add_dependencies (sphinx_${format} ${module_target})
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
configure_file (WHEEL.in WHEEL @ONLY)
install (DIRECTORY DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns-${PACKAGE_VERSION}.dist-info)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/METADATA ${CMAKE_CURRENT_BINARY_DIR}/WHEEL
	 DESTINATION ${OPENTURNS_PYTHON_MODULE_PATH}/openturns-${PACKAGE_VERSION}.dist-info)
