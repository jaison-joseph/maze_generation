function drawPoint(x, y) {
    var c = document.getElementById("canvas");
    var ctx = c.getContext("2d");
    ctx.strokeStyle = 'red';
    ctx.fillStyle = 'rgba(255,0,0,0.1)';
    ctx.beginPath();
    ctx.arc(x, y, 5, 0, 2 * Math.PI);
    ctx.stroke();
    ctx.fill();
}

function draw() {
    const nums = [50, 100, 150, 200, 250];
    let coordinates = [];
    let foo = [];
    nums.forEach(i => {
        foo = [];
        nums.forEach(j => {
            foo.push([i, j]);
        });
        coordinates.push(foo);
    });
    coordinates.forEach(row => {
        row.forEach(pt => {
            drawPoint(pt[0], pt[1]);
            // console.log(pt[0], pt[1]);
        })
    });


    //the actual maze generation begins here
    let neighbors = {};

    const indices = [0, 1, 2, 3];
    for (var i of indices) {
        for (var j of indices) {
            let edges = [];
            i -= 1;
            if (i >= 0)
                edges.push([i, j]);
            i += 2;
            if (i < 4)
                edges.push([i, j]);
            i -= 1;

            j -= 1;
            if (j >= 0)
                edges.push([i, j]);
            j += 2;
            if (j < 4)
                edges.push([i, j]);
            j -= 1;
            neighbors[[i, j]] = edges;
        }
    }
    var walls = {...neighbors};
    // for (const [key, value] of Object.entries(walls)) {
    //     console.log(key, value);
    // }

    var visited = [];
    var curPoint = [0,0];
    var newPoint = [];
    var stack = [curPoint];
    var options = [];

    // generate the maze
    while (true) {
        visited.push(curPoint);
        options = neighbors[curPoint].filter(x => !visited.includes(x));
        if (options.length == 0) {
            if (stack.length == 0)
                break;
            curPoint = stack.pop();
            continue;
        }
        stack.push(curPoint);
        newPoint = options[Math.floor(Math.random() * options.length)];
        walls[curPoint] = walls[curPoint].filter(x => x != newPoint);
        walls[newPoint] = walls[newPoint].filter(x => x != curPoint);
        curPoint = newPoint;
    }

    console.log("visited.length: ", visited.length);

    // consoling the walls
    for (const [key, value] of Object.entries(walls)) {
        console.log(key, value);
    }

    /**
     * if there's a wall between (0,0) and (0,1), 
     *      draw a line btw points[0][1] -> points[1][1]
     * if there's a wall between (i,j) and (i,j+1), 
     *      draw a line btw points[i][j+1] -> points[i+1][j+1]
     * 
     * 
     * if there's a wall between (0,0) and (1,0), 
     *      draw a line btw points[1][0] -> points[1][1]
     * if there's a wall between (i,j) and (i+1,j), 
     *      draw a line btw points[i+1][j] -> points[i+1][j+1]
     */
    

}
