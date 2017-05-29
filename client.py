# -*- coding: utf-8 -*
import threading,serial,time,struct,socket
connected = True
#port = '/dev/ttyAMA0'

keys = [0,0,0,0,0]


def handle_data(data,socket_id = None):
	#print(str(data),"sa")
	#global keys
	#print(ord(data[0]))
	# if ord(data[0]) != 0:
	# 	keys[0] = 1
	# else: 
	# 	keys[0] = 0
	socket_id.send(data)
	# for index,x in enumerate(data):
	# 	#keys[index] = ord(x)
	# 	keys[index] = data[index]
	# s = ""
	# for x in keys:
	# 	s = s + "|" + str(x)
	#print(s)
	# for data in [1, 2, 3]:
	#     # 发送数据:
	#     s.send(bytes(data))
	#     print (str(s.recv(1024)))
	# s.send(bytes('exit','UTF-8'))

def read_and_send():
	# 建立连接:
	port = 'COM3'
	baud = 9600
	ser = serial.Serial(port, baud)
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect(('192.168.1.41', 1024))
	while connected:
		time.sleep(0.01)
		if (ser.inWaiting()>4):
			serin = ser.read(5)
			ser.flushInput()	
			s.send(serin)
			# handle_data(serin,s)
	s.send(bytes(99))
	# s.close()
	ser.close()
	print("Exit Successfully")
	

def main():
	global connected
	thread_forward = threading.Thread(target=read_and_send)
	#thread_forward.deamon = True
	thread_forward.start()
	input("Enter to quit:")
	connected = False
	thread_forward.join()


if __name__ == '__main__':
	main()

