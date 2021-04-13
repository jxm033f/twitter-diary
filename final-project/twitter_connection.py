import requests
import socket
import time

UDP_IP = "160.39.190.130"
SHARED_UDP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.connect((UDP_IP, SHARED_UDP_PORT))

def tweeting(message):
	url = "https://api.thingspeak.com/apps/thingtweet/1/statuses/update"
	parameters = {'api_key':'123456789',
					'status': message} #change to appropriate API Key
	response = requests.post(url, json = parameters)
	time.sleep(.2)

def loop():
    while True:
        data, addr = sock.recvfrom(2048)
        message = data.decode()
        print("Tweet: " + message)
        tweeting(message)
        sock.send('Tweet Posted'.encode())

sock.send('Waiting for Message'.encode())
loop()
