from collections.abc import Callable

# TASK AND TASK SET #
class Task:
  def __init__(self, C: int, D: int, T: int, O: int = 0):
    self.C = C
    self.D = D
    self.T = T
    self.O = O

class TaskSet:
  def __init__(self, task_set: list[Task]):
    self.task_set = task_set

    self.i = 0


  def __iter__(self):
    self.i = 0

    return self

  def __next__(self):
    if self.i > len(self.task_set):
      raise StopIteration

    x = self.task_set[self.i]
    self.i += 1

    return x

  def __len__(self):
    return len(self.task_set)

  def __getitem__(self, item: int):
    if item < 0 or item >= len(self.task_set):
      raise IndexError

    return self.task_set[item]

  def __setitem__(self, key: int, value: Task):
    if key < 0 or key >= len(self.task_set):
      raise IndexError

    self.task_set[key] = value
