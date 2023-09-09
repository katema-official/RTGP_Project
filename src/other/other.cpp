#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "other.h"
#include "../utils/utils.h"

#include <iostream>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>


//-wContainer: the width of the container
//-hContainer: the height of the container
//-thickness: to express how thick the walls of the container should be wrt to SCR_WIDTH and SCR_HEIGHT respectively
//-maxPortionDedicatedToContainer: to express the maximum percentage of the window that should be dedicated to the container
//the remaining are data used to draw boxes or text
void drawStaticInformations(int wContainer, int hContainer, 
                            float wThickness, float hThickness,
                            float maxPortionDedicatedToContainer,
                            Shader boxShader, unsigned int* buffersForBox,
                            Shader textShader
                            )
{
    if(wThickness > 1 || wThickness <= 0 || 
        hThickness > 1 || hThickness <= 0 || 
        maxPortionDedicatedToContainer > 1 || maxPortionDedicatedToContainer <= 0)
    {
        std::cout << "ERROR::drawStaticInformations: data not between 0 and 1" << std::endl;
        exit;
    }

    glm::vec3 containerColor = glm::vec3(0.0, 0.0, 0.0);
    


    int winnerLength = wContainer > hContainer ? wContainer : hContainer;
    int loserLength =  wContainer > hContainer ? hContainer : wContainer;

    float widthPortion;
    float heightPortion;

    if(winnerLength == wContainer)
    {
        widthPortion = maxPortionDedicatedToContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + widthPortion, 0 + wThickness, containerColor);  //bottom
        heightPortion = (hContainer * widthPortion) / wContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + hThickness, 0 + heightPortion, containerColor);   //left
        drawBoxShape(boxShader, buffersForBox, 0, 0 + heightPortion, 0 + widthPortion, 0 + heightPortion - wThickness, containerColor);  //top
        drawBoxShape(boxShader, buffersForBox, 0 + widthPortion, 0, 0 + widthPortion - hThickness, 0 + heightPortion, containerColor); //right
    }
    else
    {
        heightPortion = maxPortionDedicatedToContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + hThickness, 0 + heightPortion, containerColor);  //left
        widthPortion = (wContainer * heightPortion) / hContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + widthPortion, 0 + wThickness, containerColor);    //bottom
        drawBoxShape(boxShader, buffersForBox, 0 + widthPortion, 0, 0 + widthPortion - hThickness, 0 + heightPortion, containerColor);    //right
        drawBoxShape(boxShader, buffersForBox, 0, 0 + heightPortion, 0 + widthPortion, 0 + heightPortion - wThickness, containerColor);    //top
    }

    float remainingVerticalSpace = 1.0f - heightPortion;
    float verticalOffset = remainingVerticalSpace / 7;
    RenderText(textShader, "Node ID: ", 0.02, 1.0f - verticalOffset*2, 0.5, containerColor);
    RenderText(textShader, "Father ID: ", 0.02, 1.0f - verticalOffset*4, 0.5, containerColor);
    RenderText(textShader, "Level in tree: ", 0.02, 1.0f - verticalOffset*6, 0.5, containerColor);



}

unsigned int* getBuffersToDrawBoxShape()
{
    unsigned int* buffers = new unsigned int[3]; //there will be the VAO, the VBO and the EBO

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,                //this works fine under the assumption that in the VBO the vertices
        1, 2, 3                 //are ordered like this: bottom-left, bottom-right, top-left, top-right
    };

    glGenVertexArrays(1, &buffers[0]);
    glGenBuffers(1, &buffers[1]);
    glGenBuffers(1, &buffers[2]);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    //24 because we assume there will be four vertices, each composed of xyz and rgb (and each of them is a float)
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return buffers;

}



void drawBoxShape(Shader& shader, unsigned int* buffers, float x0, float y0, float x1, float y1, glm::vec3 color)
{
    x0 = x0*2 - 1;
    y0 = y0*2 - 1;
    x1 = x1*2 - 1;
    y1 = y1*2 - 1;
    std::vector<float> vertices_vector;
    vertices_vector.push_back(x0);
    vertices_vector.push_back(y0);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    vertices_vector.push_back(x1);
    vertices_vector.push_back(y0);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    vertices_vector.push_back(x0);
    vertices_vector.push_back(y1);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    vertices_vector.push_back(x1);
    vertices_vector.push_back(y1);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    int nVertices = vertices_vector.size();
    float* vertices = (float*) malloc(nVertices * sizeof(float));
    copy(vertices_vector.begin(), vertices_vector.end(), vertices);

    //to free the vector
    std::vector<float>().swap(vertices_vector);

    /*for(int i = 0; i < nVertices; i++)
    {
        std::cout << vertices[i] << " ";
    }
    std::cout << std::endl;*/

    

    shader.use();
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nVertices * sizeof(float), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    //the memory was dinamically allocated, it must now be freed
    free(vertices);
}








void MyFunc() 
{
    std::cout << "Ohai from another .cpp file!" << std::endl;
}

unsigned int* getVAOs()
{
    unsigned int* VAOs = new unsigned int[2];
    unsigned int* VBOs = new unsigned int[2];
    unsigned int* EBOs = new unsigned int[2];

    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);



    float vertices_1[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  0.50f, 0.25f, 0.75f,  //bl
         0.0f, -0.5f, 0.0f,  0.50f, 0.25f, 0.75f,  //br
         -0.5f, 0.5f, 0.0f,  0.50f, 0.25f, 0.75f,  //tl
         0.0f, 0.5f, 0.0f, 0.50f, 0.25f, 0.75f,    //tr


         0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  //bl
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  //br
         0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f,   //tl
         0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f    //tr
    };

    unsigned int indices_1[] = {  // note that we start from 0!
        0, 1, 2,  
        1, 2, 3,

        4, 5, 6,
        5, 6, 7
    };

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_1, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    float vertices_2[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //bl
         0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //br
         -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //tl
         0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    //tr


         0.0f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  //bl
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  //br
         0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   //tl
         0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f    //tr
    };

    unsigned int indices_2[] = {  // note that we start from 0!
        0, 1, 2,  
        1, 2, 3,

        4, 5, 6,
        5, 6, 7
    };

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    return VAOs;
}

std::tuple<unsigned int, int> swapRectangles(int i, float vertices[], unsigned int indices[])
{

    return std::make_tuple(10, 5);
}
