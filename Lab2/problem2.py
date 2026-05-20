from math import floor, ceil

from common import Task, TaskSet
from helper import sign

class Task_Problem2(Task):
  def __init__(self, C: int, D: int, T: int):
    super().__init__(C, D, T)

    self.weight = self.C / self.T
    self.lag = 0.0

  def get_lag_x_period(self):
    return self.lag * self.T

class TaskSet_Problem2(TaskSet):
  def __init__(self, task_set: list[Task_Problem2]):
    super().__init__(task_set)

    self.urgent_tasks: list[Task_Problem2] = []
    self.contending_tasks: list[Task_Problem2] = []
    self.tnegru_tasks: list[Task_Problem2] = []


def get_characteristic_string(task: Task_Problem2, t: int):
  characteristic_string = sign(task.weight * (t + 1) - floor(task.weight * t) - 1)

  return "-" if characteristic_string < 0 else ("+" if characteristic_string > 0 else "0")


def get_characteristic_substring(task: Task_Problem2, t: int):
  substring = ""

  i = 0
  while True:
    characteristic_string = get_characteristic_string(task, i)

    substring += characteristic_string

    if i > t and characteristic_string == "0":
      break

    i += 1

  return substring


task_set = TaskSet_Problem2([
  Task_Problem2(3, 9, 9),
  Task_Problem2(3, 9, 9),
  Task_Problem2(2, 3, 3),
  Task_Problem2(6, 9, 9)
])

hyper_period = 9
m = 2


print("|%-3.3s|%-23.23s|%-23.23s|%-12.12s|%-20.20s|%-12.12s|" % (
  "", "Lag x period", "Characteristic String", "Urgent", "Contending", "Tnegru"))
print("|%-3.3s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-12.12s|%-20.20s|%-12.12s|" % (
  "t", "τ1", "τ2", "τ3", "τ4", "τ1", "τ2", "τ3", "τ4", "tasks", "tasks", "tasks"))
print("-"*82)

for t in range(hyper_period):
  characteristic_string = [
    get_characteristic_string(task_set[0], t),
    get_characteristic_string(task_set[1], t),
    get_characteristic_string(task_set[2], t),
    get_characteristic_string(task_set[3], t)
  ]

  characteristic_substrings = [
    get_characteristic_substring(task_set[0], t),
    get_characteristic_substring(task_set[1], t),
    get_characteristic_substring(task_set[2], t),
    get_characteristic_substring(task_set[3], t)
  ]

  period_x_lag = [
    task_set[0].T * task_set[0].lag,
    task_set[1].T * task_set[1].lag,
    task_set[2].T * task_set[2].lag,
    task_set[3].T * task_set[3].lag
  ]



  # Calculate which tasks correspond to which set
  tasks = [0, 1, 2, 3]
  urgent_tasks: list[int] = []
  contending_tasks: list[int] = []
  tnegru_tasks: list[int] = []

  for i in tasks:
    if task_set[i].lag > 0 and (characteristic_string[i] == "+" or characteristic_string[i] == "0"):
      urgent_tasks.append(i)
    elif task_set[i].lag < 0 and (characteristic_string[i] == "-" or characteristic_string[i] == "0"):
      tnegru_tasks.append(i)
    else:
      contending_tasks.append(i)


  urgent_tasks_string = ",".join(map(lambda i: "τ%s" % (i + 1), urgent_tasks))
  tnegru_tasks_string = ",".join(map(lambda i: "τ%s" % (i + 1), tnegru_tasks))

  contending_tasks_string = ""

  # Sort by the characteristic string order.
  def contending_tasks_sort(i: int):
    characteristic_substring = characteristic_substrings[i]

    def char_value(c: str):
      if c == "-":
        return 1
      elif c == "0":
        return 2
      elif c == "+":
        return 3
      else:
        return 0

    return [char_value(c) for c in characteristic_substring]


  contending_tasks = sorted(contending_tasks, key=contending_tasks_sort)

  if len(contending_tasks) > 0:
    contending_tasks_string += "τ%s" % (contending_tasks[0] + 1)

  for i in range(len(contending_tasks) - 1):
    i1 = contending_tasks[i]
    i2 = contending_tasks[i + 1]

    if characteristic_string[i1] != characteristic_string[i2]:
      contending_tasks_string += " > τ%s" % (i2 + 1)
    else:
      contending_tasks_string += " = τ%s" % (i2 + 1)

  print(
    "|%-3.3s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-5.5s|%-12.12s|%-20.20s|%-12.12s|" % (
      t,
      period_x_lag[0],
      period_x_lag[1],
      period_x_lag[2],
      period_x_lag[3],
      characteristic_string[0],
      characteristic_string[1],
      characteristic_string[2],
      characteristic_string[3],
      "{%s}" % urgent_tasks_string,
      "%s" % contending_tasks_string,
      "{%s}" % tnegru_tasks_string))


  chosen_tasks = []

  for i in urgent_tasks:
    if len(chosen_tasks) >= m:
      break

    chosen_tasks.append(i)

  for i in contending_tasks:
    if len(chosen_tasks) >= m:
      break

    chosen_tasks.append(i)

  for i in tasks:
    if i in chosen_tasks:
      task_set[i].lag -= (1 - task_set[i].weight)
    else:
      task_set[i].lag += (1 - task_set[i].weight)








