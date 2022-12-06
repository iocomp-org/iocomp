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
import argparse
import seaborn as sns
import seaborn.objects as so

localSize = 0.8
ranks = 4
plt.style.use("style.mplstyle")  # matplotlib style sheet

"""
select mapping
"""
mapping = [
    "Consecutive",
    "Hyperthread",
    "Oversubscribe",
    "Sequential"
]

stream = [
    "copy",
    "add",
    "scalar",
    "triad"
]

mapping_colour = {
    "Consecutive": "r",
    "Hyperthread": "b",
    "Oversubscribe": "g", 
    "Sequential": "c"
}

ls = {
    "copy": "--", 
    "add": ":", 
    "scalar": "-", 
    "triad": "-."
}

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
    computeTime = mydata2.iloc[0].values[1:5]
    totalTime = mydata2.iloc[1].values[1:5]
    waitTime = mydata2.iloc[2].values[1:5]
    wallTime = mydata2.iloc[2].values[5]
    data = {
        "xAxis": xAxis,
        "computeTime": computeTime,
        "totalTime": totalTime, 
        "waitTime": waitTime,
        "wallTime": wallTime 
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
    plt.title(f"Compute vs Wall time local size {localSize} GB serial ranks {ranks} ")
    plt.xlabel("STREAM benchmark category")
    plt.ylabel("Times(s)")
    plt.xticks
    plt.grid() 
    plt.legend() 
    plt.yscale('log')

    now = datetime.now()
    date_time = now.strftime("%d,%m,%Y,%H,%M")
    saveName = f"comp_wall_t_{date_time}"
    saveData_t = saveData.T 
    saveData_t.to_csv(f"CSV_files/{saveName}.csv") 
    if(flag):
        plt.show()
    else:
        plt.savefig(f"Saved_fig/{saveName}.pdf")


def readDataWriteTime(parentDir,flag):

    directory = {
        f"{parentDir}/Consecutive": "b",
        f"{parentDir}/Hyperthread": "k",
        f"{parentDir}/Overcommit": "r",
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
            ioBW = [] 
            
            with open(filename) as f: # read individual test.out for printed values of io write times
                contents = f.read()
                data_retrieve = re.findall(r"\*\* I\/O write time=(\d+.\d+) filesize\(GB\)=(\d+.\d+)",contents) 

                for x in data_retrieve: # add all individual file write times in output file to writeTime and fileSize
                    writeTime.append(float(x[0]))
                    fileSize.append(float(x[1]))
                    ioBW.append(float(x[1])/float(x[0])) # fileSize/writeTime 

            ioBandwidth[jobruns] = statistics.harmonic_mean(ioBW) # ioBandwidth will be HM of ind. BWs 
        
        ioBandwidth_avg[type]=statistics.harmonic_mean(ioBandwidth) # avg ioBandwith is harmonic mean of ioBandwidths across job runs 
        ioBandwidth_std[type]=statistics.stdev(ioBandwidth) # avg ioBandwith is harmonic mean of ioBandwidths across job runs 
        type += 1 

    X_axis = np.arange(len(ioBandwidth_avg))
    width = 0.4
    plt.bar(X_axis,ioBandwidth_avg,width, yerr=ioBandwidth_std,alpha=0.5, ecolor='black', capsize=10)
    plt.xticks(X_axis,label_ref)
    # plt.title(f"I/O bandwidth from stream benchmark;local size {round(fileSize[0],2)}GB")
    plt.title(f"I/O bandwidth;local size {localSize} GB serial ranks {ranks}")
    plt.xlabel("SLURM mapping")
    plt.ylabel("I/O bandwidth (GB/s)")
    plt.xticks
    # plt.legend() 
    # plt.yscale('log')
    # plt.show()
    # plt.savefig("Saved_fig/ioBandwidth_bar.pdf") 


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

    now = datetime.now()
    date_time = now.strftime("%d,%m,%Y,%H,%M")
    saveName = f"IO_BW_{date_time}"
    if(flag):
        plt.show()
    else:
        plt.savefig(f"Saved_fig/{saveName}.pdf")
        # save data to csv 
        saveData = pd.DataFrame({
            'avg_BW':ioBandwidth_avg,
            'std_dev':ioBandwidth_std,
            'xaxis':X_axis
        })
        saveData.to_csv(f"CSV_files/{saveName}.csv")

def comp_vs_wall_time(directory):

    # directory = glob(f"{parentDir}/*", recursive = True) # gather names of all sub directories
    data = {}
    output_data = {} 

    """
    iterate over directories, example Consecutive, Hyperthread etc.
    """
    for key in directory: 
        dir = key

        """
        Average out the many runs and output standard deviation 
        """
        data = avgJobRuns(dir)
        path = pathlib.PurePath(dir) 
        label_ = path.name 

        if os.path.isdir(key):
            mapping = os.path.basename(key) # get mapping from the directory name ex. Serial etc.   

        output_data[f"{mapping}"]=data # append mapping data to output_data dictionary 

    return(output_data)

 

        

def plot_times_vs_numcores(parentDir):

    data = {} 
    data = getStreamTimingData(parentDir)
    
    # for key, value in data.items():
    dir_list = next(os.walk(parentDir))[1]
    cores = []  
    for dir in dir_list:
        core = dir.split("_",1)[1]
        cores.append(core)

    cores_asc = cores
    cores_asc = [int(x) for x in cores]
    cores_asc.sort(key=int)

    """
    select mapping
    """
    mapping = [
        "Consecutive",
        "Hyperthread",
        "Overcommit",
        "Serial"
    ]
    
    stream = [
        "copy",
        "add",
        "scalar",
        "triad"
    ]

    colour = {
        "Consecutive": "r",
        "Hyperthread": "b",
        "Overcommit": "k", 
        "Serial": "c"
    }

    ls = {
        "copy": "--", 
        "add": ":", 
        "scalar": "-", 
        "triad": "-."
    }

    x = {} 
    for ind_mapping in mapping:
        stream_results = {} 
        for i in range(len(stream)):
            y = [] 
            for core in cores_asc:
                y.append(data[f"{core}"][ind_mapping]["avgTotalTime"][i]) # only for total time
            stream_results[stream[i]] = y  
        x[ind_mapping] = stream_results

    for ind_mapping in mapping:
        for i in range(len(stream)):
            plt.plot(list(cores_asc),x[ind_mapping][stream[i]],color=colour[ind_mapping],linestyle=ls[stream[i]]) 

    plt.title(f"Compute vs Wall time; local size {localSize}GB ")
    plt.xlabel("Number of cores")
    plt.ylabel("Times(s)")
    plt.xticks
    plt.grid() 
    plt.legend() 
    plt.yscale('log')
    plt.show()


def plot_times_vs_streams(parentDir):

    data = {} 
    data = getStreamTimingData(parentDir)


    # plt.figure(figsize=(10, 8))  # set figure size

    """
    select mapping
    """
    mapping = [
        "Consecutive",
        "Hyperthread",
        "Overcommit",
        "Serial"
    ]

    dir_list = next(os.walk(parentDir))[1]
    cores = []  
    for dir in dir_list:
        core = dir.split("_",1)[1]
        cores.append(core)
    
    # for core in cores:
    core = '32'
    for ind_map in mapping:
        plt.plot(data[core][ind_map]["xAxis"],data[core][ind_map]["avgComputeTime"],color=colour[ind_map],label=ind_map)
    plt.legend() 
    plt.show() 
    # plt.errorbar( stream_ob, list(data["avgComputeTime"]), yerr=list(data["stdComputeTime"]),color=colour_, fmt = 'o') #, color = colour_, linestyle = "--", fmt = 'o')
    # plt.plot( stream_ob, list(data["avgTotalTime"]), color = colour_, linestyle = "-", label = label_ )
    # plt.errorbar( stream_ob, list(data["avgTotalTime"]), yerr=list(data["stdTotalTime"]),color=colour_, fmt='o') # , color = colour_, linestyle = "-", fmt = 'o')

    # plt.plot(0,0, label = "computeTime",color="k", linestyle = "--") # dummy plots to label compute and total time
    # plt.plot(0,0, label = "totalTime", color="k", linestyle = "-")
    # plt.title(f"Compute vs Wall time local size {localSize} GB serial ranks {ranks} ")
    # plt.xlabel("STREAM benchmark category")
    # plt.ylabel("Times(s)")
    # plt.xticks
    # plt.grid() 
    # plt.legend() 
    # plt.yscale('log')

def save_or_show(saveName,flag,plt,data):
    if(flag):
        plt.savefig(f"Saved_fig/{saveName}.pdf")
        print("Saved in:")
        print(f"Saved_fig/{saveName}.pdf")
        bar_plot_times_vs_numcores_per_stream_CSV(saveName,data)
    else:
        plt.show()

def bar_plot_times_vs_numcores_per_stream_CSV(name,data): # save data for plots to csv 
    
    df = pd.DataFrame.from_dict(data, orient='index')
    df.to_csv(f"CSV_files/{name}.csv")
    print(f"CSV_files/{name}.csv")

def bar_plot_times_vs_numcores(parentDir, flag):

    fig1 = plt.figure(figsize=(8,6))  # set figure size
    data = {} 
    data = getStreamTimingData(parentDir)
    dir_list = next(os.walk(parentDir))[1]
    cores = []  

    for dir in dir_list:
        core = dir.split("_",1)[1]
        cores.append(core)

    cores.sort(key=int)
    core_num = 0
    totalTime_hatch="///"
    plt.rcParams['axes.grid'] = False
    
    for core in cores:
        width_ = 0.2
        computeTime = [0]*len(stream) # total compute time for all stream categories
        totalTime = [0]*len(stream) # total wall time for all stream categories
        iter = 0
        mapping_num = 0 
        
        for ind_map in mapping:
            computeTime[mapping_num] = 0 
            totalTime[mapping_num] = 0 
       
            for x in range(len(stream)): # summation of all compute steps from stream categories
                computeTime[mapping_num] += data[core][ind_map]["avgComputeTime"][x] # addition of compute steps into each other. 
                totalTime[mapping_num] += data[core][ind_map]["avgTotalTime"][x] # addition of compute steps into each other. 

            plt.bar(core_num+mapping_num*width_, totalTime[mapping_num], alpha=0.5, width=width_, color=mapping_colour[ind_map],hatch=totalTime_hatch)
            plt.bar(core_num+mapping_num*width_, computeTime[mapping_num], width=width_,color=mapping_colour[ind_map])
            mapping_num += 1 

        core_num += 1
    
    plt.xticks(ticks = np.arange(len(cores))+width_*len(mapping)/2,labels = cores, rotation = 'horizontal')

    for key, value in mapping_colour.items():
        plt.bar(x=0,height=0,label = key,color = value) # dummy plots to label compute and total time
     
    plt.bar(x=0,height=0, label = "TotalTime", color = "white", edgecolor = 'black',hatch=totalTime_hatch) # dummy plots to label compute and total time
    plt.bar(x=0,height=0, label = "ComputeTime",edgecolor = 'black', color = "white") # dummy plots to label compute and total time
    plt.legend() 
    # plt.title(f"Compute vs Wall time; local size {localSize}GB ")
    plt.xlabel("Number of cores")
    plt.ylabel("Times(s)")
    plt.yscale('log')
    fig1.tight_layout() 
    save_or_show("comp_wall_bar",flag,plt)


"""
Plotting functions for bar plot showing num cores vs STREAM categories 
"""


def avgJobRuns(dir):
    """
    Iterate over directories such as 1,2,3,4 etc to average out the times
    """
    dir_list        = next(os.walk(dir))[1] 
    maxloop         = len(dir_list) # number of job submission to loop over 
    totalTime       = np.empty((10, maxloop),dtype=np.float64)
    computeTime     = np.empty((10, maxloop),dtype=np.float64)
    waitTime        = np.empty((10, maxloop),dtype=np.float64)
    stdComputeTime  = np.empty((4), dtype=np.float64)
    stdTotalTime    = np.empty((4), dtype=np.float64)
    stdWaitTime     = np.empty((4), dtype=np.float64)
    medTotalTime    = np.empty((4), dtype=np.float64)
    medComputeTime  = np.empty((4), dtype=np.float64)
    medWaitTime     = np.empty((4), dtype=np.float64)

    """
    Initialise average counters 
    """
    for x in range(4):
        stdComputeTime[x]  =0 
        stdTotalTime[x]    =0 
        stdWaitTime[x]     =0 
        medTotalTime[x]    =0 
        medComputeTime[x]  =0 
        medWaitTime[x]     =0 

    for x in range(maxloop):

        csv_file_path=f"{dir}/{x}/compute_write_time.csv"
        
        if(os.path.isfile(csv_file_path)):
            
            data = readData(csv_file_path)
            """
            iterate over the stream benchmark code types, ex. COPY over 1,2,3, sub directories. 
            """
            for i in range(4):
                computeTime[i][x] = data["computeTime"][i]
                waitTime[i][x] = data["waitTime"][i]
                totalTime[i][x] = data["totalTime"][i]
        else:
            print("non-existant", csv_file_path)

    """
    find std deviation and median
    """
    for i in range(4):

        stdComputeTime[i] = np.std(computeTime[i])
        stdTotalTime[i] = np.std(totalTime[i])
        stdWaitTime[i] = np.std(waitTime[i])
        medComputeTime[i] = statistics.median(computeTime[i])
        medWaitTime[i] = statistics.median(waitTime[i])
        medTotalTime[i] = statistics.median(totalTime[i])

    """
    remove tab space from X-axis
    """
    stream_ob = [] 
    for el in data["xAxis"]:  
        stream_ob.append(el.replace("\t", "")) 

    """
    Insert data into dictionary
    """
    avgData = {
        "avgComputeTime": medComputeTime,
        "avgWaitTime": medWaitTime,
        "avgTotalTime": medTotalTime,
        "xAxis": stream_ob,
        "stdTotalTime": stdTotalTime,
        "stdComputeTime": stdComputeTime
    }

    return (avgData)

def getStreamTimingData(parentDir):

    """
    save data csv headers 
    """
    saveData = pd.DataFrame()
    saveData_T = pd.DataFrame()  # transposed
    saveData.insert(0, "stream", ["Copy", "Scalar", "Add", "Triad"])

    """
    get data and store them against num cores in data dict
    """
    dir_list = next(os.walk(parentDir))[1]
    data = {}
    cores = []  

    for dir in dir_list:
        core = dir.split("_",1)[1]
        cores.append(core)

        """
        Get available SLURM mappings and obtain path
        """
        directory = []  
        subDirectory = next(os.walk(f"{parentDir}/{dir}"))[1]

        """
        Get average values using median 
        """
        for x in subDirectory:
            directory.append(f"{parentDir}/{dir}/{x}")

        """
        outer level of dictionary saving under core name 
        """ 
        data[core] = comp_vs_wall_time(directory)
    
    return(data)


def bar_plot_times_vs_numcores_per_stream(parentDir, flag, name=None):

    fig1, ax1 = plt.subplots(2, 2,figsize=(8,10),sharey=True)
    data = {} 
    data = getStreamTimingData(parentDir)
    dir_list = next(os.walk(parentDir))[1]
    cores = []  

    for dir in dir_list:
        core = dir.split("_",1)[1]
        cores.append(core)

    cores.sort(key=int)
    core_num = 0
    totalTime_hatch="///"
    waitTime_hatch="****"

    
    for core in cores:
        width_ = 0.2
        computeTime = [0]*len(stream) # total compute time for all stream categories
        totalTime = [0]*len(stream) # total wall time for all stream categories
        iter = 0
        mapping_num = 0 
        
        for ind_map in mapping:
       
            for x in range(len(stream)): # summation of all compute steps from stream categories
                computeTime = data[core][ind_map]["avgComputeTime"][x] # addition of compute steps into each other. 
                totalTime = data[core][ind_map]["avgTotalTime"][x] # addition of compute steps into each other. 
                waitTime = data[core][ind_map]["avgWaitTime"][x] # addition of compute steps into each other. 
                i=int(x/2)
                j=int(x%2)
                ax1[i,j].bar(core_num+mapping_num*width_, waitTime, alpha=0.5, width=width_, color=mapping_colour[ind_map],hatch=waitTime_hatch)
                ax1[i,j].bar(core_num+mapping_num*width_, totalTime, alpha=0.5, width=width_, color=mapping_colour[ind_map],hatch=totalTime_hatch)
                ax1[i,j].bar(core_num+mapping_num*width_, computeTime, width=width_,color=mapping_colour[ind_map])
       
            mapping_num += 1 

        core_num += 1

    """
    ticks for each subplot
    """

    for x in range(4):
        i = int(x/2)
        j = int(x%2)
        ax1[i,j].set_xticks(np.arange(len(cores))+width_*len(mapping)/2-width_/2) 
        ax1[i,j].set_xticklabels(cores)
        ax1[i,j].title.set_text(stream[x])
        ax1[i,j].set_yscale('log')
        ax1[i,j].set_ylim(0,100)

        for key, value in mapping_colour.items():
            ax1[i,j].bar(x=0,height=0,label = key,color = value) # dummy plots to label compute and total time
            # plt.bar(x=0,height=0, label = "TotalTime", color = "white", edgecolor = 'black',hatch=totalTime_hatch) # dummy plots to label compute and total time
            # plt.bar(x=0,height=0, label = "ComputeTime",edgecolor = 'black', color = "white") # dummy plots to label compute and total time

    fig1.supxlabel('Number of compute processes')
    fig1.supylabel('Time(s)')
    ax1[0,0].legend() # legend only in 1st quad
    fig1.tight_layout() 
    plt.rcParams['grid.alpha'] = 0.5 # grid lines bit less visible
    plt.rcParams['grid.linewidth'] = 0.1 # grid lines bit less visible
    if (name == None):
        name = "comp_wall_bar_NO_MPI_TEST"
    saveName = f"{name}_{datetime.now().strftime('%d,%m,%Y,%H,%M')}"
    save_or_show(saveName,flag,plt,data)


        
