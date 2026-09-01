"""
Question 5: EDF schedulability test for task set:

Task | C  | D   | T
τ1   | 3  | 10  | 10
τ2   | 15 | 50  | 200
τ3   | 17 | 100 | 120
τ4   | 200| 1000| 2000
"""

from dataclasses import dataclass
from typing import List
from math import gcd
from functools import reduce


@dataclass
class Task:
    name: str
    wcet: int
    deadline: int
    period: int


tasks = [
    Task("τ1", 3, 10, 10),
    Task("τ2", 15, 50, 200),
    Task("τ3", 17, 100, 120),
    Task("τ4", 200, 1000, 2000),
]


def lcm(a: int, b: int) -> int:
    return a * b // gcd(a, b)


def compute_utilization(tasks: List[Task]) -> float:
    return sum(t.wcet / t.period for t in tasks)


def processor_demand(tasks: List[Task], L: int) -> int:
    """h(L) = sum of C_i for all jobs with deadline ≤ L."""
    demand = 0
    for t in tasks:
        if L >= t.deadline:
            n_jobs = (L - t.deadline) // t.period + 1
            demand += n_jobs * t.wcet
    return demand


def check_deadline_points(tasks: List[Task], limit: int) -> list:
    """Check h(L) at all deadline points up to limit."""
    points = set()
    for t in tasks:
        L = t.deadline
        while L <= limit:
            points.add(L)
            L += t.period

    failures = []
    for L in sorted(points):
        d = processor_demand(tasks, L)
        if d > L:
            failures.append((L, d))
    return failures


U = compute_utilization(tasks)
print(f"Utilization U = {' + '.join(f'{t.wcet}/{t.period}' for t in tasks)}")
print(f"             = {sum(t.wcet/t.period for t in tasks):.5f}")
print(f"             = {U:.5f}")
print(f"U ≤ 1: {U <= 1}")
print()

# Check at key deadline points
hyperperiod = reduce(lcm, [t.period for t in tasks])
print(f"Hyperperiod = {hyperperiod}")
print(f"Checking processor demand at all deadline points up to hyperperiod...")

failures = check_deadline_points(tasks, hyperperiod)
if failures:
    print(f"FAILED at {len(failures)} point(s):")
    for L, d in failures[:10]:
        print(f"  L={L}: h(L)={d} > L={L}  (Δ={d - L})")
else:
    print("All deadlines met. Task set IS schedulable under EDF.")
