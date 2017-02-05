/* Code by Frank D Luna*/

#ifndef PERFORMANCECLOCK_H
#define PERFORMANCECLOCK_H

#include <windows.h>

class PerformanceClock {
	private:
		double secPerCount;
		double deltaTime;

		__int64 baseTime;
		__int64 pausedTime;
		__int64 stopTime;
		__int64 prevTime;
		__int64 currentTime;

		bool stopped;

	public:
		PerformanceClock();
		virtual ~PerformanceClock() {};

		void start();
		void tick();
		void stop();
		void reset();

		double getDeltaTime() const;
		double getTotalTime() const;
};

#endif