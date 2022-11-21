from utils import* 

# barplot(filename)
dirs = [  
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Consecutive",
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Hyperthread", 
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Overcommit", 
    "/Users/sbhardwa/Coding/iocomp/visuals/ARCHER2_runs/3Nov/Serial", 
]
flag = False

dir = f'{os.getcwd()}/ARCHER2_runs/18Nov/1_8'
onePlot(dir,flag)
readDataWriteTime(dir,flag) 
