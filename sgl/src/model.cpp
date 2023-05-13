#include "model/model.h"
#include "utils/error.h"
#include "shaders/render_shader.h"
#include "help.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <map>

SGL_BEG


vbo model_data::mesh::get_vbo() const
{
	vbo res;
	res.generate();
	res.attach_data(vertices, GL_STATIC_DRAW);
	return res;
}

ebo model_data::mesh::get_ebo() const
{
	ebo res;
	res.generate();
	res.attach_data(indices, GL_STATIC_DRAW);
	return res;
}

std::map<std::filesystem::path, texture> texture_storage;

void get_material_textures(const std::filesystem::path &directory, std::vector<const texture *> &textures, aiMaterial *mat, aiTextureType type)
{
	std::filesystem::path path;
	unsigned int count = mat->GetTextureCount(type);
	textures.resize(count);
	for (unsigned int i = 0; i < count; ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		
		path = directory;
		path /= str.C_Str();

		texture &text = texture_storage[path];
		text.load(path.string(), GL_RGBA);
		textures[i] = &text;
	}
}

model_data::material_type process_material(const std::filesystem::path &directory, aiMaterial *mat)
{
	model_data::material_type res;
	
	aiColor3D color(0, 0, 0);
	if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
		res.ambient_color = { color.r, color.g, color.b };
	if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		res.diffuse_color = { color.r, color.g, color.b };
	if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
		res.specular_color = { color.r, color.g, color.b };

	{
		aiString name;
		if (mat->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
			res.name = name.C_Str();
	}

	float val;
	if (mat->Get(AI_MATKEY_SHININESS, val) == AI_SUCCESS)
		res.shininess = val;

	get_material_textures(directory, res.ambient_textures, mat, aiTextureType_AMBIENT);
	get_material_textures(directory, res.diffuse_textures, mat, aiTextureType_DIFFUSE);
	get_material_textures(directory, res.specular_textures, mat, aiTextureType_SPECULAR);

	return res;
}

model_data::mesh process_mesh(const std::vector<model_data::material_type> &materials, aiMesh *mesh)
{
	model_data::mesh res;

	if (mesh->mTextureCoords[0])
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			res.vertices.push_back({
				{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
				{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
				{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} });
	else
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			res.vertices.push_back({
				{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
				{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
				{} });
	for (unsigned int face_i = 0; face_i < mesh->mNumFaces; ++face_i)
	{
		aiFace &face = mesh->mFaces[face_i];
		for (unsigned int i = 0; i < face.mNumIndices; ++i)
			res.indices.push_back(face.mIndices[i]);
	}

	res.material = materials.data() + mesh->mMaterialIndex;

	return res;
}

void process_node(const std::vector<model_data::mesh> &meshes, model_data::node &cur, aiNode *node)
{
	cur.meshes.resize(node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		cur.meshes[i] = meshes.data() + node->mMeshes[i];

	cur.children.resize(node->mNumChildren);
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		cur.children[i] = std::make_unique<model_data::node>();
		process_node(meshes, *cur.children[i], node->mChildren[i]);
	}
}

model_data get_model(const std::string &file_name)
{
	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile(file_name, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene)
	{
		detail::log_error(sgl::error(importer.GetErrorString(), error_code::uknown_error));
		return {};
	}

	std::filesystem::path file(file_name);
	std::filesystem::path parent_directory = file.parent_path();

	model_data res;
	res.m_materials.resize(scene->mNumMaterials);
	res.m_meshes.resize(scene->mNumMeshes);

	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		res.m_materials[i] = process_material(parent_directory, scene->mMaterials[i]);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		res.m_meshes[i] = process_mesh(res.m_materials, scene->mMeshes[i]);

	process_node(res.m_meshes, res.root, scene->mRootNode);

	return res;
}

mesh_type::mesh_type(const model_data::mesh &mesh) : mesh_type()
{
	set_mesh(mesh);
}

void mesh_type::set_mesh(const model_data::mesh &mesh)
{
	detail::ebo_lock ebo_lock;
	detail::vao_lock vao_lock;
	detail::vbo_lock vbo_lock;

	m_mesh = &mesh;
	
	m_vbo = mesh.get_vbo();
	m_ebo = mesh.get_ebo();

	if (!m_vao.index())
		m_vao.generate();

	m_vao.use();
	m_vbo.use();
	m_ebo.use();

	glEnableVertexAttribArray(render_shader::pos_attribute_loc);
	glVertexAttribPointer(render_shader::pos_attribute_loc, 3, GL_FLOAT, GL_FALSE, sizeof(model_data::mesh::vertex_type), (void *)0);

	glEnableVertexAttribArray(render_shader::normal_attribute_loc);
	glVertexAttribPointer(render_shader::normal_attribute_loc, 3, GL_FLOAT, GL_FALSE, sizeof(model_data::mesh::vertex_type), (void *)offsetof(model_data::mesh::vertex_type, normal));

	glEnableVertexAttribArray(render_shader::textPos_attribute_loc);
	glVertexAttribPointer(render_shader::textPos_attribute_loc, 2, GL_FLOAT, GL_FALSE, sizeof(model_data::mesh::vertex_type), (void *)offsetof(model_data::mesh::vertex_type, uv));
}

void mesh_type::draw(render_target &target) const
{
	detail::vao_lock lock;
	detail::fbo_lock flock;

	bind_target(target);

	m_vao.use();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_mesh->indices.size()), GL_UNSIGNED_INT, nullptr);
}

namespace model_detail
{
	render_shader &get_shader()
	{
		using namespace variables;
		static std::string vertex_source = "void main() { gl_Position = sgl_ModelViewProj * vec4(sgl_Pos, 1.0); }";
		static std::string fragment_source = "void main() { sgl_OutColor = sgl_Color; }";
		static render_shader shader(vertex_source, fragment_source, sgl_ModelViewProj | sgl_Pos | sgl_Color);
		return shader;
	}
}

texture &default_specular()
{
	static char data[4] = { 255, 255, 255, 255 };
	static texture res(GL_RGBA, data, 1, 1, 4, false);
	return res;
}

template <bool scalable, bool rotatable>
void mesh_obj<scalable, rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	render_shader &shader = settings.shader ? *settings.shader : model_detail::get_shader();

	const mesh_type *type = dynamic_cast<const mesh_type *>(render_obj::type);
	auto mesh = type->mesh();
	if (mesh->material)
	{
		const auto &mmaterial = *mesh->material;

		if (mmaterial.specular_textures.size() || mmaterial.ambient_textures.size() || mmaterial.diffuse_textures.size())
		{
			// I'm not sure what to do if there's more than one diffuse/specular texture
			texture_material mat;
			if (mmaterial.specular_textures.size())
				mat.specular = mmaterial.specular_textures.front();
			else
				mat.specular = &default_specular();
			if (mmaterial.diffuse_textures.size())
				mat.diffuse = mmaterial.diffuse_textures.front();
			if (mmaterial.shininess)
				mat.shininess = *mmaterial.shininess;
			else
				// temporary solution
				mat.shininess = 128;

			detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, &mat, settings.color);
		}
		else
		{
			material mat;
			if (mmaterial.ambient_color)
				mat.ambient = *mmaterial.ambient_color;
			if (mmaterial.diffuse_color)
				mat.diffuse = *mmaterial.diffuse_color;
			if (mmaterial.specular_color)
				mat.specular = *mmaterial.specular_color;
			if (mmaterial.shininess)
				mat.shininess = *mmaterial.shininess;
			else
				mat.shininess = 255;

			detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, &mat, settings.color);
		}
	}
	
	type->draw(target);
}

template <bool scalable, bool rotatable>
void mesh_obj<scalable, rotatable>::draw(render_target &target) const
{
	render_settings settings({ 0, 0, 0, 1 }, nullptr, nullptr, nullptr);
	draw(target, settings);
}

model_type::model_type(const model_data &model) : model_type()
{
	set_model(model);
}

void model_type::set_model(const model_data &model)
{
	m_model = &model;
	m_meshes.clear();
	for (const auto &mesh : model.meshes())
		m_meshes.emplace_back(mesh);
}

void model_type::draw(render_target &target) const
{
	for (const auto &mesh : m_meshes)
		mesh.draw(target);
}

template <bool scalable, bool rotatable>
model_obj<scalable, rotatable>::model_obj(const model_type &type) : render_obj(type)
{
	meshes.reserve(type.m_meshes.size());
	for (const auto &mesh : type.m_meshes)
		meshes.emplace_back(mesh);
}

template <bool scalable, bool rotatable>
void model_obj<scalable, rotatable>::draw(render_target &target, const render_settings &settings) const
{
	for (const auto &mesh : meshes)
		mesh.draw(target, settings);
}

template <bool scalable, bool rotatable>
void model_obj<scalable, rotatable>::draw(render_target &target) const
{
	for (const auto &mesh : meshes)
		mesh.draw(target);
}

template class model_obj<false, false>;
template class model_obj<true, false>;
template class model_obj<false, true>;
template class model_obj<true, true>;

template class mesh_obj<false, false>;
template class mesh_obj<true, false>;
template class mesh_obj<false, true>;
template class mesh_obj<true, true>;

SGL_END