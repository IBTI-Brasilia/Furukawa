import server
import os
import command
import time
import cases

dev_id=server.get_dev_id()
os.system("cls")
server.connect()

print("Comandos Manuais -- Dev ID: ",dev_id,"\n")

while True:
    option = int(input("Escolha uma opção:\n0-Enviar 0077\n1-Enviar 0033\n2-Enviar 0011\n3-Mudar Keep Alive\n"
          "4-Mudar Warn Tx\n5-Mudar Warn Period\n6-Mudar Limiar da Bateria\n7-Mudar Limiar de Luminosidade\n"
          "8-Mudar Configurações do Acelerômetro\n9-Fazer um Downlink Múltiplo\n10- Resetar a Placa\n"
                       "11- Sair da Interrupção\n\nOpção:"))

    print("\n")

    if option == 0:
        server.enviar("0077",6)
        print("Downlink 0077 agendado! Aguardando uplink...")
        server.esperar()
    elif option == 1:
        server.enviar("0033", 5)
        print("Downlink 0033 agendado! Aguardando uplink...")
        server.esperar()
    elif option == 2:
        server.enviar("0011", 3)
        print("Downlink 0011 agendado! Aguardando uplink...")
        server.esperar()
    elif option == 3:
        value = int(input("Insira o novo Keep Alive em segundos: "))
        command.change_keepAlive(value)
    elif option == 4:
        value = int(input("Insira o novo Warn Tx em segundos: "))
        command.change_warnTx(value)
    elif option == 5:
        value = int(input("Insira o novo Warn Period em segundos: "))
        command.change_warnPeriod(value)
    elif option == 6:
        value = float(input("Insira o novo Limiar de Bateria: "))
        command.change_batteryLevel(value)
    elif option == 7:
        value = int(input("Insira o novo Limiar de Luz: "))
        command.change_lum(value)
    elif option == 8:
        time = float(input("Insira o novo Tempo em segundos: "))
        queda = int(input("Insira o novo Limiar de queda (em mg): "))
        ang = int(input("Insira o novo Limiar de inclinação (em graus): "))
        command.change_acc(time,ang,queda)
    elif option == 9:
        ka = int(input("Insira o novo Keep Alive em segundos: "))
        wtx = int(input("Insira o novo Warn Tx em segundos: "))
        wp = int(input("Insira o novo Warn Period em segundos: "))
        bl = float(input("Insira o novo Limiar de Bateria: "))
        lum = int(input("Insira o novo Limiar de Luz: "))
        time = float(input("Insira o novo Tempo em segundos: "))
        queda = int(input("Insira o novo Limiar de queda (em mg): "))
        ang = int(input("Insira o novo Limiar de inclinação (em graus): "))
        command.multiple_downlink(ka,wp,wtx,bl,lum,time,ang,queda)
    elif option == 10:
        command.reiniciar()
    elif option == 11:
        cases.stop_interruption()
    else:
        print("Comando fora das opções dadas")
    print("Downlink Enviado!")
    time.sleep(2)
    os.system("cls")
