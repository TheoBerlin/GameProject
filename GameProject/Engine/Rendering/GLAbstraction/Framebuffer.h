#pragma once
class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void attachTexture();
	void attachRenderBuffer();

	void setTexture();
	void getTexture(unsigned index) const;
	void getTextures() const;

	void bind() const;
	void unbind() const;

private:
	unsigned id;

};
