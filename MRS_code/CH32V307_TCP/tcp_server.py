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
    server.listen(5)  # 队列留5个，模式切换时新连接能排队等待accept
    print(f"[服务器启动] 监听 {HOST}:{PORT}")
    print("WiFi(Station模式)操作：电脑开移动热点 ESP-01S（密码00000000，2.4G），ESP会连入")
    print("本机热点网关应为 192.168.137.1；ESP连入后会自动发数据到这里")
    # 打印本机所有IPv4，方便确认
    try:
        hostname = socket.gethostname()
        ips = sorted(set(i[4][0] for i in socket.getaddrinfo(hostname, None, socket.AF_INET)))
        for ip in ips:
            print(f"  本机IP: {ip}")
    except Exception:
        pass
    print("-" * 50)

    while True:
        try:
            conn, addr = server.accept()
            print(f"\n[客户端连接] {addr[0]}:{addr[1]}")
            conn.settimeout(5.0)
            idle = 0  # 连续无数据的超时次数（每5秒+1）

            while True:
                try:
                    data = conn.recv(1024)
                    if not data:
                        print("[客户端断开]")
                        break
                    idle = 0  # 收到数据说明连接活着，重置计数
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
                    # 模式切换时旧连接可能半开（板子没发FIN），recv一直等。
                    # 连续15秒(3×5秒)无数据，判定旧连接已死，回accept取新连接。
                    # 板子每500ms发一次，正常绝不可能15秒无数据。
                    idle += 1
                    if idle >= 3:
                        print("[空闲15秒无数据，判定旧连接已断开，等待新连接]")
                        break
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
