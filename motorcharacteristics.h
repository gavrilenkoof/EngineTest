#ifndef MOTORCHARACTERISTICS_H
#define MOTORCHARACTERISTICS_H

#include <QObject>

class MotorCharacteristics
{
public:
    MotorCharacteristics();

    void clearMaxTorque();
    void clearMaxRpm();

    void checkMaxTorque(double torque, double rpm);
    void checkMaxRpm(double rpm);

private:
    struct MaxTorque{
        double torque;
        double rpm;
    };

    double m_max_rpm;
    MaxTorque m_max_torque;
};

#endif // MOTORCHARACTERISTICS_H
