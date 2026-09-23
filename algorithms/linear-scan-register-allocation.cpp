/*

https://web.cs.ucla.edu/~palsberg/course/cs132/linearscan.pdf
https://hassamuddin.com/blog/reg-alloc/
https://github.com/dxhj/linear-scan-register-allocator/

LinearScanRegisterAllocation
    active ← {}
    for each live interval i, in order of increasing start point do
        ExpireOldIntervals(i)
        if length(active) = R then
            SpillAtInterval(i)
        else
            register[i] ← a register removed from pool of free registers
            add i to active, sorted by increasing end point

ExpireOldIntervals(i)
    for each interval j in active, in order of increasing end point do
        if endpoint[j] ≥ startpoint[i] then
            return
        remove j from active
        add register[j] to pool of free registers

SpillAtInterval(i)
    spill ← last interval in active
    if endpoint[spill] > endpoint[i] then
        register[i] ← register[spill]
        location[spill] ← new stack location
        remove spill from active
        add i to active, sorted by increasing end point
    else
        location[i] ← new stack location

*/

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

struct Symbol
{
	string name;
	int reg;
	int location;
};

struct Interval
{
	Symbol symbol;
	int start;
	int end;
};

bool compare_by_start(const Interval &a, const Interval &b)
{
	return a.start < b.start;
}

bool compare_by_end(const Interval *a, const Interval *b)
{
	return a->end < b->end;
}

// This function looks at the end points of the lifetime of active registers in use
// Any register that has an endpoint greater than the interval start point means it's not in use anymore
// Therefore, we can free those registers to the pool and reuse them
void expire_old_intervals(vector<Interval *> &actives, Interval *interval, vector<int> &registers)
{
	// for each interval j in active, in order of increasing end point do
	sort(actives.begin(), actives.end(), compare_by_end);
	while (actives.size() > 0)
	{
		if (actives[0]->end >= interval->start)
			return;

		// add register[j] to pool of free registers
		registers.push_back(actives[0]->symbol.reg);

		// remove j from active
		actives.erase(actives.begin() + 0);
	}
}

// This function handles stack allocation (spilling) when there are no more free registers
void spill_at_interval(vector<Interval *> &actives, Interval *interval, int &stack)
{
	// spill ← last interval in active
	sort(actives.begin(), actives.end(), compare_by_end);
	auto spill = actives[actives.size() - 1];

	// if endpoint[spill] > endpoint[i] then
	if (spill->end > interval->end)
	{

		// if the last active interval (spill) happens later than this live interval
		// this means that we can assign this live interval the last active interval register
		// while the last active interval gets assigned the new stack location, this is a heuristic
		// to try and maximize the number of registers being free later

		printf("ACTION: SPILL INTERVAL(%d, %d)\n", spill->start, spill->end);
		printf("ACTION: ALLOCATE REGISTER %d TO INTERVAL(%d, %d)\n", spill->symbol.reg, interval->start, interval->end);

		// register[i] ← register[spill]
		// location[spill] ← new stack location
		// remove spill from active
		interval->symbol.reg = spill->symbol.reg;
		spill->symbol.location = stack;
		actives.pop_back();

		// add i to active, sorted by increasing end point
		actives.push_back(interval);
	}
	else
	{
		printf("ACTION: SPILL INTERVAL(%d, %d)\n", interval->start, interval->end);

		// the last active interval happens before this live interval end
		// this means that there is really nothing we can do except allocate from the stack
		// location[i] ← new stack location
		interval->symbol.location = stack;
	}
	stack += 1;
}

int main()
{
	vector<Interval> lives = {
	    {.symbol = {.name = "a", .reg = -1, .location = -1}, .start = 1, .end = 4},
	    {.symbol = {.name = "b", .reg = -1, .location = -1}, .start = 2, .end = 6},
	    {.symbol = {.name = "c", .reg = -1, .location = -1}, .start = 3, .end = 10},
	    {.symbol = {.name = "d", .reg = -1, .location = -1}, .start = 5, .end = 9},
	    {.symbol = {.name = "e", .reg = -1, .location = -1}, .start = 7, .end = 8},
	};

	// register pool, on startup, it will be populated with all usable registers
	int stack = 0;
	vector<int> registers;
	for (auto i = 1; i <= 2; i++)
		registers.push_back(i);

	// The algorithm below can handle non empty active list on startup, this is just to signify the the beginning of the algorithm
	// active <- {}
	vector<Interval *> actives;

	// begin register allocation here

	// there are two lists used, the active list and the live list
	// the live list represents a worklist that we need to handle
	// the goal is to satisfy the live list requests by allocating registers/stack/memory for each variable in the list

	// while we are allocating registers, the allocated register will go into the active list to signify that it is being used
	// this tells us if we need to spill

	// the start/end values are used as a interval of when a register becomes used/free; if it becomes free we can reuse it

	// for each live interval i, in order of increasing start point do
	sort(lives.begin(), lives.end(), compare_by_start);
	for (size_t i = 0; i < lives.size(); i++)
	{
		printf("SYMBOL: %s | REGISTER: %d LOCATION: %d | START: %d | END: %d\n",
		       lives[i].symbol.name.c_str(), lives[i].symbol.reg, lives[i].symbol.location, lives[i].start, lives[i].end);

		// expire old intervals
		expire_old_intervals(actives, &lives[i], registers);

		// if length(active) = R then
		//		SpillAtInterval(i)
		// else
		//		register[i] ← a register removed from pool of free registers
		//		add i to active, sorted by increasing end point
		if (registers.size() == 0)
		{
			// we need to spill since we don't have enough registers
			spill_at_interval(actives, &lives[i], stack);
		}
		else
		{
			// since it is already sorted, we can just add it to the current live set
			// the idea is that if we don't spill just allocate whatever in the pool
			// which register we allocate doesn't matter here, we assume any register is the same as any other
			printf("ACTION: ALLOCATE REGISTER (%d) TO INTERVAL(%d, %d)\n", registers[0], lives[i].start, lives[i].end);

			lives[i].symbol.reg = registers[0];
			registers.erase(registers.begin() + 0);

			// add it to the active list
			actives.push_back(&lives[i]);
		}
	}

	// at the end of the allocation all live intervals should either have a register or a stack/memory location
	puts("");
	printf("ALLOCATED:\n");
	for (size_t i = 0; i < lives.size(); i++)
	{
		printf("SYMBOL: %s | REGISTER: %d LOCATION: %d | START: %d | END: %d\n",
		       lives[i].symbol.name.c_str(), lives[i].symbol.reg, lives[i].symbol.location, lives[i].start, lives[i].end);
	}

	return 0;
}
