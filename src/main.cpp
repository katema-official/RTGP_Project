#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader_s.h>

#include <other/other.h>
#include <classes/Box.h>
#include <classes/TreeNode.h>
#include <classes/Projection.h>

#include <iostream>
#include <tuple>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int CURRENT_VAO = 0;

int main()
{
    MyFunc();
    Box* b1 = new Box(0,0,10,20,0);
    Box* b2 = new Box(10,0, 30, 40, 1);
    std::vector<Box*> vectorOfBoxes;
    vectorOfBoxes.push_back(b1);
    vectorOfBoxes.push_back(b2);
    TreeNode* tn1 = new TreeNode(0, -1, 10, 20, 1);
    tn1->setBoxes(vectorOfBoxes);

    std::vector<Projection*> vectorOfProjs;
    Projection* p1 = new Projection(10, 20, 30, 40);
    vectorOfProjs.push_back(p1);
    tn1->setProjections(vectorOfProjs);

    delete tn1;
    
    
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader_standard.vs", "shader_standard.fs"); // you can name your shader files however you like

    /*float vertices[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  //bl
         0.0f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  //br
         -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  //tl
         0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    //tr


         0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  //bl
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  //br
         0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f,   //tl
         0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f    //tr
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  
        1, 2, 3,

        4, 5, 6,
        5, 6, 7
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    unsigned int* VAOs = getVAOs();

    glBindVertexArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the triangle
        ourShader.use();
        glBindVertexArray(VAOs[CURRENT_VAO]);
        //std::cout << "Vao: " << VAOs[CURRENT_VAO] << std::endl;
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);   //12 is the total number of vertices used, basically 3 * the number of triangles used

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if(CURRENT_VAO == 0)
        { 
            CURRENT_VAO = 1;
            std::cout << "Changed to 1" << std::endl;
        }
        else
        { 
            CURRENT_VAO = 0;
            std::cout << "Changed to 0" << std::endl;
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}