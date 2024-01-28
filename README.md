# Autonomous Ground Vehicle Control System

This repository contains the firmware and documentation for implementing a basic control system for an autonomous ground vehicle using a microcontroller board. The system controls two motors forming a differential drive and communicates with a control PC via a serial interface. The firmware ensures safe operation of the vehicle by adhering to hardware specifications and implementing various functionalities as outlined below.

## Hardware Specifications

- Each motor can run from -50 to +50 RPMs.
- Motors are controlled through a PWM signal with a frequency of 1 kHz.
- Dead time of at least 3 microseconds is used to prevent H-bridge problems.
- Motors should not be run above +-50 RPMs to avoid damage.
- Analog-to-Digital Converter (ADC) module is utilized for acquiring temperature sensor readings.
- Pulse Width Modulation (PWM) module is used for controlling motor speeds.

## Firmware Requirements

- Computes desired RPMs of wheels using a differential drive kinematic model.
- Ensures PWM signals generated are within motor specifications.
- Implements timeout mode if no references are received for more than 5 seconds.
- Supports receiving references at a minimum frequency of 10 Hz.
- Refreshes PWM value at least at 10 Hz frequency.
- Acquires temperature sensor readings at 10 Hz and sends averaged values to PC at 1 Hz.
- Sends feedback message to PC at 5 Hz.
- Implements safe mode functionality triggered by button press.
- Displays status information on an LCD screen.

## Messages Protocol

### Messages from PC

- `$HLREF,omega,speed*`: Desired angular velocity (omega) and linear velocity (speed) for the robot.
- `$HLENA*`: Enables firmware to send references to motors (exit safe mode).

### Messages to PC

- `$MCFBK,n1,n2,state*`: Applied RPM values (n1, n2) and state of the microcontroller.
- `$MCALE,n1,n2*`: Computes wheel RPM values outside the maximum allowed values.
- `$MCTEM,temp*`: Temperature readings.
- `$MCACK,msg_type,value*`: Acknowledgement of commands sent.

## Communication Protocols

- **Serial Communication (UART)**: Used for communication between the microcontroller and the control PC.
- **Serial Peripheral Interface (SPI)**: Potentially utilized for interfacing with external sensors or peripherals.

## Usage

1. Connect the microcontroller board to the motors, temperature sensor, LCD display, and buttons following the hardware specifications.
2. Upload the provided firmware onto the microcontroller board.
3. Configure the UART communication settings to match those of the control PC.
4. Power on the autonomous ground vehicle and monitor status through the LCD display.
5. Use the control PC to send reference values for desired speed and angular velocity.
6. In case of emergency, press the designated button to activate safe mode and stop the motors.


## Contributors
- Hussein Ahmed Fouad Hassan (s5165612@studenti.unige.it) 
- Daniele Martino Parisi (s4670964@studenti.unige.it)
- Davide Leo Parisi (s4329668@studenti.unige.it)
- Sara Sgambato (s4648592@studenti.unige.it)


