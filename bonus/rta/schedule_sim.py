"""
Schedule simulation for Question 3 of the real-time systems exam.
Generates Gantt charts for:
  (a) Preemptive Rate Monotonic (RM)
  (b) Preemptive Earliest Deadline First (EDF)
  (c) Non-preemptive Earliest Deadline First (EDF)
"""

import math
from dataclasses import dataclass, field
from typing import List, Optional, Tuple
from prettyTables import Table


# ── Task definition ──────────────────────────────────────────────────────────

@dataclass(frozen=True)
class Task:
    name: str
    wcet: int       # C
    period: int     # T
    deadline: int = None

    def __post_init__(self) -> None:
        if self.deadline is None:
            object.__setattr__(self, "deadline", self.period)


# ── Simulation helpers ───────────────────────────────────────────────────────

@dataclass
class Job:
    task_name: str
    release: int
    deadline: int
    remaining: int

    @property
    def done(self) -> bool:
        return self.remaining <= 0


@dataclass
class Slot:
    """A contiguous run of the same task on the timeline."""
    task: Optional[str]  # None = idle
    start: int
    end: int


# ── Scheduler base ───────────────────────────────────────────────────────────

class SchedulerSim:
    def __init__(self, tasks: List[Task], time_limit: int = 40):
        self.tasks = tasks
        self.time_limit = time_limit
        self.events: List[str] = []

    def simulate(self) -> List[Slot]:
        raise NotImplementedError

    def print_explanation(self) -> None:
        print("\n Schedule walkthrough:")
        for event in self.events:
            print(f"  {event}")

    @staticmethod
    def slots_to_gantt(slots: List[Slot], time_limit: int) -> str:
        """Render a list of Slots as a readable space-separated Gantt chart."""
        task_ids = sorted({s.task for s in slots if s.task is not None})
        timeline: List[Optional[str]] = [None] * time_limit
        for s in slots:
            for t in range(s.start, min(s.end, time_limit)):
                timeline[t] = s.task

        # Header with 5-step markers
        header = "        "
        for t in range(time_limit):
            marker = str(t // 10) if t % 10 == 0 else ""
            header += marker.center(3) if marker else "   "
        header = header.rstrip()

        # Sub-header with units 0-9 repeating
        sub = "        "
        for t in range(time_limit):
            sub += f" {t % 10} "

        # Separator
        sep = "        " + "─" * (time_limit * 3)

        lines = [f"\n{header}", sub, sep]

        for name in task_ids:
            chars = []
            for t in range(time_limit):
                if timeline[t] == name:
                    chars.append(f"[{name}]")
                else:
                    chars.append(" · ")
            lines.append(f" {name}     " + "".join(chars))

        return "\n".join(lines)

    @staticmethod
    def slots_to_table(slots: List[Slot], time_limit: int) -> str:
        """Render a list of Slots as a text table with context switches."""
        table = Table(style_name="pretty_grid", missing_val="?")
        table.add_column("Time", [str(t) for t in range(time_limit)])

        task_ids = sorted({s.task for s in slots if s.task is not None})
        for name in task_ids:
            row: List[str] = []
            for t in range(time_limit):
                running = None
                for s in slots:
                    if s.start <= t < s.end and s.task == name:
                        running = name
                        break
                row.append("█" if running else " ")
            table.add_column(name, row)

        # Context switch row
        cs_row: List[str] = []
        prev = None
        for t in range(time_limit):
            curr = None
            for s in slots:
                if s.start <= t < s.end:
                    curr = s.task
                    break
            cs_row.append("⇄" if prev is not None and curr != prev and curr is not None else " ")
            prev = curr
        table.add_column("CS", cs_row)

        return str(table)


# ── Preemptive Rate Monotonic ────────────────────────────────────────────────

class RMSim(SchedulerSim):
    def simulate(self) -> List[Slot]:
        # Priority order: shortest period = highest priority
        priority = sorted(self.tasks, key=lambda t: t.period)
        priority_names = [t.name for t in priority]
        prio_map = {t.name: i for i, t in enumerate(priority)}

        self.events.append(f"RM priority order (highest → lowest): {', '.join(priority_names)}")
        self.events.append("")
        self.events.append("  Task     | Period  | Priority")
        self.events.append("  ─────────┼─────────┼──────────")
        for i, t in enumerate(priority):
            self.events.append(f"  {t.name:<8} | {t.period:<7} | {i+1}")

        jobs: List[Job] = []
        slots: List[Slot] = []
        last_name: Optional[str] = None
        last_start = 0

        for t in range(self.time_limit):
            for task in self.tasks:
                if t % task.period == 0:
                    jobs.append(Job(task.name, t, t + task.deadline, task.wcet))

            jobs = [j for j in jobs if not j.done]

            if jobs:
                jobs.sort(key=lambda j: prio_map[j.task_name])
                current = jobs[0]
                current_name = current.task_name
            else:
                current = None
                current_name = None

            if current_name != last_name:
                if last_name is not None:
                    slots.append(Slot(last_name, last_start, t))
                    self.events.append(
                        f"  t={last_start:>2} - {t:<2} : {last_name} runs"
                    )
                    if current_name is not None:
                        self.events.append(
                            f"  t={t:>2}         : {current_name} preempts "
                            f"(higher priority, period={[x.period for x in self.tasks if x.name==current_name][0]})"
                        )
                elif t > 0:
                    slots.append(Slot(last_name, last_start, t))
                if current_name is not None and last_name is None:
                    self.events.append(
                        f"  t={t:>2}         : {current_name} starts "
                        f"(highest priority ready)"
                    )
                last_name = current_name
                last_start = t

            if current is not None:
                current.remaining -= 1
                if current.done:
                    self.events.append(f"  t={t+1:>2}         : {current.task_name} finishes (C={[x.wcet for x in self.tasks if x.name==current.task_name][0]})")
                    jobs.remove(current)

        slots.append(Slot(last_name, last_start, self.time_limit))
        if last_name is not None:
            self.events.append(
                f"  t={last_start:>2} - {self.time_limit:<2}: {last_name} runs (end of window)"
            )

        return slots


# ── Preemptive EDF ───────────────────────────────────────────────────────────

class EDFPreemptiveSim(SchedulerSim):
    def simulate(self) -> List[Slot]:
        self.events.append("EDF picks the ready task with the EARLIEST ABSOLUTE DEADLINE")
        self.events.append("")

        jobs: List[Job] = []
        slots: List[Slot] = []
        last_name: Optional[str] = None
        last_start = 0

        for t in range(self.time_limit):
            for task in self.tasks:
                if t % task.period == 0:
                    jobs.append(Job(task.name, t, t + task.deadline, task.wcet))

            jobs = [j for j in jobs if not j.done]

            if jobs:
                jobs.sort(key=lambda j: j.deadline)
                current = jobs[0]
                current_name = current.task_name
            else:
                current = None
                current_name = None

            if current_name != last_name:
                if last_name is not None:
                    slots.append(Slot(last_name, last_start, t))
                    self.events.append(
                        f"  t={last_start:>2} - {t:<2} : {last_name} runs"
                    )
                    if current_name is not None:
                        self.events.append(
                            f"  t={t:>2}         : {current_name} preempts "
                            f"(earlier deadline d={current.deadline})"
                        )
                elif t > 0:
                    slots.append(Slot(last_name, last_start, t))
                if current_name is not None and last_name is None:
                    self.events.append(
                        f"  t={t:>2}         : {current_name} starts "
                        f"(deadline={current.deadline}, earliest in ready queue)"
                    )
                last_name = current_name
                last_start = t

            if current is not None:
                current.remaining -= 1
                if current.done:
                    self.events.append(
                        f"  t={t+1:>2}         : {current.task_name} finishes "
                        f"(deadline={current.deadline}, met)"
                    )
                    jobs.remove(current)

        slots.append(Slot(last_name, last_start, self.time_limit))
        if last_name is not None:
            self.events.append(
                f"  t={last_start:>2} - {self.time_limit:<2}: {last_name} runs (end of window)"
            )

        return slots


# ── Non-preemptive EDF ───────────────────────────────────────────────────────

class EDFNonPreemptiveSim(SchedulerSim):
    def simulate(self) -> List[Slot]:
        self.events.append("Non-preemptive EDF: once a task starts, it runs to completion.")
        self.events.append("Scheduling decisions only happen at task completion.")
        self.events.append("")

        jobs: List[Job] = []
        slots: List[Slot] = []
        running: Optional[Job] = None
        last_name: Optional[str] = None
        last_start = 0
        running_name: Optional[str] = None
        just_finished = False

        for t in range(self.time_limit):
            for task in self.tasks:
                if t % task.period == 0:
                    jobs.append(Job(task.name, t, t + task.deadline, task.wcet))

            # If job just finished, log the slot and pick next
            if just_finished and running is not None:
                slots.append(Slot(running_name, last_start, t))
                self.events.append(
                    f"  t={last_start:>2} - {t:<2} : {running_name} completes its execution"
                )
                running = None
                running_name = None
                last_name = None
                last_start = t
                just_finished = False

            # Remove any done jobs from the list
            jobs = [j for j in jobs if not j.done]

            # If nothing running, pick the earliest-deadline ready job
            if running is None:
                ready = [j for j in jobs if j.remaining > 0]
                if ready:
                    ready.sort(key=lambda j: j.deadline)
                    running = ready[0]
                    running_name = running.task_name
                    last_name = running_name
                    last_start = t
                    self.events.append(
                        f"  t={t:>2}         : {running_name} starts (deadline={running.deadline}, earliest ready)"
                    )
                else:
                    if last_name is not None:
                        slots.append(Slot(last_name, last_start, t))
                        last_name = None
                        last_start = t

            # Execute 1 unit
            if running is not None:
                running.remaining -= 1
                if running.done:
                    just_finished = True

        # Final slot
        if running is not None or last_name is not None:
            name = running_name if running is not None else last_name
            slots.append(Slot(name, last_start, self.time_limit))
            self.events.append(
                f"  t={last_start:>2} - {self.time_limit:<2}: {name} running (end of window)"
            )
        else:
            slots.append(Slot(None, last_start, self.time_limit))

        return slots


# ── Main ─────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    tasks = [
        Task("a", 7, 40),
        Task("b", 2, 8),
        Task("c", 3, 10),
        Task("d", 4, 18),
    ]

    TIME_LIMIT = 40

    sims: List[Tuple[str, SchedulerSim]] = [
        ("(a) Preemptive RM", RMSim(tasks, TIME_LIMIT)),
        ("(b) Preemptive EDF", EDFPreemptiveSim(tasks, TIME_LIMIT)),
        ("(c) Non-preemptive EDF", EDFNonPreemptiveSim(tasks, TIME_LIMIT)),
    ]

    for label, sim in sims:
        print(f"\n{'=' * 60}")
        print(f" {label}")
        print(f"{'=' * 60}")
        slots = sim.simulate()
        sim.print_explanation()
        print()
        print(SchedulerSim.slots_to_gantt(slots, TIME_LIMIT))
        print()
        events = [s for s in slots if s.task is not None]
        cs_count = 0
        for i in range(1, len(events)):
            if events[i].task != events[i - 1].task:
                cs_count += 1
        print(f" Context switches: {cs_count}")
        slot_strs = []
        for s in slots:
            name = s.task if s.task is not None else "idle"
            slot_strs.append(f"{name}[{s.start}-{s.end})")
        print(f" Timeline slots: {slot_strs}")
