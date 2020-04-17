from multiprocessing import Process
import queue
import time
import os
import random

process_list = [
	'procss-001',
	'procss-002',
	'procss-003',
]

def child_task(taski):
	print("child process {} begin, parent {}".format(os.getpid(), os.getppid()))
	t_start = time.time()

	print("child process {} executing task {}...".format(os.getpid(), taski))
	time.sleep(random.randint(1,5))
	
	t_end = time.time()
	print("child process {} end, execute time {}".format(os.getpid(), t_end - t_start))


if __name__ == '__main__':
	print("parent process {} begin.".format(os.getpid()))


	processes = []
	for i in process_list:
		pi = Process(target=child_task, args=("task..", ), name=i)
		pi.start()
		processes.append(pi)

	for pi in processes:
		pi.join()

	#print("parent process {}: child status p1 {}, p2 {}, p3 {}".format(os.getpid(), p1.is_alive(), p2.is_alive(), p3.is_alive()))
	#print("parent process {}: p1.name {}, p1.pid {}.".format(os.getpid(), p1.name, p1.pid))
	#print("parent process {}: p2.name {}, p2.pid {}.".format(os.getpid(), p2.name, p2.pid))
	#print("parent process {}: p3.name {}, p3.pid {}.".format(os.getpid(), p3.name, p3.pid))

	print("parent process {} end.".format(os.getpid()))


