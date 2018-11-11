#include "MyView.hpp"
#include <scene/scene.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const scene::Context * scene)
{
    scene_ = scene;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
    assert(scene_ != nullptr);

	scene::GeometryBuilder builder_mesh;
	const auto& scene_meshes_ = builder_mesh.getAllMeshes();
	/*
	Loops through all meshes in the scene and gets the data from them
	I then assign these into my own containers to bind them to vbo's and vao's

	I use 1 vbo for the position, normal and tex coord;
	This saves having to put more vbos inside my whole vao so creating less processes needed.
	So instead of having 3 vbos of size vec3 and vec2 I have one size of a structure I made in the header.

	The elements are in a seperate vbo since this data will need to be accessed and moved through easily
	Having this outside the struct means I don't have to manually say how far through the vbo the code needs to go to get the info to draw.
	*/
	
	for (const auto& scene_mesh : scene_meshes_)
	{
		MeshGL& newMesh = meshes_[scene_mesh.getId()];
		scene_mesh.getPositionArray();
		//My local containers
		std::vector<Vertex> vertexStruct;
		std::vector<unsigned int> Elements;
	
		//Getting values from scene file
		const auto& positions = scene_mesh.getPositionArray();
		const auto& normals = scene_mesh.getNormalArray();
		const auto& texCoord = scene_mesh.getTextureCoordinateArray();
		const auto& elements = scene_mesh.getElementArray();

		vertexStruct.resize(positions.size());
		//Give my local containers the scenes values
		for (size_t i = 0; i < vertexStruct.size(); ++i)
		{
			vertexStruct[i].pos = positions[i];
			vertexStruct[i].norm = normals[i];
			vertexStruct[i].texCoord = texCoord[i];
		}

		//Assigning the files element count to my own
		Elements.resize(elements.size());
		for (size_t i = 0; i < Elements.size(); ++i)
		{
			Elements[i] = elements[i];
		}

		//Binding vertex vbo
		glGenBuffers(1, &newMesh.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertexStruct.size() * sizeof(Vertex), // size of data in bytes
			vertexStruct.data(), // pointer to the data
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		//Binding element vbo
		glGenBuffers(1, &newMesh.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			Elements.size() * sizeof(unsigned int),
			Elements.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kNullId);
		newMesh.element_count = int(Elements.size());

		//Making the vertex arrays and giving them info from my VAO
		glGenVertexArrays(1, &newMesh.vao);
		glBindVertexArray(newMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.element_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertex_vbo);
		glEnableVertexAttribArray(kVertexPosition);
		glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, pos));

		glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertex_vbo);
		glEnableVertexAttribArray(kNormal);
		glVertexAttribPointer(kNormal, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, norm));

		glBindBuffer(GL_ARRAY_BUFFER, newMesh.vertex_vbo);
		glEnableVertexAttribArray(kTexCoord);
		glVertexAttribPointer(kTexCoord, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, texCoord));

		glBindVertexArray(kNullId);
	}

//Attatches the vertex and fragment shader to the program
#pragma region Attatching shaders and the program
	GLint compile_status = GL_FALSE;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string
		= tygra::createStringFromFile("content:///sponza_vs.glsl");
	const char * vertex_shader_code = vertex_shader_string.c_str();
	glShaderSource(vertex_shader, 1,
		(const GLchar **)&vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_string
		= tygra::createStringFromFile("content:///sponza_fs.glsl");
	const char * fragment_shader_code = fragment_shader_string.c_str();
	glShaderSource(fragment_shader, 1,
		(const GLchar **)&fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	first_program_ = glCreateProgram();
	glAttachShader(first_program_, vertex_shader);
	glBindAttribLocation(first_program_, kVertexPosition, "vertex_position");
	glBindAttribLocation(first_program_, kNormal, "vertex_normal");
	glBindAttribLocation(first_program_, kTexCoord, "vertex_texcoord");
	glDeleteShader(vertex_shader);
	glAttachShader(first_program_, fragment_shader);
	glBindFragDataLocation(first_program_, kFragmentColour, "fragment_colour");
	glDeleteShader(fragment_shader);

	glLinkProgram(first_program_);

	GLint link_status = GL_FALSE;
	glGetProgramiv(first_program_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(first_program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
#pragma endregion 	

//Gets all texture info
#pragma region Byte and texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Gets both texture files from the content folder
	tygra::Image texture_image_1
		= tygra::createImageFromPngFile("content:///diff0.png");

	tygra::Image texture_image_2
		= tygra::createImageFromPngFile("content:///diff1.png");
	//If to check the picture contains data and puts these into the gluint of test texture which i have 2 of.
	//In the render below decides which test texture to use
	//2 Warnings with texture_image_.pixelData() below with conversion from size_t to GLsizei
	//Acknowledged and left since I see no issues with this happening.
	if (texture_image_1.doesContainData()) {
		glGenTextures(1, &test_texture_1_);
		glBindTexture(GL_TEXTURE_2D, test_texture_1_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image_1.width(),
			texture_image_1.height(),
			0,
			pixel_formats[texture_image_1.componentsPerPixel()],
			texture_image_1.bytesPerComponent() == 1
			? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
			texture_image_1.pixelData());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, kNullId);
	}

	if (texture_image_2.doesContainData()) {
		glGenTextures(1, &test_texture_2_);
		glBindTexture(GL_TEXTURE_2D, test_texture_2_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image_2.width(),
			texture_image_2.height(),
			0,
			pixel_formats[texture_image_2.componentsPerPixel()],
			texture_image_2.bytesPerComponent() == 1
			? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
			texture_image_2.pixelData());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, kNullId);
	}
#pragma endregion 
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height)
{
    glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	//Delete all the buffers and program on stop
	glDeleteProgram(first_program_);
	for (int i = 0; i < meshes_.size(); ++i)
	{
		glDeleteBuffers(1, &meshes_[i].vertex_vbo);
		glDeleteBuffers(1, &meshes_[i].element_vbo);
		glDeleteVertexArrays(1, &meshes_[i].vao);
	}
	scene_lights_.clear();
}

void MyView::windowViewRender(tygra::Window * window)
{
	assert(scene_ != nullptr);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//Calculations to get camera matrix
//This is completed within the mesh loop to do the model and combined xform
#pragma region Camera Xform
	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)viewport_size[3];

	glm::mat4 projection_xform = glm::perspective
	(
		glm::radians(scene_->getCamera().getVerticalFieldOfViewInDegrees()),
		aspect_ratio,
		scene_->getCamera().getNearPlaneDistance(),
		scene_->getCamera().getFarPlaneDistance()
	);

	glm::mat4 view_xform = glm::lookAt
	(
		scene_->getCamera().getPosition(),
		scene_->getCamera().getPosition() + scene_->getCamera().getDirection(),
		scene_->getUpDirection()
	);

	glUseProgram(first_program_);
#pragma endregion  
	
	//Scenes ambient light vec3
	glm::vec3 ambient_light = scene_->getAmbientLightIntensity();
	GLuint ambient_light_id = glGetUniformLocation(first_program_, "ambient_light");
	glUniform3fv(ambient_light_id, 1, glm::value_ptr(ambient_light));

	//Before getting all the lights clear the vector from last render call to cover incase the lights change
	//Loops through all the lights and pass them into the shaders as uniform as an array of lights.
	//They use the struct inside the fragment shader so when inside I can use .position and access this.
	scene_lights_.clear();
	scene_lights_ = scene_->getAllLights();

	int num_of_lights = int(scene_lights_.size());
	GLuint num_of_lights_id = glGetUniformLocation(first_program_, "num_of_lights");
	glUniform1i(num_of_lights_id, num_of_lights);

	for(int i = 0; i < num_of_lights; ++i)
	{
		glm::vec3 position = scene_lights_[i].getPosition();
		GLuint light_pos_id = glGetUniformLocation(first_program_, std::string("lights[" + std::to_string(i) + "].position").c_str());
		glUniform3fv(light_pos_id, 1,  glm::value_ptr(position));

		float range = scene_lights_[i].getRange();
		GLuint light_range_id = glGetUniformLocation(first_program_, std::string("lights[" + std::to_string(i) + "].range").c_str());
		glUniform1f(light_range_id, range);

		float intensity = scene_lights_[i].getIntensity();
		GLuint light_intensity_id = glGetUniformLocation(first_program_, std::string("lights[" + std::to_string(i) + "].intensity").c_str());
		glUniform1f(light_intensity_id, intensity);

		glm::vec3 colour = scene_lights_[i].getColour();
		GLuint light_colour_id = glGetUniformLocation(first_program_, std::string("lights[" + std::to_string(i) + "].colour").c_str());
		glUniform3fv(light_colour_id, 1, glm::value_ptr(colour));
	}

	//Loop to go through all meshes in the scene and draw them.
	for (auto it : scene_->getAllInstances())
	{
		const MeshGL & mymesh = meshes_[it.getMeshId()];
		
		
		const auto& scene_materials_ = scene_->getAllMaterials();

		
		int num_of_materials = int(scene_materials_.size());
		GLuint num_of_materials_id = glGetUniformLocation(first_program_, "num_of_materials");
		glUniform1i(num_of_materials_id, num_of_materials);

		//Gets all the data for the current material the mesh uses. This means each loop the right data is passed without having to use an array.
		//These are all passed as uniform using the .colour type syntax so it uses the Structure inside the fragment shader.
		for (auto iterator : scene_materials_)
		{
			//mat 0 - id 200 - texture diff0, 0 shinyness
			//mat 1 - id 201 - texture diff1, 8 shinyness
			//mat 2 - id 202 - no text, 64 shinyness
			//mat 3 - id 203 - no text, 128 shinyness

			if (iterator.getId() == it.getMaterialId())
			{
				int use_texture;
				int is_shiny;
				glm::vec3 ambient_diffuse_colour = iterator.getAmbientAndDiffuseColour();
				GLuint scene_mat_id = glGetUniformLocation(first_program_, std::string("materials.ambient_diffuse_colour").c_str());
				glUniform3fv(scene_mat_id, 1, glm::value_ptr(ambient_diffuse_colour));

				float shininess = iterator.getShininess();
				GLuint shininess_id = glGetUniformLocation(first_program_, std::string("materials.shininess").c_str());
				glUniform1f(shininess_id, shininess);

				glm::vec3 specular_colour = iterator.getSpecularColour();
				GLuint specular_colour_id = glGetUniformLocation(first_program_, std::string("materials.specular_colour").c_str());
				glUniform3fv(specular_colour_id, 1, glm::value_ptr(specular_colour));
				//Checks if the material uses a texture to change an int to represent a bool for the shaders
				if (iterator.getDiffuseTexture() == "")
					use_texture = 0;
				else
					use_texture = 1;

				GLuint diffuse_texture_id = glGetUniformLocation(first_program_, std::string("materials.use_texture").c_str());
				glUniform1i(diffuse_texture_id, use_texture);
				//Does the same for shininess as texture
				if (iterator.isShiny())
					is_shiny = 1;
				else
					is_shiny = 0;

				GLuint is_shiny_id = glGetUniformLocation(first_program_, std::string("materials.is_shiny").c_str());
				glUniform1i(is_shiny_id, is_shiny);

			}
		}
		glActiveTexture(GL_TEXTURE0);

		//Offset since the material id is 200 more than the element count inside the array
		int id_offset = 200;
		//If's to check if the current mesh uses the first or second material.
		if (scene_materials_[it.getMaterialId() - id_offset].getDiffuseTexture() == "diff0.png")
		{
			glBindTexture(GL_TEXTURE_2D, test_texture_1_);
			glUniform1i(glGetUniformLocation(first_program_, "texture_data"), 0);

		}
		else if (scene_materials_[it.getMaterialId() - id_offset].getDiffuseTexture() == "diff1.png")
		{
			glBindTexture(GL_TEXTURE_2D, test_texture_2_);
			glUniform1i(glGetUniformLocation(first_program_, "texture_data"), 0);
		}

		glm::mat4 model_xform =
			it.getTransformationMatrix();

		GLuint model_xform_id = glGetUniformLocation(first_program_,
			"model_xform");
		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

		glm::mat4 combined_xform =
			projection_xform
			* view_xform
			* model_xform;

		GLuint combined_xform_id = glGetUniformLocation(first_program_,
			"combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		glBindVertexArray(mymesh.vao);
		glDrawElements(GL_TRIANGLES, mymesh.element_count, GL_UNSIGNED_INT, 0);

	}
}