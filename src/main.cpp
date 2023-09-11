#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

    

#include <shader_s.h>

#include <other/other.h>
#include <classes/Box.h>
#include <classes/TreeNode.h>
#include <classes/Projection.h>
#include <utils/utils.h>
#include <InputReading/InputReading.h>

#include <iostream>
#include <tuple>
#include <vector>




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);


bool shiftPressed = false;


// settings
unsigned int SCR_WIDTH = 1280;//800;
unsigned int SCR_HEIGHT = 720;//600;

int CURRENT_VAO = 0;

int currentNodeIndex = 0;
int numberOfNodes;


int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2DKPO_Execution_Visualizer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, key_callback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    Shader textShader = initTextRendering(SCR_WIDTH, SCR_HEIGHT);

    // build and compile our shader program
    // ------------------------------------
    Shader boxShader("shader_standard.vs", "shader_standard.fs"); // you can name your shader files however you like





    numberOfNodes = readNodesNumber();
    std::cout << "numberOfNodes = " << numberOfNodes << std::endl;

    int wContainer = 0;
    int hContainer = 0;
    std::vector<Box*> obstaclesVector;
    std::vector<TreeNode*> treeNodesVector;
    float wThickness = 0.02;
    float hThickness = 0.012;
    float maxPortionDedicatedToContainer = 0.8;

    readNodesInformations(wContainer, hContainer, obstaclesVector, treeNodesVector);
    /*std::cout << "w = " << wContainer << " h = " << hContainer << std::endl;
    std::cout << "Obstacles: " << std::endl;
    for(Box* i : obstaclesVector) i->printBoxInfo();
    std::cout << "Tree nodes informations: " << std::endl;
    for(TreeNode* t : treeNodesVector) t->printTreeNode();*/


    
    unsigned int* buffersForBox = getBuffersToDrawBoxShape();


    unsigned int* VAOs = getVAOs();

    glBindVertexArray(0);

    currentNodeIndex = 0;


    glm::vec2 dimsTrue = getContainerEffectiveDimensions(wContainer, hContainer, maxPortionDedicatedToContainer);
    float wContainerTrue;
    float hContainerTrue;
    wContainerTrue = dimsTrue.x;
    hContainerTrue = dimsTrue.y;


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
        /*boxShader.use();
        glBindVertexArray(VAOs[CURRENT_VAO]);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);   //12 is the total number of vertices used, basically 3 * the number of triangles used
        */

        drawStaticInformations(wContainer, hContainer, wContainerTrue, hContainerTrue, wThickness, hThickness, maxPortionDedicatedToContainer, boxShader, buffersForBox, textShader, obstaclesVector);

        drawTreeNode_v1(treeNodesVector.at(currentNodeIndex), buffersForBox, wContainer, hContainer, wContainerTrue, hContainerTrue, wThickness, hThickness, maxPortionDedicatedToContainer, boxShader, textShader);
        

        //RenderText(textShader, "This is sample text", 0.01f, 0.01f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        //RenderText(textShader, "(C) LearnOpenGL.com", 0.5f, 0.5f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));


        


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
    
    /*if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
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
    }*/

    if(shiftPressed)
    {
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            if(currentNodeIndex < numberOfNodes - 1)
            {
                currentNodeIndex += 1;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if(currentNodeIndex > 0)
            {
                currentNodeIndex -= 1;
            }
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
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



    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    {
        shiftPressed = true;
    }
    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
    {
        shiftPressed = false;
    }

    if(!shiftPressed)
    {
        if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            if(currentNodeIndex < numberOfNodes - 1)
            {
                currentNodeIndex += 1;
            }
        }

        if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            if(currentNodeIndex > 0)
            {
                currentNodeIndex -= 1;
            }
        }
    }
    
}

