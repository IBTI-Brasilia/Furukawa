import ttn
import codecs
import time
import base64
from print import printLog

devices= ["placa-ibti-fisico","placa_antena_fio","placa_antena_porco","placa_atena_fio_a","placa_porco_2",
          "placa_prototipo","placa_sem_antena", "teste_fwk_auto", "teste_fwk_auto_2", "tests_mode"]

global wait_uplink
wait_uplink = True
global rx_msg
rx_msg = ['Payload','Porta','Airtime','Counter']
global device_id
device_id="tests_mode"

def get_dev_id():
    global device_id
    print("Qual device você deseja utilizar (1-10)?")
    for i in range(0, 10):
        string = str(i + 1) + '-' + devices[int(i)]
        print(string)

    id = int(input("Insira o Número: "))

    device_id = devices[id - 1]
    return device_id

def store(payload,port,airtime,counter):
    rx_msg.clear()
    rx_msg.append(base64.b64decode(payload).hex())
    rx_msg.append(port)
    rx_msg.append(airtime)
    rx_msg.append(counter)

def enviar(msg,port,schedule="replace"):
    b64 = codecs.encode(codecs.decode(msg, 'hex'), 'base64').decode()
    mqtt_client.send(dev_id=device_id, pay=b64, port=port, conf=False, sched=schedule)

def connect():
    app_id = "ibti-furukawa-us"
    access_key = "ttn-account-v2.lOTHnrZuE3tl9Medq1eIeQrHinmrMSscCxmEFegvpo8"
    handler = ttn.HandlerClient(app_id, access_key, discovery_address="discovery.thethings.network:1900")
    printLog(handler)
    # using mqtt client
    global mqtt_client
    mqtt_client = handler.data()
    mqtt_client.set_uplink_callback(uplink_callback)
    mqtt_client.connect()

def uplink_callback(msg, client):
    if (msg.dev_id == device_id):
        global wait_uplink
        global rx_msg
        wait_uplink = False
        #printLog("Uplink Recebido!")
        store(msg.payload_raw,msg.port,msg.metadata.airtime,msg.counter)
        #printLog(wait_uplink)
    else:
        printLog ("Uplink Recebido em outro device")


def esperar():
    global wait_uplink
    while wait_uplink:
        time.sleep(1)
    wait_uplink = True

def esperar_2():
    esperar()
    esperar()