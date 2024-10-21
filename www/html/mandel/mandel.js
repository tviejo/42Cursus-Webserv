// mandel.js : Mandelbrot Set in WebGL2 by mig29-at-noos.fr

"use strict";

function main() {
  //
  var bDrawFractal = true;
  var bDrawKishi = true;
  var enableFractRot = true;
  var iterMax = 250;
  var fsRect = [-2.0, -1.275, 0.55, 1.275];   // région de l'ensemble fractal à dessiner
  var zoomTarget3 = [-0.7808, 0.11995/*0.1201*/];
  var zoomTarget2 = [-0.7775, 0.11995/*0.1201*/];
  var zoomTarget1 = [-1.3652, 0.2201];
  //var zoomTarget = [-0.7775, 0.11995/*0.1201*/];
  //var zoomTarget = [-1.3652, 0.0201];
  //var zoomTarget = [0.37498, 0.08964];
  var zoomTargetRecenterFactor = 0.5;//0.33;
  var zoomFactor = 1.2;//1.25;
  var colorRotSpeed = 22.0;//10.0;
  var colorShift = 0.0;
  
  var zNear = 0.01;
  var zFar = 100;
  var fieldOfViewRadians = degToRad(60);
  var translation = [0, 0, -1.73];
  var rotation = [degToRad(0), degToRad(0), degToRad(0)];
  var rotSpeed = 0.0;
  if (enableFractRot) {
    translation = [0, 0, -1.73 + 0.00033 + 0.0];
    //rotation = [degToRad(190), degToRad(40), degToRad(30)];
    //rotation = [degToRad(-20), degToRad(40), degToRad(30)];
    rotSpeed = 0.7*0.96;
    zoomFactor = 1.2;
    colorRotSpeed = 27.0;
    //zoomTargetRecenterFactor = 0.7;
  }
  //var rotationsSpeed = [0.17*rotSpeed, 0.22*rotSpeed, 0.15*rotSpeed];
  var rotationsSpeed = [0.0*rotSpeed, 0.0*rotSpeed, 0.15*rotSpeed];
  var scale = [1, 1, 1];
  
  function radToDeg(r) {
    return r * 180 / Math.PI;
  }

  function degToRad(d) {
    return d * Math.PI / 180;
  }
  
  var gl;
  var matFSetRange_uloc, matFSetMVP_uloc;
  var matKishiMVP_uloc;
  var progFract, progKishi;
  var rect;
  var vao;
  var iterMax_uloc, colorShift_uloc;
  var timeSec_uloc;
  var time0Sec, timeSec, duréeSec;
  var uvShiftPerS_uloc;

  // Initialize WebGL and setup scene
  function init() {
    // Get A WebGL context
    var canvas = document.querySelector("#c");
    gl = canvas.getContext("webgl2");
    if (!gl) {
      return;
    }

    progFract = webglUtils.createProgramFromScripts(gl, ['shader-vs', 'shader-fs']);

    matFSetRange_uloc = gl.getUniformLocation(progFract, "matFSetRange");
    matFSetMVP_uloc = gl.getUniformLocation(progFract, "matMVP");
    colorShift_uloc = gl.getUniformLocation(progFract, "colorShift"); 
    iterMax_uloc = gl.getUniformLocation(progFract, "iterMax");
    
    // look up where the vertex data needs to go.
    var position_aloc = gl.getAttribLocation(progFract, "a_position");

    rect = [-1,-1.0,0.0, 1,1.0,0.0];   // used by fractal to setup matrixFSetRange
    
    var x = 4.0;
    var positions = [
      -x,-x,0,  -x, x,0,   x, x,0,
       x, x,0,   x,-x,0,  -x,-x,0
    ];

    // Create a buffer and put three 2d clip space points in it
    var positionBuffer = gl.createBuffer();

    // Bind it to ARRAY_BUFFER (think of it as ARRAY_BUFFER = positionBuffer)
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);

    // Create a vertex array object (attribute state)
    vao = gl.createVertexArray();

    // and make it the one we're currently working with
    gl.bindVertexArray(vao);

    // Turn on the attribute
    gl.enableVertexAttribArray(position_aloc);

    // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
    var size = 2;          // components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 3*4;        // 0 = move forward size * sizeof(type) each iteration to get the next position
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(position_aloc, size, type, normalize, stride, offset);
    
    // Kishi shader program
    progKishi = webglUtils.createProgramFromScripts(gl, ['shader-vs_pos_uv', 'shader-fs_kishi']);
    matKishiMVP_uloc = gl.getUniformLocation(progKishi, "matMVP");
    timeSec_uloc = gl.getUniformLocation(progKishi, "timeSec");
    uvShiftPerS_uloc = gl.getUniformLocation(progKishi, "uvShiftPerS");
    
    // a_position
    position_aloc = gl.getAttribLocation(progKishi, "a_position");
    gl.enableVertexAttribArray(position_aloc);
    gl.vertexAttribPointer(position_aloc, 2, gl.FLOAT, false, 3*4, 0);

    // a_uv
    var uv = [-1,-1, -1,1, 1,1,  1,1, 1,-1, -1,-1,];
    var uvBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, uvBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(uv), gl.STATIC_DRAW);
    var uv_aloc = gl.getAttribLocation(progKishi, "a_uv");
    gl.enableVertexAttribArray(uv_aloc);
    gl.vertexAttribPointer(uv_aloc, 2, gl.FLOAT, false, 0, 0);
  }
  
  // Draw the scene.
  function drawScene() {
    // Resize canvas
    webglUtils.resizeCanvasToDisplaySize(gl.canvas);

    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    // Clear the canvas
    //gl.clearColor(0.0, 0.0, 0.0, 1.0);
    //gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    //gl.enable(gl.DEPTH_TEST);   
    //gl.enable(gl.CULL_FACE);
    //gl.cullFace(gl.BACK);
    gl.enable(gl.BLEND);
    //gl.blendFunc(gl.SRC_COLOR, gl.DST_COLOR);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    // Tell it to use our program (pair of shaders)
    gl.useProgram(progFract);

    // Bind the attribute/buffer set we want.
    gl.bindVertexArray(vao);

    // Compute the matrix
    var aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
    var matrixProjection = m4.perspective(fieldOfViewRadians, aspect, zNear, zFar);
    var matrixMVP = m4.copy(matrixProjection);
    
    m4.translate(matrixMVP, translation[0], translation[1], translation[2], matrixMVP);
    m4.xRotate(matrixMVP, rotation[0], matrixMVP);
    m4.yRotate(matrixMVP, rotation[1], matrixMVP);
    m4.zRotate(matrixMVP, rotation[2], matrixMVP);
    m4.scale(matrixMVP, scale[0]*(16.0/8.0), scale[1], scale[2], matrixMVP);
    
    //var fsRect = [-2.0, -1.275, 0.55, 1.275];   // région de l'ensemble fractal à dessiner
    var x = [fsRect[0], fsRect[2]];
    var y = [fsRect[1], fsRect[3]];
    var rxsize = rect[3] - rect[0];   // largeur du rectangle surface (géometrie)
    var rysize = rect[4] - rect[1];   // hauteur du rectangle surface (géometrie)
    var xscale = (x[1] - x[0]) / rxsize;
    var yscale = (y[1] - y[0]) / rysize;
    var matrixFSetRange = m4.identity();
    
    m4.translate(matrixFSetRange, x[0] - xscale * rect[0],
                                  y[0] - yscale * rect[1], 0, matrixFSetRange);
    m4.scale(matrixFSetRange, xscale*(16.0/8.0), yscale, 1, matrixFSetRange);
          
    gl.uniformMatrix4fv(matFSetRange_uloc, false, matrixFSetRange);
    gl.uniformMatrix4fv(matFSetMVP_uloc, false, matrixMVP);
    gl.uniform1i(colorShift_uloc, colorShift);
    gl.uniform1i(iterMax_uloc, iterMax);
    
    // draw fractal
    //gl.drawArrays(primitiveType, offset, count);
    
    // draw kishi
    gl.useProgram(progKishi);
    //gl.bindVertexArray(vao);
    m4.copy(matrixProjection, matrixMVP);
    m4.translate(matrixMVP, 0.0, -1.0, -10.0, matrixMVP);
    m4.xRotate(matrixMVP, degToRad(-90+timeSec*20), matrixMVP);
    var delai1 = 10.0;
    if (duréeSec > delai1)
      m4.yRotate(matrixMVP, degToRad((duréeSec-delai1)*7), matrixMVP);
    m4.scale(matrixMVP, 10.0, 10.0, 10.0, matrixMVP);
    gl.uniformMatrix4fv(matKishiMVP_uloc, false, matrixMVP);
    gl.uniform1f(timeSec_uloc, timeSec);
    if (duréeSec < delai1)
      gl.uniform2f(uvShiftPerS_uloc, 0.02, 0.3);
    else
      gl.uniform2f(uvShiftPerS_uloc, 0.02, 0.3 + 0.02*(duréeSec-delai1));
    
    if (bDrawKishi)
      gl.drawArrays(gl.TRIANGLES, 0, 2*3);
    
    // draw fractal
    gl.useProgram(progFract);
    if (bDrawFractal && duréeSec<55.0)
      gl.drawArrays(gl.TRIANGLES, 0, 2*3);
    
    /*
    webglUtils.resizeCanvasToDisplaySize(gl.canvas);

    // Every frame increase the rotation a little.
    rotation[1] += rotationSpeed * deltaTime;

    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    // Clear the canvas
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // turn on depth testing
    gl.enable(gl.DEPTH_TEST);

    // tell webgl to cull faces
    gl.enable(gl.CULL_FACE);

    // Tell it to use our program (pair of shaders)
    gl.useProgram(program);

    // Bind the attribute/buffer set we want.
    gl.bindVertexArray(vao);

    // Compute the matrix
    var aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
    var zNear = 1;
    var zFar = 2000;
    var matrix = m4.perspective(fieldOfViewRadians, aspect, zNear, zFar);
    matrix = m4.translate(matrix, translation[0], translation[1], translation[2]);
    matrix = m4.xRotate(matrix, rotation[0]);
    matrix = m4.yRotate(matrix, rotation[1]);
    matrix = m4.zRotate(matrix, rotation[2]);
    matrix = m4.scale(matrix, scale[0], scale[1], scale[2]);

    // Set the matrix.
    gl.uniformMatrix4fv(matrixLocation, false, matrix);

    // Draw the geometry.
    var primitiveType = gl.TRIANGLES;
    var offset = 0;
    var count = 16 * 6;
    gl.drawArrays(primitiveType, offset, count);
  */
  }
  
  var prevTime = 0;
  //
  function renderFrame(time) {
    // Convert to seconds
    time *= 0.001;
    timeSec = time;
    
    // Subtract the previous time from the current time
    var deltaTime = time - prevTime;
    if (prevTime == 0) { // first call
      time0Sec = timeSec;
      deltaTime = 0.0;
    }
    // Remember the current time for the next frame.
    prevTime = time;
    duréeSec = timeSec - time0Sec;

    // Every frame increase the rotation a little.
    rotation[0] += rotationsSpeed[0] * deltaTime;
    rotation[1] += rotationsSpeed[1] * deltaTime;
    rotation[2] += rotationsSpeed[2] * deltaTime;
    
    // zoom inside fractal: update fsRect
    var zoom = (1.0 + (zoomFactor -1.0) * deltaTime);
    var xc = (fsRect[0] + fsRect[2]) / 2.0;  // x center
    var yc = (fsRect[1] + fsRect[3]) / 2.0;  // y center
    var x_zoom, y_zoom;
    var zoomTarget = duréeSec < 5.5 ? zoomTarget1 : duréeSec < 30.0 ? zoomTarget2 : zoomTarget3;
    var x_zoom = zoomTarget[0] + zoomTargetRecenterFactor * (zoomTarget[0] - xc);
    var y_zoom = zoomTarget[1] + zoomTargetRecenterFactor * (zoomTarget[1] - yc);
    fsRect = [
      (fsRect[0] - x_zoom) / zoom + x_zoom, (fsRect[1] - y_zoom) / zoom + y_zoom,
      (fsRect[2] - x_zoom) / zoom + x_zoom, (fsRect[3] - y_zoom) / zoom + y_zoom
    ];

    // color animation
    colorShift += colorRotSpeed*deltaTime;
    
    drawScene();
    requestAnimationFrame(renderFrame);
  }
  
  init();
  requestAnimationFrame(renderFrame);  
}

main();