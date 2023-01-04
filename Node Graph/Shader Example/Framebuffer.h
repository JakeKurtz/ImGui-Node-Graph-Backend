#pragma once

#include "Attachment.h"
#include <vector>

class FrameBuffer {
public:
	FrameBuffer(const GLsizei width, const GLsizei height);
	~FrameBuffer();

	void bind();
	void bind(const GLsizei width, const GLsizei height);

	void bind_rbo();
	void bind_rbo(const GLsizei width, const GLsizei height);

	void unbind();
	void unbind_rbo();

	int attach(const GLenum attachmentType, const GLint internalformat, const GLenum format, const GLenum type);
	int attach_rbo(const GLenum renderbuffertarget, const GLenum internalformat);

	GLuint get_color_tex_id(int i = 0) const;
	GLuint get_depth_tex_id() const;
	GLuint get_stencil_tex_id() const;

	std::shared_ptr<Attachment> get_color_attachment(int i = 0) const;
	std::shared_ptr<Attachment> get_depth_attachment() const;
	std::shared_ptr<Attachment> get_stencil_attachment() const;

	int check_status() const;
	void construct();

	int32_t get_id() const { return id; };
	GLsizei get_width() const { return width; };
	GLsizei get_height() const { return height; };

	void set_attachment_size(const GLsizei width, const GLsizei height);

private:
	GLuint id;
	GLuint rbo_id;

	GLsizei width;
	GLsizei height;

	GLsizei attachment_width;
	GLsizei attachment_height;

	GLsizei rbo_width;
	GLsizei rbo_height;

	GLint rbo_internal_format;
	GLint rbo_target;

	std::shared_ptr<Attachment> depth_attachment = nullptr;
	std::shared_ptr<Attachment> stencil_attachment = nullptr;
	std::vector<std::shared_ptr<Attachment>> color_attachments;

	int glClearValue = 1;

	bool fbo_constructed = false;

	int32_t nmb_color_attachemnts = 0;
	int32_t nmb_depth_attachemnts = 0;
	int32_t nmb_stencil_attachemnts = 0;

};