/*
 * MotorTask.h
 *
 *  Created on: Jun 26, 2025
 *      Author: Ahmed Ragab
 */

#ifndef INC_MOTORTASK_H_
#define INC_MOTORTASK_H_

#include "TaskHandle.h"
#include "Motor_interface.h"

/* Pin definitions for two L298N drivers (4 motors) */

#define MOTOR1_IN1_GPIO   GPIOB      /* Left motors  */
#define MOTOR1_IN2_GPIO   GPIOB      /* Left motors  */
#define MOTOR2_IN3_GPIO   GPIOB      /* Right motors */
#define MOTOR2_IN4_GPIO   GPIOB      /* Right motors */


#define MOTOR1_IN1_PIN   GPIO_PIN_12 /* Left motors   */
#define MOTOR1_IN2_PIN   GPIO_PIN_13 /* Left motors   */
#define MOTOR2_IN3_PIN   GPIO_PIN_14 /* Right motors  */
#define MOTOR2_IN4_PIN   GPIO_PIN_15 /* Right motors  */

/** Direction constants for motor movement **/
#define MOTOR_DIR_STOP      0U  /** Stop all motors **/
#define MOTOR_DIR_FORWARD   1U  /** Move forward **/
#define MOTOR_DIR_BACKWARD  2U  /** Move backward **/
#define MOTOR_DIR_LEFT      3U  /** Turn left **/
#define MOTOR_DIR_RIGHT     4U  /** Turn right **/

/**
 * @brief  Structure representing a single motor command:
 *         direction + speed percentage
 */
typedef struct {
    uint8_t direction;  /** 0=STOP, 1=FORWARD, 2=BACKWARD, 3=LEFT, 4=RIGHT, ....... **/
    uint8_t speed;      /** 0..100 percent **/
} MotorCommand_t;


#endif /* INC_MOTORTASK_H_ */
