NODESIZE = 128
# ranks = range(0,256)

# under populated node 
# globalSize=60

# 1/2 populated node 
# globalSize=128

# more than 1/2 populated node 
#globalSize=190

# 1 full populated node 
#globalSize=256

# more than 1 full populated node 
#globalSize=300

# 2 full populated node 
globalSize=512
ranks = range(0,globalSize)
i = 0 
print("testing", globalSize%(NODESIZE)/2)

for globalRank in ranks: 
    if (globalRank % (NODESIZE*2) == 0):
        i = i+1
        print("** Node ", i)
    
    if(globalSize <= NODESIZE*2):
        if(globalRank < globalSize/2):
            print("comp", globalRank)
        else:
            print("io", globalRank)

    else: # size > 256 
        
        lastwholeNode = int(globalSize/(NODESIZE*2)) *NODESIZE*2
        if(globalRank < lastwholeNode ): # globalRank comes within full node? 
            if(globalRank < lastwholeNode/2):
                print("comp", globalRank)
            else:
                print("io", globalRank)
        
        elif(globalRank >= lastwholeNode): # globalRank is not within full node. 
             
            if(globalRank  < (globalSize + lastwholeNode)/2):
                print("comp", globalRank)
            else:
                print("io", globalRank)
