import numpy as np
from matplotlib import pyplot as plt

vals = np.array([
    [1e8,1e12],
    [-1e8, -1e12],
    [-1000, 1000],
    [60000, 80000],
    [-60000, -80000],
    [-20000, -30000],
    [-1000,0],
    [0,1000],
    [-10,10]
])

def transform_val(vals):
    #avals = np.arcsinh(vals)
    #most_important = np.arcsinh(10000)
    #avals = np.tanh((avals-most_important)*0.5) + np.tanh((avals+most_important)*0.5)
    #return avals
    #return np.tanh(np.arcsinh(vals/100)/8)
    return np.tanh(np.arcsinh(vals)/9)
    #return np.arcsinh(vals/100)

avals = transform_val(vals)
print(avals)
means = np.mean(avals, axis=1)
errors = np.sum(np.abs((avals.T - means).T), axis=1)
print(errors)

x = np.logspace(-1,13)
tx = transform_val(x)
tx2 = transform_val(-x)
plt.plot(x,tx)
plt.plot(-x,tx2)
plt.xscale("symlog")
plt.show()