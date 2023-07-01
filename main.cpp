
#include <iostream>
#include <glad/glad.h>
#include <GLFW\glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void processInput(GLFWwindow* window);

// Function re-calculate the Viewport when the dimensions of the screen change
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Func
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// VERTEX SHADER 
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// FRAGMENT SHADER
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    // We manage this memory via so called vertex buffer objects (VBO) that can store a large number
    //  of vertices in the GPU’s memory
    // The unsigned int used to create this variable that keeps a space in the memory well suit for the 
    //  use like storage.
    // Size of 4 bytes.
    // Later the stored vertex data within memory on the graphics card will be managed by this
    // Vertex buffer object named VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // A vertex array object(also known as VAO) can be bound just like a vertex buffer objectand any
    // subsequent vertex attribute calls from that point on will be stored inside the VAO.This has the
    // advantage that when configuring vertex attribute pointers you only have to make those calls once
    // and whenever we want to draw the object, we can just bind the corresponding VAO.
    // Core OpenGL requires that we use a VAO so it knows what to do with our vertex inputs. STORE:
    //  - Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
    //  - Vertex attribute configurations via glVertexAttribPointer.
    //  - Vertex buffer objects associated with vertex attributes by calls to glVertexAtt
    //     ribPointer.
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // OpenGL has many types of buffer objects and the buffer type of a vertex buffer object is
    // GL_ARRAY_BUFFER. From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be
    // Used to configure the currently bound buffer, which is VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copies the previously defined vertex data (vertices[]) into the buffer’s memory
    // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    // GL_STATIC_DRAW: the data is set only once and used many times.
    // GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create a shader object, again referenced by an ID. So we store the vertex shader as an unsigned int
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Next we attach the shader source code to the shader object and compile the shader
    // 1 - shader object to compile to
    // 2 - how many strings we’re passing as source code
    // 3 - source code of the vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check if compilation was successful after the call to glCompileShader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        // Error message with glGetShaderInfoLog
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    // New create the fragment shader like the vertex shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        // Error message with glGetShaderInfoLog
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }

    // A shader program object is the final linked version of multiple shaders combined.
    // The glCreateProgram function creates a program and returns the ID reference to the newly
    //  created program object.
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // Now we need to attach the previously compiled shaders to the program
    //  objectand then link them with glLinkProgram
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
            infoLog << std::endl;
    }

    // OpenGL does not yet know how it should interpret the vertex data in memory and how it should
    //  connect the vertex data to the vertex shader’s attributes.
    // Tell OpenGL how it should interpret the vertex data (per vertex attribute)
    // 1 - Specifies which vertex attribute we want to configure
    // 2 - Specifies the size of the vertex attribute. (attribute is position (x,y,z))
    // 3 - Specifies the type of the data
    // 4 - Specifies if we want the data to be normalized.
    // 5 - Is known as the stride and tells us the space between consecutive vertex attributes.
    //        vertex size = 3 * float; 
    // 6 - (void*)0 is the way to specify the offset of the data to 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Can delete the shaders because they already have been linked to the program 
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);
        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // Check and call envents and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}