import os
from matplotlib import pyplot as plt
import numpy as np
from scipy.signal import savgol_filter

pntsArray = []
origDynamic = list(map(float, open("./outputs/1/dynamic.out").read().split(",")))
origQueue = list(map(float, open("./outputs/1/queue.out").read().split(",")))
d = []
q = []
t = []
x = []
for subdir, dirs, files in os.walk('outputs'):
    ans = []
    for file in files:
        print(file)
        if file == 'dynamic.out':
            currentDynamic = list(map(float, open(os.path.join(subdir, file)).read().split(",")))
            if (len(currentDynamic) > len(origDynamic)):
                currentDynamic = currentDynamic[:len(origDynamic)]
            else:
                origDynamic = origDynamic[:len(currentDynamic)]
            #ans += [np.divide(np.abs(np.subtract(currentDynamic,origDynamic))*100, origDynamic).mean()]
            ans += [np.abs(np.subtract(currentDynamic,origDynamic)).mean()]
        elif file == 'queue.out':
            currentqueue = list(
                map(float, open(os.path.join(subdir, file)).read().split(",")))
            if (len(currentqueue) > len(origQueue)):
                currentqueue = currentqueue[:len(origQueue)]
            else:
                origQueue = origQueue[:len(currentqueue)]
            #ans += [np.divide(np.abs(np.subtract(currentqueue,origQueue))*100, origQueue).mean()]
            ans += [np.abs(np.subtract(currentqueue,origQueue)).mean()]
        elif file == 'timetaken.out':
            x = float(open(os.path.join(subdir, file)).read())
            ans += [int(subdir[subdir.index('/')+1:]), x]
            # x = float(open(os.path.join(subdir, file)).read())
            # #print("xx" + subdir[10:12])
            # ans += [int(subdir[-2:]), x]
            # #print(ans)
            pntsArray += [ans]

pntsArray.sort(key=lambda x : x[0])
# x = []
# y = []
print(pntsArray)
x = []
y = []
z = []
w = []
for i in pntsArray:
    z += [i[2]]
    y += [i[3]]
    x += [i[0]]
    w += [i[1]]
plt.plot(x, z)
plt.xlabel("Number of threads")
plt.ylabel('Error in Dynamic Density')
#plt.show()
plt.savefig("dynamic error")
plt.close()
plt.plot(x, y)
plt.xlabel("Number of threads")
plt.ylabel('Error in Queue Density')
#plt.show()
plt.savefig("queue error")
plt.close()
plt.plot(x, w)
plt.xlabel("Number of threads")
plt.ylabel('Execution time')
#plt.show()
plt.savefig("time_taken")
plt.close()