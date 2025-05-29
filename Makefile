CC = gcc
CFLAGS = -Wall -pthread
OBJS = scheduler.o process.o queue.o simulation.o sched_ops.o proc_ops.o
TARGET = simulation_scheduler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

scheduler.o: scheduler.c scheduler.h process.h queue.h
	$(CC) $(CFLAGS) -c scheduler.c

process.o: process.c process.h scheduler.h
	$(CC) $(CFLAGS) -c process.c

queue.o: queue.c queue.h process.h
	$(CC) $(CFLAGS) -c queue.c

simulation.o: simulation.c scheduler.h
	$(CC) $(CFLAGS) -c simulation.c

sched_ops.o: sched_ops.c sched_ops.h scheduler.h
	$(CC) $(CFLAGS) -c sched_ops.c

proc_ops.o: proc_ops.c proc_ops.h process.h
	$(CC) $(CFLAGS) -c proc_ops.c

clean:
	rm -f $(OBJS) $(TARGET) 