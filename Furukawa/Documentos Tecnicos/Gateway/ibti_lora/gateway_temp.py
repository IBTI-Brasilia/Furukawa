import serial
import time
from gpiozero import CPUTemperature
from builtins import bytes

# Conexao com o modem Rising HF
modem = serial.Serial("/dev/ttyACM0", baudrate=9600, timeout=1.0)

# Desativando o ADR 
modem.write(("AT+ADR=OFF\r\n").encode())
result = modem.read(100)
print(result)

while(1):
	# Obtem a temperatura da CPU
	cpu_temp = CPUTemperature()

	# Realiza o calculo para o decodificador do payload RisingHF
	reverse_temp = ((cpu_temp.temperature + 46.85) * 65536) / 175.62
	#print(int(reverse_temp))

	# Obtem o hexadecimal da temperatura calculada em decimal
	hex_temp = hex(int(reverse_temp))
	_, temp = str(hex_temp).split('x')
	#print(temp)

	# Completa se for menor que 2 bytes
	while len(temp) < 4:
		temp = "0" + temp
	#print(temp)

	# Coloca a temperatura no formato lido pelo decodificador (little endian -> big endian)
	final_temp = str(temp[-2]) + str(temp[-1]) + str(temp[0]) + str(temp[1])
	#print(final_temp)

	# Periodo de repeticao da mensagem
	setted_period = 120

	# Realiza o calculo para o decodificador do payload RisingHF
	reverse_period = (setted_period * 30) / 60
	#print(int(reverse_period))

	# Obtem o hexadecimal do periodo calculado em decimal
	hex_period = hex(int(reverse_period))
	_, period = str(hex_period).split('x')
	#print(period)

	# Completa se for menor que 2 bytes
	while len(period) < 4:
		period = "0" + period
	#print(period)

	# Coloca o periodo no formato lido pelo decodificador (little endian -> big endian)
	final_period = str(period[-2]) + str(period[-1]) + str(period[0]) + str(period[1])
	#print(final_period)

	# Envia a mensagem por comando AT
	modem.write(('AT+MSGHEX="' + "01" + final_temp + "00" + final_period + '"\r\n').encode())
	result = modem.read(100)
	print(result)

	# Espera 60 segundos para enviar a mensagem novamente
	time.sleep(setted_period)
