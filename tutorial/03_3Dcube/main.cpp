#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

const char* vertexShaderSource = R"(
	// Vertex shader
	#version 330 core
	
	// Input vertex data, different for all executions of this shader.
	layout(location = 0) in vec3 vertexPosition_localspace;
	layout(location = 1) in vec3 vertexColor;
	layout(location = 2) in vec3 vertexNormal;
	
	// Output data ; will be interpolated for each fragment.
	out vec3 fragmentPosition_worldspace;  
	out vec3 fragmentBaseColor;
	out vec3 fragmentNormal;

	// Values that stay constant for the whole mesh.
	uniform mat4 Model, View, Projection;
	
	void main(){	
	
		// Output position of the vertex, in clip space : ModelViewProjection * position
		gl_Position =  Projection * View * Model * vec4(vertexPosition_localspace, 1.0); // Remember, matrix multiplication is the other way around
	
	    // Pass vertex attributes to the fragment shader
		fragmentPosition_worldspace = vec3(Model * vec4(vertexPosition_localspace, 1.0));
	    fragmentBaseColor = vertexColor;
	    fragmentNormal = vertexNormal;
	}
)";

const char* fragmentShaderSource = R"(
	// Fragment shader
	#version 330 core

	// Interpolated values from the vertex shaders
	in vec3 fragmentPosition_worldspace;
	in vec3 fragmentBaseColor;
	in vec3 fragmentNormal;

	// Camera position
	uniform vec3 cameraPosition;

	// Light properties
	uniform vec3 lightPosition, lightColor;
	uniform float lightPower;

	// Material properties
	uniform vec3 materialDiffuse, materialAmbient, materialSpecular;
	uniform float materialShininess;

	// Ouput data
	out vec4 fragmentColor;

	void main(){
		// Normalize the interpolated normal
	    vec3 normal = normalize(fragmentNormal);

		// Calculate the direction from the fragment to the light


		// Calculate Lighting and Materials
		// Ambient component.
		vec3 ambient = materialAmbient * lightColor * materialDiffuse;

		// Diffuse component.
		vec3 vectorFtoL = lightPosition - fragmentPosition_worldspace;
		float lightDistance = length(vectorFtoL);
		vec3 lightDirection = normalize(vectorFtoL);
	    float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	    vec3 diffuse = diffuseStrength * lightColor * materialDiffuse * lightPower / (lightDistance * lightDistance);

		// Specular component.
		vec3 viewDirection = normalize(cameraPosition - fragmentPosition_worldspace);
	    vec3 reflectDirection = reflect(-lightDirection, normal); // reflect(I, N) == I - 2 * dot(N, I) * N
	    float specularStrength = pow(max(dot(viewDirection, reflectDirection), 0.0), materialShininess);
	    vec3 specular = specularStrength * lightColor * materialSpecular * lightPower / (lightDistance * lightDistance);

		// Calculate final color
		vec3 result = (ambient + diffuse + specular) * fragmentBaseColor;
		fragmentColor = vec4(result, 1.0);
	}
)";

// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

// One color for each vertex. They were generated randomly.
static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
};

// Calculate normal data for each vertex
std::vector<glm::vec3> calculateVertexNormals(const GLfloat* vertices, size_t numVertices) {
    std::vector<glm::vec3> vertexNormals(numVertices / 3, glm::vec3(0.0f));

    for (size_t i = 0; i < numVertices; i += 9) {
        glm::vec3 v0(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 v1(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        glm::vec3 v2(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Accumulate normals for each vertex of the triangle
        vertexNormals[i / 3] += normal;
        vertexNormals[i / 3 + 1] += normal;
        vertexNormals[i / 3 + 2] += normal;
    }

    // Normalize the accumulated normals
    for (glm::vec3& normal : vertexNormals) {
        normal = glm::normalize(normal);
    }

    return vertexNormals;
}

void checkShaderCompilationLinking(bool debug, GLuint vertexShader, GLuint fragmentShader, GLuint shaderProgramID) {
	if (debug) {
		GLint success;
		GLchar infoLog[512];

		// Check vertex shader compilation
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
		    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		    std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
		}

		// Check fragment shader compilation
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
		    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		    std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
		}

		// Check program linking
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
		if (!success)
		{
		    glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		    std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
		}
	}
}

void checkGLError(std::string pointName, bool debug) {
	if (debug) {
		std::cout << pointName << std::endl;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error: " << err << std::endl;
		}
	}
}

const bool DEBUG = true;

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

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	checkGLError("initializing", DEBUG);
	//------------------------------------------------------------

    // Create normal buffer data
    std::vector<glm::vec3> vertexNormals = calculateVertexNormals(g_vertex_buffer_data, sizeof(g_vertex_buffer_data) / sizeof(GLfloat));
	GLfloat g_normal_buffer_data[3 * vertexNormals.size()];
	for (int i = 0; i < vertexNormals.size(); i++) {
		g_normal_buffer_data[3 * i + 0] = (vertexNormals.at(i)).x;
		g_normal_buffer_data[3 * i + 1] = (vertexNormals.at(i)).y;
		g_normal_buffer_data[3 * i + 2] = (vertexNormals.at(i)).z;
	}

    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	checkShaderCompilationLinking(true, vertexShader, fragmentShader, shaderProgramID);

	// Create VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer; // VBO
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer; // VBO
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	GLuint normalbuffer; // VBO
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_normal_buffer_data), g_normal_buffer_data, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
	    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	    3,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
	    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
	    3,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);

	checkGLError("VAO and VBOs", DEBUG);

	// Get uniform locations for lighting properties and material properties
    GLuint LightPositionID = glGetUniformLocation(shaderProgramID, "lightPosition");
    GLuint LightColorID = glGetUniformLocation(shaderProgramID, "lightColor");
	GLuint LightPowerID = glGetUniformLocation(shaderProgramID, "lightPower");
    GLuint CameraPositionID = glGetUniformLocation(shaderProgramID, "cameraPosition");

    GLuint MaterialDiffuseID = glGetUniformLocation(shaderProgramID, "materialDiffuse");
	GLuint MaterialAmbientID = glGetUniformLocation(shaderProgramID, "materialAmbient");
	GLuint MaterialSpecularID = glGetUniformLocation(shaderProgramID, "materialSpecular");
	GLuint MaterialShininessID = glGetUniformLocation(shaderProgramID, "materialShininess");

	// Set world properties(lighting, camera) and material properties(Diffuse, Specular, Ambient).
	glm::vec3 lightPosition, lightColor, cameraPosition, cameraTarget;
	lightPosition = glm::vec3(5.0f, 3.0f, 0.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	GLfloat lightPower = 50.0f;
	cameraPosition = glm::vec3(4.0f, 3.0f, -3.0f);
	cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Camera looks at the origin

	glm::vec3 materialAmbient, materialDiffuse, materialSpecular;
	materialDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	materialAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	materialSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	GLfloat materialShininess = 32.0f;



	// Get a handle for our "Model View Projection" uniform
	GLuint ModelID = glGetUniformLocation(shaderProgramID, "Model");
	GLuint ViewID = glGetUniformLocation(shaderProgramID, "View");
	GLuint ProjectionID = glGetUniformLocation(shaderProgramID, "Projection");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								cameraPosition,   // Camera is at (4,3,-3), in World Space
								cameraTarget,     // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);

	checkGLError("Model View Projection", DEBUG);

	int time = 0.0f;

    // Render loop
	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(shaderProgramID);

		// Animate Light Position
		lightPosition = glm::vec3(5 * glm::cos(time/100.0f), 3.0f, 5 * glm::sin(time/100.0f));

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &Projection[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Set light properties and material properties
        glUniform3f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z);
        glUniform3f(LightColorID, lightColor.x, lightColor.y, lightColor.z);
		glUniform1f(LightPowerID, lightPower);
        glUniform3f(CameraPositionID, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glUniform3f(MaterialDiffuseID, materialDiffuse.x, materialDiffuse.y, materialDiffuse.z);
		glUniform3f(MaterialAmbientID, materialAmbient.x, materialAmbient.y, materialAmbient.z);
		glUniform3f(MaterialSpecularID, materialSpecular.x, materialSpecular.y, materialSpecular.z);
		glUniform1f(MaterialShininessID, materialShininess);


		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		time+=1.0f;
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(shaderProgramID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
