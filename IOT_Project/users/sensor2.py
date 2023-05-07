import socket
import time
import datetime
from pathlib import Path
import atexit
import sys
import json

configId = "sensor2"

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 5007
MULTICAST_TTL = 2

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)

location = str(Path().absolute()) + "/" +str(configId) + ".json"

host_msg = "HOST: 192.168.1.1 \n"
location_msg = "LOCATION: file://" + location + "\n"
cinfigid_msg = "CONFIGID.UPNP.ORG: " + configId + " \n"	
alive_msg = "NTS: ssdp:alive \n"
byebye_msg = "NTS: ssdp:byebye \n"

msg_2 = host_msg + location_msg + cinfigid_msg + byebye_msg

def exit_handler():
    sock.sendto(msg_2.encode(), (MCAST_GRP, MCAST_PORT))

atexit.register(exit_handler)

while True:
	try:
		time_msg = "TIME: " + time.strftime('%H:%M:%S') + " \n"
		msg_1 = host_msg + location_msg + cinfigid_msg + time_msg + alive_msg
		
		with open(location, 'r') as f:
			data = json.load(f)
		
		json_str = json.dumps(data)
	
		msg_1 += "JSON: " + json_str
		
		sock.sendto(msg_1.encode(), (MCAST_GRP, MCAST_PORT))
			
		time.sleep(2)
	except:
		print("\nDisconnected")
		sys.exit(0)

