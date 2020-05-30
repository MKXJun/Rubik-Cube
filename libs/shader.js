//创建着色器
function createShader(gl, type, source) {
    let shader = gl.createShader(type);//创建着色器
    gl.shaderSource(shader, source);//附加着色器源文件到Shader
    gl.compileShader(shader);//编译着色器
    let success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);//判断着色器是是否编译成功
    if(success) {
        return shader;
    } else {
        console.log("着色器创建失败！");
    }

    console.log(gl.getShaderInfoLog(shader));//获取着色器信息
    gl.deleteShader(shader);//删除着色器
}

//链接着色器程序
function createProgram(gl, vertexShader, fragmentShader) {
    let program = gl.createProgram();//生成程序对象
    gl.attachShader(program, vertexShader);//向程序对象分配着色器
    gl.attachShader(program, fragmentShader);//向程序对象分配着色器
    gl.linkProgram(program);//链接着色器

    let success = gl.getProgramParameter(program, gl.LINK_STATUS);//判断着色器是否链接成功
    if(success) {
        return program;
    } else {
        console.log("着色器链接失败！");
    }

    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

//初始化着色器
function initShader(gl, vShaderText, fShaderText) {
    let vShader = createShader(gl, gl.VERTEX_SHADER, vShaderText);
    let fShader = createShader(gl, gl.FRAGMENT_SHADER, fShaderText);

    return createProgram(gl, vShader, fShader);
}