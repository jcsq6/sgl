#pragma once
#include "context_lock/context_lock.h"
#include "gl_object.h"
#include "gtexture.h"

SGL_BEG

template <GLenum t>
class buffer_view;

class vao : public gl_object
{
public:
	vao(const vao &) = delete;
	vao &operator=(const vao &) = delete;

	inline vao() : id{} {}

	inline ~vao()
	{
		destroy();
	}

	inline vao(vao &&other) noexcept : id{other.id}
	{
		other.id = 0;
	}
	inline vao &operator=(vao &&other) noexcept
	{
		destroy();
		id = other.id;
		other.id = 0;
		return *this;
	}

	inline void generate() override
	{
		destroy();
		glGenVertexArrays(1, &id);
	}

	inline void use() const override
	{
		glBindVertexArray(id);
	}

	inline void destroy() override
	{
		glDeleteVertexArrays(1, &id);
		id = 0;
	}

	inline unsigned int index() const override
	{
		return id;
	}

	inline static void quit()
	{
		glBindVertexArray(0);
	}

	buffer_view<GL_ARRAY_BUFFER> get_attribute(GLuint index) const;

private:
	GLuint id;
};

template <GLenum t>
class buffer;

template <GLenum t>
class buffer_view
{
public:
	static constexpr GLenum target = t;

	buffer_view(const buffer_view &) = default;
	buffer_view &operator=(const buffer_view &) = default;

	inline buffer_view() : id{0} {}
	inline buffer_view(GLuint _id) : id{_id} {}

	buffer_view(const buffer<target> &_buffer);
	buffer_view &operator=(const buffer<target> &_buffer);

	inline void use() const
	{
		glBindBuffer(t, id);
	}

	inline unsigned int index() const
	{
		return id;
	}

	inline static void quit()
	{
		buffer<target>::quit();
	}

	template <typename C>
	inline void attach_sub_data(const C &data, GLintptr byte_offset = 0) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferSubData(t, byte_offset, data.size() * sizeof(typename C::value_type), &data[0]);
	}

	template <typename C>
	inline void attach_sub_data(GLintptr byte_offset, GLsizeiptr byte_size, const C *data) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferSubData(t, byte_offset, byte_size, data);
	}

	template <typename T, GLsizeiptr N>
	inline void attach_sub_data(T (&data)[N], GLintptr byte_offset = 0) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferSubData(t, byte_offset, sizeof(data), data);
	}

	template <typename C>
	inline void attach_data(const C &data, GLenum usage = GL_STATIC_DRAW) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferData(t, data.size() * sizeof(typename C::value_type), &data[0], usage);
	}
	template <typename C>
	inline void attach_data(GLsizeiptr byte_size, const C *data, GLenum usage = GL_STATIC_DRAW) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferData(t, byte_size, data, usage);
	}
	template <typename T, GLsizeiptr N>
	inline void attach_data(T (&data)[N], GLenum usage = GL_STATIC_DRAW) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferData(t, sizeof(data), data, usage);
	}
	
	inline void reserve_data(GLsizeiptr byte_size, GLenum usage = GL_STATIC_DRAW) const
	{
		detail::context_lock<detail::binding<target>> lock;
		use();
		glBufferData(t, byte_size, nullptr, usage);
	}

	template <typename T>
	class mapped_data_ptr
	{
	public:
		using value_type = T;

		mapped_data_ptr(const mapped_data_ptr &) = delete;
		mapped_data_ptr &operator=(const mapped_data_ptr &) = delete;

		inline mapped_data_ptr(GLuint id, GLenum access) : m_id{id}, m_data{}
		{
			detail::context_lock<detail::binding<target>> lock;

			glBindBuffer(target, id);
			m_data = reinterpret_cast<T *>(glMapBuffer(target, access));
		}

		inline mapped_data_ptr(mapped_data_ptr &&other) : m_id{other.m_id}, m_data{other.m_data}
		{
			other.m_id = 0;
			other.m_data = nullptr;
		}

		inline mapped_data_ptr &operator=(mapped_data_ptr &&other)
		{
			destroy();

			m_id = other.m_id;
			m_data = other.m_data;

			other.m_id = 0;
			other.m_data = nullptr;
		}

		inline ~mapped_data_ptr()
		{
			destroy();
		}

		value_type &operator[](std::size_t i) const
		{
			return m_data[i];
		}

	private:
		inline void destroy()
		{
			if (m_data && m_id)
			{
				detail::context_lock<detail::binding<target>> lock;

				glBindBuffer(target, m_id);
				glUnmapBuffer(target);
			}

			m_data = nullptr;
			m_id = 0;
		}

		T *m_data;
		GLuint m_id;
	};

	template <typename T>
	inline auto get_data(GLenum access) const
	{
		mapped_data_ptr<T> res(id, access);
		return res;
	}

private:
	GLuint id;

	template <GLenum>
	friend class buffer;
};

template <>
class buffer_view<GL_RENDERBUFFER>
{
public:
	static constexpr GLenum target = GL_RENDERBUFFER;
	static constexpr GLenum target_binding = GL_RENDERBUFFER_BINDING;

	buffer_view(const buffer_view &) = default;
	buffer_view &operator=(const buffer_view &) = default;

	inline buffer_view() : id{0} {}
	inline buffer_view(GLuint _id) : id{_id} {}

	buffer_view(const buffer<target> &_buffer);
	buffer_view &operator=(const buffer<target> &_buffer);

	inline void use() const
	{
		glBindRenderbuffer(target, id);
	}

	inline unsigned int index() const
	{
		return id;
	}

	inline void reserve_data(GLenum type, GLenum format, GLsizei width, GLsizei height) const
	{
		detail::rbo_lock lock;
		use();
		glRenderbufferStorage(target, format, width, height);
	}

	inline static void quit()
	{
		glBindRenderbuffer(target, 0);
	}

private:
	GLuint id;

	template <GLenum>
	friend class buffer;
};

template <>
class buffer_view<GL_FRAMEBUFFER>
{
public:
	static constexpr GLenum target = GL_FRAMEBUFFER;
	static constexpr GLenum target_binding = GL_FRAMEBUFFER_BINDING;

	buffer_view(const buffer_view &) = default;
	buffer_view &operator=(const buffer_view &) = default;

	inline buffer_view() : id{0} {}
	inline buffer_view(GLuint _id) : id{_id} {}

	buffer_view(const buffer<target> &_buffer);
	buffer_view &operator=(const buffer<target> &_buffer);

	inline void use() const
	{
		glBindFramebuffer(target, id);
	}

	inline unsigned int index() const
	{
		return id;
	}

	inline void attach_data(const gtexture &text, GLenum type) const
	{
		detail::fbo_lock lock;
		use();
		glFramebufferTexture2D(target, type, GL_TEXTURE_2D, text.index(), 0);
	}

	inline void attach_data(buffer_view<GL_RENDERBUFFER> text, GLenum type) const
	{
		detail::fbo_lock lock;
		use();
		glFramebufferRenderbuffer(target, type, GL_RENDERBUFFER, text.index());
	}

	inline static void quit()
	{
		glBindFramebuffer(target, 0);
	}

private:
	GLuint id;

	template <GLenum>
	friend class buffer;
};

template <GLenum t>
class buffer : public gl_object
{
public:
	static constexpr GLenum target = t;

	buffer(const buffer &) = delete;
	buffer &operator=(const buffer &) = delete;

	inline buffer() : id{}
	{
	}

	inline ~buffer()
	{
		destroy();
	}

	inline buffer(buffer &&other) noexcept : id{other.id}
	{
		other.id.id = 0;
	}
	inline buffer &operator=(buffer &&other)
	{
		destroy();
		id.id = other.id.id;
		other.id.id = 0;
		return *this;
	}

	inline void generate() override
	{
		destroy();
		glGenBuffers(1, &id.id);
	}

	inline void use() const override
	{
		id.use();
	}

	inline void destroy() override
	{
		glDeleteBuffers(1, &id.id);
		id.id = 0;
	}

	inline unsigned int index() const override
	{
		return id.index();
	}

	template <typename C>
	inline void attach_sub_data(const C &data, GLintptr byte_offset = 0) const
	{
		id.attach_sub_data(data, byte_offset);
	}
	template <typename C>
	inline void attach_sub_data(GLintptr byte_offset, GLsizeiptr byte_size, const C *data) const
	{
		id.attach_sub_data(byte_offset, byte_size, data);
	}
	template <typename T, GLsizeiptr N>
	inline void attach_sub_data(T (&data)[N], GLintptr byte_offset = 0) const
	{
		id.attach_sub_data(data, byte_offset);
	}

	template <typename C>
	inline void attach_data(const C &data, GLenum usage = GL_STATIC_DRAW) const
	{
		id.attach_data(data, usage);
	}
	template <typename C>
	inline void attach_data(GLsizeiptr byte_size, const C *data, GLenum usage = GL_STATIC_DRAW) const
	{
		id.attach_data(byte_size, data, usage);
	}
	template <typename T, GLsizeiptr N>
	inline void attach_data(T (&data)[N], GLenum usage = GL_STATIC_DRAW) const
	{
		id.attach_data(data, usage);
	}
	
	inline void reserve_data(GLsizeiptr byte_size, GLenum usage = GL_STATIC_DRAW) const
	{
		id.reserve_data(byte_size, usage);
	}

	inline static void quit()
	{
		buffer_view<target>::quit();
	}

	template <typename T>
	inline auto get_data(GLenum access) const
	{
		return id.template get_data<T>(access);
	}

private:
	buffer_view<target> id;
};

template <>
class buffer<GL_RENDERBUFFER> : public gl_object
{
public:
	static constexpr GLenum target = GL_RENDERBUFFER;
	static constexpr GLenum target_binding = GL_RENDERBUFFER_BINDING;

	buffer(const buffer &) = delete;
	buffer &operator=(const buffer &) = delete;

	inline buffer() : id{} {}

	inline ~buffer()
	{
		destroy();
	}

	inline buffer(buffer &&other) noexcept : id{other.id}
	{
		other.id.id = 0;
	}
	inline buffer &operator=(buffer &&other) noexcept
	{
		destroy();
		id.id = other.id.id;
		other.id.id = 0;
		return *this;
	}

	inline void generate() override
	{
		destroy();
		glGenRenderbuffers(1, &id.id);
	}

	inline void use() const override
	{
		id.use();
	}

	inline void destroy() override
	{
		glDeleteRenderbuffers(1, &id.id);
		id.id = 0;
	}

	inline unsigned int index() const override
	{
		return id.index();
	}

	inline void reserve_data(GLenum type, GLenum format, GLsizei width, GLsizei height) const
	{
		id.reserve_data(type, format, width, height);
	}

	inline static void quit()
	{
		buffer_view<target>::quit();
	}

private:
	buffer_view<target> id;
};

template <>
class buffer<GL_FRAMEBUFFER> : public gl_object
{
public:
	static constexpr GLenum target = GL_FRAMEBUFFER;
	static constexpr GLenum target_binding = GL_FRAMEBUFFER_BINDING;

	inline buffer(const buffer &) = delete;
	inline buffer &operator=(const buffer &) = delete;

	inline buffer() : id{} {}

	inline ~buffer() { destroy(); }

	inline buffer(buffer &&other) noexcept
	{
		id.id = other.id.id;
		other.id.id = 0;
	}

	inline buffer &operator=(buffer &&other) noexcept
	{
		destroy();
		id.id = other.id.id;
		other.id.id = 0;
		return *this;
	}

	inline void generate() override
	{
		destroy();
		glGenFramebuffers(1, &id.id);
	}

	inline void use() const override
	{
		glBindFramebuffer(target, id.id);
	}

	inline void destroy() override
	{
		glDeleteFramebuffers(1, &id.id);
		id.id = 0;
	}

	inline unsigned int index() const override
	{
		return id.index();
	}

	inline void attach_data(const gtexture &text, GLenum type) const
	{
		id.attach_data(text, type);
	}

	inline void attach_data(const buffer<GL_RENDERBUFFER> &text, GLenum type) const
	{
		id.attach_data(text, type);
	}

	inline static void quit()
	{
		buffer_view<target>::quit();
	}

private:
	buffer_view<target> id;
};

template <GLenum target>
buffer_view<target>::buffer_view(const buffer<target> &_buffer) : id{_buffer.id} {}

template <GLenum target>
buffer_view<target> &buffer_view<target>::operator=(const buffer<target> &_buffer) { id = _buffer.id; return *this; }

inline buffer_view<GL_ARRAY_BUFFER> vao::get_attribute(GLuint index) const
{
	detail::vao_lock lvao;
	use();
	GLint id;
	glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &id);
	return buffer_view<GL_ARRAY_BUFFER>(id);
}

#define vbo_target GL_ARRAY_BUFFER
#define ebo_target GL_ELEMENT_ARRAY_BUFFER
#define ubo_target GL_UNIFORM_BUFFER
#define ssbo_target GL_SHADER_STORAGE_BUFFER
#define rbo_target GL_RENDERBUFFER
#define fbo_target GL_FRAMEBUFFER

using vbo = buffer<vbo_target>;
using ebo = buffer<ebo_target>;
using ubo = buffer<ubo_target>;
using ssbo = buffer<ssbo_target>;
using rbo = buffer<rbo_target>;
using fbo = buffer<fbo_target>;

using vbo_view = buffer_view<vbo_target>;
using ebo_view = buffer_view<ebo_target>;
using ubo_view = buffer_view<ubo_target>;
using ssbo_view = buffer_view<ssbo_target>;
using rbo_view = buffer_view<rbo_target>;
using fbo_view = buffer_view<fbo_target>;

SGL_END