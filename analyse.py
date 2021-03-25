import os
from matplotlib import pyplot as plt
import numpy as np
from scipy.signal import savgol_filter


def takeThird(a):
    return a[2]


def diffCalc(arr1, arr2):
    ans = 0
    for i in range(len(arr1)):
        ans += abs(arr1[i]-arr2[i])/arr1[i]
        print(ans)
    return ans


pntsArray = []
dynamic = open("./outputs/4/dynamic.out", "r")
dynamicArray = list(map(float, dynamic.read().split(",")))
dynamicArray = savgol_filter(dynamicArray, 17, 2)
for i in dynamicArray:
    if(i < 0):
        print(i)

static = open("./outputs/4/static.out", "r")
staticArray = list(map(float, static.read().split(",")))


for subdir, dirs, files in os.walk('outputs'):
    ans = []

    for file in files:
        print(file)
        if file == 'dynamic.out':
            currentDynamic = list(
                map(float, open(os.path.join(subdir, file)).read().split(",")))
            currentDynamic = savgol_filter(currentDynamic, 17, 2)
            ans += [np.divide(np.abs(np.subtract(currentDynamic,
                                                 dynamicArray))*100, dynamicArray).mean()]
            # ans += [diffCalc(dynamicArray, currentDynamic)]
        elif file == 'static.out':
            currentStatic = list(
                map(float, open(os.path.join(subdir, file)).read().split(",")))
            ans += [np.divide(np.abs(np.subtract(staticArray,
                                                 currentStatic))*100, staticArray).mean()]
            # ans += [diffCalc(staticArray, currentStatic)]
        elif file == 'timetaken.out':
            x = float(open(os.path.join(subdir, file)).read()[23:29])
            ans += [int(subdir[8:]), x]
            print(ans)
            pntsArray += [ans]
pntsArray.sort(key=takeThird)
x = []
y = []
w = []
z = []
for i in pntsArray:
    w += [i[0]]
    x += [i[1]]
    y += [i[2]]
    z += [i[3]]

print(x)
print(y)
print("A")
print(pntsArray)
# plt.plot(y, x, label='Static Error')
# plt.plot(y, w, label='Dynamic Error')
# plt.xlabel('Parameter (x where N+x are processed)')
# plt.ylabel('Error')
# plt.legend()

# plt.show()


# plt.xlabel('Parameter (x where N+x are processed)')
# plt.ylabel('Dynamic Error')
# plt.show()

plt.plot(y, z)
plt.xlabel('Parameter (x where N+x are processed)')
plt.ylabel('Time taken')
plt.show()
