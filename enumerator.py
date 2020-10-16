class Enumerator:
    def __init__(self):
        self.indices = {}
        self.num_objects = 0

    def exists(self, value):
        return value in self.indices

    def index(self, value):
        if not value in self.indices:
            self.indices[value] = self.num_objects
            self.num_objects += 1
        return self.indices[value]
