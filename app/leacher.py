import serial
import time
import sys
import socket
import selectors
import types

if sys.platform == 'win32' or sys.platform == 'cygwin':
    PORT = "COM3"
elif sys.platform == 'darwin':
    PORT = "/dev/tty.SLAB_USBtoUART"
else:
    PORT = "/dev/ttyUSB0"

BAUD = 115200
HOST = "127.0.0.1"
SOCK_PORT = 54549

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

print("Connected to", PORT)

msg = "LEACHER"
ser.write(msg.encode())
print("Sent:", msg.strip())

sel = selectors.DefaultSelector()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, SOCK_PORT))
s.listen()
print(f"Socket listening on {HOST}:{SOCK_PORT}")
s.setblocking(False)
sel.register(s, selectors.EVENT_READ, data=None)

def accept_wrapper(sock):
    conn, addr = sock.accept()
    print('Connected by', addr)
    conn.setblocking(False)
    data = types.SimpleNamespace(addr=addr, inb=b'', db=b'', outb=b'')
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data=data)

def service_connection(key, mask):
    sock = key.fileobj
    data = key.data
    if mask & selectors.EVENT_READ:
        recv_data = sock.recv(1024)
        if not recv_data:
            print("Closing connection to", data.addr)
            sel.unregister(sock)
            sock.close()
            return

        data.db += recv_data
    elif (mask & selectors.EVENT_WRITE) and data.db:
        decoded_data = data.db.decode().strip()
        print("Received from socket:", decoded_data)
        ser.write(("LEACHER_REQUEST: " + decoded_data).encode())
        response = None
        while response is None:
            time.sleep(0.001)
            response = ser.readline().decode(errors='ignore').strip()
            print("Read from serial:", response)
            if response.startswith("LEACHER: "):
                response = response[len("LEACHER: "):]
                break
            response = None

        print("Received from serial:", response)
        data.db = b''
        data.outb = response.encode()
    elif (mask & selectors.EVENT_WRITE) and data.outb:
        sent = sock.send(data.outb)
        data.outb = data.outb[sent:]

try:
    while True:
        events = sel.select(timeout=None)
        for key, mask in events:
            if key.data is None:
                accept_wrapper(key.fileobj)
                continue
            service_connection(key, mask)
except KeyboardInterrupt:
    print("Exiting....")
finally:
    sel.close()
    ser.close()