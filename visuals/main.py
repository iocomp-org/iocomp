from utils import* 

parser = argparse.ArgumentParser(
                    prog = 'ProgramName',
                    description = 'What the program does',
                    epilog = 'Text at the bottom of help')

parser.add_argument('--save', action='store_true')  # if save used then fig is saved, otherwise plt.show
args = parser.parse_args()
# onePlot(dir,flag)
# readDataWriteTime(dir,flag) 
dir = f'{os.getcwd()}/ARCHER2_runs/22Nov'
# plot_times_vs_numcores(dir)
# plot_times_vs_streams(dir)
bar_plot_times_vs_numcores(dir,args.save)