import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
import pandas as pd
import numpy as np
import pathlib 
import os 
from glob import glob 
from datetime import datetime 

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
        "computeTime" : computeTime, 
        "totalTime" : totalTime
    }
    return data 

def barplot(filename, flag):
    """
    Bar plot for one directory 
    """
    data = {}
    data = readData(filename)
    X_axis = np.arange(len(xAxis))
    width = 0.4
    plt.bar(X_axis, data["computeTime"], 0.4, label = "computeTime")
    plt.bar(X_axis + width, data["totalTime"], 0.4, label = "totalTime")
    plt.xticks(X_axis + width/2, data["xAxis"])
    plt.legend()
    plt.title("iocomp comparison")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")

    if(flag):
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
    avgComputeTime = np.empty((4))
    avgTotalTime = np.empty((4))
    stdComputeTime = np.empty((4))
    stdTotalTime = np.empty((4))

    for x in range(maxloop):

        data = readData(f"{dir}/{x+1}/compute_write_time.csv") # x+1 as the runs are numbered from 1 to 10 

        """
        iterate over the stream benchmark code types
        """ 
        for i in range(4): 
            avgComputeTime[i] += data["computeTime"][i]
            avgTotalTime[i] += data["totalTime"][i]
            computeTime[i][x] = data["computeTime"][i]
            totalTime[i][x] = data["totalTime"][i]
    """
    find average 
    """    
    for i in range(4): 
        avgComputeTime[i] = avgComputeTime[i]/maxloop 
        avgTotalTime[i] = avgTotalTime[i]/maxloop 
        stdComputeTime[i] = np.std(computeTime[i]) 
        stdTotalTime[i] = np.std(totalTime[i]) 

    """
    Insert data into dictionary 
    """ 
    avgData = {
        "avgComputeTime": avgComputeTime, 
        "avgTotalTime": avgTotalTime, 
        "xAxis": data["xAxis"], 
        "stdTotalTime" : stdTotalTime,
        "stdComputeTime" : stdComputeTime
    } 

    return(avgData)

def saveDatatoPD(data, label, saveData):
    """
    save information of dictionary data into csv files 
    """
    saveData.insert(1,f"{label}_avgComputeTime",list(data["avgComputeTime"]))
    saveData.insert(1,f"{label}_stdComputeTime",list(data["stdComputeTime"]))
    saveData.insert(1,f"{label}_avgTotalTime",list(data["avgTotalTime"]))
    saveData.insert(1,f"{label}_stdTotalTime",list(data["stdTotalTime"]))


def onePlot(parentDir,flag):
   
    # directory = glob(f"{parentDir}/*", recursive = True) # gather names of all sub directories 
    directory = {
        f"{parentDir}/Consecutive",
        f"{parentDir}/Hyperthread",
        f"{parentDir}/Overcommit", 
        f"{parentDir}/Serial"
    }

    saveData = pd.DataFrame() 
    saveData_T = pd.DataFrame() # transposed 
    saveData.insert(0,"stream",["Copy", "Scalar", "Add", "Triad"])

    """
    Line plot for multi directories 
    """
    data = {}
    plt.figure(figsize=(10,8)) # set figure size 
    colour = iter(cm.rainbow(np.linspace(0, 1, len(directory))))
   
    """
    iterate over directories, example Consecutive, Hyperthread etc. 
    """ 
    for dir in directory:
       """
       Average out the many runs and output max/min 
       """
       data = avgJobRuns(dir) 
       path = pathlib.PurePath(dir) 
       label_ = path.name 
       saveDatatoPD(data,label_, saveData)
       colour_ = next(colour)
       plt.plot( list(data["xAxis"]), list(data["avgComputeTime"]), color = colour_, linestyle = "--")
       plt.errorbar( list(data["xAxis"]), list(data["avgComputeTime"]), yerr=list(data["stdComputeTime"]),color=colour_, fmt = 'o') #, color = colour_, linestyle = "--", fmt = 'o')
       plt.plot( list(data["xAxis"]), list(data["avgTotalTime"]), color = colour_, linestyle = "-", label = label_ )
       plt.errorbar( list(data["xAxis"]), list(data["avgTotalTime"]), yerr=list(data["stdTotalTime"]),color=colour_, fmt='o') # , color = colour_, linestyle = "-", fmt = 'o')


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

