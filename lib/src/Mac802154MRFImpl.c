#include "lib/include/Mac802154.h"
#include "lib/include/Mac802154MRFImpl.h"
#include "lib/src/MRFInternalConstants.h"
#include "lib/src/MRFHelperFunctions.h"
#include "CException.h"
#include "lib/include/Exception.h"

/**
 * ## Memory Layout ##
 * The memory mapped area for the TX Normal FIFO (the fifo buffer
 * that the mrf module uses for normal TX frames) has the address space
 * from 0x00 to 0x7F (i.e. 128 Bytes). Using a word length of one byte,
 * and 16bit addresses
 * this address space is mapped to an 802.15.4 packet as follows:
 *
 * |Header Length (m) | Frame (n) | Header                 | Payload                           |
 * |------------------|-----------|------------------------|-----------------------------------|
 * |0x000             | 0x001     | 0x002 - (0x002 + m -1) | (0x002 + m) - (0x002 + m + n - 1) |
 *
 * (see http://www.microchip.com/wwwproducts/en/en027752 (03.02.2018))
 *
 * ## Header Format ##
 * Additionally we have to know how the header has to be build:
 * The header is comprised of the following fields
 *
 * | Number of Octets:| Field Name:               |
 * |------------------|---------------------------|
 * | 1/2              | Frame Control             |
 * | 0/1              | Sequence Number           |
 * | 0/2              | Destination PAN ID        |
 * | 0/2/8            | Destination Address       |
 * | 0/2              | Source PAN ID             |
 * | 0/2/8            | Source Address            |
 * | 0/2/8            | Auxiliary Security Header |
 * | variable         | Header IEs                |
 *
 * Not all fields have to be used and we will restrict the use of 802.15.4 features so
 * creating frames and keeping the state of the TX Normal FIFO consistent becomes easier
 * and produces less overhead.
 *
 * In the following the choosen values of the fixed fields and general functionality of
 * some other fields are explained.
 *
 * ### Frame Control ###
 * The Frame Control field itself consists of several other fields.
 * The following table shows the chosen fixed values as well as a short description
 * of each values meaning
 *
 * |Name               | Value | Description                                                                      |
 * |------------------ |-------|----------------------------------------------------------------------------------|
 * | Frame Type        | 0b001 | Data Frame (other possible values are e.g. Acknowledgment or MAC Command)        |
 * | Security Enabled  | 0b0   | Disabled                                                                         |
 * | Frame Pending     | 0b0   | only relevant for specific modes and frames, we say no frame pending here        |
 * | AR                | 0b0   | tell the recepient we don't want an Acknowledgement                              |
 * | PAN ID Compression| 0b1   | only use one PAN ID field because source and destination PAN ID will be the same |
 *
 */

typedef struct MRFImpl MRFImpl;

struct MRFImpl {
  Mac802154 interface;
  SPISlave *output_device;
  DelayFunction delayMicroseconds;
  uint16_t pan_id;
};

static void init(Mac802154 *self, const Mac802154Config *config);
static void setInterfaceFunctionPointers(Mac802154 *self);
static void setPrivateFunctionPointers(MRFImpl *self, DelayFunction delay_microseconds);
static void reset(MRFImpl *self);
static void setShortRegister(MRFImpl *self, uint8_t address, uint8_t value);
static void setLongRegister(MRFImpl *self, uint16_t address, uint8_t value);
static void enableRXInterrupt(MRFImpl *self);
static void setInitializationValuesFromDatasheet(MRFImpl *impl);
static void selectChannel(MRFImpl *self, uint8_t channel_number);
static void setTransmitterPower(MRFImpl *impl);
static void resetInternalStateMachine(MRFImpl *impl);
static void setPanId(MRFImpl *self, uint16_t pan_id);
static void setShortSourceAddress(MRFImpl *self, uint16_t short_address);
static void setExtendedSourceAddress(MRFImpl *self, const uint8_t *extended_address);

Mac802154 *Mac802154_createMRF(SPISlave *output_device,
                               Allocator allocate,
                               DelayFunction delay_microseconds) {
  MRFImpl *impl = allocate(sizeof(MRFImpl));
  impl->output_device = output_device;
  Mac802154 *interface = (Mac802154*) impl;
  setInterfaceFunctionPointers(interface);
  setPrivateFunctionPointers(impl, delay_microseconds);
  return interface;
}

void setInterfaceFunctionPointers(Mac802154 *interface) {
  interface->init = init;
}

void setPrivateFunctionPointers(MRFImpl *self, DelayFunction delay_microseconds) {
  self->delayMicroseconds = delay_microseconds;
}



/*
 * Initialize the MRF Chip as shown in the datasheets initialization
 * example. I don't think we need to set all of these, but we do
 * for now just to be sure until we have figured out what each of these
 * does exactly.
 */
void init(Mac802154 *self, const Mac802154Config *config) {
  MRFImpl *impl = (MRFImpl *) self;
  CEXCEPTION_T is_busy_exception;
  Try {
        reset(impl);
        setInitializationValuesFromDatasheet(impl);
        enableRXInterrupt(impl);
        selectChannel(impl, 11);
        setTransmitterPower(impl);
        resetInternalStateMachine(impl);
        setPanId(impl,config->pan_id);
        setShortSourceAddress(impl, config->short_source_address);
        setExtendedSourceAddress(impl, config->extended_source_address);
      } Catch(is_busy_exception) {
    Throw(NETWORK_HARDWARE_IS_BUSY_EXCEPTION);
  }
}

void reset(MRFImpl *self) {
  uint8_t reset_power_circuit = 1 << 2;
  uint8_t reset_baseband_circuit = 1 << 1;
  uint8_t reset_all_mac_control_registers = 1;
  uint8_t complete_reset = (
          reset_all_mac_control_registers |
          reset_baseband_circuit |
          reset_power_circuit
  );
  setShortRegister(self, mrf_register_software_reset, complete_reset);
}

void setPanId(MRFImpl *self, uint16_t pan_id) {
  CEXCEPTION_T spi_busy_exception;
  Try {
        uint8_t pan_id_buffer[] = {
                MRF_writeShortCommand(mrf_register_pan_id_low_byte),
                (uint8_t)pan_id,
                (uint8_t)(pan_id >> 8)
        };
        SPIMessage set_pan = {
                .length = 3,
                .incoming_data = NULL,
                .outgoing_data = pan_id_buffer
        };
        SPI_transferSync(self->output_device, &set_pan);
      } Catch (spi_busy_exception) {
        Throw(NETWORK_HARDWARE_IS_BUSY_EXCEPTION);
  }
}

uint16_t getPanId(const Mac802154 *self) {
  MRFImpl *impl = (MRFImpl*) self;
  return impl->pan_id;
}

void enableRXInterrupt(MRFImpl *self) {
  // setting a bit to zero enables the corresponding interrupt
  uint8_t rx_interrupt_enabled = (uint8_t)(~(1 << 3));
  setShortRegister(self, mrf_register_interrupt_control, rx_interrupt_enabled);
}

void setInitializationValuesFromDatasheet(MRFImpl *impl) {
  setShortRegister(impl, mrf_register_power_amplifier_control2, 0x98);
  setShortRegister(impl, mrf_register_tx_stabilization, 0x09);
  setLongRegister(impl, mrf_register_rf_control0, 0x03);
  setLongRegister(impl, mrf_register_rf_control1, 0x01);
  setLongRegister(impl, mrf_register_rf_control2, 0x80); // enable PLL, this is certainly necessary
  setLongRegister(impl, mrf_register_rf_control6, 0x90);
  setLongRegister(impl, mrf_register_rf_control7, 0x80);
  setLongRegister(impl, mrf_register_rf_control8, 0x10);
  setLongRegister(impl, mrf_register_sleep_clock_control1, 0x21);
  setShortRegister(impl, mrf_register_base_band2, 0x80);
  setShortRegister(impl, mrf_register_energy_detection_threshold_for_clear_channel_assessment, 0x60);
  setShortRegister(impl, mrf_register_base_band6, 0x40);
}

void setShortRegister(MRFImpl *self, uint8_t address, uint8_t value) {
  uint8_t command = MRF_writeShortCommand(address);
  uint8_t sequence[] = {command, value};
  SPIMessage message = {
          .length = 2,
          .outgoing_data = sequence,
          .incoming_data = NULL
  };
  SPI_transferSync(self->output_device, &message);
}

void setLongRegister(MRFImpl *self, uint16_t address, uint8_t value) {
  uint16_t command = MRF_writeLongCommand(address);
  uint8_t sequence[] = { (uint8_t)(command >> 8), (uint8_t)command, value};
  SPIMessage message = {
          .length = 3,
          .outgoing_data = sequence,
          .incoming_data = NULL
  };
  SPI_transferSync(self->output_device, &message);
}

/*
 * The conversion from channel_number to register_value is derived
 * from the table 3-4 in section 3.4 of the datasheet.
 */
void selectChannel(MRFImpl *impl, uint8_t channel_number) {
  uint8_t register_value = (uint8_t)((channel_number % 10 - 1) * 4 + 0x03);
  setLongRegister(impl, mrf_register_rf_control0, register_value);
}

// see data sheet description of the RFCON3 register for more info on how
// to setup signal strength
void setTransmitterPower(MRFImpl *impl) {
  uint8_t minus_thirty_db = 3 << 6;
  setLongRegister(impl, mrf_register_rf_control3, minus_thirty_db);
}

void resetInternalStateMachine(MRFImpl *impl) {
  uint8_t reset_bit_enabled = 0x04;
  setShortRegister(impl, mrf_register_rf_mode_control, reset_bit_enabled);
  uint8_t start_internal_state_machine = 0x00;
  setShortRegister(impl, mrf_register_rf_mode_control,
                   start_internal_state_machine);
  impl->delayMicroseconds(200);
}

void setShortSourceAddress(MRFImpl *self, uint16_t short_address) {
  uint8_t command_sequence[] = {
          MRF_writeShortCommand(mrf_register_short_address_low_byte),
          (uint8_t) short_address,
          (uint8_t) (short_address >> 8)
  };
  SPIMessage message = {
          .length = 3,
          .outgoing_data = command_sequence,
          .incoming_data = NULL,
  };
  SPI_transferSync(self->output_device, &message);
}

void setExtendedSourceAddress(MRFImpl *self, const uint8_t *extended_address) {
  uint8_t command_sequence[9];
  command_sequence[0] = MRF_writeShortCommand(mrf_register_extended_address0);
  for (uint8_t address_index = 0; address_index < 8; address_index++) {
    command_sequence[address_index+1] = extended_address[address_index];
  }
  SPIMessage message = {
          .length = 9,
          .outgoing_data = command_sequence,
          .incoming_data = NULL,
  };
  SPI_transferSync(self->output_device, &message);
}