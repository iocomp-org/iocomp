from utils import* 

# barplot(filename)
dirs = [  
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Consecutive",
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Hyperthread", 
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Overcommit", 
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Serial", 
]
flag = False
parentDir = f'{os.getcwd()}/ARCHER2_runs/5Nov_small' 
# onePlot(parentDir,flag)
readDataWriteTime() 
