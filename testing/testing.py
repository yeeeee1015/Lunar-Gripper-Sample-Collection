import csv
import statistics
import matplotlib.pyplot as plt
import matplotlib
import math

nums = []
time = 0
times = []
distUnit = "cm"
timeUnit = "ms"
fileTitle = "datagood.csv"

def truncate(number, digits):
    nbDecimals = len(str(number).split('.')[1])
    if nbDecimals <= digits:
        return number
    stepper = 10.0 ** digits
    return math.trunc(stepper*number) / stepper

with open(fileTitle, "r") as file:
    fileReader = csv.reader(file)
    for line in fileReader:
        cleaned = line[0].replace(" ", "")
        
        try:
            temp = int(cleaned)
        except:
            temp = -1

        if temp != -1:
            nums.append(temp)
            times.append(time)
        time += 100 #ms

title = '\n'.join((

f"Mean: {truncate(statistics.mean(nums), 3)}",
f"Median: {statistics.median(nums)}",
f"Mode: {statistics.mode(nums)}",
f"Standard Deviation: {truncate(statistics.stdev(nums),3)}",
f"Min: {min(nums)}",
f"Max: {max(nums)}",
f"Range: {max(nums) - min(nums)}"

))
plt.ylim(0, int(max(nums) * 1.05))
plt.title(title)
plt.plot(times,nums)
plt.ylabel(distUnit)
plt.xlabel(timeUnit)
plt.show()