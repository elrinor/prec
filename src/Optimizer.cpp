#include "config.h"
#include "Optimizer.h"
#include <arx/Collections.h>
#include <arx/LinearAlgebra.h>
#include "LevMar.h"
#include "Homography.h"

using namespace arx;

namespace prec {
  /**
   * Residual represents a single match between two images that is used in 
   * Levenberg-Marquardt for residual calculation.
   */
  struct Residual {
    float x0, y0, x1, y1;
    size_t index0, index1;
    
    Residual(size_t index0, size_t index1, float x0, float y0, float x1, float y1): index0(index0), index1(index1), x0(x0), y0(y0), x1(x1), y1(y1) {}
  };


  /**
   * Bundle adjustment model for Levenberg-Marquardt
   *
   * @see LevMarModel
   */
  class BundleAdjustmentLevMarModel {
  private:
    ArrayList<Residual> residuals;
    size_t paramN;
    size_t residN;

    float getDerivative(const Homography& h0, const Homography& h1, const Matrix3f& h0m, const Matrix3f& h1m1, const Vector2f& ab, const Matrix<float, 2, 3>& dab_dxyz, const float r, const Vector3f& u1, unsigned int derivativeIndex) const {
      assert(derivativeIndex < 8);

      if(r == 0.0f)
        return 0.0f; // TODO: what should I do with that? O_O

      Vector3f dxyz;
      if(derivativeIndex < 4)
        dxyz = h0.getMatrixDerivative(derivativeIndex) * h1m1 * u1;
      else
        dxyz = h0m * h1.getInverseMatrixDerivative(derivativeIndex - 4) * u1;

      Vector2f dab = arx::operator*(dab_dxyz, dxyz); // TODO: what the hell??? what's with adl?
      return (ab[0] * dab[0] + ab[1] * dab[1]) / r;
    }

  public:
    BundleAdjustmentLevMarModel(size_t paramN, ArrayList<Residual> residuals): paramN(paramN), residuals(residuals), residN(residuals.size()) {}

    typedef float value_type;
    typedef VectorXf param_vector_type; 
    typedef VectorXf resid_vector_type;
    typedef MatrixXf jacob_matrix_type;
    typedef MatrixXf hessn_matrix_type;

    size_t getParamNumber() const {
      return this->paramN;
    }

    size_t getResidualNumber() const {
      return this->residN;
    }

    float calculateResidualError(const VectorXf& p) const {
      /* Get homographies array. */
      const Homography* homographies = reinterpret_cast<const Homography*>(p.data());

      /* This one was dangerous. */
      assert(p.size() * sizeof(float) == this->paramN * sizeof(Homography) / 4);

      float result = 0.0f;

      for(unsigned int i = 0; i < getResidualNumber(); i++) {
        Vector2f u0;
        u0[0] = this->residuals[i].x0;
        u0[1] = this->residuals[i].y0;
        Vector3f u1;
        u1[0] = this->residuals[i].x1;
        u1[1] = this->residuals[i].y1;
        u1[2] = 1;

        size_t paramIndex0 = this->residuals[i].index0;
        size_t paramIndex1 = this->residuals[i].index1;
        const Homography h0 = homographies[paramIndex0];
        const Homography h1 = homographies[paramIndex1];
        Matrix3f h0m  = h0.getMatrix();
        Matrix3f h1m1 = h1.getInverseMatrix();

        Vector<float, 3> xyz = h0m * h1m1 * u1;

        Vector<float, 2> xy;
        xy[0] = xyz[0] / xyz[2];
        xy[1] = xyz[1] / xyz[2];

        Vector<float, 2> ab = u0 - xy;
        result += ab.normSqr();
      }

      return result;
    }


    void nextIteration(const VectorXf& p, MatrixXf& j, VectorXf& r) const {
      /* Get homographies array. */
      const Homography* homographies = reinterpret_cast<const Homography*>(p.data());

      /* This one was dangerous. */
      assert(p.size() * sizeof(float) == this->paramN * sizeof(Homography) / 4);

      /* Fill jacobian with zeros. */
      j.fill(0);

      for(unsigned int i = 0; i < r.size(); i++) {
        Vector2f u0;
        u0[0] = this->residuals[i].x0;
        u0[1] = this->residuals[i].y0;
        Vector3f u1;
        u1[0] = this->residuals[i].x1;
        u1[1] = this->residuals[i].y1;
        u1[2] = 1;
        
        size_t paramIndex0 = this->residuals[i].index0;
        size_t paramIndex1 = this->residuals[i].index1;
        const Homography h0 = homographies[paramIndex0];
        const Homography h1 = homographies[paramIndex1];
        Matrix3f h0m  = h0.getMatrix();
        Matrix3f h1m1 = h1.getInverseMatrix();
        
        Vector<float, 3> xyz = h0m * h1m1 * u1;
        
        Vector<float, 2> xy;
        xy[0] = xyz[0] / xyz[2];
        xy[1] = xyz[1] / xyz[2];

        Vector<float, 2> ab = u0 - xy;
        r[i] = ab.norm();

        /* Then we fill jacobian.
         *
         * The basic idea here is simple:
         * Each residual has 8 non-zero derivatives - 4 per each homography involved in its calculation.
         * The formula for residual is as follows: 
         *   r = sqrt(a^2 + b^2), where a and b are functions of model parameters,
         * therefore
         *   r' = (a*a' + b*b') / sqrt(a^2 + b^2). Here derivative is taken by one of the parameters.
         * For (a, b) vector we have:
         *   (a, b) = u0 - (x / z, y / z), 
         *   (x, y, z) = H0 * H1^-1 * u1,
         * therefore we can calculate (a, b)' using chain rule:
         *   d(a, b)/dO = d(a, b)/d(x, y, z) * d(x, y, z)/dO,
         * where
         *   d(a, b)/d(x, y, z) = 
         *     | -1/z    0   x/z^2 |
         *     |   0   -1/z  y/z^2 |
         * So the only problem left is to calculate (x, y, z)' (denoted above as d(x, y, z)/dO).
         * It is pretty simple too, since (x, y, z) = H0 * H1^-1 * u1.
         */
        Matrix<float, 2, 3> dab_dxyz;
        dab_dxyz[0][0] = -1 / xyz[2];
        dab_dxyz[1][0] = 0;
        dab_dxyz[0][1] = 0;
        dab_dxyz[1][1] = -1 / xyz[2];
        dab_dxyz[0][2] = xyz[0] / sqr(xyz[2]);
        dab_dxyz[1][2] = xyz[1] / sqr(xyz[2]);

        for(unsigned int k = 0; k < 4; k++)
          j[i][4 * paramIndex0 + k] = getDerivative(h0, h1, h0m, h1m1, ab, dab_dxyz, r[i], u1, k);

        for(unsigned int k = 0; k < 4; k++)
          j[i][4 * paramIndex1 + k] = getDerivative(h0, h1, h0m, h1m1, ab, dab_dxyz, r[i], u1, k + 4);
      }
    }
  };



  void Optimizer::optimize(Panorama& p) {
    /* Create id -> index map. */
    Map<int, size_t> indexes;
    for(size_t i = 0; i < p.size(); i++)
      indexes[p.getImage(i).getId()] = i;

    /* Create residuals array. */
    ArrayList<Residual> residuals;
    for(size_t i = 0; i < p.getImageMatches().size(); i++) {
      ImageMatch im = p.getImageMatch(i);
      size_t index0 = indexes[im.getPanoImage(0).getId()];
      size_t index1 = indexes[im.getPanoImage(1).getId()];
      for(size_t j = 0; j < im.getMatches().size(); j++) {
        Match m = im.getMatch(j);
        residuals.push_back(Residual(index0, index1, m.getKey(0).getX(), m.getKey(0).getY(), m.getKey(1).getX(), m.getKey(1).getY()));
      }
    }

    /* Prepare initial params. */
    VectorXf params(p.size() * 4);
    for(size_t i = 0, t = 0; i < p.size(); i++) {
      /* Axis angle. */
      params[t++] = 0;
      params[t++] = 0;
      params[t++] = 0;

      /* Scale. */
      params[t++] = 1;
    }

    /* Launch LevMar. */
    LevMar<BundleAdjustmentLevMarModel> levMar;
    BundleAdjustmentLevMarModel levMarModel(params.size(), residuals);
    levMar.fit(levMarModel, params);

    /* Write homographies back. */
    for(size_t i = 0; i < params.size() / 4; i++)
      p.getImage(i).setHomography(Homography(params[i * 4], params[i * 4 + 1], params[i * 4 + 2], params[i * 4 + 3]));
  }
}






