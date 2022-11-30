size_ = 30
entrance_ = (0, 0)
exit_ = (size_-1, size_-1)
checkpoints_ = set([(6, 24), (12, 18), (18, 12), (24, 6)])
checkpoints_ = set([(0, 1), (0, 2)])
generations_ = 1
matingEventsPerGeneration_ = 2_000
fill_ = 0.05    # sparse initialization parameter
Pc_ = 0.05      # uniform crossover rate
Pm_ = 0.01      # unform mutation rate


def fitness_3(maze):

    # list(map(
    #     print,
    #     maze
    # ))

    global exit_, entrance_, checkpoints_, size_

    # if  the exit is blocked
    if maze[exit_[0]][exit_[1]] == 1:
        # print("exit blocked")
        return 0
    # if the entrance is blocked
    if maze[entrance_[0]][entrance_[1]] == 1:
        # print("entrance blocked")
        return 0
    # if any of the checkpoints are blocked
    for c in checkpoints_:
        if maze[c[0]][c[1]] == 1:
            # print("checkpoint blocked")
            return 0

    q = [entrance_]

    lk = {}
    for i in range(size_):
        for j in range(size_):
            lk[(i, j)] = (1_000_000, 0)

    lk[entrance_] = (0, 0)

    count = 0
    while len(q) > 0:
        count += 1
        # print(max(sum(i) for i, j in lk.items() if j[0] != 1_000_000))
        # print(q[-15:])
        pt = q.pop()
        dist, checkpointCount = lk[pt]
        dist += 1
        newPts = []

        if dist > lk[exit_][0]:
            continue
        
        foo = (pt[0]+1, pt[1])
        if foo[0] < size_ and lk[foo][0] > dist and maze[foo[0]][foo[1]] == 0:
            newPts.append(foo)
        
        foo = (pt[0], pt[1]+1)
        if foo[1] < size_ and lk[foo][0] > dist and maze[foo[0]][foo[1]] == 0:
            newPts.append(foo)

        foo = (pt[0]-1, pt[1])
        if foo[0] >= 0 and lk[foo][0] > dist and maze[foo[0]][foo[1]] == 0:
            newPts.append(foo)
        
        foo = (pt[0], pt[1]-1)
        if foo[1] >= 0 and lk[foo][0] > dist and maze[foo[0]][foo[1]] == 0:
            newPts.append(foo)
        
        for newPt in newPts:
            if newPt == exit_:
                if lk[newPt][0] == 1_000_000:
                    lk[newPt] = (dist, checkpointCount)
                elif dist < lk[newPt][0]:
                    lk[newPt] = (dist, checkpointCount)
                elif dist == lk[newPt][0]:
                    if lk[newPt][1] < checkpointCount:
                        lk[newPt] = (dist, checkpointCount)
                # print("found a path")
            else:
                if newPt in checkpoints_:
                    checkpointCount += 1
                if lk[newPt][0] == 1_000_000:
                    lk[newPt] = (dist, checkpointCount)
                    q.append(newPt)
                elif dist < lk[newPt][0]:
                    lk[newPt] = (dist, checkpointCount)
                    q.append(newPt)
                elif dist == lk[newPt][0]:
                    if lk[newPt][1] < checkpointCount:
                        lk[newPt] = (dist, checkpointCount)
                        q.append(newPt)
                if newPt in checkpoints_:
                    checkpointCount -= 1
        # print(len(q))

    # print("steps: ", count)
    return lk[exit_][0] if lk[exit_][1] == len(checkpoints_) else 0

def readMazes():
    mazes = []
    with open("mazes.txt") as f:
        buffer = []

        while True:
            l = f.readline()
            if not l:
                break
            if len(l) < 3:
                mazes.append([
                    list(map(int, r.strip().split(", "))) for r in buffer        
                ])
                buffer = []
            else:
                buffer.append(l)
        if len(buffer) > 0:
            mazes.append([
                list(map(int, r.strip().split(", "))) for r in buffer        
            ])
        return mazes

mazes = readMazes()
print(len(mazes))

ft = [fitness_3(m) for m in mazes]
print(ft)