# 动态规划解决找零钱问题
# 需要找零 change 元,可以用的面额在 coinList, 找零最少coin数目是 minCoins(存储找零0-change最少需要的coin)
def dpMakeChange(coinList, change, minCoins):
    for cents in range(change+1):
        coinCount = cents   #预设一个最显而易见的值,全部找1元的硬币需要的数目

        for j in [c for c in coinList if c <= cents]:
            if minCoins[cents - j] + 1 < coinCount:
                coinCount = minCoins[cents - j] + 1
        minCoins[cents] = coinCount
    return minCoins[change] #需要解决的是找零change元需要的最少货币个数,但是吧从0-change的都求出来,这就是动态规划

# minCoins = [0 for i in range(change+1)]
# minTargets = [1 for i in range(change+1)]
def dpMakeChange(coinList, change, minCoins, minTargets):
    for cents in range(change+1):
        coinCount = cents
        coinTarget = 1

        for j in [c for c in coinList if c <= cents]:   #每一步,所有可能的选择里,有最优的话
            if minCoins[cents - j] + 1 < coinCount:
                coinCount = minCoins[cents - j] + 1
                coinTarget = j
        minCoins[cents] = coinCount
        minTargets[cents] = coinTarget
    #return minCoins[change]

def printResoultion(change):
    cl = [1, 2, 5, 10, 20, 50]
    minCoins = [0 for i in range(change+1)]
    minTargets = [1 for i in range(change+1)]
    dpMakeChange(cl, change, minCoins, minTargets)
    res = []
    while change >= 1:
        i = minTargets[change]
        res.append(i)
        change = change - i
    return res

if __name__ == "__main__":
    #cl = [1, 2, 5, 10, 20, 50]
    #change = 173
    #mcs = [0 for i in range(change+1)]
    #print(dpMakeChange(cl, change, mcs))
    print(printResoultion(173))
