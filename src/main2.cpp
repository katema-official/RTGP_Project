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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);


bool shiftPressed = false;


int currentNodeIndex = 0;
int numberOfNodes;
int nodesToAdvance = 1;


float lastFrame = 0.0;
float deltaTime = 0.0;


int CURRENT_SCR_WIDTH;
int CURRENT_SCR_HEIGHT;
glm::mat4 projection;
glm::mat4 view;
glm::vec4 worldCoordsClicked;
bool clicked = false;

int checkIfClickedOnNode(double xWorld, double yWorld, TreeNode* currentNode, std::vector<TreeNode*> treeNodesVector);


bool focusOnNode = false;   //false = I'm watching the whole tree, true = I'm watching a particular node


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
    CURRENT_SCR_WIDTH = SCR_WIDTH;
    CURRENT_SCR_HEIGHT = SCR_HEIGHT;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


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
    Shader boxShader("shader_node_info.vs", "shader_node_info.fs");

    Shader nodeInTreeShader("shader_node_in_tree.vs", "shader_node_in_tree.fs");
    Shader textInTreeShader("./shadersTextInstancing/shader_text_in_space.vs", "./shadersTextInstancing/shader_text_in_space.fs");

    


    numberOfNodes = readNodesNumber();
    std::cout << "numberOfNodes = " << numberOfNodes << std::endl;

    int wContainer = 0;
    int hContainer = 0;
    std::vector<Box*> obstaclesVector;
    std::vector<TreeNode*> treeNodesVector;
    float hThickness = 0.012;
    float wThickness = hThickness * ((float) SCR_WIDTH) / ((float) SCR_HEIGHT);
    float maxPortionDedicatedToContainer = 0.8;

    readNodesInformations(wContainer, hContainer, obstaclesVector, treeNodesVector);
    /*std::cout << "w = " << wContainer << " h = " << hContainer << std::endl;
    std::cout << "Obstacles: " << std::endl;
    for(Box* i : obstaclesVector) i->printBoxInfo();
    std::cout << "Tree nodes informations: " << std::endl;
    for(TreeNode* t : treeNodesVector) t->printTreeNode();*/

    
    std::cout << "Global Optimum ID = " << globalOptimumID << std::endl;
    
    unsigned int* buffersForBox = getBuffersToDrawBoxShape();   //for a specific node
    

    glBindVertexArray(0);

    currentNodeIndex = 0;


    glm::vec2 dimsTrue = getContainerEffectiveDimensions(wContainer, hContainer, maxPortionDedicatedToContainer);
    float wContainerTrue;
    float hContainerTrue;
    wContainerTrue = dimsTrue.x;
    hContainerTrue = dimsTrue.y;


    //additional code (different from main1) for the tree rendering
    std::vector<int> nodesIndices;  //vector with the explorationIDs in the order they will be explored by addModelMatrix_Nodes (recursive function called by getVAOWithDataToDrawNodesInTree)
    std::vector<glm::vec3> nodesPositions;   //these two vectors will be useful to know which node (through its explorationID) is associated to which position in space
    unsigned int VAO_Nodes = getVAOWithDataToDrawNodesInTree(nodesIndices, nodesPositions, treeNodesVector);

    int bridgesCount = 0;
    unsigned int VAO_Bridges = getVAOWithDataToDrawBridgesInTree(bridgesCount, treeNodesVector);


    //############################################################################
    std::string fontName = "Impact";    //Change this name accordingly to change the font
    std::tuple<bool, unsigned int, int*> generatedTexture = generateBitmapFont("../fonts/" + fontName + ".ttf", 50, "bitmapfont" + fontName + ".bmp", "bitmapfont_widths" + fontName + ".txt");
    
    unsigned int textTexture;
    int* textWidths;
    if(std::get<0>(generatedTexture)) textTexture = std::get<1>(generatedTexture);
    if(std::get<0>(generatedTexture)) textWidths = std::get<2>(generatedTexture);
    
    //we have to convert the explorationIDs of the indicesVector, that are ints, in strings
    std::vector<std::string> explorationIDsVector;
    for(int expID : nodesIndices)
    {
        explorationIDsVector.push_back(std::to_string(expID));
    }
    float xOffset = -0.35;
    float yOffset = -0.15;
    float scale = 0.3;
    int lettersCount_explorationIDs = 0;   //how many letters need to be drawn
    unsigned int VAOText_explorationIDs = getVAONodesText(explorationIDsVector, nodesPositions, textWidths, lettersCount_explorationIDs, xOffset, yOffset, scale);

    std::vector<std::string> explorationIDsVector_plainText;
    for(int i = 0; i < numberOfNodes; i++)
    {
        explorationIDsVector_plainText.push_back("Exploration ID:");
    }
    xOffset = -0.40;
    yOffset = 0.10;
    scale = 0.15;
    int lettersCount_explorationIDs_plainText = 0;
    unsigned int VAOText_explorationIDs_plainText = getVAONodesText(explorationIDsVector_plainText, nodesPositions, textWidths, lettersCount_explorationIDs_plainText, xOffset, yOffset, scale);



    //DEBUG
    /*
    for(TreeNode* node : treeNodesVector)
    {
        std::cout << "node " << node->explorationID << ", pos: (" << node->positionInTree.x << ", " <<
            node->positionInTree.y << ", " << node->positionInTree.z << ")" << std::endl;
    }
    */

    
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

        //check if left mouse was pressed: in such case, reset state and check if it clicked
        //on something meaningful
        if(clicked && !focusOnNode)
        {
            clicked = false;
            int eID = checkIfClickedOnNode(worldCoordsClicked.x, worldCoordsClicked.y, treeNodesVector.at(0), treeNodesVector);
            //std::cout << "CLICKED ON NODE " << eID << std::endl;
            if(eID != -1)
            {
                glDisable(GL_DEPTH_TEST);
                focusOnNode = true;
                currentNodeIndex = eID;
            }
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //############################################################

        if(focusOnNode)
        {
            drawStaticInformations(wContainer, hContainer, wContainerTrue, hContainerTrue, wThickness, hThickness, maxPortionDedicatedToContainer, boxShader, buffersForBox, textShader, obstaclesVector);
            drawNode_v1(treeNodesVector.at(currentNodeIndex), buffersForBox, wContainer, hContainer, wContainerTrue, hContainerTrue, wThickness, hThickness, maxPortionDedicatedToContainer, boxShader, textShader);
            drawSpeed(textShader, nodesToAdvance, false, 0.0, currentFrame);
        }
        
        if(!focusOnNode)
        {
            float aspect = ((float) SCR_WIDTH) / ((float) SCR_HEIGHT);
            projection = glm::ortho(-aspect * camera.Zoom, aspect * camera.Zoom, -1.0f * camera.Zoom, 1.0f * camera.Zoom, -1.1f, 1000.0f);     //https://stackoverflow.com/questions/71810164/glmortho-doesnt-display-anything
            // camera/view transformation
            view = camera.GetViewMatrix();
            

            drawAllNodesInTree(treeNodesVector, nodesIndices, nodeInTreeShader, VAO_Nodes, camera, view, projection);
            drawAllBridgesInTree(treeNodesVector, bridgesCount, nodeInTreeShader, VAO_Bridges, camera, view, projection);
            //drawAllNodesTextInTree(treeNodesVector, modelIndices, nodeInTreeShader, ???, camera, view, projection);

            drawTextInTree(lettersCount_explorationIDs, textInTreeShader, VAOText_explorationIDs, camera, view, projection, textTexture);
            drawTextInTree(lettersCount_explorationIDs_plainText, textInTreeShader, VAOText_explorationIDs_plainText, camera, view, projection, textTexture);
        
        }
        

        //############################################################

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);



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

    if(!focusOnNode)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(UPWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWNWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    

    if(focusOnNode)
    {
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

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    CURRENT_SCR_WIDTH = width;
    CURRENT_SCR_HEIGHT = height;
    //std::cout << "window width = " << width << ", window height = " << height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if(focusOnNode)
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

        if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            focusOnNode = false;
            glEnable(GL_DEPTH_TEST);
        }
    }
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(!focusOnNode)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

//glfw: detect mouse click
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(!focusOnNode && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        //std::cout << "Cursor Position at (" << xpos << " : " << ypos << ")" << std::endl;

        double xPos_NDC = xpos / ((double) CURRENT_SCR_WIDTH / 2.0) - 1.0;
        double yPos_NDC = (ypos / ((double) CURRENT_SCR_HEIGHT / 2.0) - 1.0) * (-1.0);
        //no need to reverse perspective devide: https://antongerdelan.net/opengl/raycasting.html
        glm::vec4 viewCoords = glm::inverse(projection) * glm::vec4(xPos_NDC, yPos_NDC, 0, 1);
        worldCoordsClicked = glm::inverse(view) * viewCoords;
        //std::cout << "World coords clicked: (" << worldCoordsClicked.x << " : " << worldCoordsClicked.y << ")" << std::endl;
        clicked = true;
    }
}





int checkIfClickedOnNode(double xWorld, double yWorld, TreeNode* currentNode, std::vector<TreeNode*> treeNodesVector)
{
    float halfLength = 0.5f;

    //rejection test: the current node examined in the search is below the coordinates requested
    if(currentNode->positionInTree.y + halfLength < yWorld)
    {
        return -1;
    }

    //first, check if the mouse click intersected exactly this node
    if(xWorld > (currentNode->positionInTree.x - halfLength) && xWorld < (currentNode->positionInTree.x + halfLength) &&
        yWorld > (currentNode->positionInTree.y - halfLength) && yWorld < (currentNode->positionInTree.y + halfLength))
    {
        return currentNode->explorationID;
    }
    
    //if not, proceed the search, looking at all the children of this node. Their positions
    //are ordered from the leftmost to the rightmost (in world coordinates), so we have to continue
    //the search on the last node that is still on the left wrt the click position.
    //To check if a node is on the left wrt the click position, we check its left face/edge.
    int promisingExpID = -1;
    for(int child_eID : currentNode->childNodesExplorationID)
    {
        TreeNode* childNode = treeNodesVector.at(child_eID);
        if((childNode->positionInTree.x - halfLength) < xWorld)
        {
            promisingExpID = child_eID;
        }
    }

    //If I had no children, I return failure
    if(promisingExpID == -1) return -1;

    //If I have a promising children, I continue the search
    return checkIfClickedOnNode(xWorld, yWorld, treeNodesVector.at(promisingExpID), treeNodesVector);
}



}