#pragma once

#include <scene/scene_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

    void setScene(const scene::Context * scene);

private:

    void windowViewWillStart(tygra::Window * window) override;

    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;

    void windowViewRender(tygra::Window * window) override;

private:

    const scene::Context * scene_{ nullptr };

	GLuint test_texture_1_{ 0 };

	GLuint test_texture_2_{ 0 };

	GLuint first_program_{ 0 };

	//Structure to combine pos, norm, texcoord together to put into 1 vbo
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 texCoord;
	};

	struct MeshGL
	{
		// VertexBufferObject for the vertex positions
		GLuint vertex_vbo{ 0 };

		// VertexBufferObject for the elements (indices)
		GLuint element_vbo{ 0 };

		// VertexArrayObject for the shape's vertex array settings
		GLuint vao{ 0 };

		// Needed for when we draw using the vertex arrays
		int element_count{ 0 };
	};

	std::unordered_map<scene::MeshId, MeshGL> meshes_;

	const static GLuint kNullId = 0;
	enum VertexAttribIndexes {
		kVertexPosition = 0,
		kVertexColour = 1,
		kNormal = 2,
		kTexCoord = 3
	};
	enum FragmentDataIndexes {
		kFragmentColour = 0
	};
	enum TextureIndexes {
		kTextureTest = 0
	};

	std::vector<scene::Light> scene_lights_;
};
