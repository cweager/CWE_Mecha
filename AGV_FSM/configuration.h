//*****************************************************************************
//**
//**  PROGRAM:  AGV_FSM
//**  AUTHOR:   Cody Eager, 1/c
//**
//**  MODULE:   configuration.h
//**  DATE:     21 October 2014
//**
//**  DESCRIPTION:  AGV_FSM is the software control algorithm used to control
//**                AGV movement.  The Arduino on-board AGV receives navigation
//**                commands from the on-board processor (MATLAB) in the form
//**                of integer numbers.  Each integer (0-9) represents a state
//**                that MATLAB commands the Arduino to enter.  The following
//**                table represents the MATLAB integer command and the
//**                commanded state:
//**
//**                MATLAB CX  |  ARDUINO STATE
//**                ---------------------------
//**                    0      |      STOP
//**                    1      |      FWD
//**                    2      |      FWD, LFT
//**                    3      |      FWD, RGT
//**                    4      |      REV
//**                    5      |      REV, LFT
//**                    6      |      REV, RGT
//**                    7      |      SPIN LFT
//**                    8      |      SPIN RGT
//**                    9      |      N/A
//**                ---------------------------
//**
//**                *Note: Prototype includes state control using serial
//**                commands from the serial monitor.
//**                *Note: The use of ISR prevents us from using Arduino the
//**                Servo library.  This algorithm uses PWM signals via Arduino
//**                analogWrite to move the AGV.  The following table relates
//**                the pulse width to motor response:
//**
//**                ARDUINO CX | SABERTOOTH STATE
//**                ----------------------------
//**                  1000us   |   REV, FULL
//**                  1500us   |   ALL STOP
//**                  2000us   |   FWD, FULL
//**                ----------------------------
//**
//*****************************************************************************


/*
 * Autonomous Ground Vehicle FSM Algorithm
 *
 * Copyright 2014 Cody W. Eager 
 *
 *     cody.w.eager@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */
 
 
#ifndef configuration_H
    #define configuration_H

    #define F_CLK 16000000UL
    #define BAUD_RATE 9600L

    #define LINE_TERMINATOR 0x0A
    #define BUF_LEN 100
    #define MAX_FIELDS 20

    #define LED_PIN 13  // Arduino Board LED
    
    #define S1_PIN 10  // Sabertooth S1 Input
    #define S2_PIN 9  // Sabertooth S2 Input
#endif
