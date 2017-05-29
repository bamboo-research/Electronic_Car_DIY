from picamera.array import PiRGBArray
from picamera import PiCamera
from functools import partial

import multiprocessing as mp
import cv2
import os
import time

from cradle import servo
resX = 320
resY = 240
cx = resX / 2
cy = resY / 2


# Setup the camera
camera = PiCamera()
camera.resolution = ( resX, resY )
camera.framerate = 60

# Use this as our output
rawCapture = PiRGBArray( camera, size=( resX, resY ) )

# The face cascade file to be used
face_cascade = cv2.CascadeClassifier('/home/pi/opencv/data/lbpcascades/lbpcascade_frontalface.xml')

t_start = time.time()
fps = 0

s = servo()
s.init_position()

def get_faces( img ):
    gray = cv2.cvtColor( img, cv2.COLOR_BGR2GRAY )
    faces = face_cascade.detectMultiScale( gray )
    return img, faces

def draw_frame( img, faces ):
    global xdeg
    global ydeg
    global fps
    global time_t

    # Draw a rectangle around every face
    for ( x, y, w, h ) in faces:
        cv2.rectangle( img, ( x, y ),( x + w, y + h ), ( 200, 255, 0 ), 2 )
        control(x, y, w, h)

    # Calculate and show the FPS
    fps = fps + 1
    sfps = fps / (time.time() - t_start)
    print(sfps)
    #cv2.putText(img, "FPS : " + str( int( sfps ) ), ( 10, 10 ), cv2.FONT_HERSHEY_SIMPLEX, 0.5, ( 0, 0, 255 ), 2 )

    #cv2.imshow( "Frame", img )
    cv2.waitKey( 1 )

def control(x, y, w, h):
    tx = x + w/2
    ty = y + h/2

    if   ( cx - tx > 0):
        xdeg = (cx - tx) // 50
        s.turnleft(abs(xdeg))
        #print("left")
    elif ( cx - tx < 0):
        xdeg = (cx - tx) // 50
        s.turnright(abs(xdeg))
        #print("right")

    if   ( cy - ty < 0):
        ydeg = (cy - ty) // 50
        s.turndown(abs(ydeg))
        #print("down")
    elif ( cy - ty > 0):
        ydeg = (cy - ty) // 50
        s.turnup(abs(ydeg))
        #print("up")


def main():
    pool = mp.Pool( processes=4 )
    fcount = 0

    camera.capture( rawCapture, format="bgr" )
    r1 = pool.apply_async( get_faces, [ rawCapture.array ] )
    r2 = pool.apply_async( get_faces, [ rawCapture.array ] )
    r3 = pool.apply_async( get_faces, [ rawCapture.array ] )
    r4 = pool.apply_async( get_faces, [ rawCapture.array ] )

    i1, f1 = r1.get()
    i2, f2 = r2.get()
    i3, f3 = r3.get()
    i4, f4 = r4.get()

    rawCapture.truncate( 0 )

    for frame in camera.capture_continuous( rawCapture, format="bgr", use_video_port=True ):
        image = frame.array

        if   fcount == 1:
            r1 = pool.apply_async( get_faces, [ image ] )
            i2, f2 = r2.get()
            draw_frame( i2, f2 )

        elif fcount == 2:
            r2 = pool.apply_async( get_faces, [ image ] )
            i3, f3 = r3.get()
            draw_frame( i3, f3 )

        elif fcount == 3:
            r3 = pool.apply_async( get_faces, [ image ] )
            i4, f4 = r4.get()
            draw_frame( i4, f4 )

        elif fcount == 4:
            r4 = pool.apply_async( get_faces, [ image ] )
            i1, f1 = r1.get()
            draw_frame( i1, f1 )

            fcount = 0

        fcount += 1

        rawCapture.truncate( 0 )

if __name__ == '__main__':
    main()
