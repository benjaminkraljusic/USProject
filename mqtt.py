#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import matplotlib as mpl
from scipy import signal
import matplotlib.pyplot as plt
import numpy as np
from pylab import *

mpl.use('WebAgg')

def DrawStepResponse():
	sys = signal.TransferFunction(brojnik, nazivnik)
	t = np.linspace(0,25,1000)
	t,h = signal.step(sys, T=t)
	plt.figure()
	plt.title('Step odziv')
	plt.plot(t,h)
	plt.xlabel('t[s]')
	plt.ylabel('h(t)')
	plt.grid()
	plt.show()

def DrawNyquist():
	sys = signal.TransferFunction(brojnik, nazivnik)
	w, H = signal.freqresp(sys)
	plt.figure()
	plt.title('Nyquistov dijagram')
	plt.plot(H.real, H.imag, "b")
	plt.plot(H.real, -H.imag, "r")
	plt.xlabel('u('r'$\omega$)')
	plt.ylabel('v('r'$\omega$)')
	plt.grid()
	plt.show()

def DrawBode():
	sys = signal.TransferFunction(brojnik, nazivnik)
	w, mag, phase = signal.bode(sys)

	plt.figure()
	plt.title('LAFK')
	plt.semilogx(w, mag)
	plt.xlabel('log'r'$\omega$')
	plt.ylabel('A('r'$\omega$) [dB]')
	plt.grid(True)
	plt.grid(which='minor')

	plt.figure()
	plt.title('LFFK')
	plt.xlabel('log'r'$\omega$')
	plt.ylabel(' 'r'$\varphi$ ('r'$\omega$) [$^\circ$]')
	plt.semilogx(w, phase) 
	plt.grid()
	plt.grid(which='minor')
	plt.show()

	
def on_message(client, userdata, message):
    if message.topic == 'ugradbeni/brojnik':
        global brojnik
        brojnik = str(message.payload.decode("utf-8")).split(",")
        brojnik.reverse()
        brojnik = [double(numeric_string) for numeric_string in brojnik]
    if message.topic == 'ugradbeni/nazivnik':
        global nazivnik
        nazivnik = str(message.payload.decode("utf-8")).split(",")
        nazivnik.reverse()
        nazivnik = [double(numeric_string) for numeric_string in nazivnik]
    if message.topic == 'ugradbeni/brojGrafa':
        brojgrafa = str(message.payload.decode("utf-8"))
        if brojgrafa == '1':
            DrawNyquist()
        if brojgrafa == '2':
            DrawBode()
        if brojgrafa == '3':
            DrawStepResponse()
	
    
   	    
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print('Konekcija uspjesna')
    else:
        print('Konekcija nije uspjela')


client = mqtt.Client("USprojekat")
client.connect("broker.hivemq.com", 1883)
client.subscribe("ugradbeni/brojGrafa")
client.subscribe("ugradbeni/brojnik")
client.subscribe("ugradbeni/nazivnik")
client.on_message=on_message
client.on_connect=on_connect

while True:
	client.loop(0.1)

