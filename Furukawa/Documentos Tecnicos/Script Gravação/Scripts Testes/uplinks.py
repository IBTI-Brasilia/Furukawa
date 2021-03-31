#Porta 6
from print import printLog

def decodeKA(payload_hex):
    keep_alive = int(payload_hex[:3], 16) * 30
    return keep_alive
def decodeWP(payload_hex):
    warn_period = int(payload_hex[3:6],16) * 5
    return warn_period
def decodeWTX(payload_hex):
    warn_tx_timeout = int(payload_hex[6:9],16) * 5
    return warn_tx_timeout
def decodeBat(payload_hex):
    bat_ths = int(payload_hex[9:12],16)/100
    return bat_ths
def decodeLum(payload_hex):
    lux_ths = int(payload_hex[15:18],16)
    return lux_ths
def decodeAccAng(payload_hex):
    acc = int(payload_hex[12:15],16)
    det_ang = acc >> 10
    if det_ang == 0:
        st_detecao_angular = 80
    elif det_ang == 1:
        st_detecao_angular = 70
    elif det_ang == 2:
        st_detecao_angular = 60
    elif det_ang == 3:
        st_detecao_angular = 50
    else:
        st_detecao_angular = 80
    return st_detecao_angular
def decodeAccQueda(payload_hex):
    acc = int(payload_hex[12:15], 16)
    queda_livre = (acc >> 7) & 0x07
    if queda_livre == 0:
        st_queda_livre = 156
    elif queda_livre == 1:
        st_queda_livre = 219
    elif queda_livre == 2:
        st_queda_livre = 250
    elif queda_livre == 3:
        st_queda_livre = 312
    elif queda_livre == 4:
        st_queda_livre = 344
    elif queda_livre == 5:
        st_queda_livre = 406
    elif queda_livre == 6:
        st_queda_livre = 469
    elif queda_livre == 7:
        st_queda_livre = 500
    else:
        st_queda_livre = 156
    return st_queda_livre
def decodeAccTime(payload_hex):
    acc = int(payload_hex[12:15], 16)
    dur_time = acc & 0x3F
    dur_time = dur_time * 0.625
    return dur_time

def decodeAll(payload_hex):
    printLog("Keep Alive: ", decodeKA(payload_hex))
    printLog("Warn period: ", decodeWP(payload_hex))
    printLog("Warn tx: ",decodeWTX(payload_hex))
    printLog("Battery: ",decodeBat(payload_hex))
    printLog("Luminosity: ",decodeLum(payload_hex))
    printLog ("Tempo de Duração: ", decodeAccTime(payload_hex))
    printLog("Angulo: ",decodeAccAng(payload_hex), '°')
    printLog("Queda Livre: ",decodeAccQueda(payload_hex),"mg")

def verifyAll(payload_hex , ka, wp, wtx, bat, lum, time, ang, queda):
    if verifyKA(payload_hex,ka) & verifyWP(payload_hex,wp) & verifyWTX(payload_hex,wtx) & \
    verifyBat(payload_hex,bat) & verifyLum(payload_hex,lum) & verifyAccTime(payload_hex,time) & \
    verifyAccAng(payload_hex,ang) & verifyAccQueda(payload_hex,queda):
        return True
    else:
        return False

def verifyKA(payload_hex,ka):
    return ka == decodeKA(payload_hex)
def verifyWP(payload_hex,wp):
   return wp == decodeWP(payload_hex)
def verifyWTX(payload_hex,wtx):
    return wtx==decodeWTX(payload_hex)
def verifyBat(payload_hex,bat):
    return bat==decodeBat(payload_hex)
def verifyLum(payload_hex,lum):
    return lum==decodeLum(payload_hex)
def verifyAccTime(payload_hex,time):
    return time==decodeAccTime(payload_hex)
def verifyAccAng(payload_hex,ang):
    return ang==decodeAccAng(payload_hex)
def verifyAccQueda(payload_hex,queda):
    return queda==decodeAccQueda(payload_hex)

#Porta 1

def decodePort1(payload_hex):
    bat=int(payload_hex[:4],16)*3*1.22/4096 + 0.01
    printLog("Bateria: ", bat)
    temp=int(payload_hex[4:8],16)
    if (temp > 0x8000):
        temp = (temp - 65536)/256 + 25
    else:
        temp = temp/256 + 25
    flags = int(payload_hex[8:10],16)
    printLog("Temperatura: ", temp)
    printLog("Flags: ",bin(flags))

def verifyFlags(payload_hex,lum, mov, bat):
    flags = int(payload_hex[8:10],16) >> 5
    if (flags == lum*4 + mov*2 + bat*1):
        return True
    else:
        return False

def verifyLumFlags(payload_hex):
    flags = int(payload_hex[8:10],16) >> 5
    flags = (flags & 0x4) >> 2

    if (flags):
        return True
    else:
        return False

def verifyBatFlags(payload_hex):
    flags = int(payload_hex[8:10],16) >> 5
    flags = (flags & 0x1)

    if (flags):
        return True
    else:
        return False

def verifyMovFlags(payload_hex):
    flags = int(payload_hex[8:10],16) >> 5
    flags = (flags & 0x2) >> 1
    if (flags):
        return True
    else:
        return False


#Porta 3

def decodePort3(payload_hex):
    bat = int(payload_hex[:4], 16) * 3 * 1.22 / 4096 + 0.01
    printLog("Bateria: ", bat)
    temp = int(payload_hex[4:6], 16)
    if (temp > 0x80):
        temp = (temp - 256) + 25
    else:
        temp = temp + 25
    printLog("Temperatura: ", temp,"°C")
    lum = int(payload_hex[6:8],16)
    printLog ("Luminosidade: ",lum,"lux")
    x = int(payload_hex[8:12],16)
    y = int(payload_hex[12:16],16)
    z = int(payload_hex[16:20],16)
    if (x> 0x8000):
        x = (x-65536)*0.00061
    else:
        x = x*0.00061
    printLog("X: ",x,"m/s²")
    if (y> 0x8000):
        y = (y-65536)*0.00061
    else:
        y = y*0.00061
    printLog("Y: ",y,"m/s²")
    if (z> 0x8000):
        z = (z-65536)*0.00061
    else:
        z = z*0.00061
    printLog("Y: ",z,"m/s²")

def verify_Sensors(payload_hex):
    x = int(payload_hex[8:12], 16)
    y = int(payload_hex[12:16], 16)
    z = int(payload_hex[16:20], 16)
    if (x > 0x8000):
        x = (x - 65536) * 0.00061
    else:
        x = x * 0.00061
   # printLog("X: ", x, "m/s²")
    if (y > 0x8000):
        y = (y - 65536) * 0.00061
    else:
        y = y * 0.00061
   # printLog("Y: ", y, "m/s²")
    if (z > 0x8000):
        z = (z - 65536) * 0.00061
    else:
        z = z * 0.00061

    acc = (x**2 + y**2 + z**2)**0.5
    #printLog(acc)
    if abs(acc - 10) > 1.5:
        printLog("Sensores em mau funcionamento!")
        return False
    else:
        return True


def verify_time_interval (time_measure, time_correct, margem ):
    return ((time_measure > (time_correct - margem )) and (time_measure < (time_correct + margem ) ))