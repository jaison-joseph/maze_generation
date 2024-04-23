import random
from collections import deque

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

def genMaze():
    global size_, fill_
    # sparse initialization
    maze = [
        [1 if random.random() <= fill_ else 0 for _ in range(size_)] for __ in range(size_)
    ] 
    return maze

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

# hardcoded random test case
if True:
    population[0] = [
        [0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1],
        [1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1],
        [0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1],
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1],   
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0]
    ]

    list(map(print, population[0]))

    print(fitness_3_debug(population[0]))

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