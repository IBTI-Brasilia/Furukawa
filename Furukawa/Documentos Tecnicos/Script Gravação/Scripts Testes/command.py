import server
import uplinks
from print import printLog

def mask(value,maskNumber):
    if (value <= 0xf):
        value = maskNumber + "00" + hex(value)[2:]
    elif (value <= 0xff):
        value = maskNumber + "0" + hex(value)[2:]
    else:
        value = maskNumber + hex(value)[2:]
    return value

def change_keepAlive(time):
    timecv = int(time/30)
    payload=mask(timecv,'1')
    server.enviar(payload,1,"first")
    printLog("Configurando Keep Alive em ", time,"s.")
    server.esperar()

def change_warnPeriod(time):
    timecv = int(time/5)
    payload=mask(timecv,'2')
    server.enviar(payload,1,"first")
    printLog("Configurando Warn Period em ", time, "s.")
    server.esperar()

def change_warnTx(time):
    timecv = int(time/5)
    payload=mask(timecv,'3')
    server.enviar(payload,1,"first")
    printLog("Configurando Warn Tx em ", time, "s.")
    server.esperar()

def change_batteryLevel(value):
    valuecv = int(value/0.001)
    payload=mask(valuecv,'5')
    server.enviar(payload,1,"first")
    printLog("Configurando limite da bateria em ", value, "V.")
    server.esperar()

def change_lum(lux):
    payload=mask(lux,'8')
    server.enviar(payload,1,"first")
    printLog("Configurando limite de luminosidade em ", lux, "lux.")
    server.esperar()

def change_acc(time,ang,queda):
    angulos = [80, 70, 60, 50]
    quedas = [156, 219, 250, 312, 344, 406, 469, 500]
    x = angulos.index(ang) + 1
    y = quedas.index(queda) + 1
    z = int(time / 0.625 + 1)
    z = hex(z)[2:]
    payload = "90" + str(x) + str(y) + z
    server.enviar (payload,1,"first")
    printLog("Configurando Acelerometro: Angulo: ",ang," Queda: ",queda," Tempo: ",time, " Z: ", z, ".")
    server.esperar()

def mask_downlink(value):
    if (value <= 0xf):
        value = "000" + hex(value)[2:]
    elif (value <= 0xff):
        value = "00" + hex(value)[2:]
    elif (value <= 0xfff):
        value = "0" + hex(value)[2:]
    else:
        value = hex(value)[2:]
    return value


def multiple_downlink(ka=30, wp=25, wd=15, bl=1, lum=800, time=20, ang=50, queda=156):
    angulos = [80,70,60,50]
    quedas = [156,219,250,312,344,406,469,500]
    kacv = int(ka/30)
    wpcv = int(wp/5)
    wdcv = int(wd/5)
    blcv = int(bl/0.02)
    x = angulos.index(ang) + 1
    y = quedas.index(queda) + 1
    z=int(time/0.625 + 1)
    payload = mask_downlink(kacv) + mask_downlink(wpcv) + mask_downlink(wdcv) + hex(blcv)[2:] + \
              mask_downlink(lum) + str(x) + str(y) + hex(z)[2:].zfill(2)
    printLog("Configurando: ")
    printLog ("Keep Alive: ",ka,"s, Warn Period: ",wp,"s, Warn tx: ",wd,"s, Bateria: ",bl,"V, Luminosidade: ",lum
           ,"lux, Ângulo: ",ang," Queda: ",queda, "Tempo: ",time)
    server.enviar(payload, 10)
    server.esperar()

def see_configs():

    config_received = True
    while config_received:
        server.enviar("0077",6)
        server.esperar_2()
        if (server.rx_msg[1] == 6):
            config_received = False
        else:
            config_received = True
            printLog("Uplink não foi na Porta 6, tentando novamente")


def init_configure(ka=30, wp=25, wd=15, bl=1, lum=800, time=20, ang=50, queda=156):
    multiple_downlink(ka, wp, wd, bl, lum, time, ang, queda)
    see_configs()
    if uplinks.verifyAll(server.rx_msg[0],ka,wp,wd,bl,lum,time,ang,queda):
        printLog("Dispositivo configurado corretamente!")
        return True
    else:
        printLog("A configuração falhou.")
        uplinks.decodeAll(server.rx_msg[0])
        return False

def reiniciar():
    printLog("Reiniciando...")
    server.enviar("FFFF",1)
    server.esperar()