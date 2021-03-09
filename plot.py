import subprocess
import sys
try:
    from scipy.signal import savgol_filter
    from matplotlib import pyplot as plt
    print(99)
except:
    print("Some dependencies are missing...\nInstalling required modules...")
    subprocess.call(['pip', 'install','-r', 'requirements.txt'])
    # import pip
    # pip.main(['install', '-r', 'requirements.txt'])
    from scipy.signal import savgol_filter
    from matplotlib import pyplot as plt


def plotter(x, y, z, nameOfFig):
    plt.plot(x, y, label="Queue Density")
    plt.plot(x, z, label="Dynamic Density")
    plt.xlabel('Time (in Seconds)')
    plt.ylabel('Density')
    plt.title('Two or more lines on same plot with suitable legends ')
    plt.legend()
    plt.savefig("./outputs/"+nameOfFig)
    plt.draw()
    plt.waitforbuttonpress(0)
    plt.close()


frames = open("./outputs/frames.out", "r")
framesArray = list(map(float, frames.read().split(",")))
dynamic = open("./outputs/dynamic.out", "r")
dynamicArray = list(map(float, dynamic.read().split(",")))
static = open("./outputs/static.out", "r")
staticArray = list(map(float, static.read().split(",")))


print(framesArray)
print(dynamicArray)
print(staticArray)
print(len(framesArray))
print(len(dynamicArray))
print(len(staticArray))

plotter(framesArray, staticArray, dynamicArray, "Original")
plotter(framesArray, staticArray, savgol_filter(dynamicArray, 17, 2), "Smooth")
plotter(framesArray, staticArray, savgol_filter(
    dynamicArray, 151, 2), "Smoother")
