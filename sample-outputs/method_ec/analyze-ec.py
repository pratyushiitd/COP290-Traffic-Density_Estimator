import os
from matplotlib import pyplot as plt
import numpy as np
from scipy.signal import savgol_filter


paths = ["BlockSize", "QualityLevel", "minDistance", "maxCorners"]
for path in paths:
    dict_ = {}
    pntsArray = []
    for subdir, dirs, files in os.walk(path):
        ans = []
        if (len(subdir) > len(path)): 
            dict_[subdir[subdir.index('/')+1:]] =  []
        for file in files:
            #print(file)
            if file == 'dynamic.out':
                currentDynamic = list(map(float, open(os.path.join(subdir, file)).read().split(",")))
                dict_[subdir[subdir.index('/')+1:]].append(currentDynamic)
            elif file == 'sparse.out':
                sparse = list(
                    map(float, open(os.path.join(subdir, file)).read().split(",")))
                dict_[subdir[subdir.index('/')+1:]].append(sparse)
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
    x_ = []
    y_ = []
    for i in pntsArray:
        x_ += [i[0]]
        y_ += [i[1]]
    #     y += [i[2]]
    #     z += [i[3]]
    x = []
    y = []
    keys = list(dict_.keys())
    keys.sort(key = lambda x : int(x))
    print(keys)
    for keys in keys:
        x.append(keys)
        y.append(100 * np.abs(np.divide(
            np.subtract(dict_[keys][0],dict_[keys][1]), dict_[keys][0]
            )
            ).mean())
    print(x)
    print(y)
    print("A")

    plt.plot(x, y)
    plt.xlabel('Parameter: ' + path)
    plt.ylabel('% Error in Sparse vs Dense Optical Flow')
    #plt.show()
    plt.savefig("./" + path + "/"+"error_vs_"+path)
    plt.close()
    plt.plot(x_, y_)
    plt.xlabel('Parameter: ' + path)
    plt.ylabel('Execution time (sec)')
    #plt.show()
    plt.savefig("./" + path + "/"+"time_vs_"+path)
    plt.close()