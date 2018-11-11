#pragma once

#include <scene/context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tygra/FileHelper.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

    void setScene(const scene::Context * scene);

    void toggleShading();

private:

    void windowViewWillStart(tygra::Window * window) override;

    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;

    void windowViewRender(tygra::Window * window) override;

private:

    const scene::Context * scene_{ nullptr };

    GLuint terrain_sp_{ 0 };
	
    bool shade_normals_{ false };
	struct QuadPatches
	{
		glm::vec3 TL_min_pos;
		glm::vec3 TL_max_pos;
		glm::vec3 TR_min_pos;
		glm::vec3 TR_max_pos;
		glm::vec3 BL_min_pos;
		glm::vec3 BL_max_pos;
		glm::vec3 BR_min_pos;
		glm::vec3 BR_max_pos;

		int num_of_indices;
		int base_vertex;
		bool isRendered;
	};
	int number_of_mesh_patches{ 0 };
	int quad_per_patch{ 0 };

	std::vector<QuadPatches> mesh_patches;
	std::vector<unsigned int> patch_elements;

    struct MeshGL
    {
        GLuint position_vbo{ 0 };
        GLuint normal_vbo{ 0 };
        GLuint element_vbo{ 0 };
        GLuint vao{ 0 };
        int element_count{ 0 };
    };

    MeshGL terrain_mesh_;
	//Containers
	std::vector<glm::vec3> vertex;
	std::vector<glm::vec2> uv_coords;
	std::vector<unsigned int> elements;
	std::vector<glm::vec3> bezier_patch_set_1;
	std::vector<glm::vec3> bezier_patch_set_2;
		
	std::vector<glm::vec3> normals;

    enum
    {
        kVertexPosition = 0,
        kVertexNormal = 1,
    };

	//Functions
	void ElementGen(std::vector<unsigned int> &elements, const int &subU, const int &subV, const int &patch_subU, const int &patch_subV, const int &patches_U, const int &patches_V);
	glm::vec3 CubicBezier(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const float u);
	glm::vec3 BezierSurface(const std::vector<glm::vec3> &cps, const float &u, const float &v);
	void CalculateNormals(std::vector<glm::vec3> &normals, const std::vector<unsigned int> &ele, const std::vector<glm::vec3> &vertex);
	void ApplyBezier(std::vector<glm::vec3> &vertex, const std::vector<glm::vec2> &uv_coords,const std::vector<Array2d<glm::vec3, 4, 4>> &bezier_patches);
	void ApplyDisplacement(std::vector<glm::vec3> &vertex, const std::vector<glm::vec2> &uv_coords, const std::vector<glm::vec3> &normals, const tygra::Image &displace_image);
	float Noise(const int &x, const int &y);
	float CosineLerp(const float &a, const float &b, const float &x);
	float KenPerlin(const float &xPos, const float &zPos);
	float FractionalBrownianMotion(glm::vec3 x);
	void ApplyNoise(std::vector<glm::vec3> &vertex, const std::vector<glm::vec3> &normals);
	void MyView::MakePatches(std::vector<QuadPatches> &mesh_patches, const int &subU, const int &subV, const int &patch_subU, const int &patch_subV, const int &patches_U, const int &patches_V);
	bool PointInFrustum(const glm::vec4 plane[], const QuadPatches &mesh_patches);
};
