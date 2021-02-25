import socket

serverAddr = '192.168.191.129'
serverPort = 80

getReq = b'GET /HelloWorld.html HTTP/1.1\n'
getReq_2 = b'GET /Passwords.html HTTP/1.1\n'


def sendReq(msg):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((serverAddr, serverPort))
        sock.sendall(msg)
        received = sock.recv(4096)
        print("RECEIVED: %s" % received)

sendReq(getReq)
sendReq(getReq_2)




