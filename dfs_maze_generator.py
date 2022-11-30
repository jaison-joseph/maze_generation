import random

'''
let's generate a 4*4 square maze

(0,0) (0,1) (0,2) (0,3) 
(1,0) (1,1) (1,2) (1,3) 
(2,0) (2,1) (2,2) (2,3) 
(3,0) (3,1) (3,2) (3,3) 

---------------------------------
| (0,0) | (0,1) | (0,2) | (0,3) |
---------------------------------
| (1,0) | (1,1) | (1,2) | (1,3) |
---------------------------------
| (2,0) | (2,1) | (2,2) | (2,3) |
---------------------------------
| (3,0) | (3,1) | (3,2) | (3,3) |
---------------------------------

'''

'''
key value pair representation of the walls 
key: a edge
values: each value v in the values of edge e implies there's a wall btw e and v
this means all v's have to be adjacent to edge e

if walls contains {
    v1: set(v2, v3, v4),
    ....
}

then walls will also contain {
    v2: set(v1, ...)
    v3: set(v1, ...)
    v4: set(v1, ...)
}

'''
neighbors = {}

for i in range(4):
    for j in range(4):
        edges = set()
        i -= 1
        if i >= 0:
            edges.add((i, j))
        i += 2
        if i < 4:
            edges.add((i, j))
        i -= 1

        j -= 1
        if j >= 0:
            edges.add((i, j))
        j += 2
        if j < 4:
            edges.add((i, j))
        j -= 1

        neighbors[(i, j)] = edges

walls = neighbors.copy()

list(map(
    print, 
    walls.items()
))

print("\n\n ----------------------------------------- \n\n")

visited = set()
curPoint = (0,0)
stack = [curPoint]

# generate the maze
while True:
    visited.add(curPoint)
    options = list(filter(lambda x: x not in visited, neighbors[curPoint]))
    if len(options) == 0:
        if not stack:
            break

        curPoint = stack.pop()
        continue
    stack.append(curPoint)
    newPoint = random.choice(options)
    walls[curPoint].remove(newPoint)
    walls[newPoint].remove(curPoint)
    curPoint = newPoint

print(len(visited))
list(map(
    print,
    walls.items()
))

# print the maze
# rows = []
# for i in range(4):
#     row = []
#     for j in range(3):
#         pt = (i, j)
#         right = (i+1, j)
#         row.append("|" if right in walls[pt] else " ")
#     rows.append(row)

# cols = []
# for i in range(3):
#     col = []
#     for j in range(4):
#         pt = (i, j)
#         right = (i, j+1)
#         col.append("|" if right in walls[pt] else " ")
#     cols.append(col)


# print("visited: ", visited)
# list(map(print, walls.items()))

# list(map(print, rows))