from socket import *
proxy_ip = '127.0.0.1'
port = 53

client_socket = socket(AF_INET, SOCK_STREAM)
message = "google.com:443|"
client_socket.connect((proxy_ip, port))
client_socket.send(message.encode())

# print(client_socket.recv(1024))
response = client_socket.recv(4096)
print(f"Response: '{response.decode()}'\nFrom address: '{proxy_ip}:{port}'")
# client_socket.close()
#
# time.sleep(1)
#
# client_socket = socket(AF_INET, SOCK_DGRAM)
print("\n\n")
message = "python.org:443|GET / HTTP/2.1\r\nHost:python.org\r\n\r\n"
# client_socket.connect((proxy_ip, port))
client_socket.send(message.encode())
response = client_socket.recv(4096)
print(f"Response: '{response.decode()}'\nFrom address: '{proxy_ip}:{port}'")
client_socket.close()
