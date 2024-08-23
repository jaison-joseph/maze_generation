
var solution = [];
var checkpointSolution = [];
var c = null;
var ctx = null;
var matrix = null;
var checkpoints = [[6, 24], [12, 18], [18, 12], [24, 6]];

function traceBack_3(lk, fitness, start, end) {
    const size_ = 30;
    let nextSteps = [];
    let stacks = [[start]];
    let debug = false;
    if (start[0] == 6 && start[1] == 12 && end[0] == 29 && end[1] == 29) {
        debug = true;
    }
    if  (debug) {
        console.log("initial stacks: ", stacks)
        console.log("fitness: ", fitness)
        console.log("lk: ", lk)
    }
    
    let newStacks = [];
    console.log("debug: ", debug ? "true" : "false");

    // stacks.forEach(s => {console.log("stack: ", s);});

    while(stacks[0].length != fitness) {
        newStacks = [];
        // if (stacks[0].length >= fitness) {
        //     break;
        // }
        for (let i = 0 ; i < stacks.length ; i++) {
        // stacks.forEach(stack => {
            stack = stacks[i];
            if (debug) {
                console.log("stack: ", stack);
            }
            pt = stack[stack.length-1];
            dist = lk[pt[0]][pt[1]];
            dist += 1;
            nextSteps = [];
    
            foo = [pt[0]+1, pt[1]];
            if (foo[0] < size_ && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }
        
            foo = [pt[0], pt[1]+1];
            if (foo[1] < size_ && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }
            foo = [pt[0]-1, pt[1]];
            if (foo[0] >= 0 && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }
        
            foo = [pt[0], pt[1]-1];
            if (foo[1] >= 0 && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }

            nextSteps.forEach(s => {
                stack.push(s);
                newStacks.push([...stack]);
                stack.pop();
            });
        };
        stacks = newStacks;
        if (stacks.length == 0) {
            return;
        }
        if (debug) {
            // console.log("newStacks: ", newStacks);
            console.log("len: ", stacks[0].length);
        }
    }
    // return the path on the stacks with last element == end square
    return stacks.filter(x => x[x.length-1][0] == end[0] && x[x.length-1][1] == end[1])[0];
}

// instead of start -> end, do end -> start
function traceBack_4(lk, fitness, start, end) {
    const size_ = 30;
    let nextSteps = [];
    let stacks = [[end]];
    console.log("initial stacks: ", stacks);
    console.log("fitness: ", fitness);
    console.log("lk: ", lk);
    
    let newStacks = [];
    // console.log("[0]: ", stacks[0]);

    // stacks.forEach(s => {console.log("stack: ", s);});

    // while(Math.max(stacks.map(x=>x.length)) != fitness) {
    for (let i = 0 ; i <= fitness ; i++) {
        newStacks = [];
        // if (stacks[0].length >= fitness) {
        //     break;
        // }
        // for (let i = 0 ; i < stacks.length ; i++) {
        stacks.forEach(stack => {
            // stack = stacks[i];
            // console.log("stack: ", stack);
            pt = stack[stack.length-1];
            dist = lk[pt[0]][pt[1]];
            dist -= 1;
            nextSteps = [];
    
            foo = [pt[0]+1, pt[1]];
            if (foo[0] < size_ && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }
        
            foo = [pt[0], pt[1]+1];
            if (foo[1] < size_ && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }
            foo = [pt[0]-1, pt[1]];
            if (foo[0] >= 0 && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }
        
            foo = [pt[0], pt[1]-1];
            if (foo[1] >= 0 && lk[foo[0]][foo[1]] === dist) {
                nextSteps.push(foo);
            }

            nextSteps.forEach(s => {
                stack.push(s);
                newStacks.push([...stack]);
                stack.pop();
            });
        });
        // console.log("newStacks: ", newStacks);
        stacks = newStacks;
        // console.log("len: ", stacks[0].length);
    }
    // stacks.forEach(s => {
    //     console.log("stack: ", s);
    // });
    // return the path on the stacks with last element == end square
    return stacks.filter(x => x[x.length-1][0] == start[0] && x[x.length-1][1] == start[1])[0];
}

function splitJobs(lk, fitness, start, end) {
    console.log(`entering split for ${start}->${end} | fitness: ${fitness}`);
    console.log(lk);
    var midPtDist = Math.floor(fitness/2);
    var pt1 = null;
    var pt2 = null;
    var gotcha = false;
    for (let i = 0 ; i < 30 ; i++) {
        for (let j = 0 ; j < 30 ; j++) {
            if (lk[i][j] == midPtDist) {
                console.log(`found: ${i}, ${j}`);
                console.log(`PART 1: calling traceBack_3 on: ${midPtDist+1}, ${start}, ${[i, j]}`);
                pt1 = traceBack_3(lk, midPtDist+1, start, [i, j]);
                if (pt1 === undefined)
                    continue;
                if (fitness%2 == 0) {
                    console.log(`PART 2: calling traceBack_3 on: ${midPtDist+1}, ${[i, j]}, ${end}`);
                    pt2 = traceBack_3(lk, midPtDist+1, [i, j], end);
                }
                else {
                    console.log(`PART 2: calling traceBack_3 on: ${midPtDist+2}, ${[i, j]}, ${end}`);
                    pt2 = traceBack_3(lk, midPtDist+2, [i, j], end);
                }
                if (pt2 === undefined)
                    continue;
                gotcha = true;
                break
            }
            if (gotcha)
                break;
        }
        if (gotcha)
            break;
    }
    if (pt1 == null || pt2 == null)
        return ;
    return pt1.concat(pt2);
    
}

// returns path length, and array of steps from start to end through 'maze'
// the traceBack itself creates the lk array
// it uses traceBack to actually find the route
function pathFinder(start, end) {
    const size_ = 30;
    let lk = new Array(size_).fill(0).map(() => new Array(size_).fill(100_000));
    lk[start[0]][start[1]] = 0;

    
    let q = [start]; // Start from the upper left corner (i.e. array[0][0])
    let pt = [0, 0];
    let foo = [0, 0];
    let dist = 0;
    
    while (q.length > 0) {
        pt = q.pop();
        dist = lk[pt[0]][pt[1]];
        dist += 1;

        if (dist > lk[end[0]][end[1]]) 
            continue;

            
        foo = [pt[0]+1, pt[1]];
        if (foo[0] < size_ && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            else {
                break;
            }
        }

        foo = [pt[0], pt[1]+1];
        if (foo[1] < size_ && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            else {
                break;
            }
        }

        foo = [pt[0]-1, pt[1]];
        if (foo[0] >= 0 && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            else {
                break;
            }
        }

        foo = [pt[0], pt[1]-1];
        if (foo[1] >= 0 && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            else {
                break;
            }
        }
    }

    const fitness = lk[end[0]][end[1]];
    let traceBackResult = null;
    if (fitness > 170) {
        traceBackResult = splitJobs(lk, fitness, start, end);
    }
    else {
        traceBackResult = traceBack_3(lk, lk[end[0]][end[1]]+1, start, end);  
    }


    console.log("traceBack: ", traceBackResult);

    return [traceBackResult, lk[end[0]][end[1]]];
    return lk;
}
  

// for drawing
document.addEventListener("DOMContentLoaded", function(event) { 
    c = document.getElementById("canvas");
    ctx = document.getElementById("canvas").getContext("2d");
});

function process(input) {
    matrix = [];
    input.split("\n").forEach(l => {
        l = l.trim();
        l = l.split(", ");
        l[l.length- 1] = l[l.length- 1][0];
        matrix.push(l.map(x=>Number(x)));
    });
    console.log("matrix", matrix);
    let soln = pathFinder([0, 0], [29, 29]);
    solution = soln[0];
    console.log("from start to end: ", soln);

    let longSolution = [
        pathFinder([0, 0], checkpoints[0]),
        pathFinder(checkpoints[0], checkpoints[1]),
        pathFinder(checkpoints[1], checkpoints[2]),
        pathFinder(checkpoints[2], checkpoints[3]),
        pathFinder(checkpoints[3], [29, 29]),
    ];

    checkpointSolution = longSolution[0][0].concat(
        longSolution[1][0].slice(1),
        longSolution[2][0].slice(1),
        longSolution[3][0].slice(1),
        longSolution[4][0].slice(1),
    );

    console.log("checkpointSolution: ", checkpointSolution);
    
    console.log("path from pathFinder([0, 0], checkpoints[0]): ", longSolution[0]);
    console.log("path from pathFinder(checkpoints[0], checkpoints[1]): ", longSolution[0]);
    console.log("path from pathFinder(checkpoints[1], checkpoints[2]): ", longSolution[1]);
    console.log("path from pathFinder(checkpoints[2], checkpoints[3]): ", longSolution[2]);
    console.log("path from pathFinder(checkpoints[3], [29, 29]): ", longSolution[3]);

    for (let [i, row] of matrix.entries()) {
        for (let [j, val] of row.entries()) {
            if (val === 1) {
                // Draw a black square
                ctx.fillStyle = "black";
                ctx.fillRect(i*30, j*30, 30, 30);
            } else {
                // Draw a white square
                ctx.fillStyle = "white";
                ctx.fillRect(i*30, j*30, 30, 30);
            }
        }  
    }
}

function debugProcess(input) {
    matrix = [];
    input.split("\n").forEach(l => {
        l = l.trim();
        l = l.split(", ");
        l[l.length- 1] = l[l.length- 1][0];
        matrix.push(l.map(x=>Number(x)));
    });
    console.log("matrix", matrix);
    let soln = pathFinder([0, 0], [29, 29]);
    solution = soln[0];
    console.log("from start to end: ", soln);

    let longSolution = [
        pathFinder([0, 0], checkpoints[0]),
        pathFinder(checkpoints[0], checkpoints[1]),
        pathFinder(checkpoints[1], checkpoints[2]),
        pathFinder(checkpoints[2], checkpoints[3]),
        pathFinder(checkpoints[3], [29, 29]),
    ];

    checkpointSolution = longSolution[0][0].concat(
        longSolution[1][0].slice(1),
        longSolution[2][0].slice(1),
        longSolution[3][0].slice(1),
        longSolution[4][0].slice(1),
    );

    console.log("checkpointSolution: ", checkpointSolution);

    console.log("path from pathFinder([0, 0], checkpoints[0]): ", longSolution[0]);
    console.log("path from pathFinder(checkpoints[0], checkpoints[1]): ", longSolution[1]);
    console.log("path from pathFinder(checkpoints[1], checkpoints[2]): ", longSolution[2]);
    console.log("path from pathFinder(checkpoints[2], checkpoints[3]): ", longSolution[3]);
    console.log("path from pathFinder(checkpoints[3], [29, 29]): ", longSolution[4]);

    for (let [i, row] of matrix.entries()) {
        for (let [j, val] of row.entries()) {
            if (val === 1) {
                // Draw a black square
                ctx.fillStyle = "black";
                ctx.fillRect(i*30, j*30, 30, 30);
            } else {
                // Draw a white square
                ctx.fillStyle = "white";
                ctx.fillRect(i*30, j*30, 30, 30);
            }
        }  
    }
}

function showSolution() {
    solution.forEach(s => {
        ctx.fillStyle = "yellow";
        ctx.fillRect(s[0]*30, s[1]*30, 30, 30);
    });
}

function hideSolution() {
    solution.forEach(s => {
        ctx.fillStyle = "white";
        ctx.fillRect(s[0]*30, s[1]*30, 30, 30);
    });
}

function showCheckpoints() {
    checkpoints.forEach(s => {
        ctx.fillStyle = "red";
        ctx.fillRect(s[0]*30, s[1]*30, 30, 30);
    });
}

function hideCheckpoints() {
    checkpoints.forEach(s => {
        ctx.fillStyle = "white";
        ctx.fillRect(s[0]*30, s[1]*30, 30, 30);
    });
}

function showCheckpointSolution() {
    checkpointSolution.forEach(s => {
        ctx.fillStyle = "yellow";
        ctx.fillRect(s[0]*30, s[1]*30, 30, 30);
    })
    showCheckpoints();
}

function hideCheckpointSolution() {
    checkpointSolution.forEach(s => {
        ctx.fillStyle = "white";
        ctx.fillRect(s[0]*30, s[1]*30, 30, 30);
    })
    showCheckpoints();
}

function previewFile() {
    // const content = document.querySelector('.content');
    const [file] = document.querySelector('input[type=file]').files;
    const reader = new FileReader();
  
    reader.addEventListener("load", () => {
      // this will then display a text file
    //   content.innerText = reader.result;
      debugProcess(reader.result);
    }, false);
  
    if (file) {
      reader.readAsText(file);
    }
}