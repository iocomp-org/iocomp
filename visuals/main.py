from utils import* 
# True for show, False for save 
flag = True

dir = f'{os.getcwd()}/ARCHER2_runs/22Nov/1_8'
# onePlot(dir,flag)
# readDataWriteTime(dir,flag) 
dir = f'{os.getcwd()}/ARCHER2_runs/22Nov'
timings_against_cores(dir)