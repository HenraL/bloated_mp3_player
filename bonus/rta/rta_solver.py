# Response Time Analysis (RTA) for fixed-priority scheduling (RM)

from math import ceil

# Task set: (name, WCET C, period T)
tasks_raw = [
    ("b", 2, 8),
    ("c", 3, 10),
    ("d", 4, 18),
    ("a", 7, 40),
]

# Already sorted by RM priority (highest = shortest period)
tasks = [{"name": n, "C": c, "T": t} for n, c, t in tasks_raw]

for i, task in enumerate(tasks):
    higher = tasks[:i]  # only tasks above in priority

    R_prev = task["C"]  # first guess
    while True:
        # Compute interference from all higher-priority tasks
        interference = 0
        for h in higher:
            interference += ceil(R_prev / h["T"]) * h["C"]

        R = task["C"] + interference

        if R == R_prev:
            print(f"{task['name']}: R = {R} ≤ T={task['T']} → SCHEDULABLE")
            break
        if R > task["T"]:
            print(f"{task['name']}: R = {R} > T={task['T']} → NOT SCHEDULABLE")
            break

        R_prev = R
