
import socket
from itertools import chain

UDP_IP = "10.10.10.97"
UDP_PORT = 6454
print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT


class Universe:
    def __init__(self, number, mapping, matrix):
        self.number = number
        self.mapping = mapping
        self.matrix = matrix


class Sender:
    def __init__(self, width, height, universeSizes, fromLeft, fromTop):
        self.width = width
        self.height = height
        self.universes = len(universeSizes)
        self.universeSizes = universeSizes
        self.udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.mapping = snake(width, height, fromLeft, fromTop)
        print self.mapping
        self.outputMatrices = []
        for universe, size in enumerate(universeSizes):
            o = bytearray(19 + size * 3)
            o[14] = universe + 1
            self.outputMatrices.append(o)

    def setRGB(self, x, y, red, green, blue):
        i = (y * width + x)
        for universe, size in enumerate(self.universeSizes):
            skipped = 0
            if i < size:
                matrix = self.outputMatrices[universe]
                i = self.mapping[i]
                i = 19 + (i - skipped) * 3
                matrix[i] = red
                matrix[i + 1] = green
                matrix[i + 2] = blue
                break
            else:
                skipped += size

    def send(self):
        for outputMatrix in self.outputMatrices:
            self.udp.sendto(outputMatrix, (UDP_IP, UDP_PORT))


mapping20x14 = [
    19,  18,  17,  16,  15,  14,  13,  12,  11,  10,  9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
    20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
    59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48,  47,  46,  45,  44,  43,  42,  41,  40,
    60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    99,  98,  97,  96,  95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 206, 205, 204, 203, 202, 201, 200,
    220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    259, 258, 257, 256, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240,
    260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279]


def snake(width, height, fromLeft, fromTop):
    if fromLeft:
        left = 1
    else:
        left = 0
    rows = [[]] * height
    for y in range(height):
        rows[y] = range(y * width, (y + 1) * width)
        if y % 2 == left:
            rows[y].reverse()
    if not fromTop:
        rows.reverse()
    return list(chain.from_iterable(rows))


width = 25
height = 10
universeSize = width * height

sender = Sender(width, height, [universeSize / 2, universeSize / 2], True, False)
sender.setRGB(2, 2, 200, 0, 0)    # NV RED
sender.setRGB(2, 8, 0, 200, 0)    # SV GREEN
sender.setRGB(23, 2, 0, 0, 200)   # NE BLUE
sender.setRGB(23, 8, 200, 200, 0) # SE YELLOW
sender.send()
