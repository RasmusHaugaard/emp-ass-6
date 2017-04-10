# rtcs

Run to complete scheduler with lcd, keyboard and uart driver.

This project is based on work done by the professors, Morten Hansen and Karsten Holm Andersen at SDU.
Their original work can be found as the initial commit.

### Purpose
The purpose is to make the run to complete scheduler (rtcs) more generic and show it in context.  
The superloop is now always running. All tasks control their own timing and does not depend on being called with a certain interval.  
The system clk and systick reload value can now easily be changed without affecting driver- and application implementions.  
Tasks, Queues, Semaphores and Files are all initialized dynamically in main.c which makes it easier to see what is happening.  

Suggestions are welcome! Send a pull-requst!

### How to make it run
- Install Code Composer Studio (CCS).
- Download this project with git or [as a .zip](https://github.com/RasmusHaugaard/emp-ass-6/archive/master.zip).
	- (git instructions):
		- [Install git](https://git-scm.com/).
		- Open a terminal and go to the directory where you want the project.
		- Type in terminal: `git clone https://github.com/RasmusHaugaard/rtcs.git`.
- In CCS: File -> Open Projects from Filesystem...

### Changes from the original
- The superloop is always running and a global time is implemented in ms and us.
	- Tasks not waiting are run as frequently as possible.
	- Tasks can now use wait() with millis(ms) or micros(us) to decouple timing from the systick timer and the system clk.
	- The time from calling wait() to reentry can be a bit more or less than asked. Just as with the original.
	- Note that wait() can now 'loose' sys_ticks. The global timer can be used for precise intervals, e.q. clocks (see rtc.c).
- Dynamic initialization of tasks, queues, semaphores and files.
	- 'tmodel.h' is not needed. Tasks, queues etc. are given ids dynamically in main.c.
	- file.h and file.c does not contain application specific code.
	- Possible performance loss.
- UART hardware fifos are enabled.
