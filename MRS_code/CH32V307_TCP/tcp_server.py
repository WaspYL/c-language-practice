"""
CH32V307 震动报警器 - TCP测试服务器
用法：python tcp_server.py
监听 0.0.0.0:8888，接收开发板发送的数据并打印
"""
import socket
import time

HOST = '0.0.0.0'  # 监听所有网卡
PORT = 8888

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((HOST, PORT))
    server.listen(1)
    print(f"[服务器启动] 监听 {HOST}:{PORT}")
    print(f"[等待连接] 请确保开发板网线已连接，电脑IP设为192.168.1.10")
    print("-" * 50)

    while True:
        try:
            conn, addr = server.accept()
            print(f"\n[客户端连接] {addr[0]}:{addr[1]}")
            conn.settimeout(5.0)

            while True:
                try:
                    data = conn.recv(1024)
                    if not data:
                        print("[客户端断开]")
                        break
                    # 解码并打印
                    text = data.decode('utf-8', errors='ignore').strip()
                    if text:
                        for line in text.split('\n'):
                            line = line.strip()
                            if line:
                                parts = line.split(',')
                                if len(parts) == 5:
                                    ax, ay, az, diff, level = parts
                                    level_name = ["安全", "轻微", "防盗", "地震"][int(level)] if level.isdigit() and 0 <= int(level) <= 3 else level
                                    print(f"[数据] X={ax:>5} Y={ay:>5} Z={az:>5} 强度={diff:>5} 等级={level_name}")
                                else:
                                    print(f"[原始] {line}")
                except socket.timeout:
                    # 超时不处理，继续等
                    pass
                except ConnectionResetError:
                    print("[连接重置]")
                    break
                except Exception as e:
                    print(f"[接收错误] {e}")
                    break

            conn.close()
            print("[等待重新连接]")
        except KeyboardInterrupt:
            print("\n[服务器关闭]")
            break
        except Exception as e:
            print(f"[错误] {e}")
            time.sleep(1)

    server.close()

if __name__ == '__main__':
    main()
