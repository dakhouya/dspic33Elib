#ifndef _CAN_CHINOOK3_MODULE
#define _CAN_CHINOOK3_MODULE

/***Reserved address***/
#define CAN_MSG_START_SID                 0x000
#define CAN_MSG_STOP_SID                  0x7FF

/***Warning***/
/*from 0x700 to 0x7FE*/

// Info
// -----
// - **CAN Speed** : *125Kbits/s*
// - **CAN id** : *11* bits

// ### Id Structure

// - *0x[7F][F]* : *0x[Data Id][Sending board]*
// - *Data Id* : Le nom de la donnée [wind speed]
// - *Sending board* : L'id du board qui envoi le message sur le bus
// - **Minimum** : *0x000*
// - **Maximum** : *0x7FF*


// ### Sending Board ids

// - **9** : Mât/transmission
// - **A** : Volant
// - **B** : Éolienne
// - **C** : Commande
// - **D** : Sensor
// - **E** : batmon/fujin

// ### Adresse : Data : Owner

// - Prioritaire [0x0XX - 0X4FF]
// - Pas prioritaire [0x5XX - 0X6FF]
// - Warnings [0x7XX - 0X7FF]
//   - *0x700 to 0x7FE* : Warning : All
//   - *0x7FF* : Last **Reserved**

// Battery monitor / Calculation :
// -------------------------------

// **Output:**
// - *0x10E* : Orientation du Pitch [0-100%] [float]
#define CAN_MSG_PITCH_ORIENTATION_SID     0x10E
// - *0x28E* : Gearage de la transmission [0-16] [byte]
#define CAN_MSG_GEAR_FUJIN_SID            0x28E
// - *0x6DE* : Voltage Sense [V] [float]
#define CAN_MSG_VOLTAGE_MONITOR_SID       0x6DE
// - *0x6EE* : Current Sense [A] [float]
#define CAN_MSG_CURRENT_MONITOR_SID       0x6EE
// - *0x010* : HeartBeat     [bool] (Pour assurer que le Chinook est en marche / Sinon le frein s'active)
#define CAN_MSG_HEARTBEAT_SID             0x010
// - *0x6FE* : Time (int) HHMMSS
#define CAN_MSG_TIME_SID                  0x6FE

// Commande :
// ----------
// **Output:**
// - ** : Orientation du Pitch en manuel [% 0-100] [float]
#define CAN_MSG_MANUAL_PITCH_SID          0x10C
// - ** : Frein éolienne [on-off] [bool]
#define CAN_MSG_BRAKE_SID                 0x11C
// - ** : Bouton [0x0 - 0xF] [byte]
#define CAN_MSG_BUTTON_CMD_SID            0x12C

// Volant :
// --------
// **Output:**
// - *0x1EA* : Shift [0x0 - 0x3] [byte]
#define CAN_MSG_SHIFT_SID                  0x1EA

// Moteur (Éolienne) :
// ------------------------
// **Output:**
// - ** : Vitesse de l'éolienne    [RPM] [float]
#define CAN_MSG_TURBINE_RPM_MOTOR_SID     0x23B
// - ** : Torque                   [Nm]  [float]

// Sensor Board :
// ------------------------

// **Output:**
// - Vitesse du vent          [m/s]
#define CAN_MSG_WIND_SPEED_SID            0x20D
// - Orientation du vent      [Degrés]
#define CAN_MSG_WIND_DIRECTION_SID        0x21D
// - Vitesse de l'éolienne    [RPM]  {Si le capteur de tête d'éolienne n'est pas installé}
#define CAN_MSG_TURBINE_RPM_SENSOR_SID    0x22D
// - Vitesse des roues        [RPM]
#define CAN_MSG_WHEEL_RPM_SID             0x24D
// - Trust                    [Lb]   {À confirmer pour les unités}
#define CAN_MSG_TRUST_SID                 0x25D
// - Torque                   [Nm]   {Si le capteur de tête d'éolienne n'est pas installé}
#define CAN_MSG_TORQUE_SID                0x26D
// - Température de la clutch [Degrés C]
#define CAN_MSG_CLUTCH_TEMPERATURE_SID    0x6CD
// - Vibration/Accélération   [m/s^2]
#define CAN_MSG_ACCELERATION_SID          0x6BD
// - Message de configuration de gear
#define CAN_MSG_EEPROM_CONFIG_ANSWER_SID  0x10D

// Mât et transmission :
// ------------------------
// Input:
// - Orientation du vent      [Degrés]
// - Shift Up                 []
// - Shift Down               []
// - Mât Right                []
// - Mât Left                 []
// - CAN bus alive            []

// **Output:**
// - Position du mât          [Degrés]
#define CAN_MSG_TURBINE_DIRECTION_SID     0x279
// - Gear de la transmission  []         {1 à 14 :  à confirmer}
#define CAN_MSG_GEAR_SID                  0x289
// - Demande de la dernière gear
#define CAN_MSG_EEPROM_CONFIG_REQUEST_SID 0x109

#endif
