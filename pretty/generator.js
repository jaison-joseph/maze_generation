function findShortestPath(array) {
    // Check if the input array is valid (i.e. a 30 by 30 2D array of 1's and 0's)
    if (!Array.isArray(array) || array.length !== 30 || !Array.isArray(array[0]) || array[0].length !== 30) {
      return;
    }
  
    // Create a 30 by 30 2D array to store the shortest path
    const path = new Array(30).fill(0).map(() => new Array(30).fill(0));
  
    // Use BFS (Breadth-First Search) to find the shortest path
    const queue = [[0, 0]]; // Start from the upper left corner (i.e. array[0][0])
    const visited = new Set();
    visited.add("0,0");
    while (queue.length > 0) {
      console.log(queue);
      const [i, j] = queue.shift();
      if (i === 29 && j === 29) { // Stop when reaching the lower right corner (i.e. array[29][29])
        break;
      }
  
      // Check if the adjacent cells (up, down, left, and right) can be visited
      if (i < 30 && array[i + 1][j] === 0 && !visited.has(`${i + 1},${j}`)) {
        queue.push([i + 1, j]);
        path[i + 1][j] = 1;
        visited.add(`${i + 1},${j}`);
      }
      if (j < 30 && array[i][j + 1] === 0 && !visited.has(`${i},${j + 1}`)) {
        queue.push([i, j + 1]);
        path[i][j + 1] = 1;
        visited.add(`${i},${j + 1}`);
      }
      if (i > 0 && array[i - 1][j] === 0 && !visited.has(`${i - 1},${j}`)) {
        queue.push([i - 1, j]);
        path[i - 1][j] = 1;
        visited.add(`${i - 1},${j}`);
      }
      if (j > 0 && array[i][j - 1] === 0 && !visited.has(`${i},${j - 1}`)) {
        queue.push([i, j - 1]);
        path[i][j - 1] = 1;
        visited.add(`${i},${j - 1}`);
      }
    }
    return path;
}

// making way from entrance to exit
function traceBack(lk, s) {
    const size_ = 30;
    let pt = s[s.length - 1];
    let dist = lk[pt[0]][pt[1]];
    if (pt[0] == 29 && pt[1] == 29)
        return s;
    dist += 1;
    let foo = [0,0];
    let nextSteps = [];
    let result = [];

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

    nextSteps.forEach(pt => {
        s.push(pt);
        result = traceBack(lk, s);
        if (result.length > 0) {
            return result;
        }
        s.pop();
    });

    return [];
}

function pathFinder(maze, start, end) {
    const size_ = 30;
    let lk = new Array(size_).fill(0).map(() => new Array(size_).fill(100_000));
    lk[start[0]][start[1]] = 0;

    
    let q = [[0, 0]]; // Start from the upper left corner (i.e. array[0][0])
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
        if (foo[0] < size_ && lk[foo[0]][foo[1]] > dist && maze[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
        }

        foo = [pt[0], pt[1]+1];
        if (foo[1] < size_ && lk[foo[0]][foo[1]] > dist && maze[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
        }
        foo = [pt[0]-1, pt[1]];
        if (foo[0] >= 0 && lk[foo[0]][foo[1]] > dist && maze[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
        }

        foo = [pt[0], pt[1]-1];
        if (foo[1] >= 0 && lk[foo[0]][foo[1]] > dist && maze[foo[0]][foo[1]] === 0) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo[0] != end[0] || foo[1] != end[1]) {
                q.push(foo);
            }
        }
    }

    console.log(lk);

    console.log(traceBack(lk, [[0, 0]]));

    return lk[end[0]][end[1]] == 100_000 ? 0 : lk[end[0]][end[1]];
}

function drawArray(array) {
    // Create a canvas element
    var c = document.getElementById("canvas");
    var ctx = c.getContext("2d");

    const cellWidth = 30;
    const cellHeight = 30;

    console.log(cellWidth, ", ", cellHeight);
  
    // Loop through the array and draw each cell
    for (let i = 0, i_ = 0; i < 30; i++, i_+=cellWidth) {
      for (let j = 0, j_ = 0; j < 30; j++, j_+=cellHeight) {
        if (array[i][j] === 1) {
          // Draw a black square
          ctx.fillStyle = "black";
          ctx.fillRect(i_, j_, cellWidth, cellHeight);
        } else {
          // Draw a white square
          ctx.fillStyle = "white";
          ctx.fillRect(i_, j_, cellWidth, cellHeight);
        }
      }
    }
}
  

function process(input) {
    var lines = [];
    input.split("\n").forEach(l => {
        l = l.trim();
        lines.push(l.split(", ").map(x=>Number(x)));
    });
    drawArray(lines);
    console.log(pathFinder(lines, [0, 0], [29, 29]));
}

function previewFile() {
    // const content = document.querySelector('.content');
    const [file] = document.querySelector('input[type=file]').files;
    const reader = new FileReader();
  
    reader.addEventListener("load", () => {
      // this will then display a text file
    //   content.innerText = reader.result;
      process(reader.result);
    }, false);
  
    if (file) {
      reader.readAsText(file);
    }
}