# TODO: Dedup against /python/profiler.h

import psutil
import time


class Profiler:
    def __init__(self, *, active: bool = True):
        self.active = active
        self.start = Profiler._times()

    @staticmethod
    def _times():
        cpu_times = psutil.cpu_times()
        return [time.time(), cpu_times.user, cpu_times.system]

    def finish(self, stage_name):
        if not self.active:
            return
        end = Profiler._times()
        t = [t1 - t0 for t1, t0 in zip(end, self.start)]
        print(f'Profiler: {stage_name} took {t[0]:.3f} s (user: {t[1]:.3f} s, system: {t[2]:.3f} s)')
        self.start = end
