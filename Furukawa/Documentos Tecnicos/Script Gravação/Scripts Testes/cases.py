import uplinks
import command
import server
import time
from print import printLog

def time_measure():
    inicio = time.time()
    server.esperar()
    fim = time.time()
    return fim-inicio

def verify_Port(port):
    if server.rx_msg[1] != port:
        printLog ("Mensagem recebida fora da porta ",port)
        return False
    else:
        return True

def verify_warn_tx(wtx):

    for i in range(3):
        server.esperar()
        if not(verify_Port(2)):
            return False
        tempo = time_measure()
        if not(verify_Port(2)):
            return False
        printLog("Tempo entre os pacotes no modo 2: ", tempo)
        if abs(tempo - wtx) > 5:
            printLog("WarnTx está diferente do configurado em mais do que 5s")
            return False
    return True

def verify_warn_period(wp):

    mudou = False
    fim = 0

    while not(mudou):

        server.enviar("0077", 6, "last")
        command.change_lum(800)
        init = time.time()
        server.esperar_2()
        if verify_Port(6):
            mudou = uplinks.verifyLum(server.rx_msg[0], 800)

    server.esperar()
    while server.rx_msg[1] == 2:
        fim = time.time()
        server.esperar()

    if not verify_Port(2) and fim == 0:
        return False

    if abs(fim - init - wp) > 11:
        printLog("Warn Period diferente de", wp, "s! Tempo medido: ",fim - init)
        return False
    printLog("Warn period medido: ", fim - init)
    return True

def verify_keep_alive(ka):
    server.esperar()
    if verify_Port(1):
        tempo = time_measure()
        if not verify_Port(1):
            return False
    else:
        return False
    return ka - tempo < 5


def stop_interruption(ka=30,wp=25,wd=15,bl=1.0,lum=800,time=20,ang=50,queda=156):
    while not(command.init_configure(ka, wp, wd, bl, lum, time, ang, queda)):
        printLog("Tentando novamente...")

    while server.rx_msg[1] == 2:
        server.esperar()
    for i in range(2):
        server.esperar()
        if server.rx_msg[1]!=1:
            printLog("Dispositivo ainda está enviando dados na Porta 2")
            return False

    if not(uplinks.verifyFlags(server.rx_msg[0], False, False, False)):
        printLog("Flags de interrupção ainda estão setadas")
        return False

    return True

def init_interruption():

    command.change_lum(10)
    server.esperar()
    while not(verify_Port(2)):
        command.change_lum(10)

#Testes 0 a 4

def t0():
    while not(command.init_configure(30, 25, 15, 1.0, 800, 20, 50, 156)):
        printLog ("Tentando novamente.")
    printLog("Reiniciando...")
    command.reiniciar()
    server.esperar()
    if not(verify_Port(1)):
        return False
    tempo=time_measure()
    if not(verify_Port(1)):
        return False
    printLog("Tempo entre os pacotes no modo 1: ",tempo)
    if tempo - 30 > 5:
        printLog("KeepAlive está diferente do configurado em mais do que 5s")
        return False
    elif not(uplinks.verifyFlags(server.rx_msg[0],False,False,False)):
        printLog ("Dispositivo iniciou em modo de interrupção!")
        return False
    else:
        return True

def t1():
    server.enviar("0077", 6)
    server.esperar()
    server.esperar()
    if verify_Port(6):
        return True
    else:
        return False

def t2():
    server.enviar("0011",3)
    server.esperar_2()
    if not(verify_Port(3)):
        return False
    if uplinks.verify_Sensors(server.rx_msg[0]):
        return True
    else:
        uplinks.decodePort3(server.rx_msg[0])
        return False

def t3():
    server.enviar("0033",5)
    server.esperar_2()
    if verify_Port(5):
        return True
    else:
        return False

def t4():
    command.see_configs()
    ka = uplinks.decodeKA(server.rx_msg[0])
    server.esperar()
    if not(verify_Port(1)):
        return False
    tempo = time_measure()
    if not(verify_Port(1)):
        return False
    printLog("Tempo entre os pacotes no modo 1: ", tempo)
    if abs(tempo - ka) > 5:
        printLog("KeepAlive está diferente do configurado em mais do que 5s")
        return False
    elif not(uplinks.verifyFlags(server.rx_msg[0],False,False,False)):
        printLog ("Dispositivo está em modo de interrupção!")
        return False
    else:
        return True

def t5():
    command.change_batteryLevel(4)
    for i in range(3):
        server.esperar()
        if not(verify_Port(1)):
            printLog("Falhou na Tentativa ", i)
            return False
        time = time_measure()
        if not(verify_Port(2)):
            printLog("Falhou na Tentativa ", i)
            return False
        if abs(time - 15) > 3:
            printLog("O Tempo entre o Keep Alive e o Alerta na Porta 2 difere de 15s")
            printLog("Tempo medido: ",time,"s")
            printLog("Falhou na Tentativa ",i)
            return False
        if int(server.rx_msg[0][:2],16) != 0x20:
            printLog("Flags não correspondem ao esperado!\nEsperado: 00100000")
            printLog ("Recebido: ",bin(int(server.rx_msg[0][:2],16)))
            printLog("Falhou na Tentativa ", i)
            return False
    finish = False
    while not(finish):
        command.change_batteryLevel(3)
        command.see_configs()
        finish = uplinks.verifyBat(server.rx_msg[0],3)
        if not(finish):
            printLog("Reconfiguração falhou!")
    return True

def t6():

    while not(command.init_configure(120, 60, 10, 1, 10, 20, 50, 156)):
        printLog("Tentando novamente...")

    tx_result = verify_warn_tx(10)

    if not(tx_result):
        return False

    period_result = verify_warn_period(60)

    printLog("Reconfigurando o dispositivo")

    if not(stop_interruption()):
        return False

    return tx_result and period_result

def t7():
    while not(command.init_configure(120, 60, 10, 1, 10, 20, 50, 156)):
        printLog("Tentando novamente...")
    while server.rx_msg[1] == 1:
        server.esperar()
    server.enviar("0000",1)
    server.esperar_2()
    if verify_Port(1):
            printLog("Dispositivo Enviou Mensagem na Pora 1")
            return True
    return False

def t8():
    # start function here
    # forçar luminosidade para valores baixo
    command.change_lum(1)
    # forçar reset da placa
    command.reiniciar()
    # esperar apos reinicio
    server.esperar()

    for i in range(2):
        server.esperar()
        if server.rx_msg[1] != 2:
            printLog("Dispositivo ainda está enviando dados na Porta 1 \n",
             " deveria estar em alerta - porta 2")
            command.change_lum(700)
            return False

    command.change_lum(700)
    return True
    

def t9():
    # start function here
        # forçar bateria para um valor alto
    command.change_batteryLevel(4)
    # forçar reset da placa
    command.reiniciar()
    # esperar apos reinicio
    server.esperar()
    while not verify_Port(1):
        server.esperar()

    if server.rx_msg[3] == 0:
        #uplinks.decodePort1(server.rx_msg[0])
        bat_f = uplinks.verifyFlags(server.rx_msg[0], False,False,True)
        if not(bat_f):
            printLog("Erro! Flag de Bateria não está ativada.")
            return False
        # esperar pelo proxima flag seria bom verificar o tempo [15 +- 3]
        timedif = time_measure()
        if verify_Port(2):
            if uplinks.verify_time_interval(timedif, 15, 3):
                command.change_batteryLevel(1)
                return True
            else:
                return False
        else:
            return False

    else:
        printLog("Dispositivo não reiniciou!")
        return False


def t10():
    # start function here

    command.change_keepAlive(90)

    server.esperar()

    if verify_Port(1):
        dif = time_measure()
        command.see_configs()
        b_ret = uplinks.verifyKA(server.rx_msg[0], 90)
        if uplinks.verify_time_interval(dif, 90, 3) and b_ret :
            return True
        else:
            printLog("As Configurações não foram obedecidas, Keep Alive diferente")
            uplinks.decodeAll(server.rx_msg[0])
            return False
    else:
        return False

def t11():
    command.multiple_downlink(ka=60)
    server.esperar()
    if verify_Port(1):
        dif = time_measure()
        command.see_configs()
        b_ret = uplinks.verifyKA(server.rx_msg[0], 60)
        if uplinks.verify_time_interval(dif, 60, 3) and b_ret:
            return True
        else:
            printLog("As Configurações não foram obedecidas, Keep Alive diferente")
            uplinks.decodeAll(server.rx_msg[0])
            return False
    else:
        return False

def t12():
    # start function here
    command.init_configure(ka=120)
    command.change_warnPeriod(90)
    init_interruption()
    return verify_warn_period(90)

def t13():
    # start function here
    command.multiple_downlink(ka=60, wp=45)
    init_interruption()
    return verify_warn_period(45)


def t14():
    # start function here
    command.change_warnTx(20)
    init_interruption()
    return verify_warn_tx(20)

def t15():
    # start function her
    command.multiple_downlink(wd=15)
    init_interruption()
    return verify_warn_tx(15)

def t16():
    # start function here
    command.change_batteryLevel(1)
    command.see_configs()
    return uplinks.verifyBat(server.rx_msg[0], 1)

def t17():
    # start function here
    command.multiple_downlink(bl=3.1)
    command.see_configs()
    return uplinks.verifyBat(server.rx_msg[0], 3.1)


def t18():
    # start function here
    command.change_lum(500)
    command.see_configs()
    return uplinks.verifyLum(server.rx_msg[0], 500)

def t19():
    # start function here
    command.multiple_downlink(lum=700)
    command.see_configs()
    return uplinks.verifyLum(server.rx_msg[0], 700)

def t20():
    # start function here
    command.change_acc(25, 60, 406)
    command.see_configs()
    return uplinks.verifyAccAng(server.rx_msg[0], 60) and \
           uplinks.verifyAccTime(server.rx_msg[0],25) and uplinks.verifyAccQueda(server.rx_msg[0], 406)
def t21():
    # start function here
    command.multiple_downlink(time=15, ang=80, queda=500)
    command.see_configs()
    return uplinks.verifyAccAng(server.rx_msg[0], 80) and \
           uplinks.verifyAccTime(server.rx_msg[0], 15) and uplinks.verifyAccQueda(server.rx_msg[0], 500)

def t22():
    # start function here
    init_interruption()
    server.enviar("0077", 6)
    server.esperar_2()
    result = verify_Port(6)
    stop_interruption()
    return result

def t23():
    # start function here
    while not(verify_Port(2)):
        command.change_lum(10)

    result = t2()

    stop_interruption()

    return result

def t24():
    # start function here
    while not(verify_Port(2)):
        command.change_lum(10)

    result = t3()

    stop_interruption()

    return result

def t25():
    # start function here
    init_interruption()

    command.multiple_downlink(ka=150,bl=4,lum=10)

    mudou = False
    while not mudou:
        command.change_lum(800)
        command.see_configs()
        mudou = uplinks.verifyLum(server.rx_msg[0], 800)

    server.esperar()

    while not (verify_Port(1)):
        server.esperar()

    result = uplinks.verifyFlags(server.rx_msg[0],False,False,True)

    time = time_measure()

    if not (verify_Port(2)):
        return False

    if abs(time - 15) > 3:
        printLog("O Tempo entre o Keep Alive e o Alerta na Porta 2 difere de 15s")
        printLog("Tempo medido: ", time, "s")
        return False

    stop_interruption()

    return result

def t26():
    # start function here
    while not(verify_Port(2)):
        command.change_lum(10)
    server.esperar()
    command.change_keepAlive(60)
    while verify_Port(2):
        command.change_lum(800)
    command.see_configs()
    return uplinks.verifyKA(server.rx_msg[0],60)


def t27():
    # start function here
    init_interruption()
    stop_interruption(ka=30)
    return verify_keep_alive(30)


def t28():
    # start function here
    command.change_keepAlive(120)
    command.reiniciar()
    command.see_configs()
    return uplinks.verifyKA(server.rx_msg[0],120)

def t29():
    # start function here
    init_interruption()
    command.change_warnPeriod(75)
    return verify_warn_period(75)

def t30():
    # start function here
    init_interruption()
    command.multiple_downlink(ka=240,wp=90,lum=10)
    return verify_warn_period(90)
def t31():
    # start function here
    command.change_warnPeriod(45)
    command.reiniciar()
    command.see_configs()
    return uplinks.verifyWP(server.rx_msg[0],45)

def t32():
    # start function here
    init_interruption()
    command.change_warnTx(20)
    result = verify_warn_tx(20)
    stop_interruption()
    return result

def t33():
    # start function here
    init_interruption()
    command.multiple_downlink(ka=150,wp=140,wd=20)
    result = verify_warn_tx(20)
    stop_interruption()
    return result

def t34():
    # start function here
    command.change_warnTx(10)
    command.reiniciar()
    command.see_configs()
    return uplinks.verifyWTX(server.rx_msg[0],10)

def t35():
    # start function here

    init_interruption()

    command.change_batteryLevel(4)

    mudou = False
    while not mudou:
        command.change_lum(800)
        command.see_configs()
        mudou = uplinks.verifyLum(server.rx_msg[0], 800)

    while not (verify_Port(1)):
        server.esperar()

    result = uplinks.verifyFlags(server.rx_msg[0], False, False, True)

    stop_interruption()

    return result


def t36():
    # start function here
    command.change_batteryLevel(1.25)
    command.reiniciar()
    command.see_configs()
    return uplinks.verifyBat(server.rx_msg[0], 1.25)


def t37():
    # start function here
    init_interruption()
    command.change_lum(2)
    command.see_configs()
    result = uplinks.verifyLum(server.rx_msg[0], 2)
    stop_interruption()
    return result

def t38():
    # start function here
    init_interruption()
    command.multiple_downlink(lum=2)
    command.see_configs()
    result = uplinks.verifyLum(server.rx_msg[0], 2)
    stop_interruption()
    return result

def t39():
    # start function here
    command.change_lum(685)
    command.reiniciar()
    command.see_configs()
    return uplinks.verifyLum(server.rx_msg[0],685)


def t40():
    # start function here
    init_interruption()
    result = t20()
    stop_interruption()
    return result

def t41():
    # start function here
    init_interruption()
    result = t21()
    stop_interruption()
    return result

def t42():
    # start function here
    command.change_acc(10, 70, 312)
    command.reiniciar()
    command.see_configs()
    return uplinks.verifyAccAng(server.rx_msg[0], 70) and \
    uplinks.verifyAccTime(server.rx_msg[0], 10) and uplinks.verifyAccQueda(server.rx_msg[0], 312)


def t43(ka_change=30):
    # start function here
    command.init_configure(ka=150,wp=120,wd=30)
    command.change_keepAlive(ka_change)
    server.esperar()
    if not verify_Port(9):
        return False
    command.see_configs()
    return uplinks.verifyKA(server.rx_msg[0],150)


def t44():
    # start function here
    return t43(ka_change=90)

def t45():
    # start function here
    command.init_configure(ka=180,wp=120,wd=30)
    command.change_keepAlive(150)
    command.see_configs()
    return uplinks.verifyKA(server.rx_msg[0],150)

def t46():
    # start function here
    command.init_configure(ka=240,wp=120,wd=30)
    command.change_warnPeriod(270)
    server.enviar("0077",6)
    server.esperar()
    if not verify_Port(9):
        return False
    server.esperar()
    while not verify_Port(6):
        server.enviar("0077",6)
        server.esperar()
    return uplinks.verifyWP(server.rx_msg[0],120)

def t47(wp_change=95):
    # start function here
    command.change_warnPeriod(wp_change)
    server.enviar("0077", 6)
    server.esperar_2()
    while not verify_Port(6):
        server.enviar("0077", 6)
        server.esperar()
    return uplinks.verifyWP(server.rx_msg[0],wp_change)


def t48():
    # start function here
    return t47(50)

def t49():
    # start function here
    command.change_warnPeriod(5)
    server.enviar("0077", 6)
    server.esperar()
    if not verify_Port(9):
        return False
    server.esperar()
    while not verify_Port(6):
        server.enviar("0077", 6)
        server.esperar()
    return uplinks.verifyWP(server.rx_msg[0], 50)

def t50(wtx_change=220):
    # start function here
    command.change_warnTx(wtx_change)
    server.enviar("0077", 6)
    server.esperar()
    if not verify_Port(9):
        return False
    server.esperar()
    while not verify_Port(6):
        server.enviar("0077", 6)
        server.esperar()
    return uplinks.verifyWTX(server.rx_msg[0], 30)


def t51():
    # start function here
    return t50(50)

def t52():
    # start function here
    command.change_warnTx(5)
    server.enviar("0077", 6)
    server.esperar()
    while not verify_Port(6):
        server.enviar("0077", 6)
        server.esperar()
    return uplinks.verifyWTX(server.rx_msg[0], 5)

def t53(ka_change=30,wp_change=50,wd_change=5):
    # start function here
    command.multiple_downlink(ka_change,wp_change,wd_change)
    server.esperar()
    if verify_Port(9):
        return True
    return False


def t54(ka_change=90,wp_change=50,wd_change=45):
    # start function here
    command.multiple_downlink(ka_change,wp_change,wd_change)
    server.enviar("0077", 6)
    server.esperar()
    while not verify_Port(6):
        server.enviar("0077", 6)
        server.esperar()
    return uplinks.verifyWTX(server.rx_msg[0], 45) and uplinks.verifyWP(server.rx_msg[0],50) and \
            uplinks.verifyKA(server.rx_msg[0],90)

def t55():
    # start function here
    return t53(150,185,15)

def t56():
    # start function here
    return t53(150, 95, 100)

def t57():
    # start function here
    return t53(150, 95, 185)

def t58():
    # start function here
    return t53(60, 95, 185)

def t59():
    # start function here
    command.multiple_downlink(3600, 120, 20,2,10)
    server.enviar("0077", 6)
    server.esperar_2()
    while not verify_Port(6):
        server.enviar("0077", 6)
        server.esperar()
    result = uplinks.verifyWTX(server.rx_msg[0], 20) and uplinks.verifyWP(server.rx_msg[0], 120) and \
           uplinks.verifyKA(server.rx_msg[0],3600)
    stop_interruption(240,120,20)
    return result


def t60():
    # start function here
    init_interruption()
    result = t53(150,185,90)
    stop_interruption()
    return result

def t61():
    # start function here
    command.init_configure()
    command.multiple_downlink(390,95,25,3.2,526,38.125,50,469)
    command.see_configs()
    return uplinks.verifyAll(server.rx_msg[0],390,95,25,3.2,526,38.125,50,469)

def t62():
    # start function here
    command.multiple_downlink(180, 55, 10, 3.1, 700, 38.125, 50, 469)
    command.see_configs()
    return uplinks.verifyAll(server.rx_msg[0], 180, 55, 10, 3.1, 700, 38.125, 50, 469)

def t63():
    # start function here
    command.init_configure()
    airtime = 0
    for i in range(0,7):
        server.esperar()
        airtime += server.rx_msg[2]/1000000
        if server.rx_msg[2]/1000000 > 400:
            printLog("Pacote com Airtime maior do que 400 ms. Airtime medido na camada Física: ",server.rx_msg[2], " ms")
            return False
    printLog("Nenhum pacote com Airtime acima do limite. Airtime Médio = ",airtime/7, "s")
    return True

def t64():
    # start function here
    server.esperar_2()
    command.reiniciar()
    server.esperar()
    if server.rx_msg[3] != 0:
        printLog("A Placa não reiniciou")
        return False
    return True
