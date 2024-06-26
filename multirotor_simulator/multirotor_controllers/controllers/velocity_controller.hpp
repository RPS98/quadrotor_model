/*!*******************************************************************************************
 *  \file       velocity_controller.hpp
 *  \brief      VelocityController class definition.
 *  \authors    Rafael Pérez Seguí
 *
 *  \copyright  Copyright (c) 2022 Universidad Politécnica de Madrid
 *              All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************************************************************************/

#ifndef MULTIROTOR_CONTROLLERS_CONTROLLERS_VELOCITY_CONTROLLER_HPP_
#define MULTIROTOR_CONTROLLERS_CONTROLLERS_VELOCITY_CONTROLLER_HPP_

#include "pid_controller/pid.hpp"

namespace velocity_controller {

/**
 * @brief Velocity controller parameters
 *
 * @tparam P Precision type of the controller
 */
template <typename P = double>
struct VelocityControllerParams {
  // Check if P is a numeric type
  static_assert(std::is_floating_point<P>::value,
                "MotorParams must be used with a floating-point type");

  using PIDParams = pid_controller::PIDParams<P>;

  PIDParams pid_params = PIDParams();  // PID parameters
};

/**
 * @brief Velocity controller using PID controller
 *
 * Convert the desired linear velocity to desired linear acceleration.
 *
 * @tparam P Precision type of the controller
 */
template <typename P = double>
class VelocityController : public pid_controller::PID<P> {
  // Check if P is a numeric type
  static_assert(std::is_floating_point<P>::value,
                "VelocityController must be used with a floating-point type");

  using Scalar    = P;
  using Vector3   = Eigen::Matrix<P, 3, 1>;
  using Matrix3   = Eigen::Matrix<P, 3, 3>;
  using PID       = pid_controller::PID<P>;
  using PIDParams = pid_controller::PIDParams<P>;

public:
  /**
   * @brief Construct a new VelocityController object
   *
   * @param pid_params PID parameters
   */
  explicit VelocityController(const PIDParams &pid_params) : PID(pid_params) {}

  /**
   * @brief Construct a new VelocityController object
   *
   * @param params VelocityControllerParams parameters
   */
  explicit VelocityController(
      const VelocityControllerParams<P> &params = VelocityControllerParams<P>())
      : PID(params.pid_params) {}

  /**
   * @brief Destroy the VelocityController object
   *
   */
  ~VelocityController() {}

  /**
   * @brief Compute the control action
   *
   * @param current_linear_velocity Current velocity (m)
   * @param desired_linear_velocity Desired velocity (m)
   * @param dt Scalar with the time step (s)
   *
   * @return Vector3 with the desired vehicle linear acceleration (m/s^2)
   */
  Vector3 linear_velocity_to_linear_acceleration(const Vector3 &current_linear_velocity,
                                                 const Vector3 &desired_linear_velocity,
                                                 const Scalar dt) {
    // Compute the velocity error
    Vector3 velocity_error = this->get_error(current_linear_velocity, desired_linear_velocity);

    // Compute the desired linear velocity
    Vector3 desired_linear_acceleration = this->compute_control(dt, velocity_error);

    return desired_linear_acceleration;
  }

  /**
   * @brief Update controller parameters
   *
   * @param params VelocityControllerParams
   */
  inline void update_params(const VelocityControllerParams<P> &params) {
    this->update_pid_params(params.pid_params);
  }

  // Getters

  /**
   * @brief Get the desired linear acceleration
   *
   * @return Vector3& Desired linear acceleration (m/s)
   */
  inline const Vector3 &get_desired_linear_acceleration() const { return this->get_output(); }

  /**
   * @brief Get the velocity error
   *
   * @return Vector3& Velocity error (m)
   */
  inline const Vector3 &get_velocity_error() const { return this->get_proportional_error(); }
};  // Class VelocityController

}  // namespace velocity_controller

#endif  // MULTIROTOR_CONTROLLERS_CONTROLLERS_VELOCITY_CONTROLLER_HPP_
