/************************************************************************
 IMPORTANT NOTE : this file contains two clearly delimited sections :
 the ARCHITECTURE section (in two parts) and the USER section. Each section
 is governed by its own copyright and license. Please check individually
 each section for license and copyright information.
 *************************************************************************/

/******************* BEGIN faust-simpleclass.cpp ****************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2019 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.

 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.

 ************************************************************************
 ************************************************************************/

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(SOUNDFILE)
#include "faust/gui/SoundUI.h"
#endif

using std::max;
using std::min;

/******************************************************************************
*******************************************************************************

 DEFINE BARE-BONES VIRTUAL CLASSES

*******************************************************************************
*******************************************************************************/

struct Meta {
  void declare(const char *, const char *) {}
};


template <typename FAUSTFLOAT>
class UIClass {

public:
  enum ControlType {
    BUTTON,
    CHECK,
    VSLIDER,
    HSLIDER,
    NUMENTRY,
    HBAR,
    VBAR,
    SOUNDFILE,
    NUM_CONROL_TYPES
  };

  struct Control {
    ControlType type;
    FAUSTFLOAT *ptr;
    FAUSTFLOAT init;
  };

  UIClass() {}
  ~UIClass() {}

  std::string make_control_name(const std::vector<std::string> &v,
                                const char *label) {
    std::string result;
    for (size_t i = 0; i < v.size(); ++i) {
      result += v[i];
      result += "::";
    }

    result += label;
    return result;
  }

  // -- widget's layouts
  void openTabBox(const char *label) {
    _label_context.push_back(std::string(label));
  }

  void openHorizontalBox(const char *label) {
    _label_context.push_back(std::string(label));
  }

  void openVerticalBox(const char *label) {
    _label_context.push_back(std::string(label));
  }

  void closeBox() { _label_context.pop_back(); }

  // -- active widgets
  void addButton(const char *label, FAUSTFLOAT *zone) {
    _controls.insert(
        {make_control_name(_label_context, label), Control{BUTTON, zone, 0}});
  }

  void addCheckButton(const char *label, FAUSTFLOAT *zone) {
    _controls.insert(
        {make_control_name(_label_context, label), Control{CHECK, zone, 0}});
  }

  void addVerticalSlider(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init,
                         FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/, FAUSTFLOAT /*step*/) {
    _controls.insert({make_control_name(_label_context, label),
                      Control{VSLIDER, zone, init}});
  }

  void addHorizontalSlider(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init,
                           FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/, FAUSTFLOAT /*step*/) {
    _controls.insert({make_control_name(_label_context, label),
                      Control{HSLIDER, zone, init}});
  }

  void addNumEntry(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT init,
                   FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/, FAUSTFLOAT /*step*/) {
    _controls.insert({make_control_name(_label_context, label),
                      Control{NUMENTRY, zone, init}});
  }

  // passive widgets
  void addHorizontalBargraph(const char *label, FAUSTFLOAT *zone,
                             FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/) {
    _controls.insert(
        {make_control_name(_label_context, label), Control{HBAR, zone, 0}});
  }

  void addVerticalBargraph(const char *label, FAUSTFLOAT *zone, FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/) {
    _controls.insert(
        {make_control_name(_label_context, label), Control{VBAR, zone, 0}});
  }

  // soundfiles
#if defined(SOUNDFILE)
  void addSoundfile(const char *label, const char *filename,
                    Soundfile **sf_zone) {};
#endif
  // metadata declarations

  void declare(FAUSTFLOAT * /*zone*/, const char * /*key*/,
               const char * /*val*/) {}

  /*
   *
   * Parameter controls
   * These functions must NOT be run at the same time as process(), OK?
   */

  // (re)initialise all parameter values
  void initControls() {
    for (const auto &[name, control] : _controls) {
      *control.ptr = control.init;
    }
  }

  bool setControl(const char *label, FAUSTFLOAT value) {
    if (_controls.contains(std::string(label))) {
      *_controls[std::string(label)].ptr = value;
      return true;
    }

    return false;
  }

  bool getControl(const char *label, FAUSTFLOAT *value) {
    if (_controls.contains(std::string(label))) {
      *value = *_controls[std::string(label)].ptr;
      return true;
    }

    return false;
  }

  std::unordered_map<std::string, Control> _controls;

private:
  std::vector<std::string> _label_context;
};

// this is because we cannot modify the code that faust produces
#define UI UIClass<FAUSTFLOAT>

/******************************************************************************
 *******************************************************************************

 VECTOR INTRINSICS

**************************************************************************
*******************************************************************************/

<<includeIntrinsic>>

/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

<<includeclass>>

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

/******************************************************************************
*******************************************************************************

 CONTAINER CLASS - USE THIS

*******************************************************************************
*******************************************************************************/
template <typename FAUSTFLOAT>
class FaustProc_mydsp {

private:
  mydsp<FAUSTFLOAT> _dsp;
  UIClass<FAUSTFLOAT> _ui;

public:
  FaustProc_mydsp() {}

  ~FaustProc_mydsp() {}

  void prepare(int fs) {
    _dsp.init(fs);
    _dsp.buildUserInterface(&_ui);
    _ui.initControls();
  }

  // the faust in debian does not support separate control
  // function
  // void process_control() { _dsp.control(); }

  void process_audio(int count, FAUSTFLOAT **RESTRICT inputs,
                     FAUSTFLOAT **RESTRICT outputs) {
    _dsp.compute(count, inputs, outputs);
  }

  int getNumInputs() { return _dsp.getNumInputs(); }

  int getNumOutputs() { return _dsp.getNumOutputs(); }

  int getSampleRate() { return _dsp.getSampleRate(); }

  bool setControl(const char *label, FAUSTFLOAT value) {
    return _ui.setControl(label, value);
  }

  bool getControl(const char *label, FAUSTFLOAT *value) {
    return _ui.getControl(label, value);
  }

  std::vector<std::string> getControlNames() {
    std::vector<std::string> names;

    for (auto &[key, control] : _ui._controls) {
      names.push_back(key);
    }

    return names;
  }
};

/******************* END faust-simpleclass.cpp ****************/
