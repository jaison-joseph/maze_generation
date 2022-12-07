var solution = [];
var checkpointSolution = [];
var c = null;
var ctx = null;
var matrix = null;
var checkpoints = [[6, 24], [12, 18], [18, 12], [24, 6]];

// making way from entrance to exit
function traceBack(lk, fitness, end) {
    const size_ = 30;
    let nextSteps = [];
    let stacks = [[[0, 0]]];
    console.log("initial stacks: ", stacks)
    console.log("fitness: ", fitness)
    console.log("lk: ", lk)
    
    let newStacks = [];
    // console.log("[0]: ", stacks[0]);

    // stacks.forEach(s => {console.log("stack: ", s);});

    while(stacks[0].length != fitness) {
        newStacks = [];
        // if (stacks[0].length >= fitness) {
        //     break;
        // }
        for (let i = 0 ; i < stacks.length ; i++) {
        // stacks.forEach(stack => {
            stack = stacks[i];
            // console.log("stack: ", stack);
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
        // console.log("newStacks: ", newStacks);
        stacks = newStacks;
        console.log("len: ", stacks[0].length);
    }
    stacks.forEach(s => {
        console.log("stack: ", s);
    });
    // return the path on the stacks with last element == end square
    return stacks.filter(x => x[x.length-1][0] == end[0] && x[x.length-1][1] == end[1])[0];
}

function traceBack_2(lk, start, end) {
    const size_ = 30;
    let nextSteps = [];
    let stacks = [[start]];
    console.log("initial stacks: ", stacks);
    console.log("lk: ", lk);
    let success = false;
    let newStacks = [];
    // console.log("[0]: ", stacks[0]);

    // stacks.forEach(s => {console.log("stack: ", s);});

    while(true) {
        success = stacks.filter(x => x[x.length-1][0] == end[0] && x[x.length-1][1] == end[1]).length > 0;
        if (success)
            break;
        newStacks = [];
        // if (stacks[0].length >= fitness) {
        //     break;
        // }
        for (let i = 0 ; i < stacks.length ; i++) {
        // stacks.forEach(stack => {
            stack = stacks[i];
            // console.log("stack: ", stack);
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
        // console.log("newStacks: ", newStacks);
        stacks = newStacks;
        // console.log("len: ", stacks[0].length);
    }
    stacks.forEach(s => {
        console.log("stack: ", s);
    });
    // return the path on the stacks with last element == end square
    return stacks.filter(x => x[x.length-1][0] == end[0] && x[x.length-1][1] == end[1])[0];
}

function traceBack_3(lk, fitness, start, end) {
    const size_ = 30;
    let nextSteps = [];
    let stacks = [[start]];
    console.log("initial stacks: ", stacks)
    console.log("fitness: ", fitness)
    console.log("lk: ", lk)
    
    let newStacks = [];
    // console.log("[0]: ", stacks[0]);

    // stacks.forEach(s => {console.log("stack: ", s);});

    while(stacks[0].length != fitness) {
        newStacks = [];
        // if (stacks[0].length >= fitness) {
        //     break;
        // }
        for (let i = 0 ; i < stacks.length ; i++) {
        // stacks.forEach(stack => {
            stack = stacks[i];
            // console.log("stack: ", stack);
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
        // console.log("newStacks: ", newStacks);
        stacks = newStacks;
        // console.log("len: ", stacks[0].length);
    }
    // stacks.forEach(s => {
    //     console.log("stack: ", s);
    // });
    // return the path on the stacks with last element == end square
    return stacks.filter(x => x[x.length-1][0] == end[0] && x[x.length-1][1] == end[1])[0];
}

// returns path length, and array of steps from start to end through 'maze'
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
        // if (foo[0] == end[0] && foo[1] == end[1]) {
            // console.log("aha part 2", foo, dist, matrix[foo[0]][foo[1]]);
        // }
        if (foo[0] < size_ && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            // else {
                // console.log("gotcha");
            // }
        }

        foo = [pt[0], pt[1]+1];
        // if (foo[0] == end[0] && foo[1] == end[1]) {
            // console.log("aha part 2", foo, dist, matrix[foo[0]][foo[1]]);
        // }
        if (foo[1] < size_ && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            // else {
                // console.log("gotcha");
            // }
        }

        foo = [pt[0]-1, pt[1]];
        // if (foo[0] == end[0] && foo[1] == end[1]) {
            // console.log("aha part 2", foo, dist, matrix[foo[0]][foo[1]]);
        // }
        if (foo[0] >= 0 && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            // else {
                // console.log("gotcha");
            // }
        }

        foo = [pt[0], pt[1]-1];
        // if (foo[0] == end[0] && foo[1] == end[1]) {
            // console.log("aha part 2", foo, dist, matrix[foo[0]][foo[1]]);
        // }
        if (foo[1] >= 0 && lk[foo[0]][foo[1]] > dist && matrix[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
            // else {
                // console.log("gotcha");
            // }
        }
    }

    const traceBackResult = traceBack_3(lk, lk[end[0]][end[1]]+1, start, end);  

    console.log("traceBack: ", traceBackResult);

    return [traceBackResult, lk[end[0]][end[1]]];

    // const traceBackResult = traceBack(lk, lk[end[0]][end[1]]+1, end);  

    // return lk[end[0]][end[1]] == 100_000 ? [0, []] : [lk[end[0]][end[1]], traceBackResult];
}

// function drawArray(array) {
//     // Create a canvas element
//     var c = document.getElementById("canvas");
//     var ctx = c.getContext("2d");

//     const cellWidth = 30;
//     const cellHeight = 30;

//     console.log(cellWidth, ", ", cellHeight);
  
//     // Loop through the array and draw each cell
//     for (let i = 0, i_ = 0; i < 30; i++, i_+=cellWidth) {
//       for (let j = 0, j_ = 0; j < 30; j++, j_+=cellHeight) {
//         if (array[i][j] === 1) {
//           // Draw a black square
//           ctx.fillStyle = "black";
//           ctx.fillRect(i_, j_, cellWidth, cellHeight);
//         } else {
//           // Draw a white square
//           ctx.fillStyle = "white";
//           ctx.fillRect(i_, j_, cellWidth, cellHeight);
//         }
//       }
//     }
// }

// function drawSquare(x, y, w, h, color) {
//     var c = document.getElementById("canvas");
//     var ctx = c.getContext("2d");
//     ctx.fillStyle = color;
//     ctx.fillRect(x, y, w, h);
// }
  

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
    const pathFinderResult = pathFinder([0, 0], [29, 29]);
    solution = pathFinderResult[1];
    console.log("pathFinder path length: ", pathFinderResult[0]);
    console.log("pathFinder path solution: ", pathFinderResult[1]); 

    checkpointSolution = pathFinder([0, 0], checkpoints[0])[1].concat(
        pathFinder(checkpoints[1], checkpoints[2])[1],
        pathFinder(checkpoints[2], checkpoints[3])[1],
        pathFinder(checkpoints[3], [29, 29])[1]
    );
    
    // for drawing
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