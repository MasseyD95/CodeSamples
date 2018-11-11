#include "my_view.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

#define NUM_OCTAVES 5

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const scene::Context * scene)
{
    scene_ = scene;
}

void MyView::toggleShading()
{
    shade_normals_ = !shade_normals_;
}

void MyView::windowViewWillStart(tygra::Window * window)
{

#pragma region Attatching Shaders
    assert(scene_ != nullptr);

    GLint compile_status = 0;
    GLint link_status = 0;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string
        = tygra::createStringFromFile("resource:///terrain_vs.glsl");
    const char *vertex_shader_code = vertex_shader_string.c_str();
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
        = tygra::createStringFromFile("resource:///terrain_fs.glsl");
    const char *fragment_shader_code = fragment_shader_string.c_str();
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

	terrain_sp_ = glCreateProgram();
	glAttachShader(terrain_sp_, vertex_shader);
	glDeleteShader(vertex_shader);
	glAttachShader(terrain_sp_, fragment_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(terrain_sp_);

	glGetProgramiv(terrain_sp_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
#pragma endregion 
    
    // X and -Z are on the ground, Y is up
    const float sizeX = scene_->getTerrainSizeX();
    const float sizeY = scene_->getTerrainSizeY();
    const float sizeZ = scene_->getTerrainSizeZ();

    const auto& bezier_patches = scene_->getTerrainPatches();
    const size_t number_of_patches = bezier_patches.size();

	tygra::Image displace_image =
		tygra::createImageFromPngFile(scene_->getTerrainDisplacementMapName());

	//Amount of vertex's along the x and the z axis
	const int width = displace_image.width() * 2;
	const int height = displace_image.height() * 2;

	const int vertex_count = width * height;

	//Cells x and y
	const int subU = width - 1;// sizeX
	const int subV = height - 1;// sizeZ

	const int cells = subU * subV;
	const int triangles = cells * 2;
	const int element_count = triangles * 3;
	//Cells in a patch on x and y
	const int patch_subU = 10;
	const int patch_subV = 10;
	//Amount of patches in x and z
	int patches_U = subU / 10;
	int patches_V = subV / 10;
	number_of_mesh_patches = patches_U * patches_V;
	//Cells in a patch
	quad_per_patch = patch_subU * patch_subV;
	//Resize containers
	elements.resize(triangles * 3);
	vertex.resize(vertex_count);
	normals.resize(vertex_count);
	uv_coords.resize(vertex_count);
	mesh_patches.resize(number_of_mesh_patches);
	
	
	//UV-Coords calculation
	int it = 0;
	for (int i = 0; i <= subV; ++i)
	{
		for(int j = 0; j <= subU; ++j)
		{
			uv_coords[it].x = j / float(subU);
			uv_coords[it].y = i / float(subV);
			it++;
		}
	}

	//Function calls
	ElementGen(elements, subU, subV, patch_subU, patch_subV, patches_U, patches_V);

	ApplyBezier(vertex, uv_coords, bezier_patches);
	CalculateNormals(normals, elements, vertex);

	ApplyDisplacement(vertex, uv_coords, normals, displace_image);
	CalculateNormals(normals, elements, vertex);

	ApplyNoise(vertex, normals);
	CalculateNormals(normals, elements, vertex);

	MakePatches(mesh_patches, subU, subV, patch_subU, patch_subV, patches_U, patches_V);

#pragma region Binding Buffers
	//For vector of unsigned ints
    glGenBuffers(1, &terrain_mesh_.element_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        elements.size() * sizeof(unsigned int),
		elements.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    terrain_mesh_.element_count = elements.size();

    glGenBuffers(1, &terrain_mesh_.position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &terrain_mesh_.normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
		normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &terrain_mesh_.vao);
    glBindVertexArray(terrain_mesh_.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
    glEnableVertexAttribArray(kVertexPosition);
    glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glEnableVertexAttribArray(kVertexNormal);
	glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glDeleteProgram(terrain_sp_);
    glDeleteBuffers(1, &terrain_mesh_.position_vbo);
    glDeleteBuffers(1, &terrain_mesh_.normal_vbo);
    glDeleteBuffers(1, &terrain_mesh_.element_vbo);
    glDeleteVertexArrays(1, &terrain_mesh_.vao);
}

void MyView::windowViewRender(tygra::Window * window)
{
    assert(scene_ != nullptr);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];

    const auto& camera = scene_->getCamera();
    glm::mat4 projection_xform = glm::perspective(
        glm::radians(camera.getVerticalFieldOfViewInDegrees()),
        aspect_ratio,
        camera.getNearPlaneDistance(),
        camera.getFarPlaneDistance());
    glm::vec3 camera_pos = camera.getPosition();
    glm::vec3 camera_at = camera.getPosition() + camera.getDirection();
    glm::vec3 world_up{ 0, 1, 0 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);


    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, shade_normals_ ? GL_FILL : GL_LINE);

    glUseProgram(terrain_sp_);

    GLuint shading_id = glGetUniformLocation(terrain_sp_, "use_normal");
    glUniform1i(shading_id, shade_normals_);

    glm::mat4 world_xform = glm::mat4(1);
    glm::mat4 view_world_xform = view_xform * world_xform;

	glm::mat4 projView = projection_xform * view_xform;
	projView = glm::transpose(projView);

	//Plane calculation using the projection * view matrix used above
	//0 = left 1 = right 2 = top 3 = bottom 4 = near  5 = far
	glm::vec4 mplanes[6];
	mplanes[0] = projView[3] + projView[0];
	mplanes[1] = projView[3] - projView[0];
	mplanes[2] = projView[3] - projView[1];
	mplanes[3] = projView[3] + projView[1];
	mplanes[4] = projView[3] + projView[2];
	mplanes[5] = projView[3] - projView[2];

    GLuint projection_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    GLuint view_world_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "view_world_xform");
    glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                       glm::value_ptr(view_world_xform));
	
	//Number used to output patches rendered.Reset each render loop
	int patches_rendered = 0;
	const int size = mesh_patches.size();

    if (terrain_mesh_.vao) {
        glBindVertexArray(terrain_mesh_.vao);
		for (int i = 0; i < mesh_patches.size(); i++)
		{
			if (PointInFrustum(mplanes, mesh_patches[i]))
			{
				patches_rendered++;
				glDrawElementsBaseVertex(GL_TRIANGLES, mesh_patches[i].num_of_indices, GL_UNSIGNED_INT, 0, mesh_patches[i].base_vertex);
			}
		}
    }
	//Output to console to display culling.
	std::cout << std::string(std::to_string(patches_rendered) + " patches out of " + std::to_string(size) + " rendered.") << std::endl;
}

//Generate the element order to tell openGL how to draw the triangles
void MyView::ElementGen(std::vector<unsigned int> &elements, const int &subU, const int &subV, const int &patch_subU, const int &patch_subV , const int &patches_U, const int &patches_V)
{
	//Used to keep pattern correct
	int currenttri = 0;
	int currentcell = 0;
	int rowflag = 0;
	//Used to keep track of patch
	int offset = 0;
	int currentpatch = 0;
	for (int g = 0; g < patches_V; ++g)
	{
		for (int h = 0; h < patches_U; ++h)
		{
			for (int i = 0; i < patch_subV; ++i)
			{
				for (int j = 0; j < patch_subU; j++)
				{						
					//Formula to draw triangles anticlockwise and to change on the cell to make the diamond pattern
					if ((currentcell % 2 == 0 && rowflag % 2 == 1) || (currentcell % 2 == 1 && rowflag % 2 == 0))
					{
						elements[currenttri] = (i * (subV + 1)) + j + offset;
						elements[currenttri + 1] = (i * (subV + 1)) + j + 1 + offset;
						elements[currenttri + 2] = (i * (subV + 1)) + j + subU + 2 + offset;
						
						elements[currenttri + 3] = (i * (subV + 1)) + j + offset;
						elements[currenttri + 4] = (i * (subV + 1)) + j + subU + 2 + offset;
						elements[currenttri + 5] = (i * (subV + 1)) + j + subU + 1 + offset;

					}
					else
					{
						elements[currenttri] = (i * (subV + 1)) + j + 1 + offset;
						elements[currenttri + 1] = (i * (subV + 1)) + j + subU + 1 + offset;
						elements[currenttri + 2] = (i * (subV + 1)) + j + offset;

						elements[currenttri + 3] = (i * (subV + 1)) + j + 1 + offset;
						elements[currenttri + 4] = (i * (subV + 1)) + j + subU + 2 + offset;
						elements[currenttri + 5] = (i * (subV + 1)) + j + subU + 1 + offset;
					}
					currenttri += 6;
					currentcell += 1;
				}
				rowflag += 1;
			}
			//Add the width of the patch when each patch is drawn
			offset += patch_subU;
		}
		//Reset at the end of the row and then go up rows depending on the height of the patch
		offset = 0;
		offset = g * patch_subU * (subU + 1);
	}
}

//Basic cubic bezier function
glm::vec3 MyView::CubicBezier(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const float u)
{
	return (1 - u) * (1 - u) * (1 - u) * p0
		+ 3 * u * (1 - u) * (1 - u) * p1
		+ 3 * u * u * (1 - u) * p2
		+ u * u * u * p3;
}

//Loops through 4 times doing cubic bezier to make 4 new points using the U coord to then do cubic bezier on them once using the V coord to create a curve
//This uses the bezier patches made within the context file
glm::vec3 MyView::BezierSurface(const std::vector<glm::vec3> &cps, const float &u, const float &v)
{
	std::vector<glm::vec3> curve{ 4 };
	for (int j = 0; j < 16; j += 4)
	{
		curve[j / 4] = CubicBezier(cps[j], cps[j + 1], cps[j + 2], cps[j + 3], u);
	}
	return CubicBezier(curve[0], curve[1], curve[2], curve[3], v);
}

//Calculates new normals using the vertex's around it.
void MyView::CalculateNormals(std::vector<glm::vec3> &normals, const std::vector<unsigned int> &ele, const std::vector<glm::vec3> &vertex)
{
	//Resets normals to zero
	for (int i = 0; i < vertex.size(); ++i)
	{
		normals[i] = glm::vec3(0, 0, 0);
	}

	//Works out new normals using the triangle
	for (int i = 0; i < ele.size(); i += 3)
	{
		//Get a triangle
		glm::vec3 a = vertex[ele[i]];
		glm::vec3 b = vertex[ele[i + 1]];
		glm::vec3 c = vertex[ele[i + 2]];

		glm::vec3 sideOne = b - a;
		glm::vec3 sideTwo = c - a;

		glm::vec3 newNormal = glm::cross(sideOne, sideTwo);

		normals[ele[i]] += newNormal;
		normals[ele[i + 1]] += newNormal;
		normals[ele[i + 2]] += newNormal;
	}

	//Normalizes all the normals to finish off
	for (int i = 0; i < normals.size(); ++i)
	{
		normals[i] = glm::normalize(normals[i]);
	}
}

//Applies he bezier transformation onto the grid
void MyView::ApplyBezier(std::vector<glm::vec3> &vertex, const std::vector<glm::vec2> &uv_coords, const std::vector<Array2d<glm::vec3, 4, 4>> &bezier_patches)
{
	//gets the patch data from the context.cpp file
	std::vector<glm::vec3> bezier_patch_set_1;
	std::vector<glm::vec3> bezier_patch_set_2;

	bezier_patch_set_1.resize(16);
	bezier_patch_set_2.resize(16);

	int count = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			bezier_patch_set_1[count] = bezier_patches[0].at(j, i);
			bezier_patch_set_2[count] = bezier_patches[1].at(j, i);

			count++;
		}
	}

	//Takes the patch data and passes this into the bezier surface function to get a new position for the vertex's
	for (int i = 0; i < uv_coords.size(); ++i)
	{
		//Use patch 1 if less than .5 or patch 2 if greater
		if (uv_coords[i].x < 0.5)
		{
			//Scale the x coord so it does the 0-1 range instead of 0-0.5
			float scaled_x = uv_coords[i].x * 2.0f;
			vertex[i] = glm::vec3(BezierSurface(bezier_patch_set_1, scaled_x, uv_coords[i].y));

		}
		else
		{
			//Scales the x so its the 0-1 value instead of 0.5-1
			float scaled_x = (-0.5 + uv_coords[i].x) * 2.0f;
			vertex[i] = glm::vec3(BezierSurface(bezier_patch_set_2, scaled_x, uv_coords[i].y));
		}
	}
}

//Apply the displacement map to the grid
void MyView::ApplyDisplacement(std::vector<glm::vec3> &vertex, const std::vector<glm::vec2> &uv_coords, const std::vector<glm::vec3> &normals, const tygra::Image &displace_image)
{
	const float scale_displacement = 255; // can be any number between 0-255
	//Loop through the vertex's and use the uv coords to go from pixel to pixel to change the y value based on the texture RGB colour.
	for (int i = 0; i < vertex.size(); ++i)
	{
		uint8_t displacement = *(uint8_t*)displace_image.pixel(uv_coords[i].x * (displace_image.width() - 1), uv_coords[i].y * (displace_image.height() - 1));

		vertex[i] += normals[i] * scale_displacement * (displacement / 255.0f);
	}
}

//Basic noise function
float MyView::Noise(const int &x, const int &y)
{
	int n = x + y * 17; // 57 is the seed – can be tweaked
	n = (n >> 13) ^ n;
	int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741824.0f);
}

//Basic cosine lerp function for noise - unused
float MyView::CosineLerp(const float &a, const float &b, const float &x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft))* 0.5f;
	return a*(1.0f - f) + b*f;
}

//Sinple Perlin Noise function - unused
float MyView::KenPerlin(const float &xPos, const float &zPos)
{
	float s = Noise((int)xPos, (int)zPos);
	float t = Noise((int)xPos + 1, (int)zPos);
	float u = Noise((int)xPos, (int)zPos + 1);
	float v = Noise((int)xPos + 1, (int)zPos + 1);
	float c1 = CosineLerp(s, t, xPos);
	float c2 = CosineLerp(u, v, xPos);
	return CosineLerp(c1, c2, zPos);}

//Function for brownian motion noise
float MyView::FractionalBrownianMotion(glm::vec3 x) 
{
	//Brownian noise which increases the amplitude and frequency each loop to change the noise created. All values can be edited.
	//Industry standard for lacunarity is 2.0, for gain is 0.5. I used 1.8715 since its proven to fix some pattern inregularities
	float amplitude = 0.4;
	float frequency = 0.1f;
	float lacunarity = 1.8715f;
	float gain = 0.5f;
	float total = 0.f;
	for (int i = 0; i < NUM_OCTAVES; ++i)
	{
		total += frequency * Noise(x.x, x.z) * amplitude;
		frequency *= lacunarity;
		amplitude += gain;
	}
	return total;
}

//Function to apply the noise values onto the position using the normals
void MyView::ApplyNoise(std::vector<glm::vec3> &vertex, const std::vector<glm::vec3> &normals)
{
	for (int i = 0; i < vertex.size(); ++i)
	{
		vertex[i] += normals[i] * FractionalBrownianMotion(vertex[i]);
	}
}

//Makes the patches for the frustum culling 
void MyView::MakePatches(std::vector<QuadPatches> &mesh_patches, const int &subU, const int &subV, const int &patch_subU, const int &patch_subV, const int &patches_U, const int &patches_V)
{
	int count = 0;
	int offset = 0;

	for (int i = 0; i < patches_V; ++i)
	{
		for (int j = 0; j < patches_U; ++j)
		{
			//makes temporary patch to find corners and get the positions
			QuadPatches mesh_patch;
			mesh_patch.base_vertex = offset;

			mesh_patch.num_of_indices = (patch_subU * patch_subV) * 6;

			mesh_patch.BL_max_pos = vertex[offset];
			mesh_patch.BR_max_pos = vertex[offset + patch_subU];
			mesh_patch.TL_max_pos = vertex[offset + (patch_subV * (subU + 1))];
			mesh_patch.TR_max_pos = vertex[offset + (patch_subV * (subU + 1)) + patch_subU];

			mesh_patch.BL_min_pos = vertex[offset];
			mesh_patch.BR_min_pos = vertex[offset + patch_subU];
			mesh_patch.TL_min_pos = vertex[offset + (patch_subV * (subU + 1))];
			mesh_patch.TR_min_pos = vertex[offset + (patch_subV * (subU + 1)) + patch_subU];

			//Find the min and max of x,y,z of the patches
			int min_y = vertex[offset].y;
			int max_y = vertex[offset].y;

			for (int i = 0; i <= 120; ++i)
			{
				if (vertex[i + offset].y < min_y)
				{
					min_y = vertex[i + offset].y;
				}
				if (vertex[i + offset].y > max_y)
				{
					max_y = vertex[i + offset].y;
				}
			}
			//Gives each corner the highest and lowest y value to make a cube of 8 points.
			mesh_patch.BL_max_pos.y = max_y;
			mesh_patch.BL_min_pos.y = min_y;

			mesh_patch.BR_max_pos.y = max_y;
			mesh_patch.BR_min_pos.y = min_y;

			mesh_patch.TL_max_pos.y = max_y;
			mesh_patch.TL_min_pos.y = min_y;

			mesh_patch.TR_max_pos.y = max_y;
			mesh_patch.TR_min_pos.y = min_y;

			mesh_patches[count] = mesh_patch;

			offset += patch_subU;
			count++;
		}
		offset = 0;
		offset = i * patch_subU * (subU + 1);
	}
}

//Checks if a patch should be drawn or not
bool MyView::PointInFrustum(const glm::vec4 plane[],const QuadPatches &mesh_patches)
{
	int p;
	//Gets given the camera planes which make the frustum and gets a new patch each time the function is called.
	for (p = 0; p < 6; p++)
	{
		//If one the points is inside the frustum then it breaks out the loop and returns true to draw that patch
		//If all the points go through as a negative(not in the frustum) then it goes one up in the for loop and tries it against a new plane.
		if (plane[p].x * mesh_patches.TL_min_pos.x + plane[p].y * mesh_patches.TL_min_pos.y + plane[p].z * mesh_patches.TL_min_pos.z + plane[p].w < 0 &&
			plane[p].x * mesh_patches.TL_max_pos.x + plane[p].y * mesh_patches.TL_max_pos.y + plane[p].z * mesh_patches.TL_max_pos.z + plane[p].w < 0)
		{
			if (plane[p].x * mesh_patches.TR_min_pos.x + plane[p].y * mesh_patches.TR_min_pos.y + plane[p].z * mesh_patches.TR_min_pos.z + plane[p].w < 0 &&
				plane[p].x * mesh_patches.TR_max_pos.x + plane[p].y * mesh_patches.TR_max_pos.y + plane[p].z * mesh_patches.TR_max_pos.z + plane[p].w < 0)
			{
				if (plane[p].x * mesh_patches.BL_min_pos.x + plane[p].y * mesh_patches.BL_min_pos.y + plane[p].z * mesh_patches.BL_min_pos.z + plane[p].w < 0 &&
					plane[p].x * mesh_patches.BL_max_pos.x + plane[p].y * mesh_patches.BL_max_pos.y + plane[p].z * mesh_patches.BL_max_pos.z + plane[p].w < 0)
				{
					if (plane[p].x * mesh_patches.BR_min_pos.x + plane[p].y * mesh_patches.BR_min_pos.y + plane[p].z * mesh_patches.BR_min_pos.z + plane[p].w < 0 &&
						plane[p].x * mesh_patches.BR_max_pos.x + plane[p].y * mesh_patches.BR_max_pos.y + plane[p].z * mesh_patches.BR_max_pos.z + plane[p].w < 0)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}
