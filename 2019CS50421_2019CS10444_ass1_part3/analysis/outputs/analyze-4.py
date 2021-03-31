import os
from matplotlib import pyplot as plt
import numpy as np
from scipy.signal import savgol_filter

def divide_true( a, b ):
    with np.errstate(divide='ignore', invalid='ignore'):
        c = np.true_divide( a, b )
        c[ ~ np.isfinite( c )] = 0  # -inf inf NaN
    return c

pntsArray = []
origDynamic = list(map(float, open("./method_4/0/dynamic.out").read().split(",")))
origQueue = list(map(float, open("./method_4/0/static.out").read().split(",")))
d = []
q = []
t = []
x = []
for subdir, dirs, files in os.walk('method_4'):
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
            ans += [100*np.abs(divide_true(np.subtract(currentDynamic,origDynamic), origDynamic)).mean()]
        elif file == 'static.out':
            currentqueue = list(
                map(float, open(os.path.join(subdir, file)).read().split(",")))
            if (len(currentqueue) > len(origQueue)):
                currentqueue = currentqueue[:len(origQueue)]
            else:
                origQueue = origQueue[:len(currentqueue)]
            #ans += [np.divide(np.abs(np.subtract(currentqueue,origQueue))*100, origQueue).mean()]
            ans += [100*np.abs(divide_true(np.subtract(currentqueue,origQueue), origQueue)).mean()]
        elif file == 'timetaken.out':
            x = float(open(os.path.join(subdir, file)).read())
            print(subdir)
            print(subdir.index('/'))
            ans += [int(subdir[subdir.index('/')+1:]), x]
            # x = float(open(os.path.join(subdir, file)).read())
            # #print("xx" + subdir[10:12])
            # ans += [int(subdir[-2:]), x]
            # #print(ans)
            pntsArray += [ans]
print(pntsArray)
pntsArray.sort(key=lambda x : x[2])
# x = []
# y = []
print(pntsArray)
x = []	# static error
y = []	# Parameter array
z = []	# time taken
w = []	# dynamic error
for i in pntsArray:
    z += [i[3]]
    y += [i[2]]
    x += [i[1]]
    w += [i[0]]
print("W")
print(w)
print("X")
print(x)
print("Y")
print(y)
print("Z")
print(z)

plt.plot(y, w)
plt.xlabel("Number of threads")
plt.ylabel('% Error in Dynamic Density')
#plt.show()
plt.savefig("./method_4/dynamic error")
plt.close()
plt.plot(y, x)
plt.xlabel("Number of threads")
plt.ylabel('% Error in Queue Density')
#plt.show()
plt.savefig("./method_4/queue error")
plt.close()
plt.plot(y, z)
plt.xlabel("Number of threads")
plt.ylabel('Execution time (sec)')
#plt.show()
plt.savefig("./method_4/time_taken")
plt.close()