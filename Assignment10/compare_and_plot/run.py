import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg
from matplotlib.figure import Figure
import os

import subprocess
with open('test.txt','w+') as infile:
    subprocess.run('./a8 real_world_prog1_reg.txt < real_world_prog1.txt; ./a8 real_world_prog2_reg.txt < real_world_prog2.txt; ./a8 real_world_prog3_reg.txt < real_world_prog3.txt; ./a9 real_world_prog1_reg.txt < real_world_prog1.txt; ./a9 real_world_prog2_reg.txt < real_world_prog2.txt; ./a9 real_world_prog3_reg.txt < real_world_prog3.txt; ./a10 real_world_prog1_reg.txt 0.9 2 < real_world_prog1.txt ;./a10 real_world_prog2_reg.txt 0.9 2 < real_world_prog2.txt;./a10 real_world_prog3_reg.txt 0.9 2 < real_world_prog3.txt',shell=True)
    line = infile.readlines()[0]
os.remove('test.txt')
arr = line.split()

fig = Figure()
canvas = FigureCanvasAgg(fig)

xvals = ['prog1(non-leaf)', 'prog2(recurse)', 'prog3(leaf)']
yvals = [int(arr[0]),int(arr[1]),int(arr[2])]
yvals2 = [int(arr[3]),int(arr[4]),int(arr[5])]
yvals3 = [int(arr[6]),int(arr[7]),int(arr[8])]

ser = pd.Series(yvals, index=xvals)
ser2 = pd.Series(yvals2, index=xvals)
ser3 =pd.Series(yvals3, index=xvals)

df = pd.DataFrame({'Only stalls':yvals,'Forward + data miss':yvals3,'Forward':yvals2},index = xvals)
ax = df.plot.bar(rot=0)
plt.savefig('test.png')
