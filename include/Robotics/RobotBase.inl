/*
*   Inline file for RobotBase.hpp
*/

template <typename T>
std::list<T>& erppm::RobotBase::getSensorList(){
    static std::list<T> sensors_T;              // it has to be a list, so the pointers won't change when vector relocates data
    return sensors_T;
}

template <typename T>
void erppm::RobotBase::addSensor(T&& sensor){
    std::list<T>& sensors_T = getSensorList<T>();
    
    sensor.body.getColor() = body.getColor();
    sensor.updateDecoratorColors(body.getColor());
    sensors_T.push_back(std::move(sensor));
    sensors.push_back( &(sensors_T.back()) );
    sensors_T.back().parent = this;
    sensorData.resize(sensorData.size() + sensors_T.back().getMeasurementDataSize());
}

template <typename T>
void erppm::RobotBase::popSensors(int n){
    std::list<T>& sensors_T = getSensorList<T>();
    for(int i = 0; i < n; i++){
        sensors.erase(std::remove(sensors.begin(), sensors.end(), &(sensors_T.back())), sensors.end());
        sensors_T.pop_back();
    }
}