#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H      //bro non so che dirti, sembra che l'ordine di inclusione di queste
                            //cose sia importante. se sposti "glm/..." più giù non va

#include <shader_s.h>

#include <other/other.h>
#include <classes/Box.h>
#include <classes/TreeNode.h>
#include <classes/Projection.h>
#include <utils/utils.h>

#include <iostream>
#include <tuple>
#include <vector>
#include <map>
#include <string>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color){

}


// settings
const unsigned int SCR_WIDTH = 1280;//800;
const unsigned int SCR_HEIGHT = 720;//600;

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



    float* my_rgb = HSVtoRGB(270.0f, 0.67f, 0.77f);
    std::cout << "colors RGB: " << my_rgb[0] << " " << my_rgb[1] << " " << my_rgb[2] << std::endl;

    for(int i = 0; i < 4; i++)
    {
        std::cout << "Hoi!!!" << std::endl;
        float* new_rgb = getColorFromID(i);
        std::cout << "colors new RGB: " << new_rgb[0] << " " 
                                        << new_rgb[1] << " " 
                                        << new_rgb[2] << std::endl;
        delete [] new_rgb;
    }
    delete [] my_rgb;



    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../fonts/ARIAL.TTF", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        return -1;
    }
    
    
    
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

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader_standard.vs", "shader_standard.fs"); // you can name your shader files however you like

    std::vector<float> vertices_vector;
    vertices_vector.push_back(-1.0f);
    vertices_vector.push_back(-1.0f);
    vertices_vector.push_back(0.0f);
    float* rgb = getColorFromID(0);
    for(int i = 0; i < 3; i++)
    {
        vertices_vector.push_back(rgb[i]);
    }

    vertices_vector.push_back(0.6f);
    vertices_vector.push_back(-1.0f);
    vertices_vector.push_back(0.0f);
    rgb = getColorFromID(0);
    for(int i = 0; i < 3; i++)
    {
        vertices_vector.push_back(rgb[i]);
    }

    vertices_vector.push_back(-1.0f);
    vertices_vector.push_back(-0.6f);
    vertices_vector.push_back(0.0f);
    rgb = getColorFromID(0);
    for(int i = 0; i < 3; i++)
    {
        vertices_vector.push_back(rgb[i]);
    }

    vertices_vector.push_back(0.6f);
    vertices_vector.push_back(-0.6f);
    vertices_vector.push_back(0.0f);
    rgb = getColorFromID(0);
    for(int i = 0; i < 3; i++)
    {
        vertices_vector.push_back(rgb[i]);
    }

    int nVertices = vertices_vector.size();
    float* vertices = (float*) malloc(nVertices * sizeof(float));
    copy(vertices_vector.begin(), vertices_vector.end(), vertices);

    for(int i = 0; i < nVertices; i++)
    {
        std::cout << vertices[i] << " ";
    }
    std::cout << std::endl;

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  
        1, 2, 3
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //sizeof(vertices)
    glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);   //12 is the total number of vertices used, basically 3 * the number of triangles used

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
}