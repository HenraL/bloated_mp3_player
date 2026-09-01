"""
Question 6: Processor Demand Criterion complexity reduction.
Shows the difference between exponential (hyperperiod) and
pseudo-polynomial (busy period iteration) approaches.
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


# Task set from Q5
tasks = [
    Task("τ1", 3, 10, 10),
    Task("τ2", 15, 50, 200),
    Task("τ3", 17, 100, 120),
    Task("τ4", 200, 1000, 2000),
]


def lcm(a: int, b: int) -> int:
    return a * b // gcd(a, b)


def demand(tasks: List[Task], L: int) -> int:
    """h(L): total execution demand with deadline ≤ L."""
    total = 0
    for t in tasks:
        if L >= t.deadline:
            total += ((L - t.deadline) // t.period + 1) * t.wcet
    return total


def scheduling_points(tasks: List[Task], limit: int) -> List[int]:
    """All deadline-aligned points up to limit."""
    points = set()
    for t in tasks:
        L = t.deadline
        while L <= limit:
            points.add(L)
            L += t.period
    return sorted(points)


def busy_period_fixed_point(tasks: List[Task]) -> int:
    """Find the busy period bound via fixed-point iteration."""
    L = sum(t.wcet for t in tasks)
    iterations = 0
    while True:
        iterations += 1
        d = demand(tasks, L)
        if d == L or d == 0:
            return max(L, min(t.deadline for t in tasks if t.deadline > 0)), iterations
        if d > L:
            L = d
        else:
            L = d
            if L == 0:
                L = min(t.deadline for t in tasks if t.deadline > 0)
                return L, iterations


# ── Demonstration ─────────────────────────────────────────────

print("=" * 70)
print("COMPLEXITY OF THE PROCESSOR DEMAND CRITERION")
print("=" * 70)
print()

H = reduce(lcm, [t.period for t in tasks])
print(f"Task set: {', '.join(t.name for t in tasks)}")
print(f"Utilization U = {sum(t.wcet/t.period for t in tasks):.4f}")
print()

print("─" * 70)
print("EXPONENTIAL approach:")
print("  Check h(L) ≤ L for all scheduling points up to hyperperiod H")
print(f"  H = lcm({', '.join(str(t.period) for t in tasks)}) = {H}")
points = scheduling_points(tasks, H)
print(f"  Deadline points to check: {len(points)}")
print(f"  Problem: H grows exponentially with the number of tasks")
print(f"  (e.g., with periods being distinct primes)")
print()

print("─" * 70)
print("PSEUDO-POLYNOMIAL approach (busy period iteration):")
b, iters = busy_period_fixed_point(tasks)
print(f"  Busy period bound L* = {b} (found in {iters} iterations)")
print(f"  Check only scheduling points up to L*: {len(scheduling_points(tasks, b))} points")
print(f"  vs {len(points)} points for hyperperiod")
print(f"  Ratio: {len(points) // max(len(scheduling_points(tasks, b)), 1)}x fewer points")
print()
print("  Why pseudo-polynomial:")
print(f"  - Each iteration: O(n) work (n = number of tasks)")
print(f"  - Number of iterations: O(D_max / C_min)")
print(f"  - Total: O(n × D_max / C_min)")
print(f"  - This is polynomial in the *numeric values* of parameters,")
print(f"    not in the number of bits to represent them → pseudo-polynomial")
print()

print("─" * 70)
print("VERIFICATION:")
failures = []
for L in scheduling_points(tasks, b):
    d = demand(tasks, L)
    if d > L:
        failures.append((L, d))
if failures:
    print(f"  ❌ h(L) > L at {len(failures)} point(s) → NOT SCHEDULABLE")
else:
    print(f"  ✅ h(L) ≤ L at all {len(scheduling_points(tasks, b))} deadline points → SCHEDULABLE")
