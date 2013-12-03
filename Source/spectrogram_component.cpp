/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "spectrogram_component.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
spectrogram_component::spectrogram_component ()
{
    addAndMakeVisible (fft_size_selector = new ComboBox ("fft size controller"));
    fft_size_selector->setTooltip ("size of FFT");
    fft_size_selector->setEditableText (false);
    fft_size_selector->setJustificationType (Justification::centredLeft);
    fft_size_selector->setTextWhenNothingSelected (String::empty);
    fft_size_selector->setTextWhenNoChoicesAvailable (String::empty);
    fft_size_selector->addItem ("128", 1);
    fft_size_selector->addItem ("256", 2);
    fft_size_selector->addItem ("512", 3);
    fft_size_selector->addItem ("1024", 4);
    fft_size_selector->addItem ("2048", 5);
    fft_size_selector->addItem ("4096", 6);
    fft_size_selector->addItem ("8192", 7);
    fft_size_selector->addItem ("16384", 8);
    fft_size_selector->addItem ("32768", 9);
    fft_size_selector->addItem ("65536", 10);
    fft_size_selector->addListener (this);

    addAndMakeVisible (fft_overlap_slider = new Slider ("fft overlap controller"));
    fft_overlap_slider->setTooltip ("overlap of FFT");
    fft_overlap_slider->setRange (0, 0.99, 0.01);
    fft_overlap_slider->setSliderStyle (Slider::LinearHorizontal);
    fft_overlap_slider->setTextBoxStyle (Slider::TextBoxLeft, false, 40, 20);
    fft_overlap_slider->addListener (this);

    addAndMakeVisible (fft_window_selector = new ComboBox ("fft window controller"));
    fft_window_selector->setTooltip ("window for FFT");
    fft_window_selector->setEditableText (false);
    fft_window_selector->setJustificationType (Justification::centredLeft);
    fft_window_selector->setTextWhenNothingSelected (String::empty);
    fft_window_selector->setTextWhenNoChoicesAvailable (String::empty);
    fft_window_selector->addItem ("rectangle", 1);
    fft_window_selector->addItem ("hanning", 2);
    fft_window_selector->addListener (this);

    addAndMakeVisible (fft_size_label = new Label ("fft size label",
                                                   "FFT size"));
    fft_size_label->setFont (Font (25.00f, Font::plain));
    fft_size_label->setJustificationType (Justification::centred);
    fft_size_label->setEditable (false, false, false);
    fft_size_label->setColour (Label::textColourId, Colours::white);
    fft_size_label->setColour (TextEditor::textColourId, Colours::black);
    fft_size_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (fft_overlap_label = new Label ("fft overlap label",
                                                      "FFT overlap"));
    fft_overlap_label->setFont (Font (25.00f, Font::plain));
    fft_overlap_label->setJustificationType (Justification::centred);
    fft_overlap_label->setEditable (false, false, false);
    fft_overlap_label->setColour (Label::textColourId, Colours::white);
    fft_overlap_label->setColour (TextEditor::textColourId, Colours::black);
    fft_overlap_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (fft_window_label = new Label ("fft window label",
                                                     "FFT window"));
    fft_window_label->setFont (Font (25.00f, Font::plain));
    fft_window_label->setJustificationType (Justification::centred);
    fft_window_label->setEditable (false, false, false);
    fft_window_label->setColour (Label::textColourId, Colours::white);
    fft_window_label->setColour (TextEditor::textColourId, Colours::black);
    fft_window_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (open_gl_gui_component = new open_gl_component());
    open_gl_gui_component->setName ("openGL component");


    //[UserPreSize]
    fft_size_selector->setSelectedId(4);
    fft_window_selector->setSelectedId(1);
	comboBoxChanged(fft_size_selector);
	sliderValueChanged(fft_overlap_slider);
	comboBoxChanged(fft_window_selector);
    //[/UserPreSize]

    setSize (600, 600);

    //[Constructor] You can add your own custom stuff here...
	open_gl_gui_component->init_fft_params(fft_size, fft_overlap, fft_window_type);
#ifdef _WIN32
    open_gl_gui_component->set_wav_file("C:\\Users\\Chris Donahue\\Code\\opengl_spectrogram\\test_sound.wav");
    //open_gl_gui_component->set_wav_file("C:\\Code\\opengl_spectrogram\\test_sound.wav");
	//set_wav_file("D:\\My Code\\opengl_spectrogram\\test_sound.wav");
#else
    open_gl_gui_component->set_wav_file("../../../test_sound.wav");
#endif
    //[/Constructor]
}

spectrogram_component::~spectrogram_component()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fft_size_selector = nullptr;
    fft_overlap_slider = nullptr;
    fft_window_selector = nullptr;
    fft_size_label = nullptr;
    fft_overlap_label = nullptr;
    fft_window_label = nullptr;
    open_gl_gui_component = nullptr;

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void spectrogram_component::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::black);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void spectrogram_component::resized()
{
    fft_size_selector->setBounds (20, 50, 160, 24);
    fft_overlap_slider->setBounds (220, 50, 160, 24);
    fft_window_selector->setBounds (420, 50, 160, 24);
    fft_size_label->setBounds (20, 10, 160, 30);
    fft_overlap_label->setBounds (220, 10, 160, 30);
    fft_window_label->setBounds (420, 10, 160, 30);
    open_gl_gui_component->setBounds (20, 90, 560, 490);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void spectrogram_component::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == fft_size_selector)
    {
        //[UserComboBoxCode_fft_size_selector] -- add your combo box handling code here..
		fft_size = fft_size_selector->getText().getIntValue();
		sliderValueChanged(fft_overlap_slider);
        //[/UserComboBoxCode_fft_size_selector]
    }
    else if (comboBoxThatHasChanged == fft_window_selector)
    {
        //[UserComboBoxCode_fft_window_selector] -- add your combo box handling code here..
		fft_window_type = fft_window_selector->getText().toStdString();
        //[/UserComboBoxCode_fft_window_selector]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void spectrogram_component::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == fft_overlap_slider)
    {
        //[UserSliderCode_fft_overlap_slider] -- add your slider handling code here..
		fft_overlap = (int) (double(fft_size) * fft_overlap_slider->getValue());
        //[/UserSliderCode_fft_overlap_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
int spectrogram_component::get_fft_size() {
	return fft_size;
}

int spectrogram_component::get_fft_overlap() {
	return fft_overlap;
}

std::string spectrogram_component::get_fft_window_type() {
	return fft_window_type;
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="spectrogram_component" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="10" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="600">
  <BACKGROUND backgroundColour="ff000000"/>
  <COMBOBOX name="fft size controller" id="8b5f726db5377900" memberName="fft_size_selector"
            virtualName="" explicitFocusOrder="0" pos="20 50 160 24" tooltip="size of FFT"
            editable="0" layout="33" items="128&#10;256&#10;512&#10;1024&#10;2048&#10;4096&#10;8192&#10;16384&#10;32768&#10;65536"
            textWhenNonSelected="" textWhenNoItems=""/>
  <SLIDER name="fft overlap controller" id="f26bf579df409470" memberName="fft_overlap_slider"
          virtualName="" explicitFocusOrder="0" pos="220 50 160 24" tooltip="overlap of FFT"
          min="0" max="0.98999999999999999" int="0.01" style="LinearHorizontal"
          textBoxPos="TextBoxLeft" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1"/>
  <COMBOBOX name="fft window controller" id="2c40026d72071fac" memberName="fft_window_selector"
            virtualName="" explicitFocusOrder="0" pos="420 50 160 24" tooltip="window for FFT"
            editable="0" layout="33" items="rectangle&#10;hanning" textWhenNonSelected=""
            textWhenNoItems=""/>
  <LABEL name="fft size label" id="fca8eb586a31e581" memberName="fft_size_label"
         virtualName="" explicitFocusOrder="0" pos="20 10 160 30" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="FFT size" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="25" bold="0" italic="0" justification="36"/>
  <LABEL name="fft overlap label" id="b9dd683e1bfc7df5" memberName="fft_overlap_label"
         virtualName="" explicitFocusOrder="0" pos="220 10 160 30" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="FFT overlap" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="25" bold="0" italic="0" justification="36"/>
  <LABEL name="fft window label" id="1373ae7616ea96f9" memberName="fft_window_label"
         virtualName="" explicitFocusOrder="0" pos="420 10 160 30" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="FFT window" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="25" bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="openGL component" id="a0e9e5de66c56148" memberName="open_gl_gui_component"
                    virtualName="" explicitFocusOrder="0" pos="20 90 560 490" class="open_gl_component"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
