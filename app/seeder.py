import serial
import time
import sys

if sys.platform == 'win32' or sys.platform == 'cygwin':
    PORT = "COM3"
elif sys.platform == 'darwin':
    PORT = "/dev/tty.SLAB_USBtoUART"
else:
    PORT = "/dev/ttyUSB0"

BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

print("Connected to", PORT)

msg = "SEEDER"
ser.write(msg.encode())
print("Sent:", msg.strip())

while True:
    try:
        line = ser.readline().decode(errors='ignore').strip()
    except KeyboardInterrupt:
        break
    if line.startswith("SEEDER"):
        response = line[len("SEEDER: "):]
        print("Received from serial:", response)
        # TODO: process the gemini request
        ser.write("SEEDER_REPLY: ACK".encode())

ser.close()
