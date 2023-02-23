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
globalSize=400

# multi full populated node 
# globalSize=1024
ranks = range(0,globalSize)
i = 0 

numNodes = int(globalSize/(NODESIZE*2)) 
lastwholeNode = numNodes *NODESIZE*2
print("last whole node", lastwholeNode)

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
        
        if(globalRank < lastwholeNode ): # globalRank comes within full node? 
            for x in range(0,numNodes+1,2): # go through all the nodes 
                if(globalRank <  (x+1)*NODESIZE and globalRank >= (x)*NODESIZE):
                    print("comp", globalRank)
                if(globalRank >=  (x+1)*NODESIZE and globalRank < (x+2)*NODESIZE):
                    print("io", globalRank)

        elif(globalRank >= lastwholeNode): # globalRank is not within full node. 
            if(globalRank  < (globalSize + lastwholeNode)/2):
                print("comp", globalRank)
            else:
                print("io", globalRank)
