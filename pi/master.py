
class Canvas:
    def __init__(self, width, height, fromLeft, fromTop):

class Universe:
    def __init__(self, number, bs):
        self.number = number
        self.buffer = bs

class Mapping:
    def __init__(self, width, height, fromLeft, fromTop, universeSizes):
        self.width = width
        self.height = height
        self.fromLeft = fromLeft
        self.fromTop = fromTop
        self.pixels = width * height
        self.universeCount = len(universeSizes)
        self.universes = []
        for size, i in enumerate(universeSizes):
            buffer = bytearray(19 + size * 3)
            buffer[14] = i + 1
            self.universes.append(Universe(i + 1, buffer))


    # [[Color]] -> [ByteArray]
    def artNetUniversePackages(self, matrix):
        Universe(1 + 1, 1)

x = Universe(2, bytearray(1))

