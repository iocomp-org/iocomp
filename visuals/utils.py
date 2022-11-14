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
import seaborn as sns
import seaborn.objects as so


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
        # f"{parentDir}/Overcommit": "r",
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


def readDataWriteTime(parentDir):

    directory = {
        f"{parentDir}/Consecutive": "b",
        f"{parentDir}/Hyperthread": "k",
        # f"{parentDir}/Overcommit": "r",
        f"{parentDir}/Serial": "c"
    }

    # initialise plot 
    plt.figure(figsize=(10, 8))  # set figure size

    maxloop = 10
    avgWriteTime_avg = [0 for i in range(len(directory))]
    ioBandwidth_avg = [0 for i in range(len(directory))]
    ioBandwidth_std = [0 for i in range(len(directory))]
    label_ref = [] 

    type = 0 

    for key,value in directory.items(): # go through consecutive, hyperthread, serial etc. 
        dir = key 
        colour_ = value

        path = pathlib.PurePath(dir) 
        label_ = path.name 

        label_ref.append(label_)

        ioBandwidth = [0 for i in range(maxloop)] 
        
        for jobruns in range(maxloop): # go through 1,2,3,etc 

            filename = f'{dir}/{jobruns+1}/test.out'

            writeTime = [] 
            fileSize = [] 
            
            with open(filename) as f: # read individual test.out for printed values of io write times
                contents = f.read()
                data_retrieve = re.findall(r"\*\* I\/O write time=(\d+.\d+) filesize\(GB\)=(\d+.\d+)",contents) 

                for x in data_retrieve: # add all individual file write times in output file to writeTime and fileSize
                    writeTime.append(float(x[0]))
                    fileSize.append(float(x[1]))

            ioBandwidth[jobruns] = sum(fileSize)/sum(writeTime) # ioBandwidth will be total size of file written/ total time taken
        
        ioBandwidth_avg[type]=statistics.harmonic_mean(ioBandwidth) # avg ioBandwith is harmonic mean of ioBandwidths across job runs 
        ioBandwidth_std[type]=statistics.stdev(ioBandwidth) # avg ioBandwith is harmonic mean of ioBandwidths across job runs 
        type += 1 

    X_axis = np.arange(len(ioBandwidth_avg))
    width = 0.4

    plt.bar(X_axis,ioBandwidth_avg,width, yerr=ioBandwidth_std,alpha=0.5, ecolor='black', capsize=10)
    plt.xticks(X_axis,label_ref)
    plt.title("I/O bandwidth from stream benchmark")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("I/O bandwidth (GB/s)")
    plt.xticks
    # plt.legend() 
    # plt.yscale('log')
    plt.show() 


    """
    seaborn plot -> to be explored later 
    """
    # sns.barplot(
        # x=label_ref, y=ioBandwidth_avg,
        # errorbar=((ioBandwidth_std),2), capsize=.4, errcolor=".5"
    # )
    # plt.show() 

    # (
    #     so.Plot(ioBandwidth, x=label_ref )
    #     .add(so.Bar(alpha=.5), so.Agg(), so.Dodge())
    #     .add(so.Range(), so.Est(errorbar="sd"), so.Dodge())
    # )
