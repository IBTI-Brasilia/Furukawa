from subprocess import call
import os

os.chdir("/home/pi")
call(["sudo","./ibti_lora/lora_gateway/reset_lgw.sh", "start"])
os.chdir("ibti_lora/packet_forwarder/lora_pkt_fwd")
call(["sudo","./lora_pkt_fwd"])
