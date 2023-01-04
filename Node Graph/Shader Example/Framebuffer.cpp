#include "FrameBuffer.h"
#include <stdio.h>

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height)
{
    glGenFramebuffers(1, &id);
    glGenRenderbuffers(1, &rbo_id);

    (this)->width = width;
    (this)->height = height;

    (this)->attachment_width = width;
    (this)->attachment_height = height;

    color_attachments.reserve(32);
    for (int i = 0; i < 32; i++) color_attachments.push_back(NULL);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &id);
    glDeleteRenderbuffers(1, &rbo_id);
}

void FrameBuffer::bind_rbo()
{
    glViewport(0, 0, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
}

void FrameBuffer::bind_rbo(const GLsizei width, const GLsizei height)
{
    glViewport(0, 0, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
}

void FrameBuffer::bind()
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::bind(const GLsizei width, const GLsizei height)
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::unbind_rbo()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

int FrameBuffer::attach(const GLenum attachment_type, const GLint internal_format, const GLenum format, const GLenum type)
{
    int success = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    switch (attachment_type) {
    case GL_COLOR:
        if (nmb_color_attachemnts >= GL_MAX_COLOR_ATTACHMENTS) {
            std::cout << "Error: max color attachments allocated. Aborting attachment." << std::endl;
        }
        else {
            color_attachments[nmb_color_attachemnts] = std::make_shared<Attachment>(attachment_width, attachment_height, GL_COLOR_ATTACHMENT0 + nmb_color_attachemnts, internal_format, format, type);
            nmb_color_attachemnts++;
            success = 1;
        }
        break;
    case GL_DEPTH:
        if (nmb_depth_attachemnts >= 1) {
            std::cout << "Warning: depth attachment already exists. Overwriting attachment." << std::endl;
        }
        else {
            nmb_depth_attachemnts++;
        }
        depth_attachment = std::make_shared<Attachment>(attachment_width, attachment_height, GL_DEPTH_ATTACHMENT, internal_format, format, type);
        success = 1;
        break;
    case GL_STENCIL:
        if (nmb_stencil_attachemnts >= 1) {
            std::cout << "Warning: stencil attachment already exists. Overwriting attachment." << std::endl;
        }
        else {
            nmb_stencil_attachemnts++;
        }
        stencil_attachment = std::make_shared<Attachment>(attachment_width, attachment_height, GL_STENCIL_ATTACHMENT, internal_format, format, type);
        success = 1;
        break;
    default:
        std::cout << "Error: the attachment type \"" << attachment_type << "\" is not recognized. Try \"GL_COLOR\", \"GL_DEPTH\", or \"GL_STENCIL\"." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return success;
}

int FrameBuffer::attach_rbo(const GLenum renderbuffertarget, const GLenum internal_format)
{
    rbo_internal_format = internal_format;
    rbo_target = renderbuffertarget;

    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, rbo_internal_format, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbo_target, GL_RENDERBUFFER, rbo_id);
    check_status();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 1;
}

GLuint FrameBuffer::get_color_tex_id(int i) const
{
    if (i >= nmb_color_attachemnts) {
        std::cout << "Error: color attachment " << i << " is NULL." << std::endl;
        return 0;
    }
    else {
        return color_attachments[i]->get_id();
    }
}

void FrameBuffer::set_attachment_size(const GLsizei width, const GLsizei height)
{
    (this)->width = width;
    (this)->height = height;

    (this)->attachment_width = width;
    (this)->attachment_height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    for (int i = 0; i < nmb_color_attachemnts; i++)
    {
        color_attachments[i]->set_size(attachment_width, attachment_height);
    }
    if (nmb_stencil_attachemnts == 1) {
        stencil_attachment->set_size(attachment_width, attachment_height);
    }

    if (nmb_depth_attachemnts == 1) {
        depth_attachment->set_size(attachment_width, attachment_height);
    }

    check_status();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::get_depth_tex_id() const
{
    if (nmb_depth_attachemnts != 1) {
        std::cout << "Error: the depth attachment is NULL." << std::endl;
        return 0;
    }
    else {
        return depth_attachment->get_id();
    }
}

GLuint FrameBuffer::get_stencil_tex_id() const
{
    if (nmb_stencil_attachemnts != 1) {
        std::cout << "Error: the depth attachment is NULL." << std::endl;
        return 0;
    }
    else {
        return stencil_attachment->get_id();
    }
}

std::shared_ptr<Attachment> FrameBuffer::get_color_attachment(int i) const
{
    if (i >= nmb_color_attachemnts) {
        std::cout << "Error: color attachment " << i << " is NULL." << std::endl;
        return nullptr;
    }
    else {
        return color_attachments[i];
    }
}

std::shared_ptr<Attachment> FrameBuffer::get_depth_attachment() const
{
    if (nmb_depth_attachemnts != 1) {
        std::cout << "Error: the depth attachment is NULL." << std::endl;
        return nullptr;
    }
    else {
        return depth_attachment;
    }
}

std::shared_ptr<Attachment> FrameBuffer::get_stencil_attachment() const
{
    if (nmb_stencil_attachemnts != 1) {
        std::cout << "Error: the stencil attachment is NULL." << std::endl;
        return nullptr;
    }
    else {
        return stencil_attachment;
    }
}

void FrameBuffer::construct()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    std::vector<unsigned int> active_attachments;
    for (int i = 0; i < nmb_color_attachemnts; i++) {
        if (color_attachments[i] != NULL) active_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glDrawBuffers(active_attachments.size(), active_attachments.data());
    check_status();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int FrameBuffer::check_status() const
{
    GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "WARNING: Framebuffer is not complete!" << std::endl;
        switch (fbo_status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            std::cout << "\tFBO STATUS: UNDEFINED" << std::endl;
            std::cout << "\tDESCRIPTION: This is returned if the specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist." << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout << "\tFBO STATUS: INCOMPLETE ATTACHMENT" << std::endl;
            std::cout << "\tDESCRIPTION: This is returned if any of the framebuffer attachment points are \"framebuffer incomplete\"." << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout << "\tFBO STATUS: INCOMPLETE MISSING ATTACHMENT" << std::endl;
            std::cout << "\tDESCRIPTION: This is returned if the framebuffer does not have at least one image attached to it." << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "\tFBO STATUS: INCOMPLETE DRAW BUFFER" << std::endl;
            std::cout << "\tDESCRIPTION: This is returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi." << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "\tFBO STATUS: INCOMPLETE READ_BUFFER" << std::endl;
            std::cout << "\tDESCRIPTION: This is returned if GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER." << std::endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "\tFBO STATUS: UNSUPPORTED" << std::endl;
            std::cout << "\ttDESCRIPTION: This is returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions. " << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "\tFBO STATUS: INCOMPLETE MULTISAMPLE" << std::endl;
            std::cout << "\ttDESCRIPTION: This is returned if the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. This is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures." << std::endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            std::cout << "\tFBO STATUS: INCOMPLETE LAYER TARGETS" << std::endl;
            std::cout << "\ttDESCRIPTION: This is returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target." << std::endl;
            break;
        }
        return 0;
    }
    return 1;
}
