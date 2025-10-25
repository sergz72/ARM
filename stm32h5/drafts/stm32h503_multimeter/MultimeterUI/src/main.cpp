#include "board.h"
#include "ui.h"
#include <gtk/gtk.h>
#include <ad7793_emulator.h>
#include <ads1220_emulator.h>
#include "multimeter_init.h"

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
  multimeter.TimerEvent();
  Process_Timer_Event(keyboard_status);

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

static int update_label(GtkWidget *widget, gpointer data)
{
  char text[10];
  GtkWidget *value_label = (GtkWidget *)data;
  int value = (int)gtk_range_get_value(GTK_RANGE(widget));
  sprintf(text, "%d", value);
  gtk_label_set_label(GTK_LABEL(value_label), text);
  return value;
}

static void
v1_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.voltage_current[0].voltage_uV = value * 1000;
  ad7793_emulator_config[0].ain_uv[1] = value * 1000;
}

static void
i1_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.voltage_current[0].current_nA = value * 1000;
  ad7793_emulator_config[0].ain_uv[0] = value * 1000;
}

static void
v2_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.voltage_current[1].voltage_uV = value * 1000;
}

static void
i2_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.voltage_current[1].current_nA = value * 1000;
}

static void
f_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.frequency_hz = value;
}

static void
c_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.capacitance.pF = value;
}

static void
r1_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.resistance_mOhm[0] = value;
}

static void
r2_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  //multimeter_result_hal.resistance_mOhm[1] = value;
}

static void
temp_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  ads1220_emulator_config[0].temperature = value;
}

static void
vdda_changed (GtkWidget *widget,
              gpointer   data)
{
  int value = update_label(widget, data);
  ad7793_emulator_config[0].vdda_mv = value;
}

static GtkWidget*
create_drawing_area(GtkWidget *hbox)
{
  GtkWidget *drawing_area;

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, ZOOM * LCD_WIDTH, ZOOM * LCD_HEIGHT);
  gtk_box_append (GTK_BOX (hbox), drawing_area);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (drawing_area), draw_cb, NULL, NULL);

  return drawing_area;
}

static void
create_led_area(GtkWidget *vbox_up)
{
  led_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (led_area, LED_SIZE * 2, LED_SIZE * 2);
  gtk_box_append (GTK_BOX (vbox_up), led_area);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (led_area), draw_leds_cb, NULL, NULL);
}

static void
create_buttons(GtkWidget *vbox_down)
{
  GtkWidget *button;

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
}

static void
create_scale(GtkWidget *vbox, const char *name, const char *unit, int lower, int upper, GCallback callback)
{
  GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append (GTK_BOX (vbox), hbox);

  GtkAdjustment *hadjustment = gtk_adjustment_new (0, lower, upper, 1, 10, 0);
  GtkWidget *label = gtk_label_new (name);
  gtk_box_append (GTK_BOX (hbox), label);
  GtkWidget *value_label = gtk_label_new ("0");
  gtk_widget_set_size_request (value_label, 100, 20);
  gtk_box_append (GTK_BOX (hbox), value_label);
  label = gtk_label_new (unit);
  gtk_box_append (GTK_BOX (hbox), label);
  GtkWidget *scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, hadjustment);
  gtk_widget_set_hexpand(scale, TRUE);
  gtk_widget_set_halign (scale, GTK_ALIGN_FILL);
  g_signal_connect(scale, "value-changed", callback, value_label);
  gtk_box_append (GTK_BOX (hbox), scale);
}

static void
create_sliders(GtkWidget *vbox)
{
  create_scale(vbox, "TEMP", "C", 180, 250, G_CALLBACK(temp_changed));
  create_scale(vbox, "VDDA", "mV", 3000, 3500, G_CALLBACK(vdda_changed));
  create_scale(vbox, "V1", "mV", -15000, 15000, G_CALLBACK(v1_changed));
  create_scale(vbox, "I1", "uA", -999999, 999999, G_CALLBACK(i1_changed));
  create_scale(vbox, "V2", "mV", -15000, 15000, G_CALLBACK(v2_changed));
  create_scale(vbox, "I2", "uA", -999999, 999999, G_CALLBACK(i2_changed));
  create_scale(vbox, "F", "Hz", 0, 199999999, G_CALLBACK(f_changed));
  create_scale(vbox, "C", "tick", 0, 1999999999, G_CALLBACK(c_changed));
  create_scale(vbox, "R1", "mOhm", 0, 999999999, G_CALLBACK(r1_changed));
  create_scale(vbox, "R2", "mOhm", 0, 999999999, G_CALLBACK(r2_changed));
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *main_box, *hbox, *vbox, *vbox_up, *vbox_down;
  GtkWidget *drawing_area;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Multimeter UI");
  gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
  gtk_widget_set_size_request (window, ZOOM * LCD_WIDTH + 100, ZOOM * LCD_HEIGHT + 200);

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child (GTK_WINDOW (window), main_box);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append (GTK_BOX (main_box), hbox);

  drawing_area = create_drawing_area(hbox);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (hbox), vbox);

  vbox_up = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (vbox), vbox_up);

  create_led_area(vbox_up);

  vbox_down = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (vbox), vbox_down);
  gtk_widget_set_hexpand (vbox_down, true);
  gtk_widget_set_valign (vbox_down, GTK_ALIGN_END);

  create_buttons(vbox_down);

  gtk_window_present (GTK_WINDOW (window));

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (main_box), vbox);
  gtk_widget_set_vexpand (vbox, true);

  create_sliders(vbox);

  ads1220_emulator_init();
  ad7793_emulator_init();
  ads1220_emulator_config[0].vdda_mv = 3300;
  ads1220_emulator_config[0].temperature = 220;
  ad7793_emulator_config[0].vdda_mv = 3300;

  multimeter_init();

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
