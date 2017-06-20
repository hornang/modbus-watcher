# modbus-watcher

Parses MODBUS RTU requests and replies in a continuous stream of bytes. Packets
are found by brute force CRC checks.

Great for monitoring traffic between master and slave(s)

This code relies on Qt framework 5.x.