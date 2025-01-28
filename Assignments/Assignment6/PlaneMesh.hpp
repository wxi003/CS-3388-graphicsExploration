#ifndef PLANEMESH_HPP
#define PLANEMESH_HPP


#include "Shader.hpp"
#include "LoadPLY.hpp"
#include "Constants.hpp"

class PlaneMesh {
	
	std::vector<float> verts;
	std::vector<int> indices;
	std::vector<float> uvs;


	float min, max;


	GLuint VAO;
	
	GLuint BoatVAO, BoatVBO, BoatEBO;
    GLuint EyesVAO, EyesVBO, EyesEBO;
    GLuint HeadVAO, HeadVBO, HeadEBO;

    std::vector<Vertex> boat_vertices;
    std::vector<Face> boat_faces;
    std::vector<Vertex> eyes_vertices;
    std::vector<Face> eyes_faces;
    std::vector<Vertex> head_vertices;
    std::vector<Face> head_faces;

	GLuint vertexbuffer, elementbuffer;
	GLuint TextureID, DispID, BoatID, EyesID, HeadID;
	GLuint ProgramID;

	// Cached uniform locations specifcally for the draw method
	GLuint MatrixID, ViewMatrixID, ModelMatrixID, LightID, timeID;


	void planeMeshQuads(float min, float max, float stepsize) {

        int nCols = (max - min) / stepsize + 1;

        for (float x = min; x <= max; x += stepsize) {
            for (float z = min; z <= max; z += stepsize) {
                verts.push_back(x);
                verts.push_back(0);
                verts.push_back(z);
            }
        }

		// Quad indices
        for (int i = 0; i < nCols - 1; ++i) {
            for (int j = 0; j < nCols - 1; ++j) {
                int topLeft = i * nCols + j;
                int bottomLeft = (i + 1) * nCols + j;
                int topRight = i * nCols + j + 1;
                int bottomRight = (i + 1) * nCols + j + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
                indices.push_back(topRight);
            }
        }
    }

	void setupMesh(GLuint& VAO, GLuint& VBO, GLuint& EBO, std::vector<Vertex>& vertices, std::vector<Face>& faces) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), &faces[0], GL_STATIC_DRAW);

        // Set vertex attribute pointers
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
        glEnableVertexAttribArray(0);
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
        glEnableVertexAttribArray(1);
        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

public:

	PlaneMesh(float min, float max, float stepsize) {

		this->min = min;
		this->max = max;

		planeMeshQuads(min, max, stepsize);
		
		// Load shaders
		ProgramID = LoadShaders("Shader.vertexshader", 
			"Shader.tcs", 
			"Shader.tes", 
			"Shader.geoshader", 
			"Shader.fragmentshader");

		// Load texture
		unsigned char* texture_data;
		unsigned int texture_width, texture_height;
		loadBMP24("Assets/water.bmp", &texture_data, &texture_width, &texture_height);

		// Create texture
		glGenTextures(1, &TextureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Load displacement map
		unsigned char* disp_data;
		unsigned int disp_width, disp_height;
		loadBMP24("Assets/displacement-map1.bmp", &disp_data, &disp_width, &disp_height);

		// Create displacement map
		glGenTextures(1, &DispID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, DispID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, disp_width, disp_height, 0, GL_BGR, GL_UNSIGNED_BYTE, disp_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// // Boat
		// loadPLY("Assets/boat.ply", boat_vertices, boat_faces);

		// Load boat
		unsigned char* boat_data;
		unsigned int boat_width, boat_height;
		loadBMP32("Assets/boat.bmp", &boat_data, &boat_width, &boat_height);

		// Create boat
    	glGenTextures(1, &BoatID);
   	    glActiveTexture(GL_TEXTURE2);
    	glBindTexture(GL_TEXTURE_2D, BoatID);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, boat_width, boat_height, 0, GL_BGR, GL_UNSIGNED_BYTE, boat_data);
    	glGenerateMipmap(GL_TEXTURE_2D);
   		glBindTexture(GL_TEXTURE_2D, 0);


		// Load eyes
		unsigned char* eyes_data;
		unsigned int eyes_width, eyes_height;
		loadBMP32("Assets/eyes.bmp", &eyes_data, &eyes_width, &eyes_height);

		// Create eyes
		glGenTextures(1, &EyesID);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, EyesID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, eyes_width, eyes_height, 0, GL_BGR, GL_UNSIGNED_BYTE, eyes_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);


		// Load head
		unsigned char* head_data;
		unsigned int head_width, head_height;
		loadBMP32("Assets/head.bmp", &head_data, &head_width, &head_height);

		// Create head
		glGenTextures(1, &HeadID);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, HeadID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, head_width, head_height, 0, GL_BGR, GL_UNSIGNED_BYTE, head_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	

		 // Setup the mesh for boat
        loadPLY("Assets/boat.ply", boat_vertices, boat_faces);
        setupMesh(BoatVAO, BoatVBO, BoatEBO, boat_vertices, boat_faces);

        // Setup the mesh for eyes
        loadPLY("Assets/eyes.ply", eyes_vertices, eyes_faces);
        setupMesh(EyesVAO, EyesVBO, EyesEBO, eyes_vertices, eyes_faces);

        // Setup the mesh for head
        loadPLY("Assets/head.ply", head_vertices, head_faces);
        setupMesh(HeadVAO, HeadVBO, HeadEBO, head_vertices, head_faces);

		// Set constant uniforms
		glUseProgram(ProgramID);


		glUniform1i(glGetUniformLocation(ProgramID, "tex"), 0);
		glUniform1i(glGetUniformLocation(ProgramID, "heighTex"), 1);

		glUniform1i(glGetUniformLocation(ProgramID, "BoatTexture"), 2); // Use texture unit 2 for boat texture
	

		glUniform1i(glGetUniformLocation(ProgramID, "EyesTexture"), 3); // Use texture unit 3 for eyes texture
	

		glUniform1i(glGetUniformLocation(ProgramID, "HeadTexture"), 4); // Use texture unit 4 for head texture
	

        auto setUniform1f = [&](const char* name, float value) {
            GLuint uniformID = glGetUniformLocation(ProgramID, name);
			glUniform1f(uniformID, value);
    	};

		setUniform1f("texOffset", OFFSET_TEX);
		setUniform1f("texScale", SCALE_TEX);
		setUniform1f("displacementHeight", HEIGHT_DISPLACEMENT);
		setUniform1f("displacementCloseness", CLOSENESS_DISPLACEMENT);
		setUniform1f("outerTess", TESS_OUTER);
		setUniform1f("innerTess", TESS_INNER);
		setUniform1f("alpha", SHININESS);

		glUseProgram(0);

		// Cache uniform locations
		MatrixID = glGetUniformLocation(ProgramID, "MVP");
		ViewMatrixID = glGetUniformLocation(ProgramID, "V");
		ModelMatrixID = glGetUniformLocation(ProgramID, "M");
		LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
		timeID = glGetUniformLocation(ProgramID, "time");

		// Generate buffers
		glGenBuffers(1, &vertexbuffer);
		glGenBuffers(1, &elementbuffer);

		// Generate and bind VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Fill buffers
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

		// Set up vertex attribute pointers
		glEnableVertexAttribArray(0);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
		glEnableVertexAttribArray(0);
		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
		glEnableVertexAttribArray(1);
		// Texture coordinate attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
	}


	void draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P) {
		
		glUseProgram(ProgramID);

		glm::mat4 M = glm::mat4(1.0f);
		glm::mat4 MVP = P * V * M;

		// Set new uniforms
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform1f(timeID, glfwGetTime());

		// Activate textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, DispID);

		// Bind the plane mesh VAO and draw
		glBindVertexArray(VAO);

		// Set patch size to 4 and draw the vertices
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (void*)0);

	
    	// Bind the boat mesh VAO and draw
    	glBindVertexArray(BoatVAO);
    	glActiveTexture(GL_TEXTURE2);
    	glBindTexture(GL_TEXTURE_2D, BoatID);
    	glPatchParameteri(GL_PATCH_VERTICES, 4);
    	glDrawElements(GL_TRIANGLES, boat_faces.size() * 3, GL_UNSIGNED_INT, 0);

    	// Bind the eyes mesh VAO and draw
   	 	glBindVertexArray(EyesVAO);
    	glActiveTexture(GL_TEXTURE3);
    	glBindTexture(GL_TEXTURE_2D, EyesID);
    	glPatchParameteri(GL_PATCH_VERTICES, 4);
    	glDrawElements(GL_TRIANGLES, eyes_faces.size() * 3, GL_UNSIGNED_INT, 0);

    	// Bind the head mesh VAO and draw
    	glBindVertexArray(HeadVAO);
    	glActiveTexture(GL_TEXTURE4);
    	glBindTexture(GL_TEXTURE_2D, HeadID);
    	glPatchParameteri(GL_PATCH_VERTICES, 4);
    	glDrawElements(GL_TRIANGLES, head_faces.size() * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		// Deactivate textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Disable the vertex attribute arrays
		glDisableVertexAttribArray(0);

		glUseProgram(0);
		

	}

};

#endif