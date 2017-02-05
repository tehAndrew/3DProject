/* Code by Frank D Luna*/

#include "PerformanceClock.h"

PerformanceClock::PerformanceClock() {
	secPerCount = 0.0;
	deltaTime   = -1.0;
	baseTime    = 0;
	pausedTime  = 0;
	prevTime    = 0;
	currentTime = 0;
	stopped     = false;

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*) &countsPerSec);
	secPerCount = 1.0 / (double) countsPerSec;
}

void PerformanceClock::start() {
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &startTime);

	if (stopped) {
		pausedTime += (startTime - stopTime);
		prevTime = startTime;
		stopTime = 0;
		stopped = false;
	}
}

void PerformanceClock::tick() {
	if (stopped) {
		deltaTime = 0;
		return;
	}

	__int64 ct;
	QueryPerformanceCounter((LARGE_INTEGER*) &ct);
	currentTime = ct;
	deltaTime = (currentTime - prevTime) * secPerCount;
	prevTime = currentTime;

	if (deltaTime < 0)
		deltaTime = 0;
}

void PerformanceClock::stop() {
	if (!stopped) {
		__int64 ct;
		QueryPerformanceCounter((LARGE_INTEGER*) &ct);
		stopTime = ct;
		stopped = true;
	}
}

void PerformanceClock::reset() {
	__int64 ct;
	QueryPerformanceCounter((LARGE_INTEGER*)&ct);

	baseTime = ct;
	prevTime = ct;
	stopTime = 0;
	stopped = false;
}

double PerformanceClock::getDeltaTime() const {
	return deltaTime;
}

double PerformanceClock::getTotalTime() const {
	if (stopped)
		return (double) (((stopTime - pausedTime) - baseTime) * secPerCount);
	else
		return (double) (((currentTime - pausedTime) - baseTime) * secPerCount);
}