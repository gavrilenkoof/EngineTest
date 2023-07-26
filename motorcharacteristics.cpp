#include "motorcharacteristics.h"

MotorCharacteristics::MotorCharacteristics()
{
    clearMaxTorque();
    clearMaxRpm();
    m_power_avg = 0.0 / 0.0;
}

void MotorCharacteristics::clearMaxRpm()
{
    m_max_rpm = 0.0;
}

void MotorCharacteristics::clearMaxTorque()
{
    m_max_torque.torque = 0.0;
    m_max_torque.rpm = 0.0;
}

void MotorCharacteristics::checkMaxTorque(double torque, double rpm)
{
    if(torque >= m_max_torque.torque){
        m_max_torque.rpm = rpm;
        m_max_torque.torque = torque;
    }
}

void MotorCharacteristics::checkMaxRpm(double rpm)
{
    if(rpm >= m_max_rpm){
        m_max_rpm = rpm;
    }
}
