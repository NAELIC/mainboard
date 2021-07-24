#ifndef _SECURITY_H
#define _SECURITY_H

typedef enum {
    SECURITY_NO_ERROR = 0,
    SECURITY_HALL_FREEZE = 1,
    SECURITY_CURRENT_LIMIT = 2,
    SECURITY_CURRENT_MAX = 3,
    SECURITY_HALL_MISSING = 4,
    SECURITY_ENCODER_MISSING = 5,
    SECURITY_ENCODER_FAILURE = 6,
    SECURITY_PWM_MIN = 7,
    SECURITY_PWM_MAX = 8,
    SECURITY_NO_PHASE_IS_ON_THE_MASS = 9
} SecurityError;

typedef enum {
    SECURITY_NO_WARNING = 0,
    WARNING_MOTOR_LAG = 1,
    WARNING_INVALID_RANGE = 2,
    WARNING_SERVO_LAG = 3,
    WARNING_ENCODER_LAG = 4,
    WARNING_DELTAS_MAGNETIC = 5,
    WARNING_ERROR_FLAG_ENCODER = 6,
    WARNING_ERROR_PARITY_ENCODER = 7,
    WARNING_INCOHERENT_PACKET_ENCODER=8,
    WARNING_CURRENT_LAG = 9
} SecurityWarning;

void security_set_error(SecurityError type);
void security_set_warning(SecurityWarning type);

SecurityError security_get_error();
SecurityWarning security_get_warning();

#endif
