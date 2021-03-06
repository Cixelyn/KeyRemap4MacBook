#ifndef KEYCODE_HPP
#define KEYCODE_HPP

namespace org_pqrs_KeyRemap4MacBook {
  class KeyCode;
  class Flags;
  class Buttons;

  // ======================================================================
  class EventType {
  public:
    EventType(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(EventType other) const { return value_ == other.get(); }
    bool operator!=(EventType other) const { return ! (*this == other); }

    bool isKeyDownOrModifierDown(KeyCode key, Flags flags) const;

#include "../bridge/keycode/output/include.EventType.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class KeyboardType {
  public:
    KeyboardType(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(KeyboardType other) const { return value_ == other.get(); }
    bool operator!=(KeyboardType other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.KeyboardType.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class ModifierFlag {
  public:
    unsigned int get(void) const { return value_; }
    bool operator==(ModifierFlag other) const { return value_ == other.get(); }
    bool operator!=(ModifierFlag other) const { return ! (*this == other); }

    unsigned int operator~(void) const { return ~value_; }

    KeyCode getKeyCode(void) const;

#include "../bridge/keycode/output/include.ModifierFlag.hpp"

  private:
    ModifierFlag(unsigned int v) : value_(v) {}
    unsigned int value_;
  };
  class Flags {
  public:
    Flags(unsigned int v = 0) : value_(v) {}
    Flags(ModifierFlag v) : value_(v.get()) {}
    unsigned int get(void) const { return value_; }
    bool operator==(Flags other) const { return value_ == other.get(); }
    bool operator!=(Flags other) const { return ! (*this == other); }

    unsigned int operator~(void) const { return ~value_; }
    Flags operator|(Flags other) const { return value_ | other.get(); }
    Flags operator&(Flags other) const { return value_ & other.get(); }

    static ModifierFlag getModifierFlagByIndex(unsigned int index) {
      switch (index) {
        case 0:  return ModifierFlag::CAPSLOCK;
        case 1:  return ModifierFlag::SHIFT_L;
        case 2:  return ModifierFlag::SHIFT_R;
        case 3:  return ModifierFlag::CONTROL_L;
        case 4:  return ModifierFlag::CONTROL_R;
        case 5:  return ModifierFlag::OPTION_L;
        case 6:  return ModifierFlag::OPTION_R;
        case 7:  return ModifierFlag::COMMAND_L;
        case 8:  return ModifierFlag::COMMAND_R;
        case 9:  return ModifierFlag::CURSOR;
        //ModifierFlag::KEYPAD, // skip KEYPAD because CURSOR == KEYPAD.
        case 10: return ModifierFlag::FN;
        case 11: return ModifierFlag::EXTRA1;
        case 12: return ModifierFlag::EXTRA2;
        case 13: return ModifierFlag::EXTRA3;
        case 14: return ModifierFlag::EXTRA4;
        case 15: return ModifierFlag::EXTRA5;
        // Note: ModifierFlag::NONE must be a last item.
        default: return ModifierFlag::NONE;
      };
    }

    Flags& add(Flags flags) { value_ |= flags.get(); return *this; }
    Flags& remove(Flags flags) {
      // We consider the following case.
      //   (ModifierFlag::SHIFT_L | ModifierFlag::SHIFT_R).remove(ModifierFlag::SHIFT_L).
      //
      // The value of SHIFT_L and SHIFT_R is below.
      //
      // ModifierFlag::SHIFT_L : 0x20002
      // ModifierFlag::SHIFT_R : 0x20004
      //
      // So, the correct value of above case is 0x20004 (SHIFT_R).
      //
      // If we remove bits simple way (value_ &= ~flags),
      // the result value becomes 0x00004. It's not right.
      //
      // Therefore, we save the old value, and restore the necessary bits from it.
      //
      Flags old = *this;

      value_ &= ~flags;

      for (unsigned int i = 0;; ++i) {
        ModifierFlag f = Flags::getModifierFlagByIndex(i);

        if (! flags.isOn(f) && old.isOn(f)) {
          value_ |= f.get();
        }

        if (f == ModifierFlag::NONE) break;
      }

      return *this;
    }
    Flags& stripFN(void)     { return remove(ModifierFlag::FN); }
    Flags& stripCURSOR(void) { return remove(ModifierFlag::CURSOR); }
    Flags& stripKEYPAD(void) { return remove(ModifierFlag::KEYPAD); }
    Flags& stripNONE(void)   { return remove(ModifierFlag::NONE); }
    Flags& stripEXTRA(void) {
      return remove(Flags(ModifierFlag::EXTRA1) |
                    Flags(ModifierFlag::EXTRA2) |
                    Flags(ModifierFlag::EXTRA3) |
                    Flags(ModifierFlag::EXTRA4) |
                    Flags(ModifierFlag::EXTRA5));
    }

    bool isOn(ModifierFlag flag) const {
      return (value_ & flag.get()) == flag.get();
    }
    bool isOn(Flags flags) const {
      if (flags.isOn(ModifierFlag::NONE)) {
        return (value_ | ModifierFlag::NONE.get()) == flags.get();
      } else {
        return (value_ & flags.get()) == flags.get();
      }
    }

    bool isVirtualModifiersOn(void) const {
      if (isOn(ModifierFlag::NONE)) return true;
      if (isOn(ModifierFlag::EXTRA1)) return true;
      if (isOn(ModifierFlag::EXTRA2)) return true;
      if (isOn(ModifierFlag::EXTRA3)) return true;
      if (isOn(ModifierFlag::EXTRA4)) return true;
      if (isOn(ModifierFlag::EXTRA5)) return true;
      return false;
    }

  private:
    unsigned int value_;
  };
  inline Flags operator|(ModifierFlag lhs, ModifierFlag rhs) { return lhs.get() | rhs.get(); }

  // ======================================================================
  class KeyCode {
  public:
    KeyCode(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(KeyCode other) const { return value_ == other.get(); }
    bool operator!=(KeyCode other) const { return ! (*this == other); }

    bool operator>(KeyCode other) const { return value_ > other.get(); }
    bool operator>=(KeyCode other) const { return value_ >= other.get(); }

    static void normalizeKey(KeyCode& key, Flags& flags, EventType eventType, KeyboardType keyboardType);
    static void reverseNormalizeKey(KeyCode& key, Flags& flags, EventType eventType, KeyboardType keyboardType);

    ModifierFlag getModifierFlag(void) const;
    bool isModifier(void) const { return getModifierFlag() != ModifierFlag::NONE; }

#include "../bridge/keycode/output/include.KeyCode.hpp"

    // When FN key and Arrow key were pushed together, another key code was sent (Home,End,PageUp,PageDown or something).
    // We need to change these Home,End,PageUp,PageDown keys to FN+Arrow key before sending key code to remapper.
    // (If we change FN to Control_L, FN+Up-Arrow must be changed to Control_L+Up-Arrow. Not Control_L+PageUp).
    // We also need to change FN+Arrow to Home,End,PageUp,PageDown before outputting key code.
    //
    // This class handles the above.
    class FNKeyHack {
    public:
      FNKeyHack(const KeyCode& fk, const KeyCode& tk) : fromKeyCode_(fk), toKeyCode_(tk), active_normalize_(false), active_reverse_(false) {}
      // FN+PageUp to FN+Up-Arrow
      bool normalize(KeyCode& key, Flags flags, EventType eventType) { return remap(key, flags, eventType, active_normalize_, fromKeyCode_, toKeyCode_); }
      // FN+Up-Arrow to PageUp
      bool reverse(KeyCode& key, Flags flags, EventType eventType) { return remap(key, flags, eventType, active_reverse_, toKeyCode_, fromKeyCode_); }

    private:
      bool remap(KeyCode& key, Flags flags, EventType eventType, bool& active, KeyCode fromKeyCode, KeyCode toKeyCode);

      const KeyCode& fromKeyCode_;
      const KeyCode& toKeyCode_;
      bool active_normalize_;
      bool active_reverse_;
    };

  private:
    unsigned int value_;
  };

  class CharCode {
  public:
    CharCode(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(CharCode other) const { return value_ == other.get(); }
    bool operator!=(CharCode other) const { return ! (*this == other); }

  private:
    unsigned int value_;
  };
  class CharSet {
  public:
    CharSet(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(CharSet other) const { return value_ == other.get(); }
    bool operator!=(CharSet other) const { return ! (*this == other); }

  private:
    unsigned int value_;
  };
  class OrigCharCode {
  public:
    OrigCharCode(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(OrigCharCode other) const { return value_ == other.get(); }
    bool operator!=(OrigCharCode other) const { return ! (*this == other); }

  private:
    unsigned int value_;
  };
  class OrigCharSet {
  public:
    OrigCharSet(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(OrigCharSet other) const { return value_ == other.get(); }
    bool operator!=(OrigCharSet other) const { return ! (*this == other); }

  private:
    unsigned int value_;
  };

  // ======================================================================
  class ConsumerKeyCode {
  public:
    ConsumerKeyCode(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(ConsumerKeyCode other) const { return value_ == other.get(); }
    bool operator!=(ConsumerKeyCode other) const { return ! (*this == other); }

    bool operator>(ConsumerKeyCode other) const { return value_ > other.get(); }
    bool operator>=(ConsumerKeyCode other) const { return value_ >= other.get(); }

    bool isRepeatable(void) const;

#include "../bridge/keycode/output/include.ConsumerKeyCode.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class PointingButton {
  public:
    PointingButton(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(PointingButton other) const { return value_ == other.get(); }
    bool operator!=(PointingButton other) const { return ! (*this == other); }

    unsigned int operator~(void) const { return ~value_; }

#include "../bridge/keycode/output/include.PointingButton.hpp"

  private:
    unsigned int value_;
  };
  class Buttons {
  public:
    Buttons(unsigned int v = 0) : value_(v) {}
    Buttons(PointingButton v) : value_(v.get()) {}
    unsigned int get(void) const { return value_; }
    bool operator==(Buttons other) const { return value_ == other.get(); }
    bool operator!=(Buttons other) const { return ! (*this == other); }

    unsigned int operator~(void) const { return ~value_; }
    Buttons operator|(Buttons other) const { return value_ | other.get(); }

    Buttons& add(Buttons button) { value_ |= button.get(); return *this; }
    Buttons& remove(Buttons button) { value_ &= ~button; return *this; }

    bool isNONE(void) const { return value_ == 0; }
    bool isOn(Buttons buttons) const {
      return (value_ & buttons.get()) == buttons.get();
    }

    Buttons justPressed(Buttons previous) const {
      return value_ & ~(previous.get());
    }
    Buttons justReleased(Buttons previous) const {
      return ~value_ & (previous.get());
    }

    // population count
    unsigned int count(void) const {
      unsigned int bits = value_;

      bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
      bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
      bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
      bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
      return (bits & 0x0000ffff) + (bits >> 16 & 0x0000ffff);
    }

  private:
    unsigned int value_;
  };
  inline Buttons operator|(PointingButton lhs, PointingButton rhs) { return lhs.get() | rhs.get(); }

  // ======================================================================
  class Option {
  public:
    Option(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(Option other) const { return value_ == other.get(); }
    bool operator!=(Option other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.Option.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class ApplicationType {
  public:
    ApplicationType(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(ApplicationType other) const { return value_ == other.get(); }
    bool operator!=(ApplicationType other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.ApplicationType.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class InputMode {
  public:
    InputMode(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(InputMode other) const { return value_ == other.get(); }
    bool operator!=(InputMode other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.InputMode.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class InputModeDetail {
  public:
    InputModeDetail(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(InputModeDetail other) const { return value_ == other.get(); }
    bool operator!=(InputModeDetail other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.InputModeDetail.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class DeviceVendor {
  public:
    DeviceVendor(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(DeviceVendor other) const { return value_ == other.get(); }
    bool operator!=(DeviceVendor other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.DeviceVendor.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  class DeviceProduct {
  public:
    DeviceProduct(unsigned int v = 0) : value_(v) {}
    unsigned int get(void) const { return value_; }
    bool operator==(DeviceProduct other) const { return value_ == other.get(); }
    bool operator!=(DeviceProduct other) const { return ! (*this == other); }

#include "../bridge/keycode/output/include.DeviceProduct.hpp"

  private:
    unsigned int value_;
  };

  // ======================================================================
  namespace DeviceType {
    enum DeviceType {
      UNKNOWN,

      APPLE_INTERNAL,
      APPLE_EXTERNAL,

      USB_OVERDRIVE,
    };
  }
}

#endif
