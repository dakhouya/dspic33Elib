#ifndef _CAN_CHINOOK3_MODULE
#define _CAN_CHINOOK3_MODULE

/***MESSAGES SID***/

// Sent by : Fujin
// The new pitch orientation 0 to 100% (int)
#define CAN_MSG_PITCH_ORIENTATION_SID     0x10E
// Sent by : 
// Description
#define CAN_MSG_EEPROM_CONFIG_ANSWER_SID  0x10D
// Sent by : 
// Description
#define CAN_MSG_EEPROM_CONFIG_REQUEST_SID 0x109
// Sent by : 
// Description
#define CAN_MSG_MANUAL_PITCH_SID          0x10C
#define CAN_MSG_BRAKE_SID                 0x11C
#define CAN_MSG_BUTTON_CMD_SID            0x12C
// Sent by : 
// Description
#define CAN_MSG_SHIFT_SID              	  0x1EA
// Sent by : 
// Description
#define CAN_MSG_WIND_SPEED_SID            0x20D
// Description
#define CAN_MSG_WIND_DIRECTION_SID        0x21D
// Description
#define CAN_MSG_TURBINE_RPM_SENSOR_SID    0x22D

// Sent by : 
// Description
#define CAN_MSG_TURBINE_RPM_MOTOR_SID     0x23B

// Sent by : 
// Description
#define CAN_MSG_WHEEL_RPM_SID             0x24D
// Description
#define CAN_MSG_TRUST_SID                 0x25D
// Description
#define CAN_MSG_TORQUE_SID                0x26D

// Sent by : 
// Description
#define CAN_MSG_TURBINE_DIRECTION_SID     0x279
// Description
#define CAN_MSG_GEAR_SID                  0x289

// Sent by : Fujin
// The gear to set (byte) (1 to 16)
#define CAN_MSG_GEAR_FUJIN_SID            0x28E

// Sent by : 
// Description
#define CAN_MSG_ACCELERATION_SID          0x6BD
// Description
#define CAN_MSG_CLUTCH_TEMPERATURE_SID    0x6CD

// Sent by : Fujin
// Voltage Sense (float) V
#define CAN_MSG_VOLTAGE_MONITOR_SID       0x6DE
// Current Sense (float) A
#define CAN_MSG_CURRENT_MONITOR_SID       0x6EE
// Time (int) HHMMSS
#define CAN_MSG_TIME_SID                  0x6FE

/***Reserved address***/
#define CAN_MSG_START_SID                 0x000
#define CAN_MSG_STOP_SID                  0x7FF
#define CAN_MSG_HEARTBEAT_SID             0x010

/***Warning***/
/*from 0x700 to 0x7FE*/

#endif
