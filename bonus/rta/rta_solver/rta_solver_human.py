"""
# +==== BEGIN AsperHeader =================+
# LOGO:
# ..........####...####..........
# ......###.....#.#########......
# ....##........#.###########....
# ...#..........#.############...
# ...#..........#.#####.######...
# ..#.....##....#.###..#...####..
# .#.....#.##...#.##..##########.
# #.....##########....##...######
# #.....#...##..#.##..####.######
# .#...##....##.#.##..###..#####.
# ..#.##......#.#.####...######..
# ..#...........#.#############..
# ..#...........#.#############..
# ...##.........#.############...
# ......#.......#.#########......
# .......#......#.########.......
# .........#####...#####.........
# /STOP
# PROJECT: AsperHeader
# FILE: rta_solder_human.py
# CREATION DATE: 19-07-2026
# LAST Modified: 11:18:54 20-07-2026
# DESCRIPTION:
# This is a script that aims to resolve wether the given priorities are schedulable in a fixed test-response time or not.
# /STOP
# COPYRIGHT: (c) Asperguide
# PURPOSE:
# // AR
# +==== END AsperHeader =================+
"""
# Response Time Analysis (RTA) for fixed-priority scheduling (RM)

import math
from typing import Any, List, Optional, Union
from dataclasses import dataclass
from prettyTables import Table


@dataclass(frozen=True)
class Task:
    task: str  # Task name
    period: int  # (T) Time period
    wcet: int  # (C) Computation time
    deadline: int  # (D) Deadline (defaults to period if not set)

    def to_list(self) -> List[Union[str, int]]:
        result = []
        result.append(self.task)
        result.append(self.period)
        result.append(self.wcet)
        return result


@dataclass(frozen=True)
class FixedPriority:
    task: str
    response_time_analysis: int  # (R) or (RTA)
    deadline: int  # (T)
    status: bool  # Does it meet or miss it

    def to_list(self) -> List[Union[str, int]]:
        result = []
        result.append(self.task)
        result.append(self.response_time_analysis)
        result.append(self.deadline)
        result.append(self.status)
        return result


@dataclass(frozen=True)
class DMSResult:
    task: str
    response_time: int
    deadline: int
    status: bool

    def to_list(self) -> List[Union[str, int]]:
        result = []
        result.append(self.task)
        result.append(self.response_time)
        result.append(self.deadline)
        result.append(self.status)
        return result


class FixedPrioritySchedule:
    def __init__(self, sorted_tasks: List[Task]) -> None:
        self.tasks = sorted_tasks.copy()
        self.fixed_priority_results: List[FixedPriority] = []
        self.display: Table = Table()
        self.columns_added: bool = False
        self._reset_table()
        self.calculate()

    def __call__(self, *args: Any, **kwds: Any) -> None:
        self.display_result()

    def _reset_table(self) -> None:
        self.display = Table(style_name="pretty_grid", missing_val="?")

    def _add_row(self, node: FixedPriority) -> None:
        if self.columns_added is False:
            self.display.add_column("Task", [node.task])
            self.display.add_column(
                "R (response time)",
                [node.response_time_analysis]
            )
            self.display.add_column("T (deadline)", [node.deadline])
            self.display.add_column("Schedulable", [node.status])
            self.columns_added = True
        else:
            self.display.add_row(node.to_list())

    def _calculate_iteration(self, task: Task, resulting_tasks: List[Task], r_task_prev: int) -> int:
        """ The operation we are performing is R = task.C (+ math.ceil(R_prev/T)*C for task in resulting_tasks) """
        result = task.wcet
        for item in resulting_tasks:
            result += math.ceil(r_task_prev/item.period)*item.wcet
        return result

    def _calculate_fixed_priority_schedule_node(self, task: Task, resulting_tasks: List[Task]) -> FixedPriority:
        """ for the first round we will assume that the worst case execution time is the response time analysis (RTA) of the task we wish to calculate"""
        iteration_count = 1
        R_task = task.wcet
        R_task_prev = task.wcet-1
        while (R_task != R_task_prev and R_task < task.period):
            R_task_prev = R_task
            R_task = self._calculate_iteration(
                task,
                resulting_tasks,
                R_task_prev
            )
            iteration_count += 1
        status = False
        if R_task == R_task_prev:
            status = True
            msg = f"{self.__class__.__name__}: iteration n°{iteration_count}: {task.task}: R = {R_task} ≤ T={task.period} → SCHEDULABLE"
        else:
            msg = f"{self.__class__.__name__}: iteration n°{iteration_count}: {task.task}: R = {R_task} > T={task.period} → NOT SCHEDULABLE"
        print(msg)
        result_priority: FixedPriority = FixedPriority(
            task.task,
            R_task,
            task.period,
            status
        )
        return result_priority

    def calculate(self, sorted_tasks: Optional[List[Task]] = None) -> None:
        """ the entry function used to calculate the operation offered by the class """
        if sorted_tasks is not None:
            self.tasks = sorted_tasks
        self._reset_table()
        self.fixed_priority_results.clear()
        for index, task in enumerate(self.tasks):
            node = self._calculate_fixed_priority_schedule_node(
                task,
                self.tasks[:index]
            )
            self.fixed_priority_results.append(node)
            self._add_row(node)

    def display_result(self) -> None:
        """ Display the calculated results """
        print("Fixed Priority Schedule results")
        print(self.display)


class EDFSchedule:
    def __init__(self, tasks: List[Task], cores: int = 1) -> None:
        self.tasks = tasks.copy()
        self.cores = cores
        self.result = 0
        self.calculate()

    def __call__(self, *args: Any, **kwds: Any) -> None:
        self.display_result()

    def _calculate_task(self, task: Task) -> Union[int, float]:
        return task.wcet/task.period

    def calculate(self) -> None:
        """ The operation we perform is SUM(computation_time/period for task in tasks) """
        result = 0
        for task in self.tasks:
            result += self._calculate_task(task)
        self.result = result

    def display_result(self) -> None:
        """ display the calculated results """
        print(f"Earliest Deadline First (EDF) result: {self.result}")
        print(f"EDF ≤ {self.cores}: {self.result <= self.cores}")


class DeadlineMonotonicSchedule:
    def __init__(self, tasks: List[Task]) -> None:
        self.tasks = self._sort_by_dealine(tasks.copy())
        self.results: List[DMSResult] = []
        self.display = Table()
        self.columns_added = False
        self._reset_table()
        self.calculate()

    def __call__(self, *args: Any, **kwds: Any) -> None:
        self.display_result()

    def _reset_table(self) -> None:
        self.display = Table(style_name="pretty_grid", missing_val="?")

    def _sort_by_dealine(self, tasks: List[Task]) -> List[Task]:
        result: List[Task] = []
        src: List[Task] = tasks.copy()
        highest_deadline = 0
        item_index = 0
        while len(src) > 0:
            highest_deadline = 0
            item_index = 0
            for index, item in enumerate(src):
                if item.deadline > highest_deadline:
                    highest_deadline = item.deadline
                    item_index = index
            if (len(src) > 0):
                result.append(src.pop(item_index))
        self.tasks = result
        return result

    def _add_row(self, result: DMSResult) -> None:
        if self.columns_added is False:
            self.display.add_column("Task", [result.task])
            self.display.add_column(
                "R (response time)",
                [result.response_time]
            )
            self.display.add_column("D (deadline)", [result.deadline])
            self.display.add_column("Schedulable", [result.status])
            self.columns_added = True
        else:
            self.display.add_row(result.to_list())

    def _calculate_interference_node(self, R_prev: Union[int, float], h: Task) -> int:
        return math.ceil(R_prev/h.period) * h.wcet

    def _calculate_interference(self, R_prev: Union[int, float], higher: List[Task]) -> int:
        """ Calculate the interference of a higher priority task h on the task we are analyzing """
        interference = 0
        for h in higher:
            interference += self._calculate_interference_node(R_prev, h)
        return interference

    def _calculate_response_time(self, task: Task, higher: List[Task]) -> int:
        """ Calculate the response time of a task given the higher priority tasks """
        R = task.wcet + self._calculate_interference(task.wcet, higher)
        return R

    def _determine_schedulability(self, R: int, task: Task) -> bool:
        return R <= task.deadline

    def _rta_for_task(self, task: Task, higher: List[Task]) -> DMSResult:
        R_prev = task.wcet
        R = self._calculate_response_time(task, higher)
        while (R != R_prev and R_prev <= task.deadline):
            R = self._calculate_response_time(task, higher)
            R_prev = R
        if R > task.deadline:
            return DMSResult(task.task, R, task.deadline, False)
        status = self._determine_schedulability(R, task)
        return DMSResult(task.task, R, task.deadline, status)

    def calculate(self) -> None:
        self.results.clear()
        for i, task in enumerate(self.tasks):
            result = self._rta_for_task(task, self.tasks[:i])
            self.results.append(result)
            self._add_row(result)

    def display_result(self) -> None:
        print("Deadline Monotonic Schedule results")
        print(self.display)


class LeastLaxityFirstSchedule:
    def __init__(self, tasks: List[Task], cores: int = 1) -> None:
        self.tasks = tasks.copy()
        self.cores = cores
        self.result = 0.0
        self.calculate()

    def __call__(self, *args: Any, **kwds: Any) -> None:
        self.display_result()

    def calculate(self) -> None:
        result = 0
        for i in self.tasks:
            result += i.wcet / i.period
        self.result = result

    def display_result(self) -> None:
        schedulable_str = 'NOT SCHEDULABLE'
        if self.result <= self.cores:
            schedulable_str = 'SCHEDULABLE'
        analysis = f"LLF ≤ {self.cores}: {self.result <= self.cores} → {schedulable_str}"
        print(f"Least Laxity First (LLF) - Utilization: {self.result:.5f}")
        print(analysis)


class RMBoundUtilization:
    def __init__(self, tasks: List[Task]) -> None:
        self.tasks = tasks
        self.total_util = sum(t.wcet / t.period for t in tasks)
        self.bound = 0
        self.calculate()

    def __call__(self, *args: Any, **kwds: Any) -> None:
        self.display_result()

    def calculate(self) -> None:
        task_len = len(self.tasks)
        result = task_len * (2 ** (1/task_len) - 1)
        self.bound = result

    def display_result(self) -> None:
        print("--- RM Utilization Bound ---")
        print(f"Total Utilization (U): {self.total_util:.5f}")
        print(f"RM Theoretical Bound (n(2^(1/n)-1)): {self.bound:.5f}")
        if self.total_util <= self.bound:
            print("U ≤ Bound → Sufficient condition PASSES (task set IS schedulable)")
        else:
            print("U > Bound → Sufficient condition FAILS (RTA needed for exact result)")


class Schedulability:
    def __init__(self, tasks: List[Task]) -> None:
        self.tasks = tasks.copy()
        self.fixed_priority_results: List[FixedPriority] = []
        self.display: Table = Table()
        self.columns_added: bool = False
        self._reset_table()
        self.sort_by_rate_monotonic()
        self.fixed_priority_schedule: FixedPrioritySchedule = FixedPrioritySchedule(
            self.tasks
        )
        self.earliest_deadline_first: EDFSchedule = EDFSchedule(self.tasks)
        self.deadline_monotonic: DeadlineMonotonicSchedule = DeadlineMonotonicSchedule(
            self.tasks
        )
        self.least_laxity_first: LeastLaxityFirstSchedule = LeastLaxityFirstSchedule(
            self.tasks
        )
        self.rm_bound: RMBoundUtilization = RMBoundUtilization(self.tasks)

    def __call__(self, *args: Any, **kwds: Any) -> None:
        self.display_result()
        self.rm_bound()
        self.fixed_priority_schedule()
        self.earliest_deadline_first()
        self.deadline_monotonic()
        self.least_laxity_first()

    def _reset_table(self) -> None:
        self.display = Table(
            style_name="pretty_grid",
            missing_val="?"
        )

    def _find_task_index(self, node: Task) -> int:
        node_index = 0
        for index, item in enumerate(self.tasks):
            if item.task == node.task:
                node_index = index
                break
        return node_index

    def _add_row(self, node: Task) -> None:
        node_index = self._find_task_index(node)
        if self.columns_added is False:
            self.display.add_column(
                "Priority", [node_index + 1]
            )
            self.display.add_column("Task", [node.task])
            self.display.add_column("Period (T)", [node.period])
            self.display.add_column("WCET (C)", [node.wcet])
            self.columns_added = True
        else:
            e = node.to_list()
            e.insert(0, node_index + 1)
            self.display.add_row(e)

    def sort_by_rate_monotonic(self) -> None:
        """Sort tasks by Rate Monotonic priority (shortest period first)."""
        result: List[Task] = []
        highest_period: int = 0
        highest_index: int = 0
        while len(self.tasks) > 0:
            highest_index = 0
            highest_period = 0
            for index, item in enumerate(self.tasks):
                if highest_period < item.period:
                    highest_period = item.period
                    highest_index = index
            if (len(self.tasks) > 0):
                result.append(self.tasks.pop(highest_index))
        result.reverse()
        self.tasks = result
        for i in result:
            self._add_row(i)

    def display_result(self) -> None:
        print("Monotonic Rate sorted tasks")
        print(self.display)


if __name__ == "__main__":
    # Deadline is assumed to be the same as Period because we are in a fixed priority logic
    TASK_INPUT: List[Task] = [
        Task("a", 2, 10, 2),
        Task("b", 3, 9, 3),
        Task("c", 4, 13, 4)
    ]
    SI = Schedulability(TASK_INPUT)
    SI()
