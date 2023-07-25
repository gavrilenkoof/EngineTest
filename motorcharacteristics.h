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

    static int const m_size = 250; // 2.5 khz data - 1 s -> 250 filtering - 100 ms

    struct AvgFilter
    {
        double data[m_size];
        uint16_t index;
        double sum;
        uint16_t count;
        uint16_t filter_size;

        double getAvg() {return sum / count;};

        void initFilter()
        {
            memset(this, 0, sizeof(struct AvgFilter));

            this->filter_size = m_size;
            this->sum = 0.0;
        }

        double filterRunAvg(double newVal)
        {
            this->sum -= this->data[this->index];
            this->data[this->index] = newVal;
            this->sum += this->data[this->index];
            this->index++;
            this->index = this->index % this->filter_size;
            if(this->count < this->filter_size)
                this->count++;

            return this->sum / this->count;
        }
    };

    AvgFilter m_torque_filter;
    AvgFilter m_rpm_filter;
    AvgFilter m_power_filter;

    double m_power_avg;

private:
    struct MaxTorque{
        double torque;
        double rpm;
    };

    double m_max_rpm;
    MaxTorque m_max_torque;
};

#endif // MOTORCHARACTERISTICS_H
