#include <doctest/doctest.h>
#include "../event.h"
#include <thread>
#include <atomic>

using namespace std;

int g_bCounter = 0;
void Function()
{
	g_bCounter++;
}

TEST_CASE("Events - test events")
{
	// Add two events, call WaitForSignal() from another thread, check if events executed
	CEvents events;
	events.AddEventFunction(std::bind(&Function));
	events.AddEventFunction(std::bind(&Function));

	std::atomic<bool> done{ false };

	auto eventThread = [&done, &events]() {
		events.WaitForSignal();

		IEvent* ev = events.GetNextEvent();

		CHECK(ev);
		ev->Execute();
		ev = events.GetNextEvent();

		CHECK(ev);
		ev->Execute();
		ev = events.GetNextEvent();

		CHECK(!ev);

		done = true;
	};

	thread t(eventThread);

	// wait for a thread to end
	this_thread::sleep_for(200ms);

	// check if thread is done
	CHECK(done == true);

	t.join();

	CHECK(g_bCounter == 2);
}
