# rtcs
Run To Complete Scheduler (rtcs) with lcd, keyboard and uart driver for the TM4C123GXL Tiva LaunchPad.

This project is based on work done by the professors, Morten Hansen and Karsten Holm Andersen at SDU.  
Their original work can be found as the initial commit.

### Purpose
The purpose is to improve the provided rtcs and its drivers. See list of changes below.  
Suggestions are welcome! Send a pull-requst!

### How to make it run
- Install Code Composer Studio (CCS).
- Download this project with git or [as a .zip](https://github.com/RasmusHaugaard/rtcs/archive/master.zip).
	- (git instructions):
		- [Install git](https://git-scm.com/).
		- Open a terminal ('git bash' on windows) and go to the directory where you want the project.
		- Type in terminal: `git clone https://github.com/RasmusHaugaard/rtcs.git`
- In CCS: File -> Open Projects from Filesystem...

### Changes from the original
- The superloop is always running.
	- Tasks can use `millis(ms)` or `micros(us)` together with `wait(ticks)`, eg `wait(millis(10))`, to decouple timing from the systick timer and the superloop.
	- Two global timers are implemented in systicks and milliseconds. These timers should be used to control intervals! See (rtc.c and intervals.c).
- LCD communication is state-driven with an image buffer
	- No for-loop waits. Task execution is faster.
	- Time requirements from datasheet are implemented.
- Dynamic initialization of tasks, queues, semaphores and files.
	- No 'tmodel.h'. Tasks, queues etc. are given ids dynamically in main.c, which relieves the programmer of updating IDs upon changes.
	- file.h and file.c does not contain application specific code since files are created elsewhere.
- UART hardware fifos are enabled for higher throughput.
