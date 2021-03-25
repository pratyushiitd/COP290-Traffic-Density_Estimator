import os
from matplotlib import pyplot as plt
pntsArray = []
for subdir, dirs, files in os.walk('outputs'):
    for file in files:
        if file == 'timetaken.out':
            x = open(os.path.join(subdir, file))
            x = float(x.read()[23:29])
            print(x)
            pntsArray += [[int(subdir[8:]), x]]
pntsArray.sort()
x = []
y = []
for i in pntsArray:
    x += [i[0]]
    y += [i[1]]
print(x)
print(y)
print(pntsArray)
plt.plot(x, y)
plt.xlabel('Parameter (x where N+x are processed)')
plt.ylabel('Time taken')
plt.show()
