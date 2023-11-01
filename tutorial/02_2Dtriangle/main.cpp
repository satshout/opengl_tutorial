#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos; // position variable has attribute position 0
    layout (location = 1) in vec3 aColor; // color variable has attribute position 1
    out vec3 ourColor; // output a color to the fragment shader
    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        ourColor = aColor; // set ourColor to the input color we got from the vertex data
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor; // the input variable from the vertex shader (same name and same type)
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(ourColor, 1.0); // pass the color directly through the pipeline
    }
)";

int main()
{
	//init -------------------------------------------------------
	// initialize GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// set GLFW version 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// set GLFW to use the core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create a GLFWwindow object 
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL on MAC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// create a context
	glfwMakeContextCurrent(window);
	// GLAD: load all OpenGL function pointers
	if(!gladLoadGL(glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//------------------------------------------------------------


    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	// vertex data
	float vertices[] = {
	    // 位置             // 色
	     0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // 赤
	    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // 緑
	     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // 青
	};

    // Create Vertex Buffer Object (VBO) and Vertex Array Object (VAO)
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
	// transfer vertex data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// set position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// set color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Use shader program and draw triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
