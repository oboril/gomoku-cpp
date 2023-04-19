#include <iostream>
#include <memory>
#include <chrono>

class Timer
{
private:
	std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimePoint;

public:
	 Timer(){ // Constructor returns a chrono time pointer
	 	m_StartTimePoint = std::chrono::high_resolution_clock::now(); //STD chrono time pointer
	 }
	 void Stop(){
	 	auto m_endTimePoint = std::chrono::high_resolution_clock::now();

	 	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
	 	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(m_endTimePoint).time_since_epoch().count();

	 	auto duration = end - start;
	 	double ms = duration * 0.001;

	 	std::cout << "TIMER: elapsed " << duration << "us (" << ms << "ms)\n"; 
	 }
	 ~Timer(){
	 	Stop();
	 }
	
};