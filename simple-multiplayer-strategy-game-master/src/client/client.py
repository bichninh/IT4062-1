import socket
from tkinter import *
from _pickle import loads, dumps
from model.startFrame import startFrame
from message import message

# connect server
host = "127.0.0.1"
port = 5500

#### connect and creat Tk
def main():
    root = Tk()
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.settimeout(2.0)
    client.connect((host, port)) ### Connect to server
    mesrcv = loads(client.recv(2048))
    if mesrcv.opCode == 0x0802:
        list_team = mesrcv.payLoad ### Receive team info
        frame = startFrame(client, root, list_team) ### Start Frame "Start Game"
        frame.mainLoop()
    else:
        pass
    client.close() 

if __name__ == '__main__':
    main()