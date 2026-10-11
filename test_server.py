import socket

# TCP服务器：监听8888端口，接收板子发的数据
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('0.0.0.0', 8888))
server.listen(1)
print("等待板子连接... (192.168.1.10:8888)")

conn, addr = server.accept()
print(f"板子已连接! 地址: {addr}")

while True:
    data = conn.recv(1024)
    if not data:
        print("板子断开了")
        break
    # 打印收到的数据
    print(data.decode().strip())

conn.close()
server.close()
