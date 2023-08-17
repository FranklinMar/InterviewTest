from socket import *
import json
import ssl


data = ""
with open("config.json") as file:
    data += "".join(line for line in file)

config = json.loads(data)

context = ssl.create_default_context()

print("Config loaded")
# print(config)

listening_socket = socket(AF_INET, SOCK_STREAM)

listening_socket.bind((config["ip"], config["port"]))
# listening_socket_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
print("Starting Server")
listening_socket.listen(1)
while True:
    try:
        connection_socket, addr = listening_socket.accept()
        while True:
            message = connection_socket.recv(4096).decode().split('|')
            print(f"Received message from address '{addr}'")
            if any(forbidden_address in message[0] for forbidden_address in config["blacklist"]["list"]):
                print(f"Blacklisted host detected, returning response '{config['blacklist']['response']}'")
                send = connection_socket.send(config["blacklist"]["response"].encode())
            else:
                address = message[0].split(":")
                address[1] = int(address[1])
                print(f"Server DNS '{message[0]} has IP address: '{gethostbyname(address[0])}:{address[1]}'")
                server_socket = context.wrap_socket(socket(AF_INET, SOCK_STREAM), server_hostname=address[0])
                print(f"Connecting to: {tuple(address)}")
                server_socket.settimeout(10)
                server_socket.connect(tuple(address))
                print(f"Sending message: '{repr(message[1])}'")
                server_socket.send(message[1].encode())
                response = server_socket.recv(4096)
                server_socket.close()
                print("Returning response")
                connection_socket.send(response)
            print()
        connection_socket.close()
        print()
    except Exception:
        pass
        # print(e)
        # print(e.with_traceback())

