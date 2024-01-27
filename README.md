# Embedded Control System for Autonomous Ground Vehicle

## Overview

This repository contains the firmware for an embedded control system designed for an autonomous ground vehicle. The system enables precise control of the vehicle's movement while providing feedback to ensure safe operation. The firmware is developed to run on a microcontroller board and utilizes various peripherals such as PWM for motor control and UART for communication with an external control PC.

## Features

- Differential drive kinematic model for precise control of vehicle movement.
- PWM motor control to regulate motor speed within specified RPM range.
- UART communication for receiving reference values and sending feedback to a control PC.
- Safe mode functionality triggered by a button press for emergency stops.
- LCD display for real-time status monitoring.
- Temperature sensor integration for monitoring environmental conditions.
- Timeout mode implementation to handle communication interruptions.

## Hardware Requirements

- Microcontroller board compatible with PWM and UART communication.
- Two motors capable of running within -50 to +50 RPMs.
- Temperature sensor for environmental monitoring.
- LCD display for visual feedback.
- Buttons for triggering safe mode and switching display modes.

## Usage

1. Connect the microcontroller board to the motors, temperature sensor, LCD display, and buttons following the hardware specifications.
2. Upload the provided firmware onto the microcontroller board.
3. Configure the UART communication settings to match those of the control PC.
4. Power on the autonomous ground vehicle and monitor status through the LCD display.
5. Use the control PC to send reference values for desired speed and angular velocity.
6. In case of emergency, press the designated button to activate safe mode and stop the motors.

## Contributions

Contributions to this project are welcome! If you find any bugs, have suggestions for improvements, or want to add new features, please submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
