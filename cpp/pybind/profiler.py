# TODO: Dedup against /python/profiler.h

import time


class Profiler:
    def __init__(self, *, active: bool = True):
        self.start = time.time()
        self.active = active

    def finish(self, stage_name):
        if not self.active:
            return
        end = time.time()
        duration = end - self.start
        print(f"Profiler: {stage_name} took {duration:.3f} seconds")
        self.start = end
