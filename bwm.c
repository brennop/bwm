#include <X11/Xlib.h>
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Display *display;
XButtonEvent button_event;
XWindowAttributes window_attributes;

void motion_notify(XMotionEvent e) {
  int dx = e.x_root - button_event.x_root;
  int dy = e.y_root - button_event.y_root;

  XMoveResizeWindow(
      display, button_event.subwindow,
      window_attributes.x + (button_event.button == 1 ? dx : 0),
      window_attributes.y + (button_event.button == 1 ? dy : 0),
      MAX(1, window_attributes.width + (button_event.button == 3 ? dx : 0)),
      MAX(1, window_attributes.height + (button_event.button == 3 ? dy : 0)));
}

void button_press(XButtonEvent e) {
  XGetWindowAttributes(display, e.subwindow, &window_attributes);
  button_event = e;
  XRaiseWindow(display, e.subwindow);
}

void configure_request(XConfigureRequestEvent e) {
  XWindowChanges changes = {.x = e.x,
                            .y = e.y,
                            .width = e.width,
                            .height = e.height,
                            .border_width = e.border_width,
                            .sibling = e.above,
                            .stack_mode = e.detail};

  XConfigureWindow(display, e.window, e.value_mask, &changes);
}

void map_request(XMapRequestEvent e) {
  XSelectInput(display, e.window, StructureNotifyMask | EnterWindowMask);
  XMapWindow(display, e.window);
}

static int xerror() { return 0; }

int main() {
  XEvent event;
  display = XOpenDisplay(NULL);

  if (display == NULL)
    return 1;

  Window root = DefaultRootWindow(display);

  XSetErrorHandler(xerror);
  XSelectInput(display, root, SubstructureRedirectMask);

  XGrabButton(display, 1, Mod1Mask, root, True,
              ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
              GrabModeAsync, GrabModeAsync, None, None);
  XGrabButton(display, 3, Mod1Mask, root, True,
              ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
              GrabModeAsync, GrabModeAsync, None, None);

  for (;;) {
    XNextEvent(display, &event);
    switch (event.type) {
    case CreateNotify:
      break;
    case ConfigureRequest:
      configure_request(event.xconfigurerequest);
      break;
    case MapRequest:
      map_request(event.xmaprequest);
      break;
    case ButtonPress:
      button_press(event.xbutton);
      break;
    case MotionNotify:
      motion_notify(event.xmotion);
      break;
    default:
      break;
    }
  }

  // XCloseDisplay(display);
  return 0;
}
