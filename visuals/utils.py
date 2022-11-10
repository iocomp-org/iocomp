import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
import pandas as pd
import numpy as np
import pathlib
import os
from glob import glob
from datetime import datetime
import statistics
import re 


def readData(filename):
    """
    Read data
    """
    mydata2 = pd.read_csv(filename, index_col=False, skiprows=0)
    data = {}

    """
    Data analysis, ignore the first element
    """
    xAxis = mydata2.columns.values[1:]  # header file
    computeTime = mydata2.iloc[0].values[1:]
    totalTime = mydata2.iloc[1].values[1:]
    data = {
        "xAxis": xAxis,
        "computeTime": computeTime,
        "totalTime": totalTime
    }
    return data


def barplot(filename, flag):
    """
    Bar plot for one directory
    """
    data = {}
    data = readData(filename)
    X_axis = np.arange(len(data["xAxis"]))
    width = 0.4
    plt.bar(X_axis, data["computeTime"], 0.4, label="computeTime")
    plt.bar(X_axis + width, data["totalTime"], 0.4, label="totalTime")
    plt.xticks(X_axis + width/2, data["xAxis"])
    plt.legend()
    plt.title("iocomp comparison")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")

    if (flag):
        plt.show()
    else:
        plt.savefig(f"{filename}fig.pdf")


def avgJobRuns(dir):
    """
    Iterate over directories such as 1,2,3,4 etc to average out the times
    """

    maxloop = 10
    totalTime = np.empty((10, 10))
    computeTime = np.empty((10, 10))
    avgComputeTime = np.empty((4), dtype=float)
    avgTotalTime = np.empty((4), dtype=float)
    stdComputeTime = np.empty((4), dtype=float)
    stdTotalTime = np.empty((4), dtype=float)
    medTotalTime = np.empty((4), dtype=float)
    medComputeTime = np.empty((4), dtype=float)

    for l in range(4):
        avgComputeTime[l] = 0
        avgTotalTime[l] = 0
        stdComputeTime[l] = 0
        stdTotalTime[l] = 0
        medComputeTime[l] = 0
        medTotalTime[l] = 0

    for x in range(maxloop):

        # x+1 as the runs are numbered from 1 to 10
        data = readData(f"{dir}/{x+1}/compute_write_time.csv")

        """
        iterate over the stream benchmark code types
        """
        for i in range(4):

            avgComputeTime[i] += data["computeTime"][i]/maxloop
            avgTotalTime[i] += data["totalTime"][i]/maxloop
            computeTime[i][x] = data["computeTime"][i]
            totalTime[i][x] = data["totalTime"][i]

    """
    find std deviation
    """
    for i in range(4):

        stdComputeTime[i] = np.std(computeTime[i])
        stdTotalTime[i] = np.std(totalTime[i])
        medComputeTime[i] = statistics.median(computeTime[i])
        medTotalTime[i] = statistics.median(totalTime[i])

    """
    Insert data into dictionary
    """
    avgData = {
        "avgComputeTime": avgComputeTime,
        "avgTotalTime": avgTotalTime,
        "xAxis": data["xAxis"],
        "stdTotalTime": stdTotalTime,
        "stdComputeTime": stdComputeTime
    }

    return (avgData)


def saveDatatoPD(data, label, saveData):
    """
    save information of dictionary data into csv files
    """
    saveData.insert(1, f"{label}_avgComputeTime", list(data["avgComputeTime"]))
    saveData.insert(1, f"{label}_avgTotalTime", list(data["avgTotalTime"]))
    saveData.insert(1, f"{label}_stdComputeTime", list(data["stdComputeTime"]))
    saveData.insert(1, f"{label}_stdTotalTime", list(data["stdTotalTime"]))


def onePlot(parentDir, flag):

    # directory = glob(f"{parentDir}/*", recursive = True) # gather names of all sub directories
    directory = {
        f"{parentDir}/Consecutive": "b",
        f"{parentDir}/Hyperthread": "k",
        f"{parentDir}/Overcommit": "r",
        f"{parentDir}/Serial": "c"
    }

    saveData = pd.DataFrame()
    saveData_T = pd.DataFrame()  # transposed
    saveData.insert(0, "stream", ["Copy", "Scalar", "Add", "Triad"])

    """
    Line plot for multi directories
    """
    data = {}
    plt.figure(figsize=(10, 8))  # set figure size

    """
    iterate over directories, example Consecutive, Hyperthread etc.
    """
    for key,value in directory.items():
        dir = key
        colour_ = value

        """
        Average out the many runs and output standard deviation 
        """
        data = avgJobRuns(dir)
        path = pathlib.PurePath(dir) 
        label_ = path.name 
        saveDatatoPD(data,label_, saveData)

        stream_ob = [] 
        for el in data["xAxis"]: # remove tab space from xAxis 
            stream_ob.append(el.replace("\t", "")) 

        plt.plot( stream_ob, list(data["avgComputeTime"]), color = colour_, linestyle = "--")
        plt.errorbar( stream_ob, list(data["avgComputeTime"]), yerr=list(data["stdComputeTime"]),color=colour_, fmt = 'o') #, color = colour_, linestyle = "--", fmt = 'o')
        plt.plot( stream_ob, list(data["avgTotalTime"]), color = colour_, linestyle = "-", label = label_ )
        plt.errorbar( stream_ob, list(data["avgTotalTime"]), yerr=list(data["stdTotalTime"]),color=colour_, fmt='o') # , color = colour_, linestyle = "-", fmt = 'o')

    plt.plot(0,0, label = "computeTime",color="k", linestyle = "--") # dummy plots to label compute and total time
    plt.plot(0,0, label = "totalTime", color="k", linestyle = "-")
    plt.title("iocomp comparison - 0.016GB")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")
    plt.xticks
    plt.grid() 
    plt.legend() 
    plt.yscale('log')

    now = datetime.now()
    date_time = now.strftime("%m:%d:%Y:%H:%M:%S")
    saveName = f"{date_time}_small"
    saveData_t = saveData.T 
    saveData_t.to_csv(f"CSV_files/{saveName}.csv") 
    if(flag):
        plt.show()
    else:
        plt.savefig(f"Saved_fig/{saveName}.pdf")
def readDataWriteTime():
    filename = '/home/shrey/Coding/iocomp/test/run_dir/test.out'
    with open(filename) as f:
        contents = f.read()
        # writeTime = re.findall(r"time=(\d+(\.\d*)?)|(\.\d+)",contents) 
        # fileSize = re.findall(r"*\(B\)=(\d+)/",contents) 
        data_retrieve = re.findall(r"(\*\* I\/O write time=(\d+.\d+) filesize\(B\)=(\d+) )",contents) 
        # print(writeTime,fileSize)
        for x in data_retrieve:
            fileSize = x[2] 
            writeTime = x[1]
            print(fileSize, writeTime)