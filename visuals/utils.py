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
    plt.bar(X_axis - 0.2, computeTime, 0.4)
    plt.bar(X_axis + 0.2, totalTime, 0.4)
    plt.title("iocomp comparison")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")
    plt.show()
