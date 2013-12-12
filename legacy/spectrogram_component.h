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
#include <stdlib.h>

#include "JuceHeader.h"

#include "open_gl_component.h"
#include "audio_util.h"

class open_gl_component;
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
                               public SliderListener,
                               public ButtonListener
{
public:
    //==============================================================================
    spectrogram_component ();
    ~spectrogram_component();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	int get_fft_size();
	int get_fft_overlap();
	std::string get_fft_window_type();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);

    // Binary resources:
    static const char* button_play_png;
    static const int button_play_pngSize;
    static const char* button_pause_png;
    static const int button_pause_pngSize;
    static const char* button_stop_png;
    static const int button_stop_pngSize;
    static const char* button_hover_play_png;
    static const int button_hover_play_pngSize;
    static const char* button_hover_pause_png;
    static const int button_hover_pause_pngSize;
    static const char* button_hover_stop_png;
    static const int button_hover_stop_pngSize;
    static const char* button_down_play_png;
    static const int button_down_play_pngSize;
    static const char* button_down_pause_png;
    static const int button_down_pause_pngSize;
    static const char* button_down_stop_png;
    static const int button_down_stop_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	int fft_size;
	int fft_overlap;
	std::string fft_window_type;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ComboBox> fft_size_selector;
    ScopedPointer<Slider> fft_overlap_slider;
    ScopedPointer<ComboBox> fft_window_selector;
    ScopedPointer<Label> fft_size_label;
    ScopedPointer<Label> fft_overlap_label;
    ScopedPointer<Label> fft_window_label;
    ScopedPointer<open_gl_component> open_gl_gui_component;
    ScopedPointer<ComboBox> shader_selector;
    ScopedPointer<Label> shader_label;
    ScopedPointer<Label> audio_controls_label;
    ScopedPointer<TextButton> load_button;
    ScopedPointer<ImageButton> play_button;
    ScopedPointer<ImageButton> pause_button;
    ScopedPointer<ImageButton> stop_button;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (spectrogram_component)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_C912F8B9457FE626__
