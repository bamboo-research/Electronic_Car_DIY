# -*- coding: utf-8 -*
import socket,threading,servo,subprocess

#keys = (0,0,0,0,0)
has_initial = threading.Event()
key_update = threading.Event()
exitthread = False
'''
forward_stragit 0; 
forward_left 1;
forward_right 2;
backward 3;
backward_left 4;
backward_right 5;
left_around 6;
right_around 7;
nothing  8;
switch 9;
'''
condition_set = {(0,0,4,0,0):0,(0,0,4,0,80):1,(0,0,4,79,0):2,(29,0,0,0,0):3,(29,0,0,0,80):4
,(29,0,0,79,0):5,(0,0,0,0,80):6,(0,0,0,79,0):7,(0,0,0,0,0):8,(0,225,0,0,0):9}

def cleanup(servoi):
    global  exitthread
    key_update.clear()
    servoi.cleanup()
    exitthread = True

class face_tracking:
    def __init__(self,servo):
        self.condition = False
        self.servo = servo

    def switch(self):
        if self.condition:
            self.ps.terminate()
        else:
            self.ps = subprocess.Popen('python MultiFace.py',shell=True)


def control_motor(servo_ins):
    mo = servo.motor(servo_ins)
    face = face_tracking(servo)
    has_initial.wait()
    keys_old = keys
    print("Initialization Successfully.")
    while(True):
        key_update.wait()
        if exitthread:
            break
        if keys_old != keys:
            #print("change")
            k = tuple(keys)
            #print(k)
            if k in condition_set.keys():
                motor_status_trigger(mo,condition_set[k], face)
            else:
                motor_status_trigger(mo,8,face)
            keys_old = keys
        key_update.clear()
    mo.cleanup()

def motor_status_trigger(mo, k, face = None):
    print(k)
    if k == 0:
        mo.forward(0.6)
    elif k == 1:
        mo.turnleft(0.5)
    elif k == 2:
        mo.turnright(0.5)
    elif k == 3:
        mo.backward(0.6)
    elif k == 4:
        mo.backleft(0.6)
    elif k ==5:
        mo.backright(0.6)
    elif k == 6:
        mo.turnl_around(0.4)
    elif k == 7:
        mo.turnr_around(0.4)
    elif k == 9:
        face.switch()
    else:
        mo.stop()

def tcplink(sock,addr):
    print( 'Accept new connection from %s:%s...' % addr)
    #sock.send(bytes('Welcome!','UTF-8'))
    global keys
    while True:
        data = sock.recv(5)
        #time.sleep(1)
        #print("len", len(data))
        if len(data)<5:
            break
        has_initial.set()
        key_update.set()
        keys = list(data)
    sock.close()
    print ('Connection from %s:%s closed.' % addr)

def main():
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.bind(('0.0.0.0',1024))
    s.listen(3)
    print("Waiting for connection...")
    #global servo_instance
    servo_instance = servo.servo()
    thread_control = threading.Thread(target=control_motor, args=(servo_instance,))
    thread_control.deamon = True
    thread_control.start()
    #thread_remote = list()

    while True:
        # isexit = input("Exit Server or Not (y/n): \n")
        # if isexit == 'y':
        #     print("Exit\n")
        #     cleanup(servo_instance)
        #     break
        sock,addr = s.accept()
        t =  threading.Thread(target=tcplink, args=(sock, addr))
        #t.deamon = True
        t.start()
        #thread_remote.append(t)
    #thread_control.join()

    # for t in thread_remote:
    #     t.join()
if __name__ == '__main__':
    main()
