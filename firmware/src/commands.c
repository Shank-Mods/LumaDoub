#include "commands.h"

const int commands_fake_scannies_on_length = 1;
const device_command_t commands_fake_scannies_on[] = {
  {0x2A, 0x00, 0B10010000, 0}, // Force disable free run mode         // I2C Command to turn off the DACs           INTERRUPT TEST
};

const int commands_fake_scannies_off_length = 1;
const device_command_t commands_fake_scannies_off[] = {
  {0x2A, 0x00, 0B10011100, 0}, // Force disable free run mode         // I2C Command to turn off the DACs           INTERRUPT TEST
};

const int commands_freerun_480i_60Hz_YPbPr_out_length = 17;
const device_command_t commands_freerun_480i_60Hz_YPbPr_out[] = {
  // {0x20, 0x0F, 0x80, 10}, // Reset ADV7280A (TODO: enabling this command kills I2C, why?)
  {0x2A, 0x17, 0x02,       0 }, // Reset ADV7391

  {0x20, 0x0F, 0x00,       10}, // Exit Power Down Mode [ADV7280A writes begin]
  {0x20, 0x02, 0x04,       0 }, // Force standard to NTSC-M  --Changed from 0x54 (force ntsc m) to 0x14 (autodetect) ---changed again to 0x04 to set to non-pedestal input
  {0x20, 0x51, 0xC4,       0 }, // Evaluate horizontal lock using fewer lines
  {0x20, 0x80, 0x51,       0 }, // ADI Required Write       ***Affects reserved bits for ACE. Consider experimenting with disabling*** ***CONSIDER SETTING TO 0X00. THIS IS CONTRAST BOOST, ACE)***
  {0x20, 0x81, 0x51,       0 }, // ADI Required Write       ***Undocumented, within ACE section. Consider experimenting with disabling***
  {0x20, 0x82, 0x68,       0 }, // ADI Required Write       ***Affects Pal. Says "Not Applicable for NTSC"***
  {0x20, 0x17, 0x41,       0 }, // Enable SH1               ***This sets a non-standard "C filter" (SH1), may only affect composite***
  {0x20, 0x03, 0x0C,       0 }, // Enable Pixel & Sync output drivers (they are tri-state by default)
  {0x20, 0x6b, 0b00000001, 0 }, // Forces Vsync output on VS/FIELD/SFL pin
  {0x20, 0x04, 0x0F,       0 }, // Power-up INTRQ, HS & VS pads  ---Sets to BT.656-3 (not-4), some reserved bits, Forces HS and VS/FIELD/SFL pins on, enables SLF, Full range rather than limited. Also affects Betacam voltage
  {0x20, 0x13, 0x00,       0 }, // Enable ADV7280A for 28_63636MHz crystal (undocumented?)
  {0x20, 0x1D, 0x40,       0 }, // Enable LLC output driver (pin is tristate by default)
  {0x20, 0xFD, 0x84,       0 }, // Set VPP Map [ADV7280A writes finished]
  {0x20, 0x0E, 0x20,       0 }, // Enter Interrupt/VDP Sub map
  // {0x20, 0x43, 0x00,       0 }, // The Great LoopJ Purge
  // {0x20, 0x47, 0x00,       0 }, // The Great LoopJ Purge
  // {0x20, 0x4b, 0x00,       0 }, // The Great LoopJ Purge
  {0x20, 0x40, 0b11010000, 0 }, // Set interrupt period to be 63 XTAL periods; drive low when active // TODO: reset to open-drain mode? ---changing for loopj---
  // {0x20, 0x4C, 0x04,       0 }, // Enable HLock interrupt  ---changing for loopj---
  // {0x20, 0x48, 0x00,       0 }, // Enable field change interrupt for testing
  {0x20, 0x0E, 0x00,       0 }, // Leave Interrupt/VDP Sub map, return to sub map 1
  // {0x20, 0xF1, 0b00001101, 0 }, // TEST VALUE FOR RGB INPUT
  // {0x20, 0xED, 0b00010100, 0 }, // TEST VALUE FOR RGB INPUT
};

const int commands_7184_rgb_test_length = 38; //53 stock
const device_command_t commands_7184_rgb_test[] = {
  // standard commands (22 total)
  {0x2A, 0x17, 0x02,       0 }, // Reset ADV7391
  {0x20, 0x0F, 0x00,       10}, // Exit Power Down Mode [ADV7280A writes begin] **MANDATORY**
  {0x20, 0x02, 0b00010100, 0 }, // --was 0x04, gonna heck around with manual-- Force standard to NTSC-M  --Changed from 0x54 (force ntsc m) to 0x14 (autodetect) ---changed again to 0x04 to set to non-pedestal input
  {0x20, 0x51, 0xC4,       0 }, // Evaluate horizontal lock using fewer lines
  {0x20, 0x80, 0x51,       0 }, // ADI Required Write       ***Affects reserved bits for ACE. Consider experimenting with disabling*** ***CONSIDER SETTING TO 0X00. THIS IS CONTRAST BOOST, ACE)***
  {0x20, 0x81, 0x51,       0 }, // ADI Required Write       ***Undocumented, within ACE section. Consider experimenting with disabling***
  {0x20, 0x82, 0x68,       0 }, // ADI Required Write       ***Affects Pal. Says "Not Applicable for NTSC"***
  {0x20, 0x17, 0x41,       0 }, // Enable SH1               ***This sets a non-standard "C filter" (SH1), may only affect composite***
  {0x20, 0x03, 0x0C,       0 }, // Enable Pixel & Sync output drivers (they are tri-state by default) **MANDATORY**
  {0x20, 0x6b, 0b00000001, 0 }, // Forces Vsync output on VS/FIELD/SFL pin
  {0x20, 0x04, 0b01010111, 0 }, // --was 0x0F-- Power-up INTRQ, HS & VS pads  ---Sets to BT.656-3 (not-4), some reserved bits, Forces HS and VS/FIELD/SFL pins on, enables SLF, Full range rather than limited. Also affects Betacam voltage
  {0x20, 0x13, 0x00,       0 }, // Enable ADV7280A for 28_63636MHz crystal (undocumented?)
  {0x20, 0x1D, 0x40,       0 }, // Enable LLC output driver (pin is tristate by default) **MANDATORY**
  {0x20, 0xFD, 0x84,       0 }, // Set VPP Map [ADV7280A writes finished]
  {0x20, 0x0E, 0x20,       0 }, // Enter Interrupt/VDP Sub map
  {0x20, 0x43, 0x00,       0 }, // The Great LoopJ Purge
  {0x20, 0x47, 0x00,       0 }, // The Great LoopJ Purge
  {0x20, 0x4b, 0x00,       0 }, // The Great LoopJ Purge
  {0x20, 0x40, 0b01000111, 0 }, // --was 0xD1-- Set interrupt period to be 63 XTAL periods; drive low when active // TODO: reset to open-drain mode? ---changing for loopj---
  {0x20, 0x4C, 0x04,       0 }, // Enable HLock interrupt  ---changing for loopj---
  {0x20, 0x48, 0x00,       0 }, // Enable field change interrupt for testing
  {0x20, 0x0E, 0x00,       0 }, // Leave Interrupt/VDP Sub map, return to sub map 1

  //Input command (1 total)
  // {0x20, 0x00, 0b00000000, 0 }, // INSEL = YPbPr, Y=Ain1, Pb=Ain2, Pr=Ain3 (broke until I used the correct command lenght number)

  //Extra Stuff im gonna heck around with and find out
  // {0x20, 0x00, 0b00001001, 0 }, //INSEL
  // {0x20, 0x31, 0x02,       0 }, //VS & Field control 1, default
  // {0x20, 0x3A, 0x00,       0 }, //ADC Control, reset value
  // {0x20, 0x3B, 0x71,       0 }, //Bias Control, Weird non-standard values, including reserved
  // {0x20, 0x3D, 0xA2,       0 }, //5//Manual Window Control, color kill, non-reset values **Is this the one breaking it?**
  // {0x20, 0x3E, 0x6A,       0 }, //Undocumented?
  // {0x20, 0x3F, 0xA0,       0 }, //Undocumented?
  // {0x20, 0x67, 0x01,       0 }, //CSC_22 and 422 settings, nonstandard
  // {0x20, 0x73, 0xD0,       0 }, //CP AGC 3, nonstandard
  // {0x20, 0x74, 0x04,       0 }, //10//CP AGC 4, Nonstandard for 7280A, but reset value for 7403
  // {0x20, 0x75, 0x01,       0 }, //CP AGC 5, Nonstandard for 7280A, but reset value for 7403
  // {0x20, 0x76, 0x00,       0 }, //CP AGC 6, reset value
  // {0x20, 0x77, 0x04,       0 }, //CP OFFSET1, nonstandard for either
  // {0x20, 0x78, 0x08,       0 }, //CP OFFSET2, nonstandard for either
  // {0x20, 0x79, 0x02,       0 }, //15//CP OFFSET3, nonstandard for either
  // {0x20, 0x7A, 0x00,       0 }, //CP OFFSET4, Nonstandard for either, It is 0xF8 by default on 7280A. Sus.
  // {0x20, 0x93, 0x78,       0 }, //Undocumented, says clamp related, non-standard for either
  // {0x20, 0x94, 0x23,       0 }, //Undocumented, says clamp related, non-standard for either
  // {0x20, 0x95, 0x11,       0 }, //Undocumented, says clamp related, non-standard for either
  // {0x20, 0x96, 0xC0,       0 }, //20//Undocumented, says clamp related, non-standard for either
  // {0x20, 0xC5, 0x00,       0 }, //Clamp averaging, says clamp mode 0 for fb hc based, non-reset values

  //Fast Blank Control Registers (5 total)
  {0x20, 0xED, 0b00010100, 0 }, //startup script says 0xC4 --this is SUPPOSED to force on RGB--
  {0x20, 0xEE, 0b00000000, 0 }, //Auto CSC enabled (colorspace converter) rather than manual
  {0x20, 0xEF, 0b01000000, 0 }, //
  {0x20, 0xF0, 0b01000100, 0 }, //
  {0x20, 0xF1, 0b00001101, 0 }, //

  // {0x20, 0xF9, 0x03,       0 }, //24
  {0x20, 0xF2, 0b00000010, 0 }, //undocumented yolo flippin bits
  // {0x20, 0x69, 0b01000000},

  //Manual mux cowabunga (3 total)
  // {0x20, 0xC3, 0b00110001, 0 }, //manual mux AIN3->ADC1, AIN1->ADC0
  // {0x20, 0xC4, 0b11000010, 0 }, //manual mux enable, manual mux AIN2->ADC2
  // {0x20, 0xF3, 0b01001010, 0 }, //AA filters yuck, Manual mux AIN4->ADC3

  //Extended output control yolo
  // {0x20, 0x05, 0b00000000, 0 },
  // {0x20, 0x06, 0b00000010, 0 },

  // other test commands from earlier (2 total)
  //{0x20, 0xF1, 0b00001101, 0 }, // TEST VALUE FOR RGB INPUT --this is the conflicting info one-- try toggling far right bit. --last bit being 1 seems correct based on 7403 Datasheet

  //{0x20, 0xED, 0b11000100, 0 }, // TEST VALUE FOR RGB INPUT

  // secret map commands! (20 total)
  // {0x20, 0x0E, 0x80,       0 }, // ADI Recommended Write, Enter the secret map
  // {0x20, 0x52, 0x46,       0 }, // ADI Recommended Write
  // {0x20, 0x54, 0x00,       0 }, // ADI Recommended Write
  // {0x20, 0x7F, 0xFF,       0 }, // ADI Recommended Write
  // {0x20, 0x81, 0x30,       0 }, // ADI Recommended Write
  // {0x20, 0x90, 0xC9,       0 }, // ADI Recommended Write
  // {0x20, 0x91, 0x40,       0 }, // ADI Recommended Write
  // {0x20, 0x92, 0x3C,       0 }, // ADI Recommended Write
  // {0x20, 0x93, 0xCA,       0 }, // ADI Recommended Write
  // {0x20, 0x94, 0xD5,       0 }, // ADI Recommended Write
  // {0x20, 0xB1, 0xFF,       0 }, // ADI Recommended Write
  // {0x20, 0xB6, 0x08,       0 }, // ADI Recommended Write
  // {0x20, 0xC0, 0x9A,       0 }, // ADI Recommended Write
  // {0x20, 0xCF, 0x50,       0 }, // ADI Recommended Write
  // {0x20, 0xD0, 0x4E,       0 }, // ADI Recommended Write
  // {0x20, 0xD1, 0xB9,       0 }, // ADI Recommended Write
  // {0x20, 0xD6, 0xDD,       0 }, // ADI Recommended Write
  // {0x20, 0xD7, 0xE2,       0 }, // ADI Recommended Write
  // {0x20, 0xE5, 0x51,       0 }, // ADI Recommended Write
  // {0x20, 0x0E, 0x00,       0 }, // ADI Recommended Write, Leave the secret map

  // encoder commands (9 total)
  {0x2A, 0x02, 0x10,       0 }, // RGB output enabled; RsGsBs output sync enabled
  {0x2A, 0x82, 0xC9,       0 }, // pixel data valid; RGB out; PbPr SSAF on; step control on; ped on
  {0x2A, 0x00, 0x1C,       0 }, // Power up DACs and PLL [Encoder writes begin] --The bit toggled between these two (bit 0) is marked "reserved"--
  {0x2A, 0x01, 0x00,       0 }, // Set Encoder to SD mode                       --These 3 bits enable SD input on the ADV7391--
  {0x2A, 0x30, 0x00,       0 }, // Reset value for ED register                  --This resets ED/HD mode register 1 to default, which is External Hsync and Vsync field inputs--
  {0x2A, 0x31, 0x00,       0 }, // Reset value for ED register                  --Turns "ED/HD pixel data valid" off (default)
  {0x2A, 0x80, 0x00,       0 }, // SSAF Luma filter enabled, NTSC mode (TODO: same as reset value, so not needed here?)
  {0x2A, 0x87, 0x20,       0 }, // PAL/NTSC autodetect mode enabled
  {0x2A, 0x88, 0x00,       0 }, // 8 bit input enabled, SD noninterlaced mode off
};

const int commands_cvbs_input_length = 4;
const device_command_t commands_cvbs_input[] = {
  {0x20, 0x0C, 0x34, 0}, // Force disable free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D***
  {0x20, 0x52, 0xCD, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x03, 0}, // CVBS in on Ain4                   ***Bits 7, 6, and 5 are blank and undocumented. Investigate?***
  //{0x20, 0x00, 0b00000000, 0}, // TEST VALUE FOR RGB INPUT **TESTING CHANGE BACK TO 0x00 if you are reading this**
};

const int commands_yc_input_length = 4;
const device_command_t commands_yc_input[] = {
  {0x20, 0x0C, 0x34, 0}, // Force disable free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D [default settings]***
  {0x20, 0x53, 0xCE, 0}, // AFE IBIAS                         ***UNDOCUMENTED. This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x09, 0}, // INSEL = YC, Y - Ain3, C - Ain4
};

const int commands_ypbpr_input_length = 4;
const device_command_t commands_ypbpr_input[] = {
  {0x20, 0x0C, 0x34, 0}, // Force disable free run mode         ***But FREE RUN Still happens when no image detected. [default settings]***
  {0x20, 0x14, 0x10, 0}, // Deselect free run pattern         ***Sets free run pattern to single solid color. Color controlled by 0x0C & 0x0D [default settings]***
  {0x20, 0x54, 0xC0, 0}, // AFE IBIAS                         ***UNDOCUMENTED. Reserved bit in Interrupt clear diagnostic register? This is never set back. Can we delete or make permanent?***
  {0x20, 0x00, 0x0C, 0}, // INSEL = YPbPr, Y=Ain1, Pb=Ain2, Pr=Ain3
};

const int commands_test_pattern_input_length = 3;
const device_command_t commands_test_pattern_input[] = {
  {0x20, 0x00, 0x07, 0}, // ADI Required Write [INSEL set to unconnected input]
  {0x20, 0x0C, 0x37, 0}, // Force Free run mode
  {0x20, 0x14, 0x11, 0}, // Set Free-run pattern to 100% color bars
};

const int commands_rgb_output_rsgsbs_length = 2;
const device_command_t commands_rgb_output_rsgsbs[] = {
  {0x2A, 0x02, 0x10, 0}, // RGB output enabled; RsGsBs output sync enabled
  {0x2A, 0x82, 0xC9, 0}, // pixel data valid; RGB out; PbPr SSAF on; step control on; ped on
};

const int commands_rgb_output_no_sync_length = 2;
const device_command_t commands_rgb_output_no_sync[] = {
  {0x2A, 0x02, 0x00, 0}, // RGB output enabled; RsGsBs output sync disabled
  {0x2A, 0x82, 0xC9, 0}, // pixel data valid; RGB out; PbPr SSAF on; step control on; ped on
};

const int commands_ypbpr_output_length = 2;
const device_command_t commands_ypbpr_output[] = {
  {0x2A, 0x02, 0x20, 0}, // RGB output disabled (reset register to default value)
  {0x2A, 0x82, 0xC9, 0}, // pixel data valid; YPbPr out; PbPr SSAF on; step control on; ped on
};

const int commands_cvbs_output_length = 2;
const device_command_t commands_cvbs_output[] = {
  {0x2A, 0x02, 0x20, 0}, // RGB output disabled (reset register to default value)
  {0x2A, 0x82, 0xCB, 0}, // pixel data valid; CVBS/Y-C out; PbPr SSAF on; step control on; ped on
};

const int commands_line_double_length = 9;
const device_command_t commands_line_double[] = {
  {0x42, 0xA3, 0x00, 0}, // ADI Required Write [ADV7280A VPP writes begin]
  {0x42, 0x5B, 0x00, 0}, // Enable Advanced Timing Mode
  {0x42, 0x55, 0x80, 0}, // Enable the Deinterlacer for I2P [ADV7280A VPP writes finished]

  {0x2A, 0x00, 0x9C, 0}, // Power up DACs and PLL [Encoder writes begin]  --The bit toggled between these two is marked "reserved"---
  {0x2A, 0x01, 0x70, 0}, // ED at 54MHz input                             --These 3 bits enable ED at 54MHz input on the ADV7391--

  {0x2A, 0x30, 0x04, 0}, // 525p at 59.94 Hz with Embedded Timing         --This flips one bit that disables HSYNC and VSYNC inputs, and enables embedded EAV/SAV codes-- was 0x04, set to x00
  {0x2A, 0x31, 0x01, 0}, // ED Pixel Data Valid [Encoder Writes finished] --Turns "ED/HD pixel data valid" on

  {0x2A, 0x80, 0x00, 0}, // SSAF Luma filter enabled, NTSC mode (TODO: same as reset value, so not needed here?)
  {0x2A, 0x87, 0x00, 0}, // PAL/NTSC autodetect mode disabled (Reset value for SD register)
};

const int commands_line_undouble_length = 9;
const device_command_t commands_line_undouble[] = {
  {0x42, 0xA3, 0x00, 0}, // ADI Required Write [ADV7280A VPP writes begin]
  {0x42, 0x5B, 0x80, 0}, // Disable Advanced Timing Mode
  {0x42, 0x55, 0x00, 0}, // Disable I2P [ADV7280A VPP writes finished]

  {0x2A, 0x00, 0x1C, 0}, // Power up DACs and PLL [Encoder writes begin] --The bit toggled between these two (bit 0) is marked "reserved"--
  {0x2A, 0x01, 0x00, 0}, // Set Encoder to SD mode                       --These 3 bits enable SD input on the ADV7391--

  {0x2A, 0x30, 0x00, 0}, // Reset value for ED register                  --This resets ED/HD mode register 1 to default, which is External Hsync and Vsync field inputs--
  {0x2A, 0x31, 0x00, 0}, // Reset value for ED register                  --Turns "ED/HD pixel data valid" off (default)

  {0x2A, 0x80, 0x00, 0}, // SSAF Luma filter enabled, NTSC mode (TODO: same as reset value, so not needed here?)
  {0x2A, 0x87, 0x20, 0}, // PAL/NTSC autodetect mode enabled
};

const int commands_vaseline_on_length = 1;
const device_command_t commands_vaseline_on[] = {
  // {0x20, 0xF3, 0x1F, 0}, // Enable all antialiasing filters
  // {0x20, 0x17, 0b00000001, 0}, // Enable Chroma Filter
  // {0x42, 0x17, 0b00011000, 0}, // Enable Secret Deinterlacer
  {0x42, 0x5A, 0b00011010, 0}, // Enable  AA filters
};

const int commands_vaseline_off_length = 1;
const device_command_t commands_vaseline_off[] = {
  // {0x20, 0xF3, 0x10, 0}, // Disable all antialiasing filters
  // {0x20, 0x17, 0b00100001, 0}, // Disables Chroma FIlter
  // {0x42, 0x17, 0b00000000, 0}, // Enable Standard Deinterlacer
  {0x42, 0x5A, 0b00000010, 0}, // Disable AA filters
};

const int commands_notch_filter_on_length = 1;
const device_command_t commands_notch_filter_on[] = {
  {0x20, 0x38, 0b10000100, 0}, //Force notch filter
};

const int commands_notch_filter_off_length = 1;
const device_command_t commands_notch_filter_off[] = {
  {0x20, 0x38, 0b10000000, 0}, //Use automatic filter
};

const int commands_force_240p_length = 4;
const device_command_t commands_force_240p[] = {
  {0x2A, 0x88, 0x02,       0}, // 8 bit input enabled, SD noninterlaced mode on

  {0x20, 0x0E, 0x40,       0}, // Enter sub map 2
  {0x20, 0xE0, 0b00000001, 0}, // Enable Fast Lock control (makes 240p input work as 240p and not 480i) (this address is within Sub Map 2)
  {0x20, 0x0E, 0x00,       0}, // Leave sub map 2, return to sub map 1
};

const int commands_unforce_240p_length = 4;
const device_command_t commands_unforce_240p[] = {
  {0x2A, 0x88, 0x00,       0}, // 8 bit input enabled, SD noninterlaced mode off

  {0x20, 0x0E, 0x40,       0}, // Enter sub map 2 (leave sub map 1)
  {0x20, 0xE0, 0b00000000, 0}, // Disable Fast Lock control (probably could remove this and leave fast lock always on maybe?)
  {0x20, 0x0E, 0x00,       0}, // Leave sub map 2, return to sub map 1
};

const int commands_pbpr_sync_on_length = 1;
const device_command_t commands_pbpr_sync_on[] = {
  {0x2A, 0x35, 0x04, 0}, // Turns on Sync for Pb and Pr
};

const int commands_pbpr_sync_off_length = 1;
const device_command_t commands_pbpr_sync_off[] = {
  {0x2A, 0x35, 0x00, 0}, // Turns off Sync for Pb and Pr
};

const int commands_beta_input_voltage_on_length = 1;
const device_command_t commands_beta_input_voltage_on[] = {
  {0x20, 0x01, 0xE8, 0}, //Enables betacam input levels
};

const int commands_beta_input_voltage_off_length = 1;
const device_command_t commands_beta_input_voltage_off[] = {
  {0x20, 0x01, 0xC8, 0}, //Disable betacam input levels
};

const int commands_pedestal_output_on_length = 1;
const device_command_t commands_pedestal_output_on[] = {
  {0x20, 0x02, 0x14, 0},
};

const int commands_pedestal_output_off_length = 1;
const device_command_t commands_pedestal_output_off[] = {
  {0x20, 0x02, 0x14, 0},
};

const int commands_enter_vdp_map_length = 1;
const device_command_t commands_enter_vdp_map[] = {
  {0x20, 0x0E, 0x20, 0}, // Enter Interrupt/VDP Sub map
};

const int commands_enable_hlock_interrupt_length = 2;
const device_command_t commands_enable_hlock_interrupt[] = {
  {0x20, 0x44, 0xFF, 0}, // Enable HLock interrupt  ---changing for loopj---
  {0x20, 0x4C, 0xFF, 0}, // Enable HLock interrupt  ---changing for loopj---
};

const int commands_clear_hlock_interrupt_length = 2;
const device_command_t commands_clear_hlock_interrupt[] = {
  {0x20, 0x43, 0xFF, 0}, // Clear HLOCK interrupt; ONLY works while in the VDP map
  {0x20, 0x4B, 0xFF, 0}, // Clear HLOCK interrupt; ONLY works while in the VDP map
};

const int commands_exit_vdp_map_length = 1;
const device_command_t commands_exit_vdp_map[] = {
  {0x20, 0x0E, 0x00, 0}, // Leave Interrupt/VDP Sub map, return to sub map 1
};

int send_command_fake_scannies_on() {

};

int send_command_fake_scannies_off() {

};

int send_command_freerun_480i_60Hz_YPbPr_out() {
  return i2c_write_commands(I2C_PORT, commands_freerun_480i_60Hz_YPbPr_out, commands_freerun_480i_60Hz_YPbPr_out_length);
};

int send_command_7184_rgb_test() {
  return i2c_write_commands(I2C_PORT, commands_7184_rgb_test, commands_7184_rgb_test_length);
};

int send_command_cvbs_input() {
  return i2c_write_commands(I2C_PORT, commands_cvbs_input, commands_cvbs_input_length);
};

int send_command_yc_input() {
  return i2c_write_commands(I2C_PORT, commands_yc_input, commands_yc_input_length);
};

int send_command_ypbpr_input() {
  return i2c_write_commands(I2C_PORT, commands_ypbpr_input, commands_ypbpr_input_length);
};

int send_command_test_pattern_input() {
  return i2c_write_commands(I2C_PORT, commands_test_pattern_input, commands_test_pattern_input_length);
};

int send_command_rgb_output_rsgsbs() {
  return i2c_write_commands(I2C_PORT, commands_rgb_output_rsgsbs, commands_rgb_output_rsgsbs_length);
};

int send_command_rgb_output_no_sync() {
  return i2c_write_commands(I2C_PORT, commands_rgb_output_no_sync, commands_rgb_output_no_sync_length);
};

int send_command_ypbpr_output() {
  return i2c_write_commands(I2C_PORT, commands_ypbpr_output, commands_ypbpr_output_length);
};

int send_command_cvbs_output() {
  return i2c_write_commands(I2C_PORT, commands_cvbs_output, commands_cvbs_output_length);
};

int send_command_line_double() {
  // printf("\tline doubling start\n");
  int ret = i2c_write_commands(I2C_PORT, commands_line_double, commands_line_double_length);
  // printf("\tline doubling stop\n");
  return ret;
};

int send_command_line_undouble() {
  // printf("\tline undoubling start\n");
  int ret = i2c_write_commands(I2C_PORT, commands_line_undouble, commands_line_undouble_length);
  // printf("\tline undoubling stop\n");
  return ret;
};

int send_command_vaseline_on() {
  return i2c_write_commands(I2C_PORT, commands_vaseline_on, commands_vaseline_on_length);
};

int send_command_vaseline_off() {
  return i2c_write_commands(I2C_PORT, commands_vaseline_off, commands_vaseline_off_length);
};

int send_command_notch_filter_on() {
  return i2c_write_commands(I2C_PORT, commands_notch_filter_on, commands_notch_filter_on_length);
};

int send_command_notch_filter_off() {
  return i2c_write_commands(I2C_PORT, commands_notch_filter_off, commands_notch_filter_off_length);
};

int send_command_force_240p() {
  return i2c_write_commands(I2C_PORT, commands_force_240p, commands_force_240p_length);
};

int send_command_unforce_240p() {
  return i2c_write_commands(I2C_PORT, commands_unforce_240p, commands_unforce_240p_length);
};

int send_command_pbpr_sync_on() {
  return i2c_write_commands(I2C_PORT, commands_pbpr_sync_on, commands_pbpr_sync_on_length);
};

int send_command_pbpr_sync_off() {
  return i2c_write_commands(I2C_PORT, commands_pbpr_sync_off, commands_pbpr_sync_off_length);
};

int send_command_beta_input_voltage_on() {
  return i2c_write_commands(I2C_PORT, commands_beta_input_voltage_on, commands_beta_input_voltage_on_length);
};

int send_command_beta_input_voltage_off() {
  return i2c_write_commands(I2C_PORT, commands_beta_input_voltage_off, commands_beta_input_voltage_off_length);
};

int send_command_pedestal_output_on() {
  return i2c_write_commands(I2C_PORT, commands_pedestal_output_on, commands_pedestal_output_on_length);
};

int send_command_pedestal_output_off() {
  return i2c_write_commands(I2C_PORT, commands_pedestal_output_off, commands_pedestal_output_off_length);
};

int enter_vdp_map() {
  return i2c_write_commands(I2C_PORT, commands_enter_vdp_map, commands_enter_vdp_map_length);
};

int enable_hlock_interrupt() {
  return i2c_write_commands(I2C_PORT, commands_enable_hlock_interrupt, commands_enable_hlock_interrupt_length);
};

int clear_hlock_interrupt() {
  return i2c_write_commands(I2C_PORT, commands_clear_hlock_interrupt, commands_clear_hlock_interrupt_length);
};

int exit_vdp_map() {
  return i2c_write_commands(I2C_PORT, commands_exit_vdp_map, commands_exit_vdp_map_length);
};

int get_is_interlaced(bool *is_interlaced) {
  uint8_t register_value;

  int ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x13, &register_value);
  if (ret < 0)
    return ret;

  *is_interlaced = register_value & 0b01000000;

  return 0;
};

int get_lock_status(bool *is_inlock_set, bool *is_hlock_set) {
  uint8_t register_value;

  int ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x10, &register_value);
  if (ret < 0)
    return ret;

  *is_inlock_set = register_value & 0b00000001;

  ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x13, &register_value);
  if (ret < 0)
    return ret;

  *is_hlock_set = register_value & 0b00000001;

  return 0;
};

int get_hlock_changed(bool *is_hlock_changed) {
  uint8_t register_value;

  int ret = i2c_read_from_device_register(I2C_PORT, 0x20, 0x4A, &register_value);
  if (ret < 0)
    return ret;

  *is_hlock_changed = register_value & 0b00000100;

  return 0;
};

int write_custom_register_value(uint8_t device, uint8_t reg, uint8_t val) {
  return i2c_write_to_device_register(I2C_PORT, device, reg, val);
};