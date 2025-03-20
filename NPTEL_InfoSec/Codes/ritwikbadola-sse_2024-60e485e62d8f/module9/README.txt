This covert channel uses 4 cache sets for communicating 
from the sender to the receiver.

The first two set A0 and A1 are meant for communication
The second two set BO and BE would indicate if the bit
position is odd or even.

The receiver probes all 4 sets. Everytime it detects
a shift from BO to BE (or vice-versa), it assumes that
the sender is sending the next bit.

The receiver is tuned to run at-least twice as fast as
the sender. Therefore for a single bit sent, the receiver
may get multiple results (as seen in the video). We found
that the best result is the last obtained.

To execute.
$ make clean; make

Run the receiver first
$ taskset -c 4 ./receiver <4 distinct set numbers>

Then run the sender
$ taskset -c 0 ./sender <same  set numbers as mentioned in the receiver>

taskset is used to ensure that the two processes share threads
in the same hyperthreaded core. 
You can determine what value to put for taskset from /proc/cpuinfo

WARNING: No guarantees that this would work straight away
on your system. You would need to tune the programs appropriately

Tunable parameters:
* THRESHOLD            (receiver)
* CONTENTION_THRESHOLD (receiver)
* TIME_PERIOD          (sender & receiver)
* Cache Parameters  (sender & receiver) 
* EPOCH             (sender)
