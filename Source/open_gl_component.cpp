/*
  ==============================================================================

    open_gl_component.cpp
    Created: 2 Dec 2013 2:04:27pm
    Author:  Chris

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "open_gl_component.h"

//==============================================================================
open_gl_component::open_gl_component()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	// openGL init
	open_gl_context.setRenderer(this);
	open_gl_context.attachTo(*this);
	open_gl_context.setContinuousRepainting(true);
}

open_gl_component::~open_gl_component()
{
}

void open_gl_component::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colours::white);   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::lightblue);
    g.setFont (14.0f);
    g.drawText ("open_gl_component", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void open_gl_component::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


void open_gl_component::newOpenGLContextCreated() {
}

void open_gl_component::renderOpenGL() {
    jassert (OpenGLHelpers::isContextActive());

    const float desktopScale = (float) open_gl_context.getRenderingScale();
    OpenGLHelpers::clear (Colours::lightblue);
}

void open_gl_component::openGLContextClosing() {
}