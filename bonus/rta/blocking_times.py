"""
Calculate PIP and PCP blocking times for Question 4 of the exam.
"""

from dataclasses import dataclass
from typing import List


@dataclass
class Task:
    name: str
    period: int
    wcet: int
    critical_sections: dict  # resource_name -> CS length


# Critical section matrix from the exam:
# Rows = resources, Columns = tasks (a, b, c, d)
# Values = max CS duration on that resource
raw_cs = {
    "task_a": {"R1": 6, "R2": 1, "R5": 4},
    "task_b": {"R2": 1, "R4": 1, "R5": 1},
    "task_c": {"R1": 2, "R3": 1, "R4": 2, "R5": 1},
    "task_d": {"R2": 4, "R3": 2, "R4": 3},
}

all_resources = ["R1", "R2", "R3", "R4", "R5"]

# Create tasks sorted by RM priority (shortest period = highest)
tasks = [
    Task("τ_B", 8, 2, raw_cs["task_b"]),
    Task("τ_C", 10, 3, raw_cs["task_c"]),
    Task("τ_D", 18, 4, raw_cs["task_d"]),
    Task("τ_A", 40, 7, raw_cs["task_a"]),
]


def resources_used_by(task: Task) -> set:
    """Return the set of resources this task uses."""
    return set(task.critical_sections.keys())


def max_cs_on_resource(task: Task, resource: str) -> int:
    """Return the CS duration of a task on a given resource (0 if not used)."""
    return task.critical_sections.get(resource, 0)


def resources_used_by_higher(tasks: List[Task], current_index: int) -> set:
    """Return the set of resources used by τ_i or any higher-priority task."""
    result = set()
    for i in range(current_index + 1):
        result |= resources_used_by(tasks[i])
    return result


def compute_pcp_blocking(tasks: List[Task]) -> List[int]:
    """
    PCP blocking: B_i = max over resources of
    (max CS among lower tasks on that resource).
    Only for resources used by τ_i or higher-priority tasks.
    """
    n = len(tasks)
    results = [0] * n

    for i in range(n):
        task = tasks[i]
        higher_resources = resources_used_by_higher(tasks, i)
        lower_tasks = tasks[i + 1:]

        # For each resource, find max CS among lower tasks
        max_per_resource = {}
        for res in higher_resources:
            max_cs = 0
            for lower in lower_tasks:
                cs = max_cs_on_resource(lower, res)
                if cs > max_cs:
                    max_cs = cs
            if max_cs > 0:
                max_per_resource[res] = max_cs

        if max_per_resource:
            blocking = max(max_per_resource.values())
        else:
            blocking = 0
        results[i] = blocking

    return results


def compute_pip_blocking(tasks: List[Task]) -> List[int]:
    """
    PIP blocking: B_i = sum over lower-priority tasks j of
    (max CS of lower task j on any resource used by τ_i or higher).
    Each lower task can block at most once, and the blocking is
    the longest CS of that task on a conflicting resource.
    """
    n = len(tasks)
    results = [0] * n

    for i in range(n):
        task = tasks[i]
        higher_resources = resources_used_by_higher(tasks, i)
        lower_tasks = tasks[i + 1:]

        total_blocking = 0
        for lower in lower_tasks:
            # Max CS of this lower task on ANY resource needed by τ_i or higher
            max_cs = 0
            for res in higher_resources:
                cs = max_cs_on_resource(lower, res)
                if cs > max_cs:
                    max_cs = cs
            total_blocking += max_cs

        results[i] = total_blocking

    return results


def print_table(tasks: List[Task],
                pip_results: List[int],
                pcp_results: List[int]) -> None:
    """Pretty-print the blocking time table."""
    print()
    print(f"{'Task':<6} | R1  R2  R3  R4  R5 | B_PIP | B_PCP")
    print("-" * 50)
    for i, task in enumerate(tasks):
        cs_row = ""
        for res in all_resources:
            val = max_cs_on_resource(task, res)
            cs_row += f"{val:>3} " if val > 0 else "  - "
        print(f"{task.name:<6} | {cs_row}| {pip_results[i]:>5} | {pcp_results[i]:>5}")


def show_pip_calculation(tasks: List[Task], pip_results: List[int]) -> None:
    """Show the step-by-step PIP calculation for each task."""
    n = len(tasks)
    print()
    print("=" * 70)
    print("PIP CALCULATION (step by step)")
    print("=" * 70)
    print()
    print("Formula: B_i = sum over LOWER tasks j of")
    print("  (max CS of j on any resource used by τ_i or higher)")
    print()

    for i in range(n):
        task = tasks[i]
        higher_resources = resources_used_by_higher(tasks, i)
        lower_tasks_list = tasks[i + 1:]

        print(f"── {task.name}: Resources used by τ_{task.name[1:]} or higher = {sorted(higher_resources)}")

        if not lower_tasks_list:
            print(f"   No lower-priority tasks. B = 0")
            print()
            continue

        sum_terms = []
        for lower in lower_tasks_list:
            max_cs = 0
            max_res = ""
            for res in sorted(higher_resources):
                cs = max_cs_on_resource(lower, res)
                if cs > max_cs:
                    max_cs = cs
                    max_res = res
            if max_cs > 0:
                print(f"   Lower task {lower.name}: max CS on {higher_resources} = {max_cs} (on {max_res})")
                sum_terms.append(max_cs)
            else:
                print(f"   Lower task {lower.name}: no conflicting resources")

        total = sum(sum_terms)
        if sum_terms:
            print(f"   B = {' + '.join(str(s) for s in sum_terms)} = {total}")
        print()


def show_pcp_calculation(tasks: List[Task], pcp_results: List[int]) -> None:
    """Show the step-by-step PCP calculation for each task."""
    n = len(tasks)
    print("=" * 70)
    print("PCP CALCULATION (step by step)")
    print("=" * 70)
    print()
    print("Formula: B_i = max over resources of")
    print("  (max CS among lower tasks on that resource)")
    print("  Only considers resources used by τ_i or higher")
    print()

    for i in range(n):
        task = tasks[i]
        higher_resources = resources_used_by_higher(tasks, i)
        lower_tasks_list = tasks[i + 1:]

        print(f"── {task.name}: Relevant resources = {sorted(higher_resources)}")

        if not lower_tasks_list:
            print(f"   No lower-priority tasks. B = 0")
            print()
            continue

        max_values = []
        for res in sorted(higher_resources):
            max_cs = 0
            max_task_name = ""
            for lower in lower_tasks_list:
                cs = max_cs_on_resource(lower, res)
                if cs > max_cs:
                    max_cs = cs
                    max_task_name = lower.name
            if max_cs > 0:
                print(f"   {res}: max CS among lower tasks = {max_cs} (from {max_task_name})")
                max_values.append(max_cs)

        if max_values:
            result = max(max_values)
            print(f"   B = max({', '.join(str(v) for v in max_values)}) = {result}")
        else:
            print(f"   No lower task uses any relevant resource. B = 0")
        print()


if __name__ == "__main__":
    pip = compute_pip_blocking(tasks)
    pcp = compute_pcp_blocking(tasks)

    show_pip_calculation(tasks, pip)
    show_pcp_calculation(tasks, pcp)

    print("=" * 70)
    print("FINAL TABLE")
    print("=" * 70)
    print_table(tasks, pip, pcp)
    print()
