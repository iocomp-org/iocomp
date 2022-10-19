import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def barplot(filename):
    """
    Read data
    """
    mydata2 = pd.read_csv(filename, index_col=False, skiprows=0)

    """
    Data analysis, ignore the first element 
    """
    xAxis = mydata2.columns.values[1:]  # header file
    computeTime = mydata2.iloc[0].values[1:]
    totalTime = mydata2.iloc[1].values[1:]

    """
    Bar plot
    """
    X_axis = np.arange(len(xAxis))
    width = 0.4
    plt.bar(X_axis, computeTime, 0.4, label = "computeTime")
    plt.bar(X_axis + width, totalTime, 0.4, label = "totalTime")
    plt.xticks(X_axis + width/2, xAxis)
    plt.legend()
    plt.title("iocomp comparison")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")
    plt.show()
