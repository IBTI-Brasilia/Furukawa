import random
import time
import serial
from gpiozero import CPUTemperature
from builtins import bytes

modem = serial.Serial("/dev/ttyACM0",baudrate = 9600, timeout = 1.0)

def randomize(last,coef,bot,up):

    
     value = min(up,max(bot,random.gauss(last,coef)))

     return value

def byterize(variable,size):

    hex_var = hex(int(variable))
    var = str((hex_var.split('x')[1]))

    while len(var) < (size*2):

        var = "0" + var
    return var

temperature = 24.95
battery = 4.05
luminosity = 150
acc_x = 0.1915
acc_y = 6.3782
acc_z = 7.6806
giro_x =  0.6180
giro_y = 0.4260
giro_z = 8.600
apperture = 0
lat = -15.710710
lon = -47.910800
while True:
    temperature = randomize(temperature,0.5,-10,55)
    battery = randomize(battery,0.005,0,battery)
    luminosity = randomize(luminosity,0.5,0,5000)
    acc_x = randomize(acc_x,0.5,-12,12)
    acc_y = randomize(acc_y,0.5,-12,12)
    acc_z = randomize(acc_z,0.5,-12,12)
    giro_x = randomize(giro_x,0.5,-12,12)
    giro_y = randomize(giro_y,0.5,-12,12)
    giro_z = randomize(giro_z,0.5,-12,12)

    temperature = ((temperature + 10)*10) - 10
    battery = battery * 10
    acc_x = (acc_x+12) * 10
    acc_y = (acc_y+12) * 10
    acc_z = (acc_z+12) * 10
    giro_x = (giro_x+12) * 10
    giro_y = (giro_y+12) * 10
    giro_z = (giro_z+12) * 10
    lat = (lat + 90)*1000000
    lon = (lon + 180)*1000000
    


    temperature = byterize(temperature,2)
    battery = byterize(battery,2)
    luminosity = byterize(luminosity,2)
    acc_x = byterize(acc_x,1)
    acc_y = byterize(acc_y,1)
    acc_z = byterize(acc_z,1)
    giro_x = byterize(giro_x,1)
    giro_y = byterize(giro_y,1)
    giro_z = byterize(giro_z,1)
    apperture = byterize(apperture,1)    
    lat = byterize(lat,4)
    lon = byterize(lon,4)

    setted_period = 300
    #print(('AT+MSGHEX="' + temperature + battery +  luminosity + acc_x + acc_y + acc_z + giro_x + giro_y + giro_z + apperture + lat + lon +'"\r\n'))
    modem.write(('AT+MSGHEX="' + temperature + battery +  luminosity + acc_x + acc_y + acc_z + giro_x + giro_y + giro_z + apperture + lat + lon +'"\r\n').encode())
    result = modem.read(100)
    print(result)

    temperature = (((int(temperature,16) + 10)/10) - 10)
    battery = (int(battery,16) / 10)
    luminosity = int(luminosity,16)
    acc_x = (int(acc_x,16)/10 - 12)
    acc_y = (int(acc_y,16)/10 - 12)
    acc_z = (int(acc_z,16)/10 - 12)
    giro_x = (int(giro_x,16)/10 - 12)
    giro_y = (int(giro_y,16)/10 - 12)
    giro_z = (int(giro_z,16)/10 - 12)
    apperture = int(apperture,16)
    lat = (int(lat,16)/1000000 - 90)
    lon = (int(lon,16)/1000000 - 180)
    

    time.sleep(setted_period)



