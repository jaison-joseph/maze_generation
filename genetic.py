import random
from collections import deque
from itertools import permutations
'''
code style

variables for constants end with an _
'''

size_ = 30
entrance_ = (0, 0)
exit_ = (size_-1, size_-1)
checkpoints_ = set([(6, 24), (12, 18), (18, 12), (24, 6)])
generations_ = 1
matingEventsPerGeneration_ = 2_000
fill_ = 0.05    # sparse initialization parameter
Pc_ = 0.05      # uniform crossover rate
Pm_ = 0.01      # unform mutation rate

# used by the fitness function, easier to just shallow copy for every function call
lk_ = {}        
for i in range(size_):
    for j in range(size_):
        lk_[(i, j)] = (1_000_000, 0)

# used by the pathfinder method, easier to just shallow copy
pathFinder_lk_ = {}        
for i in range(size_):
    for j in range(size_):
        pathFinder_lk_[(i, j)] = 1_000_000

def genMaze():
    global size_, fill_
    # sparse initialization
    maze = [
        [1 if random.random() <= fill_ else 0 for _ in range(size_)] for __ in range(size_)
    ] 
    return maze

def fitness(maze):
    global exit_, entrance_, checkpoints_, size_
    openSpaces = sum(i.count(0) for i in maze)

    # if  the exit is blocked
    if maze[exit_[0]][exit_[1]] == 1:
        return 0
    # if the entrance is blocked
    if maze[entrance_[0]][entrance_[1]] == 1:
        return 0
    # if any of the checkpoints are blocked
    for c in checkpoints_:
        if maze[c[0]][c[1]] == 1:
            return 0
    # find the # of members for the exit square

    paths = [[entrance_]]
    # will contain all paths to dest containing all checkpoints
    candidates = []
    # set to true if a path is found that contains all checkpoint squares
    flag = False

    # we wont have a path longer than the number of open spaces
    for _ in range(openSpaces):
        foo = []
        for p in paths:
            tip = p[-1]
            newTips = []
            if tip[0] > 0:
                newTips.append([tip[0]-1, tip[1]])
            if tip[1] > 0:
                newTips.append([tip[0], tip[1]-1])
            if tip[0] < size_-1:
                newTips.append([tip[0]+1, tip[1]])
            if tip[1] < size_-1:
                newTips.append([tip[0], tip[1]+1])
            for newPt in newTips:
                if newPt not in p:
                    bar = p[::]
                    bar.append(newPt)
                    foo.append(bar)
                    if newPt == exit_:
                        hitAllCheckPts = True 
                        for c in checkpoints_:
                            if c not in foo[-1]:
                                hitAllCheckPts = False
                                break
                        if hitAllCheckPts:
                            flag = True
                            candidates.append(foo[-1])
        if flag:
            # pick the path with the least length
            return len(candidates[0])
        else:
            # remove duplicates
            paths = []
            for i in foo:
                if i not in paths:
                    paths.append(i)
        print(len(paths))

    # if no path was found to the destination with all checkpoints, return 0
    return 0

def fitness_2(maze):

    # list(map(
    #     print,
    #     maze
    # ))

    global exit_, entrance_, checkpoints_, size_

    # if  the exit is blocked
    if maze[exit_[0]][exit_[1]] == 1:
        return 0
    # if the entrance is blocked
    if maze[entrance_[0]][entrance_[1]] == 1:
        return 0
    # if any of the checkpoints are blocked
    for c in checkpoints_:
        if maze[c[0]][c[1]] == 1:
            return 0

    q = deque([entrance_])

    lk = {}
    for i in range(size_):
        for j in range(size_):
            lk[(i, j)] = (1_000_000, 0)

    lk[tuple(entrance_)] = (0, 0)

    while len(q) > 0:
        pt = q.popleft()
        if pt == exit_:
            break
        dist, checkpointCount = lk[pt]
        dist += 1
        newPts = []
        if pt[0] < size_-1:
            newPts.append((pt[0]+1, pt[1]))
        if pt[1] < size_-1:
            newPts.append((pt[0], pt[1]+1))
        if pt[0] > 0:
            newPts.append((pt[0]-1, pt[1]))
        if pt[1] > 0:
            newPts.append((pt[0], pt[1]-1))
        for newPt in newPts:
            if lk[newPt][0] > dist and maze[newPt[0]][newPt[1]] == 0:
                lk[newPt] = (dist, checkpointCount+1 if newPt in checkpoints_ else checkpointCount)
                q.append(newPt) 
        # print(len(q))

    # print(lk[exit_])
    return lk[exit_][0] if lk[exit_][1] == len(checkpoints_) else 0

'''
this works, returns 0 if the entrance/exit/checkpoints were blocked or 
none of the shortest paths had all the checkpoints
'''
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

def fitness_3_debug(maze):

    # list(map(
    #     print,
    #     maze
    # ))

    global exit_, entrance_, checkpoints_, size_

    # if  the exit is blocked
    if maze[exit_[0]][exit_[1]] == 1:
        print("exit blocked")
        return 0
    # if the entrance is blocked
    if maze[entrance_[0]][entrance_[1]] == 1:
        print("entrance blocked")
        return 0
    # if any of the checkpoints are blocked
    for c in checkpoints_:
        if maze[c[0]][c[1]] == 1:
            print("checkpoint blocked")
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
                print("found a path")
            else:
                if newPt in checkpoints_:
                    checkpointCount += 1
                if dist < lk[newPt][0]:
                    lk[newPt] = (dist, checkpointCount)
                    q.append(newPt)
                elif dist == lk[newPt][0]:
                    if lk[newPt][1] < checkpointCount:
                        lk[newPt] = (dist, checkpointCount)
                        q.append(newPt)
                if newPt in checkpoints_:
                    checkpointCount -= 1
        # print(len(q))

    print("steps: ", count)
    print("exit square dist: ", lk[exit_][0], " | exit square checkpoints: ", lk[exit_][1])
    return lk[exit_][0] if lk[exit_][1] == len(checkpoints_) else 0

'''
looks through a 2D array maze of 1's and 0's (0 -> empty space | 1-> wall) of dimension size_ * size_
for a path from coordinates entrance to coordinates exit, both tuples of coordinates
returns the length of the shortest path from entrance to exit
if no path was found, it returns 0. 
    Hence, start and end cannot be the same
'''
def pathFinder(maze, start, end):

    global pathFinder_lk_

    # if the start is blocked
    if maze[start[0]][start[1]] == 1:
        print("start blocked")
        return 0
    # if the end is blocked
    if maze[end[0]][end[1]] == 1:
        print("end blocked")
        return 0

    lk = pathFinder_lk_.copy()
    lk[start] = 0

    q = deque([start])
    while len(q) > 0:
        pt = q.pop()
        dist = lk[pt]
        dist += 1

        if dist > lk[end]:
            continue

        foo = (pt[0]+1, pt[1])
        if foo[0] < size_ and lk[foo] > dist and maze[foo[0]][foo[1]] == 0:
            lk[foo] = dist
            if q != end:
                q.append(foo)
            else:
                print("path found")
        
        foo = (pt[0], pt[1]+1)
        if foo[1] < size_ and lk[foo] > dist and maze[foo[0]][foo[1]] == 0:
            lk[foo] = dist
            if q != end:
                q.append(foo)
            else:
                print("path found")

        foo = (pt[0]-1, pt[1])
        if foo[0] >= 0 and lk[foo] > dist and maze[foo[0]][foo[1]] == 0:
            lk[foo] = dist
            if q != end:
                q.append(foo)
            else:
                print("path found")
        
        foo = (pt[0], pt[1]-1)
        if foo[1] >= 0 and lk[foo] > dist and maze[foo[0]][foo[1]] == 0:
            lk[foo] = dist
            if q != end:
                q.append(foo)
            else:
                print("path found")

    # if no path was found, lk[end] would remain the same value as initialized i.e. 1_000_000
    return 0 if lk[end] == 1_000_000 else lk[end]

'''
fitness_3 finds a path if it exists no matter what, but it's hard to bias it to use the checkpoints

so we can brute force a path search from 
entrance -> [the checkpoints in all possible permutations] -> exit
and see, for each path through all the checkpoints if it's length <= the shortest path found using fitness 3

the assumption here is that each checkpoint will only be traversed once, and that's a fair assumption since 
the purpose of the checkpoints is to guide the shape of the maze, and a maze which has a path that is 
supposed to be traversed, only to retreat back to reach the exit is an 'unfit maze'

the overall structure of the algorithm is:

find a path x from extrance to exit ignoring the checkpoints
if no such path exists, return 0
brute force a path search from 
    entrance -> [the checkpoints in all possible permutations] -> exit
    enforce that all the checkpoints must be traversed once exactly
return the longest path y from above satisfying the condition len(y) <= len(x)
if no such y are found, return 0
'''
def fitness_4(maze):

    global exit_, entrance_, checkpoints_, size_, lk_

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

    # look for a path from entrance to exit,ignoring the checkpoints
    shortestPath = pathFinder(maze, entrance_, exit_)
    if shortestPath == 0:
        return 0

    # brute force a path search from entrance -> [the checkpoints in all possible permutations] -> exit   
    checkpointDistances = []
    path = []
    for p in permutations(checkpoints_):
        path = [entrance_] + list(p) + [exit_]
        totalDist = 0
        pathFailed = False
        for i in range(len(path)-1):
            l = pathFinder(maze, path[i], path[i+1])
            if l == 0:
                pathFailed = True
                break
            totalDist += l
        if not pathFailed:
            checkpointDistances.append(totalDist)
    # print("checkpointDistances: ", checkpointDistances)
    # if checkpointDistances is empty, no paths were found so the fitness is 0
    if not checkpointDistances:
        return 0
    bestResult = min(checkpointDistances)
    # if we have a path shorter than or equal to the entrance->exit without checkpoints, return that shorter path
    # else the fitness is 0
    if bestResult <= shortestPath:
        return bestResult
    return 0  

# each of the inputs is a 2D array
def uniformCrossover(m1, m2):
    global Pc_
    for i in range(len(m1)):
        for j in range(len(m2)):
            if random.random() <= Pc_:
                m1[i][j], m2[i][j] = m2[i][j], m1[i][j]

# input is a 2D array
def uniformMutation(m):
    global Pm_
    for i_, i in enumerate(m):
        for j_, j in enumerate(i):
            if random.random() <= Pm_:
                m[i_][j_] = 1 if j == 0 else 0

def saveMazes(mazes):
    with open("mazes.txt", "w+") as f:
        for m in mazes:
            for r in m:
                f.write(
                    ", ".join(map(str, r))
                )
                f.write("\n")
            f.write("\n")

'''
evolution algorithm: 
    - steady state using size seven single tournament selection. 
    - works by mating events that generate pairs of new structures that are reinserted into the population 
        before the next mating event. 
    - Size seven single tournament selection chooses seven member of the population without replacement. 
    - The two best are copied over the two worst. 
    - The binary variation operator is applied to the two copies, then the unary variation operator is applied to each
        of the copies. 

what's a generation
    - The algorithm is run for 500 000 mating events,
    - saving summary fitness statistics for the population every 2000 mating events. 
    - Such a block of 2000 mating events is called a generation. 
    - Each experiment consisted of 30 replicates of the evolutionary algorithm performed with distinct random number seeds. 
    - The crossover and mutation rates are set to Pc and Pm.
    - Both of the direct representations and the positive generative representations use a population size of 120
'''

'''
create population size of 120


# loop for each generation
for _ in range(500_000//2_000):
    for __ in range(2_000):
        select 7 without replacement
        evaluate fitness for each
        replace worst two with copies of best two
        run binary variation followed by unary var...

get best two,
    select 7 without replacement,
    copy them over the two worst
    apply binary variation operator to the copies, (the uniform crossover)
    then unary variation operator to each of the copies (uniform mutation)
    replace the best two with these two, insert into new population

'''

population = [genMaze() for _ in range(120)]

def runner():
    sortedFitnesses = []
    for _ in range(generations_):
        for __ in range(matingEventsPerGeneration_):
            indices = random.sample(range(120), 7)
            choices = [population[i] for i in indices]
            fitnesses = [fitness_3(c) for c in choices]
            sortedFitnesses = fitnesses[::]
            sortedFitnesses.sort()
            print(sortedFitnesses)
            # i1 & i2 are the indices of the fittest individuals from the selection of 7
            i1 = fitnesses.index(sortedFitnesses[-1])
            i2 = fitnesses.index(sortedFitnesses[-2])
            # m1 & m2 are copies of the fittest mazes
            m1 = population[indices[i1]][::]
            m2 = population[indices[i2]][::]
            # apply some 'evolution'
            uniformCrossover(m1, m2)
            uniformMutation(m1)
            uniformMutation(m2)
            # overwrite the weakest two individuals with modified versions of m1 and m2
            population[indices[fitnesses.index(sortedFitnesses[0])]] = m1
            population[indices[fitnesses.index(sortedFitnesses[1])]] = m2
        print(sortedFitnesses)

    saveMazes(filter(lambda x: fitness_3(x) > 0, population))

def runner_2():
    sortedFitnesses = []
    for g in range(generations_):
        for __ in range(matingEventsPerGeneration_):
            indices = random.sample(range(120), 7)
            choices = [population[i] for i in indices]
            fitnesses = [fitness_4(c) for c in choices]
            sortedFitnesses = fitnesses[::]
            sortedFitnesses.sort()
            print(sortedFitnesses)
            # i1 & i2 are the indices of the fittest individuals from the selection of 7
            i1 = fitnesses.index(sortedFitnesses[-1])
            i2 = fitnesses.index(sortedFitnesses[-2])
            # m1 & m2 are copies of the fittest mazes
            m1 = population[indices[i1]][::]
            m2 = population[indices[i2]][::]
            # apply some 'evolution'
            uniformCrossover(m1, m2)
            uniformMutation(m1)
            uniformMutation(m2)
            # overwrite the weakest two individuals with modified versions of m1 and m2
            population[indices[fitnesses.index(sortedFitnesses[0])]] = m1
            population[indices[fitnesses.index(sortedFitnesses[1])]] = m2
        print(sortedFitnesses)
        print("end of generation: ", g+1)
        print([fitness_4(m) for m in population])

runner_2()

# hardcoded random test case
if False:
    population[0] = [
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1],   
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0],   
        [0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],   
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    ]

    list(map(print, population[0]))

    print(fitness_3_debug(population[0]))

# another hard coded test, but 30*30
if False:
    maze = [
        [0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,]
    ]

    for c in checkpoints_:
        i, j = c
        maze[i][j] = 0
    i, j = entrance_
    maze[i][j] = 0
    i, j = exit_
    maze[i][j] = 0

    # list(map(print, maze))

    print(fitness_3_debug(maze))

# another hard coded test, but 30*30 for pathFinder
if False:
    # (6, 24), (12, 18), (18, 12), (24, 6)
    # this maze has a single solution, a path that traverses all checkpoints
    maze = [
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,],   
        [1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,]
    ]

    # for c in checkpoints_:
    #     i, j = c
    #     maze[i][j] = 0
    # i, j = entrance_
    # maze[i][j] = 0
    # i, j = exit_
    # maze[i][j] = 0
    
    for c in checkpoints_:
        i, j = c
        # print(c, ", ", maze[i][j]) 
        maze[i][j] = 0
    i, j = entrance_
    maze[i][j] = 0
    i, j = exit_
    maze[i][j] = 0
    # list(map(print, maze))

    print("fitness_3_debug result: ", fitness_3_debug(maze))

    print("pathfinder result: ", pathFinder(maze, entrance_, exit_))

    print("fitness 4 result: ", fitness_4(maze))

if False:

    for c in checkpoints_:
        i, j = c
        population[0][i][j] = 0
    i, j = entrance_
    population[0][i][j] = 0
    i, j = exit_
    population[0][i][j] = 0

    print("fitness_3_debug result: ", fitness_3_debug(population[0]))

    print("pathfinder result: ", pathFinder(population[0], entrance_, exit_))

    print("fitness 4 result: ", fitness_4(population[0]))

# test for fitness_3 for 120 randomly generated mazes
if False:
    for p in population:
        ft = fitness_3(p)
        print(ft)
        if ft == 0:
            list(map(print, p))
            print("running debug ....")
            print(fitness_3_debug(p))

# a test case for the fitness function; seemed to work
if False:
    foo = [
        [0,0,0,0,0],
        [1,1,1,1,0],
        [0,0,0,0,0],
        [0,1,1,1,1],
        [0,0,0,0,0],
    ]

    size_ = 5
    entrance_ = (0, 0)
    exit_ = (size_-1, size_-1)
    checkpoints_ = set([(0, 1), (0, 2)])

    print(
        fitness_2(foo)
    )

    foo = [
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
    ]

    print(
        fitness_3(foo)
    )

    foo = [
        [0,0,0,0,0],
        [1,1,1,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
        [0,0,0,0,0],
    ]

    print(
        fitness_2(foo)
    )

# test case for the uniformCrossover
if False:
    # lengths are 100 since Pc (prob of crossover) = 0.05
    m1 = [1 for _ in range(200)]
    m2 = [0 for _ in range(200)]
    
    print("before crossover: ")
    print("m1: ", m1)
    print("m2: ", m2)

    uniformCrossover(m1, m2)
    
    print("after crossover: ")
    print("m1: ", m1)
    print("m2: ", m2)

# test case for the uniformMutation
if False:
    # lengths are 100 since Pm (prob of mutation) = 0.05
    m = [1 for _ in range(100)]
    
    print("before mutation: ")
    print("m: ", m)

    uniformMutation(m)
    
    print("after mutation: ")
    print("m: ", m)