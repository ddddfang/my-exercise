import threading
import queue
import time
import os
import random

# task_list is indepent if we use multiprocess!
# we should use multithread!
task_list = [
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i'
]

threads_list = [
	'thread-001',
	'thread-002',
	'thread-003',
]

class myThread(threading.Thread):

	def __init__(self, name, wq):
		threading.Thread.__init__(self)
		self.name = name
		self.wq = wq

	def run(self):
		while True:
			try:
				taski = self.wq.get(timeout=2)
				t_start = time.time()

				func(self.name, taski)
				
				t_end = time.time()
			except Exception as e:
				print("catch except {}.".format(e))
				break

def func(threadname, taskname):	
	print("{} executing task {}...".format(threadname, taskname))
	time.sleep(random.randint(1,5))

if __name__ == '__main__':
	print("parent process {} begin.".format(os.getpid()))

	workQueue = queue.Queue(1000)	#max caps is 1000
	for i in task_list:
		workQueue.put(i)

	threads = []
	for i in threads_list:
		ti = myThread(name=i, wq=workQueue)
		ti.start()
		threads.append(ti)

	for ti in threads:
		ti.join()

	print("parent process {} end.".format(os.getpid()))



