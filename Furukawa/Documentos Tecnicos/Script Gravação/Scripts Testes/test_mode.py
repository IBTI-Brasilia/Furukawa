import server
import cases
import datetime
import os
from print import printLog

dev_id = server.get_dev_id()
os.system("cls")


outfile = open("Results.txt","w")
now = datetime.datetime.now()
init = "Certificação Realizada no dia " + now.strftime("%d-%m-%y %H:%M:%S") + "\n" + "Dev_id utilizado: " + dev_id + "\n"
printLog(init)
outfile.write(init)
outfile.close()

log = open('Log.txt','w')
log.close()

test = int(input("Qual Teste você deseja realizar? (0 a 64) "))
test2 = int(input("Até qual teste? "))
printLog("Testes ", test, "ao ", test2)

server.connect()
print("Conectado! Ligue o Dispositivo")

for i in range(test, test2+1):
    outfile = open("Results.txt", "a")
    teststr = 't' + str(i)
    printLog("Teste ", i, ":\n")
    result = getattr(cases, teststr)()
    string = "Teste " + str(i) + ": " + str(result) + "\n"
    printLog(string)
    outfile.write(string)
    outfile.close()
outfile.close()
log.close()