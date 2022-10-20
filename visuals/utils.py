import matplotlib.pyplot as plt
from matplotlib.pyplot import cm
import pandas as pd
import numpy as np
import pathlib 

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

def onePlot(directory):
    """
    Line plot for multi directories 
    """
    data = {}
    colour = iter(cm.rainbow(np.linspace(0, 1, len(directory))))
    for dir in directory:
       data = readData(f"{dir}/compute_write_time.csv") 
       path = pathlib.PurePath(dir) 
       label_ = path.name 
       colour_ = next(colour)
       print(list(data["computeTime"]))
       plt.plot( list(data["xAxis"]), list(data["computeTime"]), label = f"{label_}/computeTime", color = colour_)
       plt.plot( list(data["xAxis"]), list(data["totalTime"]), label = f"{label_}/totalTime", color = colour_, linestyle = "--")

    plt.title("iocomp comparison")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")
    plt.xticks
    plt.grid() 
    plt.legend() 
    plt.yscale('log')
    plt.show()
    

