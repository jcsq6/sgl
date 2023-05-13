#pragma once
#include "macro.h"
#include "object/shape_data.h"
#include "object/render_obj.h"
#include "object/texture.h"

#include <optional>
#include <memory>
#include <list>

SGL_BEG
class model_data
{
public:
	struct material_type
	{
		std::vector<const texture *> ambient_textures;
		std::vector<const texture *> diffuse_textures;
		std::vector<const texture *> specular_textures;
		
		std::optional<std::string> name;

		std::optional<vec3> ambient_color;
		std::optional<vec3> diffuse_color;
		std::optional<vec3> specular_color;
		std::optional<float> shininess;
	};

	struct mesh
	{
		inline mesh() : material{} {}

		struct vertex_type
		{
			vec3 pos;
			vec3 normal;
			vec2 uv;
		};

		std::vector<vertex_type> vertices;
		std::vector<unsigned int> indices;
		const material_type *material;

		vbo get_vbo() const;
		ebo get_ebo() const;
	};

	struct node
	{
		std::vector<const mesh *> meshes;
		std::vector<std::unique_ptr<node>> children;
	};

	inline const auto &meshes() const { return m_meshes; }

	node root;
private:
	std::vector<mesh> m_meshes;
	std::vector<material_type> m_materials;

	friend model_data get_model(const std::string &file_name);
};

model_data get_model(const std::string &file_name);

class mesh_type : public rendervao_type
{
public:
	inline mesh_type() : rendervao_type(), m_mesh{} {}

	// assuming mesh will remain valid through mesh_type's lifetime
	mesh_type(const model_data::mesh &mesh);

	inline const model_data::mesh *mesh() const { return m_mesh; }

	void set_mesh(const model_data::mesh &mesh);

	// won't use material properties
	void draw(render_target &target) const override;
private:
	const model_data::mesh *m_mesh;
	vbo m_vbo;
	ebo m_ebo;
};

template <bool scalable, bool rotatable>
class mesh_obj : public transformable_obj<true, scalable, rotatable>
{
public:
	inline mesh_obj(const mesh_type &type) : render_obj(type) {}

	void draw(render_target &target, const render_settings &settings) const override;
	void draw(render_target &target) const override;
};

class model_type : public render_type
{
public:
	inline model_type() : render_type(), m_model{} {}
	model_type(const model_data &model);

	inline const model_data *model() const { return m_model; }
	
	void set_model(const model_data &model);

	void draw(render_target &target) const override;
private:
	const model_data *m_model;
	std::list<mesh_type> m_meshes;

	template <bool, bool>
	friend class model_obj;
};

template <bool scalable, bool rotatable>
class model_obj : public render_obj
{
public:
	// entrusting that type will not change during the lifespan of model_obj
	model_obj(const model_type &type);

	void draw(render_target &target, const render_settings &settings) const override;
	void draw(render_target &target) const override;

	std::vector<mesh_obj<scalable, rotatable>> meshes;
};

SGL_END