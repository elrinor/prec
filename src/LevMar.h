#ifndef __LEVMAR_H__
#define __LEVMAR_H__

#include "config.h"
#include <limits>
#include <iostream>
#include <arx/LinearAlgebra.h>

namespace prec {

  template<class LevMarModel>
  class LevMar {
  public:
    typedef typename LevMarModel::value_type value_type;
    typedef typename LevMarModel::param_vector_type param_vector_type; 
    typedef typename LevMarModel::resid_vector_type resid_vector_type;
    typedef typename LevMarModel::jacob_matrix_type jacob_matrix_type;
    typedef typename LevMarModel::hessn_matrix_type hessn_matrix_type;

    /**
     * Fits the given model using Levenberg-Marquardt nonlinear minimization method.
     *
     * @param model                    Model to fit.
     * @param p                        (in, out) Initial parameters approximation, final result.
     */
    void fit(const LevMarModel& model, param_vector_type& p) {
      value_type gradientMagnitudeThresholdSqr = 0.0000001f;
      value_type stepMagnitudeThresholdSqr = 0.0000001f;
      value_type errorThresholdSqr = 0.0000001f;
      unsigned int maxIterations = 100;

      unsigned int iterationN = 0;
      size_t paramN = model.getParamNumber();
      size_t residN = model.getResidualNumber();

      value_type dampingTerm = 1;

      value_type error;

      /* Allocate memory for everything. */
      jacob_matrix_type j(residN, paramN);                 /* Jacobian. */
      resid_vector_type x(residN);                         /* Residuals. */
      param_vector_type grad(paramN);                      /* Gradient. */
      param_vector_type step(paramN);                      /* Step. */
      param_vector_type newP(paramN);                      /* New parameters. */
      hessn_matrix_type a(paramN, paramN);                 /* Matrix for linear solver. */

      /* Initialize. */
      model.nextIteration(p, j, x);
      error = model.calculateResidualError(p); /* This double residual calculation doesn't add too much overhead and simplifies the LevMarModel interface. */

      /* Iterate. */
      while(true) {
        /* Calculate gradient. */
        grad = j.transpose() * -x;
        
        /* Drop out in case gradient magnitude hits threshold. */
        if(grad.normSqr() < gradientMagnitudeThresholdSqr)
          break;

        /* Inner loop - adjust dampingTerm and update current parameters approximation. */
        while(true) {
          iterationN++;

          a = j.transpose() * j + hessn_matrix_type::identity(paramN) * dampingTerm;
          step = grad;
          solveLinearSystem(a, step); /* this modifies step! */

          value_type newError = model.calculateResidualError(newP = p + step);

          if(newError < error) {
            error = newError;
            p = newP;
            dampingTerm /= 10;
            break;
          } else
            dampingTerm *= 10;

          if(iterationN > maxIterations)
            break;
        }

        if(iterationN > maxIterations)
          break;

        if(step.normSqr() < stepMagnitudeThresholdSqr)
          break;

        if(error < errorThresholdSqr)
          break;

        model.nextIteration(p, j, x);
      }
    }

  };

  template<class T> 
  class LevMarModel {
  public:
    typedef T value_type;

    /** Type for parameters vector. */
    typedef arx::DynamicVector<value_type> param_vector_type; 

    /** Type for residuals vector. */
    typedef arx::DynamicVector<value_type> resid_vector_type;

    /** Type for jacobian matrix. */
    typedef arx::DynamicMatrix<value_type> jacob_matrix_type;

    /** Type for hessian matrix, \f$H = J^T * J\f$ **/
    typedef arx::DynamicMatrix<value_type> hessn_matrix_type;

    /**
     * @return                         Number of parameters in the model.
     */
    std::size_t getParamNumber() const;

    /**
     * @return                         Number of residuals in the model.
     */
    std::size_t getResidualNumber() const;

    /**
     * This function calculates residual error at p. It may be called several times 
     * during single iteration.
     *
     * @param p                        Parameters vector.
     * @return                         Residual error.
     */
    value_type calculateResidualError(const param_vector_type& p) const;

    /**
     * This function is called by LevMar once at the beginning of each iteration. 
     * It calculates Jacobian and residuals vector at p. 
     *
     * @param p                        Parameters vector.
     * @param j                        (out) Jacobian at p.
     * @param r                        (out) Residuals vector at p.
     */
    void nextIteration(const param_vector_type& p, jacob_matrix_type& j, resid_vector_type& r) const;
  };



} // namespace prec

#endif // __LEVMAR_H__
