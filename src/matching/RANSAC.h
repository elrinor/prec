#ifndef __RANSAC_H__
#define __RANSAC_H__

#include <exception>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <arx/Collections.h>

namespace prec {
  /**
   * RANSAC class implements RANdom SAmple Consensus algorithm for parameter estimation of a 
   * mathematical model from a set of observed data points which contains outliers
   * (see article on wikipedia for details on RANSAC algorithm, http://en.wikipedia.org/wiki/RANSAC).
   *
   * In our RANSAC implementation the following cost function is used:
   *
   * \f[
   * \C_2 = \sum_i{\sigma_2(e_i)}
   * \f]
   *
   * where
   *
   * \f[
   * \sigma_2 = \begin{cases}
   *   e,      &\text{if $e < T$;}\\
   *   T,      &\text{if $e >= T$.}
   * \end{cases}
   * \f]
   *
   * Here \f$e\f$ is the fitting error and \f$T\f$ is the fitting threshold. Note that this cost
   * function differs from the standard one, which uses the the following \f$\sigma\f$:
   *
   * \f[
   * \sigma = \begin{cases}
   *   0,      &\text{if $e < T$;}\\
   *   T,      &\text{if $e >= T$.}
   * \end{cases}
   * \f]
   *
   * That means that the method implemented is not really RANSAC, but MSAC (google for <i> MLESAC: 
   * A New Robust Estimator with Application to Estimating Image Geometry </i> for details).
   *
   * 
   * @param Model                      Class representing a mathematical model for observed data.
   * @param Point                      Single point.
   *
   * @see GenericRANSACModel
   */
  template<class Model, class Point = typename Model::point_type>
  class RANSAC {
  private:
    unsigned int minPointsToAcceptModel; /**< Smallest number of points required for a model to be accepted. */
    unsigned int minPointsToFitModel; /**< Smallest number of points to be able to fit the model. */

  public:
    /**
     * Constructor.
     *
     * @param minPointsToFitModel      Smallest number of points to be able to fit the model.
     * @param minPointsToAcceptModel   Smallest number of points required for a model to be accepted.
     */
    RANSAC(unsigned int minPointsToFitModel, unsigned int minPointsToAcceptModel): 
      minPointsToFitModel(minPointsToFitModel), minPointsToAcceptModel(minPointsToAcceptModel) {
        assert(minPointsToFitModel > 1);
      }

    /**
     * Finds the best model fitting the given data.
     * 
     * @param bestModel                (out) The best model found.
     * @param points                   List of observed data points.
     * @param inlierFraction           Fraction of the sample points which are good.
     * @param targetProbability        Required probability to find a good sample.
     * @param maxFitError              Threshold value for determining when a point fits a model. 
     * @return                         true if the best model was found, false otherwise.
     */
    template<class ArrayOfPoint>
    bool fit(Model& bestModel, const ArrayOfPoint& points, float inlierFraction, float targetProbability, float maxFitError) const {
      /* Check data set size. */
      if(points.size() < minPointsToFitModel)
        throw std::runtime_error("List of data is smaller than minimum fit requires.");

      /* Initialize random number generator. */
      time_t seed;
      srand(static_cast<unsigned int>(time(&seed)));

      /* Estimate the number of iterations required. */
      unsigned int requiredIterations = estimateNumberOfIterations(targetProbability, inlierFraction, minPointsToFitModel, 1.0f); /* TODO: why 1.0? */

      /* Prepare. */
      float bestModelCost = std::numeric_limits<float>::max(); /* Cost of bestModel in terms of cost function. */

      /* Iterate. */
      for(unsigned int i = 0; i < requiredIterations; i++) {
        arx::ArrayList<Point> maybeInliers; /* List of possible inliers. */
        arx::Set<std::size_t> usedPoints; /* Set of point indexes that were randomly picked as possible inliers. */

        /* Build random samples. */
        while(true) {
          std::size_t index = static_cast<std::size_t>(static_cast<unsigned long long>(points.size()) * rand() / (RAND_MAX + 1));
          if(usedPoints.contains(index))
            continue;
          usedPoints.insert(index);
          maybeInliers.push_back(points[index]);
          if(maybeInliers.size() == minPointsToFitModel)
            break;
        }

        /* Fit model. */
        Model model;
        if(!model.fit(maybeInliers))
          continue;

        /* Cost of the current model in terms of cost function. */
        float cost = 0.0f;

        /* Clear maybeInliers. Old inliers will get there on the next step. */
        maybeInliers.clear();

        /* Check all points for fit. */
        for(unsigned int i = 0; i < points.size(); i++) {
          float fitError = model.calculateFitError(points[i]);
          if(fitError < maxFitError) {
            maybeInliers.push_back(points[i]);
            cost += fitError;
          } else
            cost += maxFitError;
        }

        /* Test whether we can accept current model. */
        if(maybeInliers.size() < minPointsToAcceptModel)
          continue;

        /* Compare with the best one. */
        if(cost < bestModelCost) {
          bestModel = model;
          bestModelCost = cost;
          bestModel.setInliers(maybeInliers);

          /* Update requiredIterations if needed */
          float currentInlierFraction = (float) maybeInliers.size() / points.size();
          if(currentInlierFraction > inlierFraction) {
            inlierFraction = currentInlierFraction;
            requiredIterations = estimateNumberOfIterations(targetProbability, inlierFraction, minPointsToFitModel, 1.0f); /* TODO: why 1.0? */
          }
        }
      }

      return bestModelCost != std::numeric_limits<float>::max();
    }

    /**
     * Calculate the expected number of iterations required to find a good sample with probability 
     * targetProbability when a fraction of inlierFraction of the sample points are good and at 
     * least minPointsToFitModel points are required to fit a model. Add sdFactor times the 
     * standard deviation to be sure.
     *
     * It seems that the black magic formula used in this function requires some comments, 
     * so here we go. Basically, RANSAC iterations can be modelled as a random
     * process, with a probability of selecting a good subsample on the \f$k\f$'th iteration 
     * given by geometric distribution:
     *
     * \f[
     * f(k) = (1 - \omega^p)^{k - 1} * \omega^p
     * \f]
     *
     * where \f$\omega\f$ is the fraction of inliers and \f$p\f$ is the number of points 
     * required to fit the model. The expression for probability that a good subsample was selected
     * after \f$k\f$ iterations is as follows:
     *
     * \f[
     * F(k) = sum_{i=1}^k P(i) = 1 - (1 - \omega^p)^{k - 1}
     * \f]
     *
     * Note that \f$f(k)\f$ is a probability mass function, and \f$F(k)\f$ is a cumulative distribution 
     * function for geometric distribution. We can easily estimate \f$k\f$ using the formula for \f$F(k)\f$.
     * Standard deviation for geometric distribution is as follows:
     *
     * \f[
     * SD = \frac{\sqrt{1 - \omega^p}}{\omega^p}
     * \f]
     *
     * To gain additional confidence, the standard deviation multiplied by sdFactor can be added to estimated
     * number of iterations.
     *
     * @param targetProbability        Required probability to find a good sample.
     * @param inlierFraction           Fraction of the sample points which are good.
     * @param minPointsToFitModel      Minimal number of points required to fit a model.
     * @param sdFactor                 Factor to multiply the added standard deviation by.
     * @return                         Guess for the expected number of iterations.
     */
    static unsigned int estimateNumberOfIterations(float targetProbability, float inlierFraction, unsigned int minPointsToFitModel, float sdFactor) {
      assert(targetProbability > 0 && targetProbability < 1);
      assert(inlierFraction > 0 && inlierFraction < 1);
      assert(minPointsToFitModel > 1);
      float successProbability = pow(inlierFraction, (int) minPointsToFitModel); /* Probability of success in a single iteration. */
      return (unsigned int) (log(1 - targetProbability) / log(1 - successProbability) + sdFactor * sqrt(1 - successProbability) / successProbability) + 1;
    }
  };


  /** 
   * Class GenericRANSACModel.
   */
  template<class Point> 
  class GenericRANSACModel {
  private:
    template<class Model, class OtherPoint> friend class RANSAC;

  protected:
    arx::ArrayList<Point> inliers;

    void setInliers(const arx::ArrayList<Point>& inliers) { this->inliers = inliers; }

  public:
    typedef Point point_type;    /**< Point type. */

    /**
     * Default constructor.
     */
    GenericRANSACModel() {};

    /**
     * Fits the model to the given set of points.
     *
     * @param points                   Set of point to fit the model to.
     * @return                         true if the fit was done, false otherwise.
     */
    bool fit(const arx::ArrayList<Point>& points);

    /**
     * Calculate the fitting error of a single point against the current model.
     *
     * @param p 
     * @return                         Fitting error.
     */
    float calculateFitError(const Point& p) const;

    /**
     * @return                         Set of inliers determined during RANSAC run.
     */
    const arx::ArrayList<Point>& getInliers() const { return this->inliers; }
  };

} // namespace prec

#endif // __RANSAC_H__
