

def getIndices(arr):
    assert len(arr) == 7, "invalid input length"
    result = [0, 0, 0, 0]
    maxele = -2
    maxele2 = -1
    minele = 1_000_001
    minele2 = 1_000_000
    maxidx = 0
    maxidx2 = 0
    minidx = 0
    minidx2 = 0
    for i_, i in enumerate(arr):
        if i > maxele:
            maxidx2 = maxidx
            maxele2 = maxele
            maxele = i
            maxidx = i_
        elif i < minele:
            minele2 = minele
            minidx2 = minidx
            minele = i
            minidx = i_
        elif i > maxele2 and i < maxele:
            maxele2 = i
            maxidx2 = i_
        elif i > minele and i < minele2:
            minele2 = i
            minidx2 = i_
    return [maxidx, maxidx2, minidx2, minidx]

def test():
    inputs = [
        [0, 1, 2, 3, 4, 5, 6],
        [6,5,4,3,3,2,1],
        [0, 0, 0, 0, 0, 0, 0],
    ]
    for i in inputs:
        print(getIndices(i))


test()