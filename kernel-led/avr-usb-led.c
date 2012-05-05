#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/leds.h>
#include <linux/workqueue.h>

#define AVR_LED_VENDOR_ID	0x16c0
#define AVR_LED_PRODUCT_ID	0x05dc
#define AVR_LED_MANUFACTURER	"www.fabiobaltieri.com"
#define AVR_LED_PRODUCT		"IR-Trigger"

#define CUSTOM_RQ_R_ON		3
#define CUSTOM_RQ_R_BLINK	4
#define CUSTOM_RQ_OFF		5

static const struct usb_device_id id_table[] = {
	{ USB_DEVICE(AVR_LED_VENDOR_ID, AVR_LED_PRODUCT_ID) },
	{ },
};
MODULE_DEVICE_TABLE(usb, id_table);

struct avr_led {
	struct usb_device	*udev;
	struct led_classdev     cdev;
	struct work_struct	work;
	enum led_brightness	brightness;
};

static void avr_led_work(struct work_struct *work)
{
	struct avr_led *dev =
		container_of(work, struct avr_led, work);
	int err;

	if (dev->brightness == 0) {
		err = usb_control_msg(dev->udev,
				      usb_sndctrlpipe(dev->udev, 0),
				      CUSTOM_RQ_OFF,
				      USB_TYPE_VENDOR | USB_DIR_OUT,
				      0, 0,
				      NULL, 0,
				      1000);
		if (err < 0)
			dev_err(&dev->udev->dev,
				"%s: error sending control request: %d\n",
				__func__, err);
	} else {
		err = usb_control_msg(dev->udev,
				      usb_sndctrlpipe(dev->udev, 0),
				      CUSTOM_RQ_R_ON,
				      USB_TYPE_VENDOR | USB_DIR_OUT,
				      0, 0,
				      NULL, 0,
				      1000);
		if (err < 0)
			dev_err(&dev->udev->dev,
				"%s: error sending control request: %d\n",
				__func__, err);
	}
}

static void avr_led_set(struct led_classdev *led_cdev,
			enum led_brightness brightness)
{
	struct avr_led *dev =
		container_of(led_cdev, struct avr_led, cdev);

	dev->brightness = brightness;

	schedule_work(&dev->work);
}


static int avr_led_probe(struct usb_interface *interface,
			 const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct avr_led *dev;
	int err;

	if (strcmp(udev->manufacturer, AVR_LED_MANUFACTURER))
		return -ENODEV;
	if (strcmp(udev->product, AVR_LED_PRODUCT))
		return -ENODEV;

	dev = kzalloc(sizeof(struct avr_led), GFP_KERNEL);
	if (dev == NULL) {
		dev_err(&interface->dev, "kzalloc failed\n");
		err = -ENOMEM;
		goto error;
	}

	dev->udev = usb_get_dev(udev);
	usb_set_intfdata(interface, dev);

	err = usb_control_msg(dev->udev,
			usb_sndctrlpipe(dev->udev, 0),
			CUSTOM_RQ_OFF, USB_TYPE_VENDOR | USB_DIR_OUT,
			0, 0,
			NULL, 0,
			1000);
	if (err < 0)
		goto error_create_file;

	dev->cdev.name = kasprintf(GFP_KERNEL, "%s-%s:r:avr-led",
				   dev_driver_string(&dev->udev->dev),
				   dev_name(&dev->udev->dev));
	dev->cdev.brightness_set = avr_led_set;
	dev->cdev.brightness = LED_OFF;
	dev->cdev.max_brightness = 1;

	INIT_WORK(&dev->work, avr_led_work);

	err = led_classdev_register(&dev->udev->dev, &dev->cdev);
	if (err < 0)
		goto error_create_file;

	return 0;

error_create_file:
	usb_put_dev(udev);
	usb_set_intfdata(interface, NULL);
error:
	kfree(dev);
	return err;
}

static void avr_led_disconnect(struct usb_interface *interface)
{
	struct avr_led *dev;

	dev = usb_get_intfdata(interface);

	led_classdev_unregister(&dev->cdev);

	cancel_work_sync(&dev->work);

	usb_set_intfdata(interface, NULL);
	usb_put_dev(dev->udev);

	kfree(dev->cdev.name);
	kfree(dev);
}

static struct usb_driver avr_led_driver = {
	.name =		"avr-micro-usb",
	.probe =	avr_led_probe,
	.disconnect =	avr_led_disconnect,
	.id_table =	id_table,
};

module_usb_driver(avr_led_driver);

MODULE_AUTHOR("Fabio Baltieri");
MODULE_DESCRIPTION("LED driver for avr-micro-usb");
MODULE_LICENSE("GPL");
