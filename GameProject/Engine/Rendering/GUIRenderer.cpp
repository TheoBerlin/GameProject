#include "GUIRenderer.h"

#include "../GUI/FontManager.h"
#include "glm/gtc/matrix_transform.hpp"

#include "../GUI/GUI.h"

GUIRenderer::GUIRenderer()
{
	this->textShader = new Shader("./Engine/Rendering/Shaders/TexShader.vert", "./Engine/Rendering/Shaders/TexShader.frag");
	this->panelShader = new Shader("./Engine/Rendering/Shaders/PanelShader.vert", "./Engine/Rendering/Shaders/PanelShader.frag");
	initTextRendering();
	EventBus::get().subscribe(this, &GUIRenderer::resizeCallback);
	EventBus::get().subscribe(this, &GUIRenderer::hideKeyCallback);
}

GUIRenderer::~GUIRenderer()
{
	delete this->vaFullQuad;
	delete this->vaQuad;
	delete this->textShader;
	delete this->panelShader;
	delete this->whiteOnePixTexture;

	EventBus::get().unsubscribe(this, &GUIRenderer::resizeCallback);
	EventBus::get().unsubscribe(this, &GUIRenderer::hideKeyCallback);
}

void GUIRenderer::draw(GUI & gui)
{
	prepareTextRendering();
	std::vector<Panel*>& panelList = gui.getPanelList();
	if (!this->hidden)
	{
		for (Panel* panel : panelList)
		{
			// Rebake panels
			panel->rebake();

			// Draw baked texture.
			this->orthoText = this->orthoDisplay;
			drawBaked(panel);
		}
	}
}

void GUIRenderer::bakeText(Text & text)
{
	// Update framebuffer dimension.
	fb.updateDimensions(0, (GLuint)text.getWidth(), (GLuint)text.getHeight());
	this->orthoText = glm::ortho(0.0f, (float)text.getWidth(), 0.0f, (float)text.getHeight());

	fb.bind();

	// Clear with transparent color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw to the texture.
	drawToBake(text);

	fb.unbind();
	// Update texture.
	text.setBakedTexture(*fb.getColorTexture(0));

	// Reset viewport.
	Display& display = Display::get();
	glViewport(0, 0, display.getWidth(), display.getHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawBaked(Text & text, float x, float y)
{
	this->textShader->bind();
	this->textShader->setUniformMatrix4fv("ortho", 1, false, &(this->orthoText[0][0]));
	this->textShader->setTexture2D("tex", 0, *text.getBakedTexture());
	this->textShader->setUniform2f("pos", x, y);
	this->textShader->setUniform2f("scale", (float)text.getWidth(), (float)text.getHeight());
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->textShader->unbind();
}

void GUIRenderer::prepareTextRendering()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GUIRenderer::bakePanel(Panel * panel)
{
	// Bake sub-panels and their text.
	std::vector<Panel*>& children = panel->getChildren();
	for (Panel* child : children)
		child->rebake();

	// Bake the text.
	for (auto& element : panel->getTextList())
		element.first->rebake();

	// Update framebuffer dimensions.
	this->orthoText = glm::ortho(0.0f, (float)panel->getSize().x, 0.0f, (float)panel->getSize().y);
	fb.updateDimensions(0, (GLuint)panel->getSize().x, (GLuint)panel->getSize().y);

	fb.bind();

	// Clear with a transparent color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render current panel
	this->panelShader->bind();
	this->panelShader->setUniform2f("pos", 0.0f, 0.0f);
	this->panelShader->setUniform2f("size", (float)panel->getSize().x, (float)panel->getSize().y);
	this->panelShader->setUniform4f("color", panel->getColor().x, panel->getColor().y, panel->getColor().z, panel->getColor().w);
	this->panelShader->setUniformMatrix4fv("ortho", 1, false, &(this->orthoText[0][0]));

	// If no texture was found => set to a 1x1 white texture.
	Texture* texture = panel->getBackgroundTexture();
	if (texture == nullptr)
		texture = this->whiteOnePixTexture;
	this->panelShader->setTexture2D("tex", 0, *texture);

	// Draw panel.
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw text.
	for (auto& element : panel->getTextList())
	{
		element.first->getBakedTexture()->bind();
		drawBaked(*element.first, (float)element.second.x, (float)element.second.y);
	}

	// Draw sub-panels baked texture.
	for (Panel* child : children)
		drawBaked(child);

	fb.unbind();
	panel->setBakedTexture(*fb.getColorTexture(0));

	// Reset viewport.
	Display& display = Display::get();
	glViewport(0, 0, (GLsizei)display.getWidth(), (GLsizei)display.getHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawBaked(Panel * panel)
{
	float x = (float)panel->getPosition().x;
	float y = (float)panel->getPosition().y;

	this->textShader->bind();
	this->textShader->setUniformMatrix4fv("ortho", 1, false, &(this->orthoText[0][0]));
	this->textShader->setTexture2D("tex", 0, *panel->getBakedTexture());
	this->textShader->setUniform2f("pos", x, y);
	this->textShader->setUniform2f("scale", (float)panel->getSize().x, (float)panel->getSize().y);
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->textShader->unbind();
}

void GUIRenderer::drawToBake(Text & text)
{
	Font* font = text.getFont();

	float y = (float)text.getHeight() - (float)text.getBearingY();
	
	font->getShader()->bind();
	font->getShader()->setUniformMatrix4fv("ortho", 1, false, &(this->orthoText[0][0]));

	// Go thorugh all characters in text and draw them.
	std::vector<Text::CharacterDrawData> charactersData = text.getCharactersDrawData();
	for (unsigned int i = 0; i < charactersData.size(); i++)
	{
		Text::CharacterDrawData character = charactersData[i];

		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, character.textureID);
		font->getShader()->setUniform2f("scale", (float)character.size.x, (float)character.size.y);
		font->getShader()->setUniform2f("pos", (float)character.pos.x, (float)character.pos.y + y);

		this->vaFullQuad->bind();
		this->vbFullQuad->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void GUIRenderer::initTextRendering()
{
	// Vao for each character
	this->vaQuad = new VertexArray();
	GLfloat quad[4][4] = {
			{ 0.0f, 1.0f, 0, 0 }, // TL
			{ 0.0f, 0.0f, 0, 1 }, // BL
			{ 1.0f, 1.0f, 1, 0 }, // TR
			{ 1.0f, 0.0f, 1, 1 }, // BR
	};
	this->vbQuad = new VertexBuffer(quad, sizeof(quad), GL_STATIC_DRAW);
	AttributeLayout layout;
	layout.push(4); // [vec4] Position and uv.
	this->vaQuad->addBuffer(vbQuad, layout);
	
	// Vao for baked texture.
	this->vaFullQuad = new VertexArray();
	GLfloat fullQuad[4][4] = {
			{ 0.0f, 1.0f, 0, 0 }, // TL
			{ 0.0f, 0.0f, 0, 1 }, // BL
			{ 1.0f, 1.0f, 1, 0 }, // TR
			{ 1.0f, 0.0f, 1, 1 }, // BR
	};
	this->vbFullQuad = new VertexBuffer(fullQuad, sizeof(fullQuad), GL_STATIC_DRAW);
	this->vaFullQuad->addBuffer(vbFullQuad, layout);
	
	Display& display = Display::get();
	this->fb.attachTexture(display.getWidth(), display.getHeight(), AttachmentType::COLOR);

	std::vector<unsigned char> data = { 0xFF, 0xFF, 0xFF, 0xFF };
	this->whiteOnePixTexture = new Texture(data.data(), 1, 1);

	this->orthoDisplay = glm::ortho(0.0f, (float)display.getWidth(), 0.0f, (float)display.getHeight());
	this->orthoText = this->orthoDisplay;
}

void GUIRenderer::resizeCallback(WindowResizeEvent * evnt)
{
	this->orthoDisplay = glm::ortho(0.0f, (float)evnt->width, 0.0f, (float)evnt->height);
}

void GUIRenderer::hideKeyCallback(KeyEvent * evnt)
{
	if (evnt->action == GLFW_PRESS && evnt->key == GLFW_KEY_F10)
	{
		this->hidden = !this->hidden;
	}
}
