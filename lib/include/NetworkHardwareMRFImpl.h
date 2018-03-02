#ifndef NETWORK_HARDWARE_PERIPHERAL_IMPL_H
#define NETWORK_HARDWARE_PERIPHERAL_IMPL_H

#include "lib/include/NetworkHardware.h"
#include "lib/include/communicationLayer/SPICommunicationLayer.h"
#include "lib/include/RuntimeLibraryInterface.h"

/**
 * Regarding the functionality of the MRF24J40 Module:
 *
 * The Module uses the 802.15.4 Protocol for wireless data
 * transmission. The transmission is controlled through a memory mapped
 * area. We do not only need to write the correct data frame to the peripheral
 * but also write each of the necessary bytes to the correct location using
 * the MRF's addressing scheme. The MRF Module has two different memory mapped
 * areas with different address range (8bit and 16bit addresses).
 * For transmission control access to the "Long Address Memory Space" is needed.
 * To write a byte to a certain address first the address type and the address
 * itself are written to the hardware, after this follows one byte of actual data.
 *
 * This software module is responsible for generating the necessary data
 * structure and writing it to the MRF hardware.
 *
 * The several set methods are used to write the corresponding field to the hardware.
 * After changing all desired fields the actual frame transmission can be triggered with
 * the send function. This means that between the transmission of two frames only the
 * values that are different for the second frame have to be set. Unchanged data is not
 * written to the hardware again.
 */

NetworkHardware *NetworkHardware_createMRF(SPISlave *output_device, Allocator allocate, DelayFunction delay_microseconds);


#endif /* end of include guard */
