import numpy as np
from matplotlib import pyplot as plt
from sys import argv
import re

with open(argv[1], "r") as f:
    raw_data = f.read()

header_data = re.findall(r"Iteration: (\d+): eval_loss (\d+(?:.\d+)?(?:e\-\d+)?), pred_loss (\d+(?:.\d+)?(?:e\-\d+)?), negamax_iters (\d+)", raw_data)
iters = np.array([float(hd[0]) for hd in header_data])
eval_loss = np.array([float(hd[1]) for hd in header_data])
pred_loss = np.array([float(hd[2]) for hd in header_data])
negamax_iters = np.array([float(hd[3]) for hd in header_data])

learned_data = re.findall(r"Iteration:(?s:.)*?Eval table: ((?:\-?\d+, )+\-?\d+)(?s:.)*?Pred table: ((?:\-?\d+, )+\-?\d+)", raw_data)
eval_params = np.array([[float(p) for p in r[0].split(", ")] for r in learned_data])
pred_params = np.array([[float(p) for p in r[1].split(", ")] for r in learned_data])

print(f"Loaded {iters[-1]} iters!")

plt.plot(iters, eval_loss, label="Eval loss")
plt.plot(iters, pred_loss, label="Pred loss")
plt.legend()
plt.show(block=False)

plt.figure()
for i in range(12):
    plt.plot(iters, eval_params[:,i], label=f"{i}")
plt.yscale("symlog")
plt.title("Eval params")
plt.legend()
plt.show(block=False)

plt.figure()
for i in range(12):
    plt.plot(iters, pred_params[:,i], label=f"{i}")
plt.yscale("symlog")
plt.title("Pred params")
plt.legend()
plt.show(block=True)