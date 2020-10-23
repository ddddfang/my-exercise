#汗诺塔:将所有disks从ori移动到dst,借助tmp
def hannuo(disks, ori, tmp, dst):
    if len(disks) == 1:
        print("move disk{}: from {} to {}.".format(disks[0], ori, dst))
    else:
        hannuo(disks[0:-1], ori, dst, tmp)
        print("move disk{}: from {} to {}.".format(disks[-1], ori, dst))
        hannuo(disks[0:-1], tmp, ori, dst)

disks = [0, 1, 2, 3]
print(disks)
print(disks[0:-1])
hannuo(disks, 0, 1, 2)
