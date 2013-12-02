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

#ifndef __JUCE_HEADER_C912F8B9457FE626__
#define __JUCE_HEADER_C912F8B9457FE626__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "audio_util.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class spectrogram_component  : public Component,
                               public ComboBoxListener,
                               public SliderListener
{
public:
    //==============================================================================
    spectrogram_component ();
    ~spectrogram_component();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void compute_fft();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    audio_util::wav_data* wav_file;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ComboBox> fft_size_selector;
    ScopedPointer<Slider> fft_overlap_slider;
    ScopedPointer<ComboBox> fft_window_selector;
    ScopedPointer<Label> fft_size_label;
    ScopedPointer<Label> fft_overlap_label;
    ScopedPointer<Label> fft_window_label;
    ScopedPointer<Component> openGL_component;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (spectrogram_component)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_C912F8B9457FE626__
