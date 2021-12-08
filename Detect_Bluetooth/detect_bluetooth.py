import paho.mqtt.client as mqtt
import os
import re



def get_bluetooth(rssi_threshold):
    os.system('cd /home/pi/Downloads/barnowl-hci-master; npm start 2>&1| tee devices.txt & sleep 5; kill $!')
    f = open('devices.txt', 'r')
    lines = f.readLines()
    
    devices = {}
    for line in lines:
        if "transmitterId:" in line:
            transmitter = re.findall(r"'(.*?)'", line)
            trans = transmitter[0]
        if "rssi:" in line:
            rssiNum = [int(s) for s in re.findall(r'-?\d+', line)]
            if rssiNum > rssi_threshold:
                devices[trans] = rssiNum[0]
                
    return len(devices)

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("room/get_email")
    client.subscribe("room/get_devices")
    client.subscribe("room/returnRSSI")
    
def on_message(client, userdata, msg):
    rssiThreshold = -100
    print("orphan in my basement")
    if msg.topic == "room/get_email":
        pass
    if msg.topic == "room/returnRSSI":
        rssiThreshold = int(msg.payload.decode())
    if msg.topic == "room/get_devices":
        
        result = get_bluetooth(rssiThreshold)
        client.publish("room/BluetoothDevices", result)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.50.61", 1883, 60)

client.loop_forever()