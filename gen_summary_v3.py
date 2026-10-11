# -*- coding: utf-8 -*-
from docx import Document
from docx.shared import Pt, RGBColor, Cm
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml.ns import qn

doc = Document()

style = doc.styles['Normal']
style.font.name = 'Microsoft YaHei'
style.font.size = Pt(11)
style.element.rPr.rFonts.set(qn('w:eastAsia'), 'Microsoft YaHei')

for level in range(1, 4):
    hs = doc.styles[f'Heading {level}']
    hs.font.name = 'Microsoft YaHei'
    hs.element.rPr.rFonts.set(qn('w:eastAsia'), 'Microsoft YaHei')

def add_h(text, level=1):
    doc.add_heading(text, level=level)

def add_p(text, bold=False):
    p = doc.add_paragraph()
    r = p.add_run(text)
    r.bold = bold
    r.font.size = Pt(11)
    p.paragraph_format.first_line_indent = Cm(0.74)
    p.paragraph_format.space_after = Pt(4)

def add_bullet(text):
    p = doc.add_paragraph(style='List Bullet')
    p.add_run(text).font.size = Pt(11)

def add_code(text):
    p = doc.add_paragraph()
    r = p.add_run(text)
    r.font.name = 'Consolas'
    r.font.size = Pt(9)
    p.paragraph_format.left_indent = Cm(1)

def add_table(headers, rows):
    t = doc.add_table(rows=1, cols=len(headers))
    t.style = 'Light Grid Accent 1'
    for i, h in enumerate(headers):
        t.rows[0].cells[i].text = h
    for row in rows:
        cells = t.add_row().cells
        for i, v in enumerate(row):
            cells[i].text = str(v)

# ===== 封面 =====
title = doc.add_paragraph()
title.alignment = WD_ALIGN_PARAGRAPH.CENTER
r = title.add_run('CH32V307嵌入式开发学习总结')
r.bold = True
r.font.size = Pt(22)
r.font.color.rgb = RGBColor(0x1F, 0x4E, 0x79)

sub = doc.add_paragraph()
sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
r = sub.add_run('从点灯到TCP上传 —— 一个新手的完整嵌入式学习笔记')
r.font.size = Pt(13)
r.font.color.rgb = RGBColor(0x66, 0x66, 0x66)

doc.add_paragraph()

# ===== 第1章 =====
add_h('第1章 写在前面：你不是在复制代码', 1)
add_p('这份笔记不是知识点罗列，而是告诉你每一步为什么这么做。每学一个外设，记住三个问题：这个外设是干嘛的？我要配置什么？代码里每一行在干什么？')
add_p('核心思路：嵌入式开发就是"配置寄存器→读写数据"。芯片是个大开关盒，你通过代码告诉它哪个引脚输出电压、哪个引脚读电压、哪个外设开始工作。')

# ===== 第2章 =====
add_h('第2章 开发环境搭建', 1)
add_h('2.1 MounRiver Studio (MRS)', 2)
add_p('WCH官方IDE，基于Eclipse。新建工程选CH32V307VCT6（100脚），不要选RCT（64脚）。')
add_h('2.2 YD-LINK调试器接线', 2)
add_table(['线色', '接板子', '作用'], [
    ['红', '3.3V', '供电'],
    ['黄', 'SWDIO', '数据'],
    ['绿', 'SWCLK', '时钟'],
    ['黑', 'GND', '接地'],
])
add_h('2.3 下载失败怎么办', 2)
add_bullet('按住BOOT0键不松 → 点下载 → 看到"开始烧录"再松开BOOT0')
add_bullet('检查接线有没有松动')
add_bullet('按一下RST复位键再试')

# ===== 第3章 GPIO =====
add_h('第3章 GPIO：LED闪烁（输出）', 1)
add_p('GPIO就是通用输入输出引脚，你可以让它输出高电平（3.3V）或低电平（0V）。')
add_h('3.1 GPIO三步曲', 2)
add_p('任何GPIO操作都是三步：')
add_bullet('第1步：开时钟 —— RCC_APB2PeriphClockCmd(APB2Periph_GPIOA, ENABLE);')
add_bullet('第2步：配引脚 —— GPIO_InitStruct结构体填引脚号、模式、速度')
add_bullet('第3步：初始化 —— GPIO_Init(GPIOA, &GPIO_InitStruct);')
add_h('3.2 8种工作模式', 2)
add_table(['模式', '通俗解释', '用在哪'], [
    ['Out_PP推挽输出', '芯片主动输出高低电压', 'LED、蜂鸣器'],
    ['Out_OD开漏输出', '只能拉低，要靠外部电阻拉高', 'I2C的SCL/SDA'],
    ['IPU上拉输入', '默认高电平，按下接地变低', '按键'],
    ['IPD下拉输入', '默认低电平', '外部信号检测'],
    ['AF_PP复用推挽', '引脚交给外设用', '串口TX、ETH'],
])
add_h('3.3 LED闪烁代码思路', 2)
add_p('PA0推挽输出 → 置高点亮 → 延时 → 置低熄灭 → 延时 → 循环。')
add_p('怎么自己写：先想"我要哪个引脚？做什么用？"然后套三步曲。')

# ===== 第4章 按键输入 =====
add_h('第4章 GPIO输入：板载按键', 1)
add_p('按键一端接地，一端接引脚。按下时引脚读到低电平（RESET），松开时内部上拉电阻让它保持高电平（SET）。')
add_p('代码思路：循环读引脚电平 → if读到低电平就执行动作。要加延时消抖（机械按键按下会抖动几毫秒）。')

# ===== 第5章 串口 =====
add_h('第5章 串口通信（USART）', 1)
add_p('串口就是芯片和电脑之间的"对讲机"，一个发一个收，按约定的速度（波特率）传字符。')
add_h('5.1 配置步骤', 2)
add_bullet('PA9复用推挽输出（TX）')
add_bullet('开GPIOA和USART1时钟')
add_bullet('波特率115200，8数据位，1停止位，无校验')
add_h('5.2 为什么不用printf', 2)
add_p('MRS工程自带的debug.c里已经写了_write函数，你再写一个会冲突（编译报错redefinition）。所以我们自己写三个发送函数：USART1_SendChar（发一个字）、SendString（发一串）、SendNum（发数字）。')

# ===== 第6章 I2C + OLED =====
add_h('第6章 I2C通信 + OLED屏幕', 1)
add_p('I2C只用两根线（SCL时钟+SDA数据）就能和多个传感器/屏幕通信，每个设备有自己的地址。')
add_h('6.1 软件模拟I2C', 2)
add_p('不用芯片硬件I2C，用两个GPIO模拟时序：')
add_bullet('起始信号：SCL高电平时SDA从高变低')
add_bullet('停止信号：SCL高电平时SDA从低变高')
add_bullet('发一个字节：8位数据，高位先发')
add_h('6.2 OLED（SSD1306）', 2)
add_p('128x64像素屏，分8页，每页8行像素。发0xFF=8个像素全亮，0x00=全灭。')
add_p('注意：OLED已烧坏待购新模块。')

# ===== 第7章 ADXL345 =====
add_h('第7章 ADXL345加速度传感器', 1)
add_p('这是个测重力加速度的芯片，能感知三个方向的倾斜和震动。I2C地址0x53。')
add_h('7.1 接线', 2)
add_table(['ADXL345引脚', '接板子', '说明'], [
    ['VCC', '3.3V', '电源'],
    ['GND', 'GND', '接地'],
    ['SCL', 'PB3', 'I2C时钟'],
    ['SDA', 'PB5', 'I2C数据'],
    ['CS', '3.3V', '接高=I2C模式'],
    ['SDO', 'GND', '接地=地址0x53'],
])
add_h('7.2 关键寄存器', 2)
add_table(['寄存器', '写入值', '作用'], [
    ['0x00 DEVID', '读', '应该读到229(0xE5)，验证芯片活着'],
    ['0x2D POWER_CTL', '0x08', '上电'],
    ['0x31 DATA_FORMAT', '0x00', '±2g量程'],
    ['0x32~0x37', '读', 'XYZ六个字节数据'],
])
add_h('7.3 读数据代码思路', 2)
add_p('连续读多字节时，寄存器地址要或上0x40（自动地址递增）。读回来的是16位有符号数，低字节在前高字节在后。')

# ===== 第8章 震动检测 =====
add_h('第8章 震动检测算法', 1)
add_p('思路很简单：比较这次读的值和上次的值差多少。')
add_p('shake = |ax-lastX| + |ay-lastY| + |az-lastZ|')
add_table(['shake值', '等级', '含义', '动作'], [
    ['<10', '0 静止', '板子没动', '无'],
    ['10~50', '1 轻微', '轻微触碰', '记录'],
    ['>=50', '2 强烈', '敲击/坠落', '蜂鸣器响+报警'],
])

# ===== 第9章 蜂鸣器 =====
add_h('第9章 蜂鸣器', 1)
add_p('有源蜂鸣器，给高电平就响，给低电平就停。I/O接PB1推挽输出。震动强烈时level=2，Beep_On()。')

# ===== 第10章 以太网TCP（新增重点）=====
add_h('第10章 以太网TCP通信（重点新增）', 1)
add_p('这是整个项目最复杂的部分，但思路不复杂：板子当客户端，主动连电脑上的Python服务器，把数据发过去。')

add_h('10.1 为什么不能自己写TCP代码', 2)
add_p('TCP协议极其复杂（IP地址、MAC、三次握手、重传、分包），几千行代码。WCH把这些封装成了一个黑盒库叫NetLib，你只要调几个函数就能上网。')
add_p('和GPIO的区别：GPIO简单自己写，以太网太复杂直接用官方库。')

add_h('10.2 IP地址是什么', 2)
add_p('就像门牌号。板子IP=192.168.1.100，电脑IP=192.168.1.10，两个必须在同一网段（都是192.168.1.x）才能通信。')
add_table(['设备', 'IP地址', '角色'], [
    ['CH32V307板子', '192.168.1.100', 'TCP客户端（主动连）'],
    ['电脑', '192.168.1.10', 'TCP服务器（监听8888端口）'],
])

add_h('10.3 静态IP怎么设', 2)
add_p('Win+R → ncpa.cpl → 右键网口 → 属性 → IPv4 → 手动填192.168.1.10/255.255.255.0。')
add_p('为什么要手动设？因为网线直连没有路由器自动分配IP，必须两边固定地址才能互相找到。')

add_h('10.4 通信协议（数据格式）', 2)
add_p('板子每200毫秒发一行文本：')
add_code('X,Y,Z,shake,level\n')
add_p('例如：-164,-1,-1,0,0\\n')
add_p('这就是板子和电脑之间的"约定语言"，Python队友按这个格式解析就行。')

add_h('10.5 TCP工作流程', 2)
add_bullet('初始化ETH_LibInit()——启动以太网')
add_bullet('WCHNET_CreateTcpSocket()——创建连接，主动连电脑')
add_bullet('连接成功后tcp_connected=1')
add_bullet('循环里WCHNET_MainTask()必须一直调用——这是网络库的"心跳"')
add_bullet('连接成功后，读ADXL数据，组包，WCHNET_SocketSend()发出去')

add_h('10.6 遇到的坑和解决', 2)
add_table(['问题', '原因', '解决'], [
    ['MRS工程没有ETH/LWIP文件', '新建工程只有NoneOS模板', '从WCH官网下载EVT包，用官方TCPClient示例工程'],
    ['打开工程显示"无效链接"', '复制工程后库文件相对路径断了', '把Core/Peripheral/NetLib等文件夹全部复制进工程目录'],
    ['下载报找不到hex', '工程名改了但下载配置还找旧名', '复制一份hex文件改名'],
    ['Python连不上', '电脑没设静态IP', '手动设192.168.1.10'],
])

add_h('10.7 核心经验：遇到新外设怎么搞', 2)
add_p('记住这个套路，以后USB、SD卡、蓝牙都一样：')
add_bullet('第1步：MRS新建工程没有这个外设的库')
add_bullet('第2步：去WCH官网下载EVT包（所有官方示例大合集）')
add_bullet('第3步：在EXAM目录里找最接近你要功能的示例')
add_bullet('第4步：直接用那个工程，只改main.c加自己的代码')
add_p('不要从零造轮子，在别人跑通的代码上改。')

# ===== 第11章 面包板和电源 =====
add_h('第11章 面包板与供电', 1)
add_p('核心板GND只有2个不够用，用MB-102面包板扩展。')
add_bullet('红轨=电源（3.3V），蓝轨=GND')
add_bullet('所有模块电源接面包板，信号线接核心板')
add_p('面包板电源模块（黑色小板）：插在面包板边缘，USB供电，输出3.3V到红轨，不用从核心板引线。')

# ===== 第12章 调试方法论 =====
add_h('第12章 调试方法论', 1)
add_bullet('分模块验证：先LED亮→再串口打印→再I2C读ID→再读数据→最后整合')
add_bullet('串口是最好的朋友：任何不确定的值都printf出来看')
add_bullet('接线问题先查：断电、查线序、查接触')
add_bullet('一个一个变量改：不要一次改一堆代码')

# ===== 第13章 学习路线 =====
add_h('第13章 接下来学什么', 1)
add_bullet('新OLED到了，把数据显示在屏幕上')
add_bullet('定时器中断（不用delay_ms阻塞）')
add_bullet('PWM呼吸灯')
add_bullet('ADC读传感器')
add_bullet('FreeRTOS操作系统')

# ===== 附录A 硬件关闭教程 =====
add_h('附录A 如何安全关闭硬件', 1)
add_p('日常暂停（下次继续）：')
add_bullet('1. MRS里Ctrl+S保存代码')
add_bullet('2. 关闭串口助手/Python窗口')
add_bullet('3. 先拔Type-C电源线（断电）')
add_bullet('4. 再拔YD-LINK的USB')
add_p('完全收纳：断电后，把杜邦线按模块归类，面包板上的线可以保留，网线收好。')

# ===== 附录B 引脚分配表 =====
add_h('附录B 当前引脚分配表', 1)
add_table(['引脚', '接什么', '方向'], [
    ['PA0', '板载绿灯', '推挽输出'],
    ['PA9', '串口TX→YD-LINK RX', '复用推挽'],
    ['PB1', '蜂鸣器', '推挽输出'],
    ['PB3', 'ADXL345 SCL / OLED SCL', '开漏输出'],
    ['PB5', 'ADXL345 SDA / OLED SDA', '开漏输出'],
    ['PB6/PB7', 'OLED（烧坏待购新）', '软件I2C'],
    ['ETH', '板载网口', '内置PHY'],
])

# ===== 附录C 通信协议 =====
add_h('附录C 通信协议（给队友）', 1)
add_p('板子作为TCP客户端，连接电脑192.168.1.10:8888。')
add_p('每200ms发送一行：')
add_code('X,Y,Z,shake,level\n')
add_p('level: 0=静止, 1=轻微, 2=强烈')

doc.save(r'C:\Users\WaspYL\Desktop\c-language-practice\CH32V307嵌入式学习总结_第三版.docx')
print('生成完成')
