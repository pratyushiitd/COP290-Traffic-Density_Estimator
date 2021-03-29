import subprocess
import sys
try:
    from scipy.signal import savgol_filter
    from matplotlib import pyplot as plt
    print(99)
except:
    print("Some dependencies are missing...\nInstalling required modules...")
    subprocess.call(['pip', 'install', '-r', 'requirements.txt'])
    # import pip
    # pip.main(['install', '-r', 'requirements.txt'])
    from scipy.signal import savgol_filter
    from matplotlib import pyplot as plt


def plotter(x, y, z, nameOfFig):
    plt.axvline(x=98, color='g', linestyle='dashed')
    plt.axvline(x=180, color='r', linestyle='dashed')
    plt.axvline(x=275, color='g', linestyle='dashed')
    plt.axvline(x=352, color='r', linestyle='dashed')
    plt.plot(x, y, label="Queue Density")
    plt.plot(x, z, label="Dynamic Density")
    plt.xlabel('Time (in Seconds)')
    plt.ylabel('Density')
    plt.title('Traffic density with time')
    plt.legend()
    plt.savefig("./outputs/"+nameOfFig)
    # plt.draw()
    # plt.waitforbuttonpress(0)
    plt.close()


# frames = open("./outputs/frames.out", "r")
# framesArray = list(map(float, frames.read().split(",")))
dynamic = open("./outputs/dynamic.out", "r")
dynamicArray = list(map(float, dynamic.read().split(",")))
static = open("./outputs/static.out", "r")
staticArray = list(map(float, static.read().split(",")))

# Creating x axis
lenOfArr = len(staticArray)
framesArray = []
timeDuration = 382.279
i = 0
while staticArray[i] == 1:
    i += 1
for j in range(i):
    staticArray[j] = staticArray[i]
# staticArray[0] = staticArray[1]
f = open("./outputs/time.out", "a")
for i in range(lenOfArr):
    val = timeDuration*i/lenOfArr
    framesArray += [val]
    if i == 0:
        f.write(str(val))
    else:
        f.write(", "+str(val))
f.close()
# print arrays for debugging
print(len(framesArray))
print(len(dynamicArray))
print(len(staticArray))

# for i in range(lenOfArr):
#     print(framesArray[i], ",", staticArray[i], ",", dynamicArray[i])
# Plot
plotter(framesArray, staticArray, savgol_filter(dynamicArray, 17, 2), "Plot")
