#ifndef __HOMOGRAPHY_H__
#define __HOMOGRAPHY_H__

#include "config.h"
#include <cassert>
#include <cmath>
#include "arx/LinearAlgebra.h"
#include "arx/Utility.h"

namespace prec {

  /**
   * Homography represents a rotate-scale transformation, parametrized with scale and axis angle.
   * 
   * What is axis angle?
   * The axis angle representation of a rotation, also known as the exponential coordinates of a rotation, parameterizes a 
   * rotation by two values: a unit vector indicating the direction of a directed axis (straight line), and an angle describing 
   * the magnitude of the rotation about the axis. The rotation occurs in the sense prescribed by the right hand rule.
   * 
   * Example
   * Say you are standing on the ground and you pick the direction of gravity to be the negative z direction. Then if you turn to 
   * your left, you will travel PI/2 radians (or 90 degrees) about the z axis. In axis angle representation, this would be
   *   <axis, angle> = <(0,0,1), PI/2>
   * This can be represented as a rotation vector with a magnitude of PI/2 pointing in the z direction:
   *   (0,0,PI/2)
   * 
   * Exponential map to matrix representation
   *   R = e^(angle * hat(axis))
   * Using Taylor expansion, we get
   *   R = I  +  hat(axis) * sin(angle)  +  hat(axis)^2 * (1-cos(angle))
   * where R is a 3x3 rotation matrix and the hat operator gives the antisymmetric matrix equivalent of the cross product
   *                      |  0  -z   y  |
   *   hat( (x, y, z) ) = |  z   0  -x  |
   *                      | -y   x   0  |
   * 
   * (taken from Wikipedia, see http://en.wikipedia.org/wiki/Axis_angle and http://en.wikipedia.org/wiki/Hat_operator)
   */
  class Homography {
  private:
    arx::Vector3f axis;
    float scale;

    static arx::Matrix3f getScalePart(float scale) {
      arx::Matrix3f result = arx::Matrix3f(0);
      result[0][0] = scale;
      result[1][1] = scale;
      result[2][2] = 1;
      return result;
    }

    arx::Matrix3f getRotationPart() const {
      float angle = this->axis.norm();
      arx::Vector3f axis = (angle == 0) ? this->axis : (this->axis * (1 / angle));

      arx::Matrix3f hat(0);

      hat[1][0] =  axis[2];
      hat[2][0] = -axis[1];
      hat[2][1] =  axis[0];
      hat[0][1] = -axis[2];
      hat[0][2] =  axis[1];
      hat[1][2] = -axis[0];

      return arx::Matrix3f::identity() + hat * sin(angle) + hat * hat * (1 - cos(angle));
    }

    arx::Matrix3f getInverseRotationPart() const {
      return getRotationPart().transpose();
    }

    arx::Matrix3f getRotationPartDerivative(unsigned int paramIndex) const {
      assert(paramIndex < 3);
      arx::Matrix3f m(0);
      if(paramIndex == 0) {
        m[2][1] = 1;
        m[1][2] = -1;
        return getRotationPart() * m;
      } else if(paramIndex == 1) {
        m[0][2] = 1;
        m[2][0] = -1;
        return getRotationPart() * m;
      } else {
        m[0][1] = -1;
        m[1][0] = 1;
        return getRotationPart() * m;
      }
    }

    arx::Matrix3f getInverseRotationPartDerivative(unsigned int paramIndex) const {
      return getRotationPartDerivative(paramIndex).transpose();
    }

    arx::Matrix3f getScalePart() const {
      return getScalePart(this->scale);
    }

    arx::Matrix3f getInverseScalePart() const {
      return getScalePart(1 / this->scale);
    }

    arx::Matrix3f getScalePartDerivative() const {
      arx::Matrix3f result = getScalePart(1);
      result[2][2] = 0;
      return result;
    }

    arx::Matrix3f getInverseScalePartDerivative() const {
      arx::Matrix3f result = getScalePart(-1 / arx::sqr(this->scale));
      result[2][2] = 0;
      return result;
    }

  public:
    arx::Matrix3f getMatrix() const {
      return getScalePart() * getRotationPart();
    }
    
    arx::Matrix3f getInverseMatrix() const {
      return getInverseRotationPart() * getInverseScalePart();
    }

    arx::Matrix3f getMatrixDerivative(unsigned int paramIndex) const {
      assert(paramIndex < 4);
      if(paramIndex < 3)
        return getScalePart() * getRotationPartDerivative(paramIndex);
      else
        return getScalePartDerivative() * getRotationPart();
    }

    arx::Matrix3f getInverseMatrixDerivative(unsigned int paramIndex) const {
      assert(paramIndex < 4);
      if(paramIndex < 3)
        return getInverseRotationPartDerivative(paramIndex) * getInverseScalePart();
      else
        return getInverseRotationPart() * getInverseScalePartDerivative();
    }

    Homography() {
      this->axis[0] = 0;
      this->axis[1] = 0;
      this->axis[2] = 0;
      this->scale = 1;
    }

    Homography(float x, float y, float z, float scale) {
      this->axis[0] = x;
      this->axis[1] = y;
      this->axis[2] = z;
      this->scale = scale;
    }

    float getParam(unsigned int paramIndex) const {
      assert(paramIndex < 4);
      return (paramIndex < 3) ? this->axis[paramIndex] : this->scale;
    }

    void setParam(unsigned int paramIndex, float value) {
      assert(paramIndex < 4);
      if(paramIndex < 3)
        this->axis[paramIndex] = value;
      else
        this->scale = value;
    }
  };

} // namespace prec

#endif