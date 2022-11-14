from utils import* 

# barplot(filename)
dirs = [  
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Consecutive",
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Hyperthread", 
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Overcommit", 
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Serial", 
]
flag = True

dir = f'{os.getcwd()}/ARCHER2_runs/11Nov/0.8' 
# onePlot(dir,flag)
readDataWriteTime(dir) 
