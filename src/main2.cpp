#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

    

#include <shader_s.h>
#include <settings.h>

#include <Drawing/Drawing.h>
#include <classes/Box.h>
#include <classes/TreeNode.h>
#include <classes/Projection.h>
#include <utils/utils.h>
#include <InputReading/InputReading.h>

#include <iostream>
#include <tuple>
#include <vector>

#include <camera.h>

#include <main2.h>

// settings
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

extern int globalOptimumID;

namespace MAIN2
{


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



bool shiftPressed = false;




int currentNodeIndex = 0;
int numberOfNodes;
int nodesToAdvance = 1;



float lastFrame = 0.0;
float deltaTime = 0.0;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 25.0f));

int main2()
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2DKPO_Tree_Execution_Visualizer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
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
    Shader nodeInTreeShader("shader_node_in_tree.vs", "shader_node_in_tree.fs");
    Shader textShaderInSpace("shader_text_in_space.vs", "shader_text_in_space.fs");


    


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

    
    std::cout << "Global Optimum ID = " << globalOptimumID << std::endl;
    
    //unsigned int* buffersForBox = getBuffersToDrawBoxShape();
    unsigned int* buffersForNodeInTree = getBuffersWithDataToDrawRectangleNode();


    glBindVertexArray(0);

    currentNodeIndex = 0;


    glm::vec2 dimsTrue = getContainerEffectiveDimensions(wContainer, hContainer, maxPortionDedicatedToContainer);
    float wContainerTrue;
    float hContainerTrue;
    wContainerTrue = dimsTrue.x;
    hContainerTrue = dimsTrue.y;


    std::vector<int> modelIndices;
    unsigned int VAO_Nodes = getVAOWithDataToDrawNodesInTree(modelIndices, treeNodesVector);

    int bridgesCount = 0;
    unsigned int VAO_Bridges = getVAOWithDataToDrawBridgesInTree(bridgesCount, treeNodesVector);


    //############################################################################
    std::string fontName = "Impact";
    std::tuple<bool, unsigned int, int*> generatedTexture = generateBitmapFont("../fonts/" + fontName + ".ttf", 50, "bitmapfont" + fontName + ".bmp", "bitmapfont_widths" + fontName + ".txt");
    
    unsigned int textTexture;
    int* textWidths;
    if(std::get<0>(generatedTexture)) textTexture = std::get<1>(generatedTexture);
    if(std::get<0>(generatedTexture)) textWidths = std::get<2>(generatedTexture);
    unsigned int VAO_provaTesto = getVAOProvaTesto();
    Shader provaTestoShader("./shadersTextInstancing/shader_prova_testo.vs", "./shadersTextInstancing/shader_prova_testo.fs");
    //unsigned int textTexture2 = loadAndReturnBitmapTexture();
    
    


    glEnable(GL_DEPTH_TEST);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        

        //drawStaticInformations(wContainer, hContainer, wContainerTrue, hContainerTrue, wThickness, hThickness, maxPortionDedicatedToContainer, boxShader, buffersForBox, textShader, obstaclesVector);

        //drawNode_v1(treeNodesVector.at(currentNodeIndex), buffersForBox, wContainer, hContainer, wContainerTrue, hContainerTrue, wThickness, hThickness, maxPortionDedicatedToContainer, boxShader, textShader);
        

        //############################################################
        float aspect = ((float) SCR_WIDTH) / ((float) SCR_HEIGHT);
        glm::mat4 projection = glm::ortho(-aspect * camera.Zoom, aspect * camera.Zoom, -1.0f * camera.Zoom, 1.0f * camera.Zoom, -1.1f, 1000.0f);     //https://stackoverflow.com/questions/71810164/glmortho-doesnt-display-anything
        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        

        drawAllNodesInTree(treeNodesVector, modelIndices, nodeInTreeShader, VAO_Nodes, camera, view, projection);
        drawAllBridgesInTree(treeNodesVector, bridgesCount, nodeInTreeShader, VAO_Bridges, camera, view, projection);
        //drawAllNodesTextInTree(treeNodesVector, modelIndices, nodeInTreeShader, ???, camera, view, projection);


        //glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textTexture);
        provaTestoShader.use();
        provaTestoShader.setMat4("projection", projection);
        provaTestoShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        provaTestoShader.setMat4("model", model);
        provaTestoShader.setVec4("textColor", glm::vec4(0.0, 0.0, 0.0, 1.0));
        glBindVertexArray(VAO_provaTesto);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
        //glDisable(GL_CULL_FACE);

        


        


        /*textShaderInSpace.use();
        textShaderInSpace.setMat4("projection", projection);
        textShaderInSpace.setMat4("view", view);
        drawWholeTreeText(treeNodesVector.at(0), treeNodesVector, textShaderInSpace, buffersForNodeInTree, camera, view, projection);*/

        

        //############################################################

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);


        
        drawSpeed(textShader, nodesToAdvance, false, 0.0, currentFrame);


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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UPWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWNWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    

    if(shiftPressed)
    {
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            /*if(currentNodeIndex < numberOfNodes - 1)
            {
                currentNodeIndex += 1;
            }*/
            if(currentNodeIndex + nodesToAdvance < numberOfNodes)
                {
                    currentNodeIndex += nodesToAdvance;
                }
                else
                {
                    currentNodeIndex = numberOfNodes - 1;
                }
            }
        

        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            /*if(currentNodeIndex > 0)
            {
                currentNodeIndex -= 1;
            }*/
            if(currentNodeIndex - nodesToAdvance > 0)
            {
                currentNodeIndex -= nodesToAdvance;
            }
            else
            {
                currentNodeIndex = 0;
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

    if((key == GLFW_KEY_KP_ADD || key == GLFW_KEY_PERIOD) && action == GLFW_PRESS)
    {
        if(nodesToAdvance < 1000000)
        {   
            nodesToAdvance *= 10;
            Shader s;
            drawSpeed(s, nodesToAdvance, true, glfwGetTime(), 0.0);
        }
    }

    if((key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_COMMA) && action == GLFW_PRESS)
    {
        if(nodesToAdvance > 1)
        {
            nodesToAdvance /= 10;
            Shader s;
            drawSpeed(s, nodesToAdvance, true, glfwGetTime(), 0.0);
        }
    }

    if((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_PRESS)
    {
        shiftPressed = true;
    }
    if((key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) && action == GLFW_RELEASE)
    {
        shiftPressed = false;
    }

    if(!shiftPressed)
    {
        if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            if(currentNodeIndex + nodesToAdvance < numberOfNodes)
            {
                currentNodeIndex += nodesToAdvance;
            }
            else
            {
                currentNodeIndex = numberOfNodes - 1;
            }
        }

        if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            if(currentNodeIndex - nodesToAdvance > 0)
            {
                currentNodeIndex -= nodesToAdvance;
            }
            else
            {
                currentNodeIndex = 0;
            }
        }
    }
    
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}















}