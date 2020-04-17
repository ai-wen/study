#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "libusb.h"
//#include "skf.h"

#define VID 0x055c
#define PID 0xe618
 
static void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0;
 
	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}
 
		printf("%04x:%04x (bus %d, device %d)\n",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));
	}
}

// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

// HID
static int get_hid_record_size(uint8_t *hid_report_descriptor, int size, int type)
{
	uint8_t i, j = 0;
	uint8_t offset;
	int record_size[3] = {0, 0, 0};
	int nb_bits = 0, nb_items = 0;
	int found_record_marker;

	found_record_marker = 0;
	for (i = hid_report_descriptor[0]+1; i < size; i += offset) {
		offset = (hid_report_descriptor[i]&0x03) + 1;
		if (offset == 4)
			offset = 5;
		switch (hid_report_descriptor[i] & 0xFC) {
		case 0x74:	// bitsize
			nb_bits = hid_report_descriptor[i+1];
			break;
		case 0x94:	// count
			nb_items = 0;
			for (j=1; j<offset; j++) {
				nb_items = ((uint32_t)hid_report_descriptor[i+j]) << (8*(j-1));
			}
			break;
		case 0x80:	// input
			found_record_marker = 1;
			j = 0;
			break;
		case 0x90:	// output
			found_record_marker = 1;
			j = 1;
			break;
		case 0xb0:	// feature
			found_record_marker = 1;
			j = 2;
			break;
		case 0xC0:	// end of collection
			nb_items = 0;
			nb_bits = 0;
			break;
		default:
			continue;
		}
		if (found_record_marker) {
			found_record_marker = 0;
			record_size[j] += nb_items*nb_bits;
		}
	}
	if ((type < HID_REPORT_TYPE_INPUT) || (type > HID_REPORT_TYPE_FEATURE)) {
		return 0;
	} else {
		return (record_size[type - HID_REPORT_TYPE_INPUT]+7)/8;
	}
}

static void display_buffer_hex(unsigned char *buffer, unsigned size)
{
	unsigned i, j, k;

	for (i=0; i<size; i+=16) {
		printf("\n  %08x  ", i);
		for(j=0,k=0; k<16; j++,k++) {
			if (i+j < size) {
				printf("%02x", buffer[i+j]);
			} else {
				printf("  ");
			}
			printf(" ");
		}
		printf(" ");
		for(j=0,k=0; k<16; j++,k++) {
			if (i+j < size) {
				if ((buffer[i+j] < 32) || (buffer[i+j] > 126)) {
					printf(".");
				} else {
					printf("%c", buffer[i+j]);
				}
			}
		}
	}
	printf("\n" );
}

static char binary_dump = 1;
static char binary_name[] = "/tmp/tt.hid";

static int test_hid(libusb_device_handle *handle, uint8_t endpoint_in)
{
	int r, size, descriptor_size;
	uint8_t hid_report_descriptor[256];
	uint8_t *report_buffer;
	FILE *fd;

	printf("\nReading HID Report Descriptors:\n");
	descriptor_size = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_STANDARD|LIBUSB_RECIPIENT_INTERFACE,
		LIBUSB_REQUEST_GET_DESCRIPTOR, LIBUSB_DT_REPORT<<8, 0, hid_report_descriptor, sizeof(hid_report_descriptor), 1000);
	if (descriptor_size < 0) {
		printf("   Failed\n");
		return -1;
	}
	display_buffer_hex(hid_report_descriptor, descriptor_size);
	if ((binary_dump) && ((fd = fopen(binary_name, "w")) != NULL)) {
		if (fwrite(hid_report_descriptor, 1, descriptor_size, fd) != (size_t)descriptor_size) {
			printf("   Error writing descriptor to file\n");
		}
		fclose(fd);
	}

	size = get_hid_record_size(hid_report_descriptor, descriptor_size, HID_REPORT_TYPE_FEATURE);
	if (size <= 0) {
		printf("\nSkipping Feature Report readout (None detected)\n");
	} else {
		report_buffer = (uint8_t*) calloc(size, 1);
		if (report_buffer == NULL) {
			return -1;
		}

		printf("\nReading Feature Report (length %d)...\n", size);
		r = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,
			HID_GET_REPORT, (HID_REPORT_TYPE_FEATURE<<8)|0, 0, report_buffer, (uint16_t)size, 5000);
		if (r >= 0) {
			display_buffer_hex(report_buffer, size);
		} else {
			switch(r) {
			case LIBUSB_ERROR_NOT_FOUND:
				printf("   No Feature Report available for this device\n");
				break;
			case LIBUSB_ERROR_PIPE:
				printf("   Detected stall - resetting pipe...\n");
				libusb_clear_halt(handle, 0);
				break;
			default:
				printf("   Error: %s\n", libusb_strerror((enum libusb_error)r));
				break;
			}
		}
		free(report_buffer);
	}

	size = get_hid_record_size(hid_report_descriptor, descriptor_size, HID_REPORT_TYPE_INPUT);
	if (size <= 0) {
		printf("\nSkipping Input Report readout (None detected)\n");
	} else {
		report_buffer = (uint8_t*) calloc(size, 1);
		if (report_buffer == NULL) {
			return -1;
		}

		printf("\nReading Input Report (length %d)...\n", size);
		r = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,
			HID_GET_REPORT, (HID_REPORT_TYPE_INPUT<<8)|0x00, 0, report_buffer, (uint16_t)size, 5000);
		if (r >= 0) {
			display_buffer_hex(report_buffer, size);
		} else {
			switch(r) {
			case LIBUSB_ERROR_TIMEOUT:
				printf("   Timeout! Please make sure you act on the device within the 5 seconds allocated...\n");
				break;
			case LIBUSB_ERROR_PIPE:
				printf("   Detected stall - resetting pipe...\n");
				libusb_clear_halt(handle, 0);
				break;
			default:
				printf("   Error: %s\n", libusb_strerror((enum libusb_error)r));
				break;
			}
		}

		// Attempt a bulk read from endpoint 0 (this should just return a raw input report)
		printf("\nTesting interrupt read using endpoint %02X...\n", endpoint_in);
		r = libusb_interrupt_transfer(handle, endpoint_in, report_buffer, size, &size, 5000);
		if (r >= 0) {
			display_buffer_hex(report_buffer, size);
		} else {
			printf("   %s\n", libusb_strerror((enum libusb_error)r));
		}

		free(report_buffer);
	}
	return 0;
}

static int test_key(libusb_device_handle *handle, uint8_t endpoint_in, uint8_t endpoint_out)
{
	int i, j = 0;
	int r, size;
#if 0
	unsigned char cb[64] = {
		0xde, 0xfe, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x0a, 0x00, 0x00, 0x00, 0x12, 0x00, 0x07, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#else
	unsigned char cb[64] = {
		0xe2, 0xfe, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x0e, 0x00, 0x00, 0x00, 0x12, 0x00, 0x0b, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 'u',
		'k', 'e', 'y'};
#endif
	unsigned char buffer[64];

	/// memset(cb, 0, sizeof(cb));

#if 1
	r = libusb_interrupt_transfer(handle, endpoint_out, cb, sizeof(cb), &size, 1000);
	printf("[%x]%d   send %d bytes\n", endpoint_out, r, size);

	do {
		r = libusb_interrupt_transfer(handle, endpoint_in, buffer, sizeof(buffer), &size, 1000);
		if (!r) for (i = 0; i < size; i++)
			printf("%02x", buffer[i]);
		printf("   received %d bytes\n", size);
	} while(size && j++ < 4);
#else

	/*
	int API_EXPORTED libusb_control_transfer(libusb_device_handle *dev_handle,
         uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
         unsigned char *data, uint16_t wLength, unsigned int timeout)
	*/

	r = libusb_control_transfer(handle, 0x21, 0x09, 0x200, 0, cb, 7, 3000);
	printf("[%x]%d   send %d bytes\n", endpoint_out, r, size);
#endif
}

int main(void)
{
#if 1
	libusb_device *dev;
	libusb_device_handle *usb_handle;
	struct libusb_device_descriptor dev_desc;
	struct libusb_config_descriptor *conf_desc;
	const struct libusb_endpoint_descriptor *endpoint;
	
	int iface, nb_ifaces;
	uint8_t endpoint_in = 0, endpoint_out = 0;
	int i, j, k;
	int r;
	ssize_t cnt;
 
	r = libusb_init(NULL);
	if (r < 0)
		return r;
 
	usb_handle = libusb_open_device_with_vid_pid(NULL, VID, PID);
	dev = libusb_get_device(usb_handle);

	printf("\nReading device descriptor:\n");
	libusb_get_device_descriptor(dev, &dev_desc);
	printf("            length: %d\n", dev_desc.bLength);
	printf("      device class: %d\n", dev_desc.bDeviceClass);
	printf("               S/N: %d\n", dev_desc.iSerialNumber);
	printf("           VID:PID: %04X:%04X\n", dev_desc.idVendor, dev_desc.idProduct);
	printf("         bcdDevice: %04X\n", dev_desc.bcdDevice);
	printf("   iMan:iProd:iSer: %d:%d:%d\n", dev_desc.iManufacturer, dev_desc.iProduct, dev_desc.iSerialNumber);
	printf("          nb confs: %d\n", dev_desc.bNumConfigurations);

	printf("\nReading first configuration descriptor:\n");
	libusb_get_config_descriptor(dev, 0, &conf_desc);
	nb_ifaces = conf_desc->bNumInterfaces;
	printf("             nb interfaces: %d\n", nb_ifaces);

	for (i=0; i<nb_ifaces; i++) {
		printf("              interface[%d]: id = %d\n", i,
			conf_desc->interface[i].altsetting[0].bInterfaceNumber);
		for (j=0; j<conf_desc->interface[i].num_altsetting; j++) {
			printf("interface[%d].altsetting[%d]: num endpoints = %d\n",
				i, j, conf_desc->interface[i].altsetting[j].bNumEndpoints);
			printf("   Class.SubClass.Protocol: %02X.%02X.%02X\n",
				conf_desc->interface[i].altsetting[j].bInterfaceClass,
				conf_desc->interface[i].altsetting[j].bInterfaceSubClass,
				conf_desc->interface[i].altsetting[j].bInterfaceProtocol);

			for (k=0; k<conf_desc->interface[i].altsetting[j].bNumEndpoints; k++) {
				endpoint = &conf_desc->interface[i].altsetting[j].endpoint[k];
				printf("       endpoint[%d].address: %02X\n", k, endpoint->bEndpointAddress);

				// Use the first interrupt or bulk IN/OUT endpoints as default for testing
				if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK | LIBUSB_TRANSFER_TYPE_INTERRUPT)) {
					if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
						if (!endpoint_in)
							endpoint_in = endpoint->bEndpointAddress;
					} else {
						if (!endpoint_out)
							endpoint_out = endpoint->bEndpointAddress;
					}
				}
				printf("           max packet size: %04X\n", endpoint->wMaxPacketSize);
				printf("          polling interval: %02X\n", endpoint->bInterval);
			}
		}
	}
	libusb_free_config_descriptor(conf_desc);
	libusb_set_auto_detach_kernel_driver(usb_handle, 1);

	for (iface = 0; iface < nb_ifaces; iface++) {
		printf("\nClaiming interface %d...\n", iface);
		r = libusb_claim_interface(usb_handle, iface);
		if (r != LIBUSB_SUCCESS) {
			printf("   Failed.\n");
		}
	}

	
	
	 test_key(usb_handle, endpoint_in, endpoint_out);
	 test_hid(usb_handle, endpoint_in);
	/*
	struct skf_dev skf_dev = {
		.handle = usb_handle,
		.ep_in = endpoint_in, 
		.ep_out = endpoint_out};
	skf_setlabel(&skf_dev, "xoxo");

	for (iface = 0; iface<nb_ifaces; iface++) {
		printf("Releasing interface %d...\n", iface);
		libusb_release_interface(usb_handle, iface);
	}
	*/

	libusb_close(usb_handle);
	libusb_exit(NULL);
#else
	
	int r;
	void *dev;
	struct usb_device_id usb_device_id_tables[] = {
		USB_DEVICE_ID(VID, PID),
		{},
	};
	r = skf_connectdev(usb_device_id_tables, &dev);
	skf_setlabel(dev, "xoox");
	skf_disconnectdev(dev);
#endif
	return 0;
}
