#ifndef __IMAGEMATCHMODEL_H__
#define __IMAGEMATCHMODEL_H__

#include "config.h"
#include <arx/LinearAlgebra.h>
#include <arx/smart_ptr.h>
#include "RANSAC.h"
#include "Match.h"

namespace prec {
  /** 
   * ImageMatchModel implements a GenericRANSACModel concept.
   */
  class ImageMatchModel: public GenericRANSACModel<Match> {
  private:
    class ImageMatchModelData {
    public:
      arx::Matrix3f trans;
    };

    arx::shared_ptr<ImageMatchModelData> data;

  public:
    ImageMatchModel(): data(new ImageMatchModelData()) {};

    bool fit(const arx::ArrayList<Match>& m) {
      using namespace arx;

      /* We need at least 2 matches. */
      if(m.size() < 2)
        return false;

      // TODO: least squares fit if matches.size() > 2

      /* Target transformation will transform d1 into d0. */
      Vector2f d0 = m[1].getKey(0).getXY() - m[0].getKey(0).getXY();
      Vector2f d1 = m[1].getKey(1).getXY() - m[0].getKey(1).getXY();;

      /* Rotation angle. */
      float angle = d0.angle() -  d1.angle();
      float sinAngle = sin(angle);
      float cosAngle = cos(angle);

      /* Scale. */
      float s1 = d1.norm();
      float s0 = d0.norm();
      
      /* d0.norm() == 0 || d1.norm() == 0 means that we're working with joint match like (A matches B), (B matches C). 
       * It obvously won't produce a good solution, so we return false. */
      if(abs(s0) < EPS || abs(s1) < EPS)
        return false;
      
      float s = s0 / s1;

      /* Fill transformation matrix. */
      data->trans[0][0] = s * cosAngle;
      data->trans[0][1] = s * -sinAngle;
      data->trans[0][2] = s * (cosAngle * (-m[0].getKey(1).getX()) - sinAngle * (-m[0].getKey(1).getY())) + m[0].getKey(0).getX();
      data->trans[1][0] = s * sinAngle;
      data->trans[1][1] = s * cosAngle;
      data->trans[1][2] = s * (sinAngle * (-m[0].getKey(1).getX()) + cosAngle * (-m[0].getKey(1).getY())) + m[0].getKey(0).getY();
      data->trans[2][0] = 0.0;
      data->trans[2][1] = 0.0;
      data->trans[2][2] = 1.0;

      /* Check whether it does what it should. */
      assert((data->trans * Vector3f(m[0].getKey(1).getXY()) - Vector3f(m[0].getKey(0).getXY())).normSqr() < EPS && 
             (data->trans * Vector3f(m[1].getKey(1).getXY()) - Vector3f(m[1].getKey(0).getXY())).normSqr() < EPS);

#if 0
      // homography failed...

      /* Target transformation will transform (a1, b1) -> (a0, b0). */
      Vector2f a1xy = m[1].getKey(0).getXY();
      Vector2f b1xy = m[1].getKey(1).getXY();

      Vector3f a0 = Vector3f(m[0].getKey(0).getXY());
      Vector3f b0 = Vector3f(m[0].getKey(1).getXY());

      /* Compute scale. */
      float a0_dot_b0 = a0.dot(b0);
      float d = sqr(a0_dot_b0) / (a0.normSqr() * b0.normSqr());
      float A = a1xy.dot(b1xy);
      float B = a1xy.normSqr();
      float C = b1xy.normSqr();

      float a = sqr(A) - d * B * C;
      float b = 2 * A - d * (B + C);
      float c = 1 - d;
      float D = sqr(b) - 4 * a * c;
      
      float k = sqrt((-b + ((a > 0) ? 1.0f : -1.0f) * sqrt(D)) / (2 * a)) * ((a0_dot_b0 > 0) ? 1.0f : -1.0f);

      Vector3f a1 = Vector3f(a1xy * k);
      Vector3f b1 = Vector3f(a1xy * k);

      float a1_dot_b1 = a1.dot(b1);
      float dd = sqr(a1_dot_b1) / (a1.normSqr() * b1.normSqr());
#endif

      /* We're done. */
      return true;
    }

    float calculateFitError(const Match& m) const {
      using namespace arx;

      /* Build homogenous coordinates for the point in the second image. */
      Vector3f v(m.getKey(1).getXY());

      /* Calculate point's expected position in the first image. */
      Vector3f expectedV = data->trans * v;
      
      /* we don't need this since we use affine transformation. */
      // expectedV /= expectedV.z(); 

      /* Now calculate the squared distance between expected and real point positions. */
      return (Vector3f(m.getKey(0).getXY()) - expectedV).normSqr();
    }

    const arx::Matrix3f& getAffineTransform() const { return data->trans; }
  };

} // namespace prec

#endif // __IMAGEMATCHMODEL_H__
