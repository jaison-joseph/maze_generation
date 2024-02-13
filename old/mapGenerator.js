/**
 * Classic Conway/cellular style cave generator
 * 
 * Code structure inspired by Sebastian Lague's procedural cave generation: Sebastian Lague
 *  (https://github.com/SebLague/Procedural-Cave-Generation/blob/master/Episode%2001/MapGenerator.cs)
 *  
 * Author: Jaison Joseph (jaisonj2001@gmail.com)
 * 
 * 
*/

/**
 * the mapGenerator class: encapsulation of the logic and GUI logic to create a 2D-cave using a finite-automata style generation algorithm
 * 
 * Some pre-defaults for decent caves:
 * 
 *      this.neighborSurvivorCount = 5;
 *      this.wallProb = 0.4;
 *      this.smoothIterations = 2;
 *      
 *      this.neighborSurvivorCount = 5;
 *      this.wallProb = 0.62;
 *      this.smoothIterations = 5;
 *      
 *      this.neighborSurvivorCount = 5;
 *      this.wallProb = 0.66;
 *      this.smoothIterations = 4;
 * 
 * Using the mapGenerator class:
 * 
 * <canvas id="myCanvas" width="500" height="500">
 * </canvas>
 * <script src="mapGenerator.js">
 * </script>
 * <script>
 *      const map = new mapGenerator(100,100);
 *      map.generateNewMap();
 * </script>
 */

class mapGenerator {
    width;                          // number of columns in the cave       

    height;                         // number of rows in the cave
    
    map;                            // 2D array representing the cave, stores 1's or 0's; 1->Wall and 0->space
    
    newMap;                         // 2D array to hold the new values for the next version of the cave, after the smooth function has been applied
    
    canvas;                         // variable referencing the canvas element in the HTML page; selects the first canvas element by using document.querySelector
    
    ctx;                            // the context in the canvas element
    
    coordinates;                    // coordinates of each of the squares drawn to 'create' the cave 
    
    neighborSurvivorCount;          // TUNING PARAMETER: the number of neighboring walls to cell [i][j] for it to survive an iteration of the smoothing function: default is 5
    
    wallProb;                       // TUNING PARAMETER: controls the odds of creating a wall at a given cell [i][j] during initialization, should be a number in range (0, 1)
    
    smoothIterations;               // TUNING PARAMETER: the number of times the smooth function is called

    /**
     * 
     * @param {number} w number of columns in the cave 
     * @param {number} h number of rows in the cave
     */
    constructor(w, h) {
        this.width = w;
        this.height = h;
        this.map = [];
        this.newMap = [];
        this.neighborSurvivorCount = 5;
        this.wallProb = 0.63;
        this.smoothIterations = 4;

        // initialize map
        for(let i = 0 ; i < this.height ; i++) {
            this.map.push(new Array(this.width));
            this.newMap.push(new Array(this.width));
        }

        // creating coordinates for the pixels
        const l = 5; 
        var x = 0, y = 0, row = [];
        this.coordinates = [];
        for(let i = 0 ; i < this.height ; i++) {
            x = 0;
            row = [];
            for(let j = 0 ; j < this.width ; j++) {
                row.push([x, y, x+l, y+l]);
                x += l;           
            }
            this.coordinates.push(row);
            y += l;
        }

        // setting drawing variables
        this.canvas = document.querySelector("canvas");
        this.ctx = this.canvas.getContext("2d");
        
    }

    generateNewMap() {
        // fill map initially based on this.wallProb, all corner cells are walls
        for(let i = 0 ; i < this.height ; i++) {
            for(let j = 0 ; j < this.width ; j++) {
                if (j == 0 || j == this.width-1 || i == 0 || i == this.height -1) {
                    this.map[i][j] = 1;
                    this.newMap[i][j] = 1;
                }
                else {
                    this.map[i][j] = (Math.random() < this.wallProb)? 1: 0;
                }
            }
        }

        // this.map.forEach(r => {
        //     console.log(r);
        // })
        // console.log(" --------------------------------------------------- ");
        for (let i = 0 ; i < this.smoothIterations ; i++) {
            this.smoothMap();
            this.map = this.newMap;
        }
        this.drawMap();
    }

    /**
     * counts # of wall neighbors of a cell @ [x][y]
     * 
     * if [x][y] is a cell on a corner, this.neighborSurvivorCount is returned
     *      this is because corner walls are to be preserved
     */ 
    getWallCount(x, y) {
        if (
            x == 0 || y == 0 || x == this.height-1 || y == this.width-1
        ) {
            return this.neighborSurvivorCount;
        }
        var count = 0;
        const r = [x-1, x, x+1].filter(f => f >= 0 && f < this.height);
        const c = [y-1, y, y+1].filter(f => f >= 0 && f < this.width);
        r.forEach(i => {
            c.forEach(j => {
                if (i != x || j != y) {
                    count += this.map[i][j];
                }
            });
        });
        return count;
    }

    /**
     * 'smoothens' the cave based on the condition (number of surviving walls >=neighborSurvivorCount) 
     */
    smoothMap() {
        for(let i = 0 ; i < this.height ; i++) {
            for(let j = 0 ; j < this.width ; j++) {
                this.newMap[i][j] = (this.getWallCount(i, j) >= this.neighborSurvivorCount) ? 1 : 0;
            }
        }
    }

    // 1's -> black -> wall
    // 0's -> white -> passage
    drawMap() {
        var foo = [];
        for(let i = 0 ; i < this.height ; i++) {
            for(let j = 0 ; j < this.width ; j++) {
                foo = this.coordinates[i][j];
                this.ctx.beginPath();
                this.ctx.rect(foo[0], foo[1], foo[2], foo[3]);
                if (this.map[i][j] == 1) {
                    this.ctx.fillStyle = "black";
                }
                else {
                    this.ctx.fillStyle = "white";
                }
                this.ctx.fill();
            }
        }
    }
}
