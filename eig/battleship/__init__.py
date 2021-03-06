from .parser import Parser
from .program import ProgramSyntaxError
#TODO: cpp version?
from .python.priors import EqualSizesDistribution
import traceback

try:
    from .cython.executor import Executor
    from .cython.hypothesis import Ship, BattleshipHypothesis, BattleshipHypothesisSpace
except ImportError as e:
    print("[warning] Cython version cannot be loaded. Pure python mode is used.")
    print(e)
    from .python.executor import Executor
    from .python.hypothesis import *
