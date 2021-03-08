from scipy.signal import savgol_filter
from matplotlib import pyplot as plt


def fibo(x, y, z):
    plt.plot(x, y, label="Queue Density")
    plt.plot(x, z, label="Dynamic Density")
    plt.xlabel('x - axis')
    plt.ylabel('y - axis')
    plt.title('Two or more lines on same plot with suitable legends ')
    plt.legend()
    plt.draw()
    plt.waitforbuttonpress(0)
    plt.close()


frames = open("./outputs/frames.out", "r")
framesArray = list(map(int, frames.read().split(",")))
dynamic = open("./outputs/dynamic.out", "r")
dynamicArray = list(map(int, dynamic.read().split(",")))
static = open("./outputs/static.out", "r")
staticArray = list(map(int, static.read().split(",")))
print(framesArray)
print(dynamicArray)
print(staticArray)
print(len(framesArray))
print(len(dynamicArray))
print(len(staticArray))

fibo(framesArray, staticArray, dynamicArray)
fibo(framesArray, staticArray, savgol_filter(dynamicArray, 151, 2))
