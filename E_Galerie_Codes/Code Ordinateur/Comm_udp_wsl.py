import struct
import socket
import errno
import time

UDP_IP = "0.0.0.0"  # listen on all available interfaces
UDP_PORT = 12345

UDP_IPWSL = "172.17.185.191"
UDP_PORTWSL = 8084

sockwsl = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print("Listening for UDP packets on port", UDP_PORT)

while True:
    try:
        data, addr = sock.recvfrom(47)
        print("packet recu")
        
        # process data
        sock.sendto(data, (UDP_IPWSL, UDP_PORTWSL))
        """
        lidar_frame = struct.unpack('<B B H H H B H B H B H B H B H B H B H B H B H B H B H B H H B', data)
        start_angle = lidar_frame[3]
        end_angle = lidar_frame[-3]
        
        print(f"Start angle: {start_angle}, End angle: {end_angle}")
        """
    except socket.error as e:
        if e.errno == errno.EWOULDBLOCK:
            time.sleep(0.001) # wait a short time before trying again
        else:
            raise e
