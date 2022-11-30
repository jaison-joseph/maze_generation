import random

'''
code style

variables for constants end with an _
'''

size_ = 30
entrance_ = [size_-2, size_-2]
exit_ = [1, 1]
checkpoints_ = [[6, 24], [12, 18], [18, 12], [24, 6]]
generations_ = 1
matingEventsPerGeneration_ = 2_000
fill_ = 0.05    # sparse initialization parameter
Pc_ = 0.05      # 
Pm_ = 0.01      # 

def genMaze():
    global size_, fill_
    # sparse initialization
    maze = [
        [1 if random.random() <= fill_ else 0 for _ in range(size_)]
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
    candidates = {}
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
                    foo.append(p[::] + newPt)
                    if newPt == exit_:
                        hitAllCheckPts = True 
                        for c in checkpoints_:
                            if c not in foo[-1]:
                                hitAllCheckPts = False
                                break
                        if hitAllCheckPts:
                            flag = True
                            candidates.append(foo[-1], len(foo[-1]))
        if flag:
            # pick the path with the least length
            result = [c for c,l in candidates.items() if l == min(candidates.values())][0]
            return len(result)
        else:
            # remove duplicates
            paths = list(set(foo))

    # if no path was found to the destination with all checkpoints, return 0
    return 0

def uniformCrossover(m1, m2):
    pass

def uniformMutation(m):
    pass

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

population = [genMaze()] * 120

for _ in range(generations_):
    for __ in range(matingEventsPerGeneration_):
        indices = random.sample(range(120, 7))
        choices = [population[i] for i in indices]
        fitnesses = [fitness(c) for c in choices]
        sortedFitnesses = fitnesses[::].sort()

