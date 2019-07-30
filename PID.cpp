#ifndef _PID_SOURCE_
#define _PID_SOURCE_

//#include <iostream.h>
//#include <cmath>
#include <math.h>
#include "PID.h"

using namespace std;

class PIDImpl
{
    public:
        PIDImpl( double dt, double maximum, double minimum, double Kp, double Kd, double Ki );
        ~PIDImpl();
        double calculate( double setpoint, double pv);

    private:
        double _dt;
        double _max;
        double _min;
        double _Kp;
        double _Kd;
        double _Ki;
        double _pre_error;
        double _integral;
};

PID::PID() {
  
}

PID::PID( double dt, double maximum, double minimum, double Kp, double Kd, double Ki )
{
    pimpl = new PIDImpl(dt,maximum,minimum,Kp,Kd,Ki);
}
double PID::calculate( double setpoint, double pv )
{
    return pimpl->calculate(setpoint,pv);
}
PID::~PID() 
{
    delete pimpl;
}


/**
 * Implementation
 */
PIDImpl::PIDImpl( double dt, double maximum, double minimum, double Kp, double Kd, double Ki ) :
    _dt(dt),
    _max(maximum),
    _min(minimum),
    _Kp(Kp),
    _Kd(Kd),
    _Ki(Ki),
    _pre_error(0),
    _integral(0)
{
}

double PIDImpl::calculate( double setpoint, double pv )
{
    
    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = _Kp * error;

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    // Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout = _Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;

    // Save error to previous error
    _pre_error = error;

    return output;
}

PIDImpl::~PIDImpl()
{
}

#endif
