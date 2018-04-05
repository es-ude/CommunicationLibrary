#include "lib/include/Mac802154.h"

const FrameHeader802154 *Mac802154_defaultHeader(void) {
  static FrameHeader802154 header = {
          .control.as_struct = {
                  .frame_type = 0b001,
                  .security_enabled = 0b0,
                  .frame_pending = 0b0,
                  .acknowledgment_request = 0b0,
                  .pan_id_compression = 0b1,
                  .reserved = 0b0,
                  .sequence_number_suppression = 0b0,
                  .information_element_present = 0b0,
                  .destination_addressing_mode = 0b10,
                  .frame_version = 0b10,
                  .source_addressing_mode = 0b10,
          },
          .destination.short_address = {0x00, 0x00},
          .destination_pan_id = {0xff, 0xff},
          .sequence_number = 0,
  };
  return &header;
}