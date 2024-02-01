//                                               -*- C++ -*-
/**
 *  @brief IpoptProblem implements the Ipopt::TNLP interface
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_IPOPTPROBLEM_HXX
#define OPENTURNS_IPOPTPROBLEM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"

#define HAVE_CSTDDEF // for 3.11 in debian
#include <IpTNLP.hpp>

#include <chrono>

BEGIN_NAMESPACE_OPENTURNS

typedef ::Ipopt::TNLP::LinearityType * LinearityTypeTable;

class IpoptProblem
  : public ::Ipopt::TNLP
{

public:

  /** Constructor with parameters */
  IpoptProblem( const OptimizationProblem & optimProblem,
                const Point & startingPoint,
                const UnsignedInteger maximumEvaluationNumber,
                const Scalar maximumTimeDuration,
                const std::chrono::steady_clock::time_point & t0);

  /** Retrieving objective function input.output history */
  Sample getInputHistory() const;
  Sample getOutputHistory() const;

  // n: number of variables
  // m: number of constraints

  bool get_nlp_info(int & n,
                    int & m,
                    int & nnz_jac_g, // Number of non-zero components in the Jacobian of g
                    int & nnz_h_lag, // Number of non-zero components in Hessian of Lagrangean
                    ::Ipopt::TNLP::IndexStyleEnum & index_style);

  bool get_variables_linearity( int n,
                                LinearityTypeTable var_types);

  bool get_constraints_linearity( int m,
                                  LinearityTypeTable const_types);

  bool get_bounds_info( int n,
                        double* x_l,  // Lower bounds
                        double* x_u,  // Upper bounds
                        int m,
                        double* g_l,  // Lower bounds
                        double* g_u); // Upper bounds

  bool get_starting_point(int n,
                          bool init_x,
                          double* x,
                          bool init_z,
                          double* z_L,
                          double* z_U,
                          int m,
                          bool init_lambda,
                          double* lambda);

  bool eval_f(int n,
              const double* x,
              bool new_x,
              double& obj_value);

  bool eval_grad_f( int n,
                    const double* x,
                    bool new_x,
                    double* grad_f);

  bool eval_g(int n,
              const double* x,
              bool new_x,
              int m,
              double* g);

  bool eval_jac_g(int n,
                  const double* x,
                  bool new_x,
                  int m,
                  int nele_jac,
                  int* iRow,
                  int *jCol,
                  double* values);

  bool eval_h(int n,
              const double* x,
              bool new_x,
              double obj_factor,
              int m,
              const double* lambda,
              bool new_lambda,
              int nele_hess,
              int* iRow,
              int* jCol,
              double* values);

  bool eval_gi(int n,
               const double* x,
               bool new_x,
               int i,
               double& gi);

  bool eval_grad_gi(int n,
                    const double* x,
                    bool new_x,
                    int i,
                    int& nele_grad_gi,
                    int* jCol,
                    double* values);

  virtual void finalize_solution(::Ipopt::SolverReturn status, ::Ipopt::Index n,
                                 const ::Ipopt::Number* x, const ::Ipopt::Number* z_L,
                                 const ::Ipopt::Number* z_U, ::Ipopt::Index m, const ::Ipopt::Number* g,
                                 const ::Ipopt::Number* lambda, ::Ipopt::Number obj_value,
                                 const ::Ipopt::IpoptData* ip_data,
                                 ::Ipopt::IpoptCalculatedQuantities* ip_cq);

  Point getOptimalPoint() const
  {
    return optimalPoint_;
  }

  Point getOptimalValue() const
  {
    return optimalValue_;
  }

  virtual void setProgressCallback(OptimizationAlgorithmImplementation::ProgressCallback callBack, void * state = 0)
  {
    progressCallback_ = std::pair<OptimizationAlgorithmImplementation::ProgressCallback, void *>(callBack, state);
  }

  virtual void setStopCallback(OptimizationAlgorithmImplementation::StopCallback callBack, void * state = 0)
  {
    stopCallback_ = std::pair<OptimizationAlgorithmImplementation::StopCallback, void *>(callBack, state);
  }

private:
  const OptimizationProblem optimProblem_;
  const Point startingPoint_;
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;
  Point optimalPoint_;
  Point optimalValue_;
  // Callbacks
  UnsignedInteger maximumEvaluationNumber_ = 0;
  Scalar maximumTimeDuration_ = -1.0;
  std::chrono::steady_clock::time_point t0_;
  std::pair< OptimizationAlgorithmImplementation::ProgressCallback, void *> progressCallback_;
  std::pair< OptimizationAlgorithmImplementation::StopCallback, void *> stopCallback_;
};

END_NAMESPACE_OPENTURNS

#endif
