from scapy.all import sniff
from matplotlib import pyplot as plt
from matplotlib import animation

print("Supported filters:")
print("- 'tcp': Capture only TCP packets.")
print("- 'udp': Capture only UDP packets.")
print("- 'icmp': Capture only ICMP packets.")
print("- 'host <ip_address>': Capture packets with the specified IP address as source or destination.")
print("- 'port <port_number>': Capture packets with the specified port number.")

filter = input("Enter filter expression (press Enter for no filter): ").strip()

timestamps = []
data_sizes = []

def handle_packet(packet):
    global data_sizes
    if data_sizes:
        data_sizes[-1] += len(packet)


def update_plot(frame):
    global timestamps, data_sizes

    sniff(prn=handle_packet, filter=filter, timeout=0.5)

    while len(data_sizes) * 0.5 > 15:
        data_sizes.pop(0)
        timestamps.pop(0)

    plot.clear()
    plot.set_title('Network activity')
    plot.set_xlabel('Seconds, s')
    plot.set_ylabel('Data, bytes')
    plot.plot(timestamps, data_sizes)

    data_sizes.append(0)
    if len(timestamps) > 0:
        timestamps.append(timestamps[-1] + 0.5)
    else:
        timestamps.append(0.5)

figure = plt.figure()
plot = figure.add_subplot(111)
ani = animation.FuncAnimation(figure, update_plot, interval=500)
plt.show()
