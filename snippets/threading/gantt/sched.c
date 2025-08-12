#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structures for the program

// represents a process state
typedef struct Process Process;
struct Process {
	// the info we read in
	int id;           // id of the proccess
	int arrival;      // time it arrived
	int process_time; // time it takes to finish the process

	// info we calculate
	int rtick;     // how much time remaining until finish
	int state;     // state the process is in executing, waiting, etc
	int *gantt;    // store gantt information for the process
	Process *next; // next process to run after this
};

// represents the scheduler
typedef struct Sched Sched;
struct Sched {
	// info we read in
	int count;      // number of processes
	int total_time; // total time interval that the processes run in

	// info we calculate
	int start_time;   // the first process starting time
	Process *process; // all the processes stored as an array
	Process **wlist;  // list of processes that hasn't arrived yet
};

// represents a process queue, which is ready to run or waiting
typedef struct Queue Queue;
struct Queue {
	int t, ti, ts;         // time, index for waiting list, timeslice
	Process *front, *back; // head and tail
};

// function pointers to allow switching of scheduling/ordering functions
typedef void (*sched_func)(Sched *, void *);
typedef void (*queue_func)(Queue *, Process *);

// scheduler functions
Sched *new_sched(const char *);
void free_sched(Sched *);
void run_sched(Sched *, const char *, sched_func, void *);

void fcfs(Sched *, void *);
void rr(Sched *, void *);
void spn(Sched *, void *);

// helper functions
void *xmalloc(size_t);
void die(const char *, ...);

// file to read in
const char *infile = "schedule.dat";

int
main(int argc, char *argv[])
{
	Sched *s;
	int quantum;

	// don't use the default infile if argument specified
	if (argc >= 2)
		infile = argv[1];

	// read in the file and process to get a scheduler
	s = new_sched(infile);

	// run the simulation with fcfs, rr 1, rr 3, spn
	run_sched(s, "FCFS", fcfs, NULL);

	quantum = 1;
	run_sched(s, "RR 1", rr, &quantum);

	quantum = 3;
	run_sched(s, "RR 3", rr, &quantum);

	run_sched(s, "SPN", spn, NULL);

	// cleanup
	free_sched(s);

#if defined(WIN32) || defined(WIN64)
	printf("Press return to quit\n");
	getchar();
#endif

	return 0;
}

// make a new sched out of a file
Sched *
new_sched(const char *fn)
{
	Process *p;
	Sched *s;
	FILE *fp;
	int i, id, arrival, process_time;

	fp = fopen(fn, "rt");
	if (!fp)
		die("%s: %s", fn, strerror(errno));

	s = xmalloc(sizeof(Sched));

	// read in count
	if (fscanf(fp, "%d", &s->count) != 1)
		die("failed to read count in file");

	// read in total time
	if (fscanf(fp, "%d", &s->total_time) != 1)
		die("failed to read total time in file");

	// see if it is valid
	if (s->count < 1)
		die("process count cannot be less than 1, got %d", s->count);

	if (s->total_time < 0)
		die("total time cannot be less than 0, got %d", s->total_time);

	s->process = xmalloc(sizeof(Process) * s->count);
	s->wlist = xmalloc(sizeof(Process *) * s->count);
	for (i = 0; i < s->count; i++)
		s->process[i].gantt = xmalloc(sizeof(int) * s->total_time);

	// read in the process info
	for (i = 0; i < s->count; i++) {
		if (fscanf(fp, "%d %d %d", &id, &arrival, &process_time) != 3)
			die("failed to read process information: %d", i);

		if (arrival < 0 || process_time < 0) {
			die("process line %d contains invalid information: %d %d %d", i, id,
			    arrival, process_time);
		}

		p = &s->process[i];
		p->id = id;
		p->arrival = arrival;
		p->process_time = process_time;
	}

	fclose(fp);
	return s;
}

void
free_sched(Sched *s)
{
	int i;

	if (!s)
		return;

	for (i = 0; i < s->count; i++)
		free(s->process[i].gantt);

	free(s->process);
	free(s->wlist);
	free(s);
}

// sort processes based on arrival time
static int
sort_proc(const void *v1, const void *v2)
{
	const Process *p1, *p2;

	p1 = v1;
	p2 = v2;

	if (p1->arrival < p2->arrival)
		return -1;

	if (p1->arrival == p2->arrival) {
		// compare pointer for stable sort if arrival time are equal
		if (p1 < p2)
			return -1;
		else if (p1 == p2)
			return 0;
		else
			return 1;
	}

	return 1;
}

static void
print_gantt(Sched *s, const char *name)
{
	Process *p;
	int i, j, etime, wtime, turn;
	double nturn, avgtime, avgturn, avgnturn;

	printf("%s\n\n", name);
	for (i = 0; i < s->count; i++) {
		p = &s->process[i];
		printf("%2d ", p->id);
		for (j = 0; j < s->total_time; j++) {
			if (p->gantt[j])
				printf("%2c ", p->gantt[j]);
			else
				printf("   ");
		}
		printf("\n");
	}
	printf("  ");
	for (i = 0; i < s->total_time; i++)
		printf(" %2d", i + 1);
	printf("\n\n");

	avgtime = 0;
	avgturn = 0;
	avgnturn = 0;
	printf("\twait\tturnaround\tnormalized turnaround\n");
	for (i = 0; i < s->count; i++) {
		p = &s->process[i];
		etime = 0;
		wtime = 0;
		turn = 0;
		for (j = 0; j < s->total_time; j++) {
			switch (p->gantt[j]) {
			case 'W':
				wtime++;
				turn++;
				break;

			case 'E':
				etime++;
				turn++;
				break;
			}
		}

		printf("%2d\t%2d\t%2d\t\t", p->id, wtime, turn);

		nturn = 0;
		if (etime != 0) {
			nturn = ((double)turn / etime);
			printf("%.2f", nturn);
		} else
			printf("N/A");

		printf("\n");

		avgtime += wtime;
		avgturn += turn;
		avgnturn += nturn;
	}
	printf(" AVG\t%.2f\t%.2f\t\t%.2f", avgtime / s->count, avgturn / s->count,
	       avgnturn / s->count);
	printf("\n\n");
}

// the scheduler driver, runs different schedulers and print out gantt chart
void
run_sched(Sched *s, const char *name, sched_func sched, void *data)
{
	Process *p;
	int i;

	// reset
	for (i = 0; i < s->count; i++) {
		p = &s->process[i];
		p->state = 0;
		p->next = NULL;

		memset(p->gantt, 0, sizeof(int) * s->total_time);
		s->wlist[i] = p;
	}
	// sort the list based on arrival time so we can start
	qsort(s->wlist, s->count, sizeof(Process *), sort_proc);
	s->start_time = s->wlist[0]->arrival;

	// run scheduler
	sched(s, data);

	print_gantt(s, name);
}

// first in first out, so this puts process into the back
static void
fifo(Queue *q, Process *w)
{
	q->back->next = w;
	q->back = q->back->next;
}

// puts in spn order, process with shortest time goes first
static void
spnorder(Queue *q, Process *w)
{
	Process *pn, *n;

	for (pn = n = q->front; n; pn = n, n = n->next) {
		// find the first process where this process
		// takes shorter time and it's not executing
		if (w->process_time < n->process_time && n->state != 'E') {
			pn->next = w;
			w->next = n;
			break;
		}
	}

	if (!n)
		fifo(q, w);
}

// fill in gantt chart state
void
fill_gantt(Sched *s, Queue *q)
{
	Process *p;
	int i;

	for (i = 0; i < s->count; i++) {
		p = &s->process[i];
		p->gantt[q->t] = p->state;
	}
}

// process the wait list, get any new arrivals
static Process *
process_wlist(Sched *s, Queue *q, queue_func order)
{
	Process *w;

	while (q->ti < s->count) {
		w = s->wlist[q->ti];
		if (w->arrival > q->t)
			break;

		w->state = 'W';
		w->rtick = w->process_time;
		w->next = NULL;
		q->ti++;

		if (!q->front) {
			q->front = q->back = w;
			w->state = 'E';
		} else
			order(q, w);
	}

	return q->front;
}

// called a process is done, gets next process
// and fill out remainder of gantt chart for the
// terminating process
static Process *
set_done(Sched *s, Queue *q)
{
	Process *p, *n;

	p = q->front;
	p->state = 'T';
	n = q->front->next;
	if (n)
		n->state = 'E';
	else
		q->back = NULL;

	q->front = n;
	return q->front;

	(void)s;
}

// put process in front to the back, used in RR
static Process *
front_to_back(Queue *q)
{
	Process *p;

	p = q->front;
	if (!p)
		return NULL;

	if (p == q->back)
		return p;

	q->front = p->next;
	q->back->next = p;
	q->back = p;

	p->next = NULL;
	p->state = 'W';
	q->front->state = 'E';

	return q->front;
}

// FCFS scheduler
void
fcfs(Sched *s, void *data)
{
	Process *p;
	Queue q;

	memset(&q, 0, sizeof(Queue));
	for (q.t = s->start_time; q.t < s->total_time; q.t++) {
		p = process_wlist(s, &q, fifo);
		if (!p) {
			fill_gantt(s, &q);
			continue;
		}

		while (p && p->rtick <= 0)
			p = set_done(s, &q);

		if (p)
			p->rtick--;

		fill_gantt(s, &q);
	}

	(void)data;
}

// Round Robin scheduler
void
rr(Sched *s, void *data)
{
	Process *p;
	Queue q;
	int quantum;

	quantum = *((int *)data);
	if (quantum <= 0) {
		printf("invalid quantum for rr: %d\n", quantum);
		return;
	}

	memset(&q, 0, sizeof(Queue));
	q.ts = quantum;

	for (q.t = s->start_time; q.t < s->total_time; q.t++) {
		p = process_wlist(s, &q, fifo);
		if (!p) {
			fill_gantt(s, &q);
			continue;
		}

		if (p->rtick <= 0) {
			while (p && p->rtick <= 0)
				p = set_done(s, &q);

			q.ts = quantum;
		} else if (q.ts == 0) {
			p = front_to_back(&q);
			q.ts = quantum;
		}

		if (p) {
			q.ts--;
			p->rtick--;
		}

		fill_gantt(s, &q);
	}
}

// Shortest Process Next scheduler
void
spn(Sched *s, void *data)
{
	Process *p;
	Queue q;

	memset(&q, 0, sizeof(Queue));
	for (q.t = s->start_time; q.t < s->total_time; q.t++) {
		p = process_wlist(s, &q, spnorder);
		if (!p) {
			fill_gantt(s, &q);
			continue;
		}

		while (p && p->rtick <= 0)
			p = set_done(s, &q);

		if (p)
			p->rtick--;

		fill_gantt(s, &q);
	}

	(void)data;
}

void *
xmalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (!ptr)
		die("out of memory");

	memset(ptr, 0, size);
	return ptr;
}

void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}
