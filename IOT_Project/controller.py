import socket
import struct
import threading
import time
from datetime import datetime
import requests
import json

def find_value(string, substring):
    index = string.find(substring)
    
    if index != -1:
        start_index = index + len(substring)
        while start_index < len(string) and string[start_index] == ' ':
            start_index += 1

        end_index = string.find(' ', start_index)
        
        if end_index != -1:
            next_word = string[start_index:end_index]
        else:
            next_word = string[start_index:]
        
        return next_word
    
    return None
		
def list_add_or_update(lst, element, substring):  
	for i, item in enumerate(lst):
		if substring in item:
			lst[i] = element
			return

	lst.append(element)
	print("\n=========== NEW CONNECTION ===========")
	print(element)
    
def list_remove(lst, substring):
    	lst[:] = [item for item in lst if substring not in item]

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 5007
IS_ALL_GROUPS = True

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
if IS_ALL_GROUPS:
  # on this port, receives ALL multicast groups
  sock.bind(('', MCAST_PORT))
else:
  # on this port, listen ONLY to MCAST_GRP
  sock.bind((MCAST_GRP, MCAST_PORT))
mreq = struct.pack('4sl', socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)

sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

print('Controller up and listening\n')

client_list = []

def client_connections():
	while True:
		try:
			data = sock.recv(4096)
			msg = data.decode()
			if "byebye" in msg:
				host = find_value(msg, "HOST: ")
				list_remove(client_list, host)
				print("============ DISCONNECTED ============")
				print(msg)
			elif "alive" in msg:
				host = find_value(msg, "HOST: ") 
				configId = find_value(msg, "CONFIGID.UPNP.ORG: ")
				
				without_json = msg.partition('JSON: ')[0]	
				list_add_or_update(client_list, without_json, host)
				
				# separating the json from the rest of the message and writing it to a local file
				json_string = msg.split("JSON: ",1)[1]	
				json_data = json.loads(json_string)
				with open(('./' + configId + '.json'), 'w') as f:
					json.dump(json_data, f, ensure_ascii=False, indent=4)	
		except:
			pass
			
                # json file na netu
                # url = "https://jsonplaceholder.typicode.com/posts/1"
                # req_data = requests.get(url)
                # data = req_data.json()
                
def server_commands():
	while True:
		command = input("")

		if command == 'list':
			if len(client_list) == 0:
				print("List is empty!\n")
			else:
				for item in client_list:
					print(item)
		else:
			found = 0
			for i, item in enumerate(client_list):
				if command in item:
					print(item)
					found = 1

			if found == 0:
				print("There are no devices matching the request\n")
					

def server_ping():	
	while True:
		for item in client_list:
			last_time = find_value(item, "TIME: ")
			current_time = time.strftime('%H:%M:%S')
			FMT = '%H:%M:%S'
			time_difference = datetime.strptime(current_time, FMT) - datetime.strptime(last_time, FMT)
			
			if time_difference.seconds > 5:
				host = find_value(item, "HOST: ")
				list_remove(client_list, host)
				print(f"device with host {host} is inactive and removed from the list")
				
		time.sleep(2)
		
def phone_access():
	data = ""
	while True:	
		url = "https://pastebin.com/raw/8zHwvcYa"
		try:
			req_data = requests.get(url)
			# data = req_data.json()
			
			if req_data.text != data:
				print(req_data.text)
				
			data = req_data.text
		except:
			print("No internet connection!")
		
		time.sleep(2)


connection_thread = threading.Thread(target=client_connections)	
commands_thread = threading.Thread(target=server_commands)
ping_thread = threading.Thread(target=server_ping)		
phone_thread = threading.Thread(target=phone_access)

connection_thread.start()	
commands_thread.start()
ping_thread.start()
phone_thread.start()

connection_thread.join()
commands_thread.join()
ping_thread.join()
phone_thread.join()

