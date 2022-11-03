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

def onePlot(directory,flag):
   
    """
    Line plot for multi directories 
    """
    data = {}
    plt.figure(figsize=(10,8)) # set figure size 
    colour = iter(cm.rainbow(np.linspace(0, 1, len(directory))))
   
    """
    iterate over directories 
    """ 
    for dir in directory:
       data = readData(f"{dir}/compute_write_time.csv") 
       path = pathlib.PurePath(dir) 
       label_ = path.name 
       colour_ = next(colour)
       print(list(data["computeTime"]))
       plt.plot( list(data["xAxis"]), list(data["computeTime"]), color = colour_, linestyle = "--")
       plt.plot( list(data["xAxis"]), list(data["totalTime"]), color = colour_, linestyle = "-", label = label_ )

    plt.plot(0,0, label = "computeTime",color="k", linestyle = "--")
    plt.plot(0,0, label = "totalTime", color="k", linestyle = "-")
    plt.title("iocomp comparison")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")
    plt.xticks
    plt.grid() 
    plt.legend() 
    plt.yscale('log')
    
    if(flag):
        plt.show()
    else:
        plt.savefig(f"{dir}.pdf")

