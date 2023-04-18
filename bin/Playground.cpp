

#include <thread>
#include <future>
#include <iostream>
#include "Simulation/Sim.hpp"

class A
{
public:
    template <typename T>
    T sum(T i1, T i2){
        return i1+i2;
    }
};

int main(){
    A a;
    // using deferent futures sum [1,68], [1.0, 3.14], ["Hello", "World"]
    std::future<int> f1 = std::async(&A::sum<int>, &a, 1, 68);
    std::future<double> f2 = std::async(&A::sum<double>, &a, 1.0, 3.14);
    std::future<std::string> f3 = std::async(&A::sum<std::string>, &a, "Hello", " World");
    // std::future<void> f4 = std::async(std::launch::async, &erppm::sim::runRobot<0,1,1,1>, &erppm::sim::get(), 5, 8);

    std::cout << "Result of f1 : " << f1.get() << std::endl;
    std::cout << "Result of f2 : " << f2.get() << std::endl;
    std::cout << "Result of f3 : " << f3.get() << std::endl;

    return 0;
}