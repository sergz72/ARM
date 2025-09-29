#include "board.h"
#include "ui.h"
#include <gtk/gtk.h>
#include "multimeter.h"

#define LED_SIZE 40

int long_delay = 0;
unsigned int long_press = 0;
unsigned int key = 0;
int led_state = 0;

static GtkWidget *led_area;

static void
draw_cb (GtkDrawingArea *drawing_area,
         cairo_t        *cr,
         int             width,
         int             height,
         gpointer        data)
{
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_paint (cr);
  cairo_set_source_rgb (cr, 1, 1, 1);
  for (int y = 0; y < LCD_HEIGHT; y++)
  {
    for (int x = 0; x < LCD_WIDTH; x++)
    {
      if (get_lcd_buffer_bit(x, y))
        cairo_rectangle (cr, x * ZOOM + ZOOM/2 , y * ZOOM + ZOOM/2, ZOOM, ZOOM);
    }
  }
  cairo_fill(cr);
}

static void
draw_leds_cb (GtkDrawingArea *drawing_area,
         cairo_t        *cr,
         int             width,
         int             height,
         gpointer        data)
{
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  if (led_state)
  {
    cairo_set_source_rgb (cr, 0, 0, 1);
    cairo_rectangle (cr, 0, 0, LED_SIZE, LED_SIZE);
    cairo_fill(cr);
  }
}

static void
close_window (void)
{
}

static gboolean time_handler(GtkWidget *widget)
{

  if (widget == NULL) return FALSE;

  unsigned char keyboard_status = get_keyboard_status();
  unsigned int multimeter_changes = multimeter_timer_event();
  Process_Timer_Event(keyboard_status, multimeter_changes);

  gtk_widget_queue_draw(widget);
  gtk_widget_queue_draw(led_area);

  return TRUE;
}

static void
modeEvent (GtkWidget *widget,
             gpointer   data)
{
  long_delay = 2;
  key = KB_MODE;
}

static void
button1Event (GtkWidget *widget,
    gpointer   data)
{
  long_delay = 2;
  key = KB_BUTTON1;
}

static void
button2Event (GtkWidget *widget,
           gpointer   data)
{
  long_delay = 2;
  key = KB_BUTTON2;
}

static void
button3Event (GtkWidget *widget,
             gpointer   data)
{
  long_delay = 2;
  key = KB_BUTTON3;
}

static void
modeLongEvent (GtkWidget *widget,
           gpointer   data)
{
  long_press = KB_MODE_LONG;
}

static void
button1LongEvent (GtkWidget *widget,
           gpointer   data)
{
  long_press = KB_BUTTON1_LONG;
}

static void
button2LongEvent (GtkWidget *widget,
           gpointer   data)
{
  long_press = KB_BUTTON2_LONG;
}

static void
button3LongEvent (GtkWidget *widget,
           gpointer   data)
{
  long_press = KB_BUTTON3_LONG;
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *hbox, *vbox, *vbox_up, *vbox_down;
  GtkWidget *button;
  GtkWidget *drawing_area;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Charger/discharger");
  gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
  gtk_widget_set_size_request (window, ZOOM * LCD_WIDTH + 100, ZOOM * LCD_HEIGHT);

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_window_set_child (GTK_WINDOW (window), hbox);

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, ZOOM * LCD_WIDTH, ZOOM * LCD_HEIGHT);

  gtk_box_append (GTK_BOX (hbox), drawing_area);

  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (drawing_area), draw_cb, NULL, NULL);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (hbox), vbox);

  vbox_up = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (vbox), vbox_up);

  led_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (led_area, LED_SIZE * 2, LED_SIZE * 2);
  gtk_box_append (GTK_BOX (vbox_up), led_area);
  gtk_widget_set_vexpand (vbox_up, true);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (led_area), draw_leds_cb, NULL, NULL);

  vbox_down = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (vbox), vbox_down);
  gtk_widget_set_hexpand (vbox_down, true);
  gtk_widget_set_valign (vbox_down, GTK_ALIGN_END);

  GtkGesture *gesture = gtk_gesture_long_press_new ();

  button = gtk_button_new_with_label ("MODE");
  g_signal_connect (button, "clicked", G_CALLBACK (modeEvent), NULL);
  g_signal_connect (gesture, "pressed", G_CALLBACK (modeLongEvent), NULL);
  if (gtk_button_get_child (GTK_BUTTON (button)))
    gtk_widget_add_controller (gtk_button_get_child (GTK_BUTTON (button)), GTK_EVENT_CONTROLLER (gesture));
  else
    gtk_widget_add_controller (button, GTK_EVENT_CONTROLLER (gesture));
  gtk_box_append (GTK_BOX (vbox_down), button);

  gesture = gtk_gesture_long_press_new ();

  button = gtk_button_new_with_label ("1");
  g_signal_connect (button, "clicked", G_CALLBACK (button1Event), NULL);
  g_signal_connect (gesture, "pressed", G_CALLBACK (button1LongEvent), NULL);
  if (gtk_button_get_child (GTK_BUTTON (button)))
    gtk_widget_add_controller (gtk_button_get_child (GTK_BUTTON (button)), GTK_EVENT_CONTROLLER (gesture));
  else
    gtk_widget_add_controller (button, GTK_EVENT_CONTROLLER (gesture));
  gtk_box_append (GTK_BOX (vbox_down), button);

  gesture = gtk_gesture_long_press_new ();

  button = gtk_button_new_with_label ("2");
  g_signal_connect (button, "clicked", G_CALLBACK (button2Event), NULL);
  g_signal_connect (gesture, "pressed", G_CALLBACK (button2LongEvent), NULL);
  if (gtk_button_get_child (GTK_BUTTON (button)))
    gtk_widget_add_controller (gtk_button_get_child (GTK_BUTTON (button)), GTK_EVENT_CONTROLLER (gesture));
  else
    gtk_widget_add_controller (button, GTK_EVENT_CONTROLLER (gesture));
  gtk_box_append (GTK_BOX (vbox_down), button);

  gesture = gtk_gesture_long_press_new ();

  button = gtk_button_new_with_label ("3");
  g_signal_connect (button, "clicked", G_CALLBACK (button3Event), NULL);
  g_signal_connect (gesture, "pressed", G_CALLBACK (button3LongEvent), NULL);
  if (gtk_button_get_child (GTK_BUTTON (button)))
    gtk_widget_add_controller (gtk_button_get_child (GTK_BUTTON (button)), GTK_EVENT_CONTROLLER (gesture));
  else
    gtk_widget_add_controller (button, GTK_EVENT_CONTROLLER (gesture));
  gtk_box_append (GTK_BOX (vbox_down), button);

  gtk_window_present (GTK_WINDOW (window));

  UI_Init();

  g_timeout_add(TIMER_DELAY, (GSourceFunc)time_handler, drawing_area);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("multimeter.ui", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
