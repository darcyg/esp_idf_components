import socket
import time

broadcaster = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

broadcaster.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

while True:
	print("sending packet")
	broadcaster.sendto(b'hello', ("255.255.255.255", 55667))
	time.sleep(5)
