#include "board.h"
#include "controller.h"
#include "ui.h"
#include <gtk/gtk.h>

int keyboard_state = 0xF;
extern double ws2812_red, ws2812_green, ws2812_blue;
static GtkWidget *darea;

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
  for (int y = 0; y < LCD_HWHEIGHT; y++)
  {
    for (int x = 0; x < LCD_HWWIDTH; x++)
    {
      if (get_lcd_buffer_bit(x, y))
        cairo_rectangle (cr, x * ZOOM + ZOOM/2 , y * ZOOM + ZOOM/2, ZOOM, ZOOM);
    }
  }
  cairo_fill(cr);
}

static void
draw2_cb (GtkDrawingArea *drawing_area,
         cairo_t        *cr,
         int             width,
         int             height,
         gpointer        data)
{
  cairo_set_source_rgb (cr, ws2812_red, ws2812_green, ws2812_blue);
  cairo_paint (cr);
}

void redraw_ws2812(void)
{
  gtk_widget_queue_draw(darea);
}

static void
close_window (void)
{
}

static gboolean time_handler(GtkWidget *widget)
{

  if (widget == NULL) return FALSE;

  unsigned int keyboard_status = get_keyboard_status();
  unsigned int temperature = get_temperature();
  Process_Timer_Event(keyboard_status, temperature, update_status(temperature));

  gtk_widget_queue_draw(widget);

  return TRUE;
}

static void
upEvent (GtkWidget *widget,
             gpointer   data)
{
  keyboard_state = KB_UP;
}

static void
downEvent (GtkWidget *widget,
    gpointer   data)
{
  keyboard_state = KB_DOWN;
}

static void
startEvent (GtkWidget *widget,
    gpointer   data)
{
  keyboard_state = KB_START;
}

static void
stopEvent (GtkWidget *widget,
    gpointer   data)
{
  keyboard_state = KB_STOP;
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  //GtkWidget *frame;
  GtkWidget *hbox, *vbox;
  GtkWidget *button;
  GtkWidget *drawing_area;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Oven control");
  gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
  gtk_widget_set_size_request (window, ZOOM * LCD_HWWIDTH + 100, ZOOM * LCD_HWHEIGHT);

  g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_window_set_child (GTK_WINDOW (window), hbox);

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, ZOOM * LCD_HWWIDTH, ZOOM * LCD_HWHEIGHT);

  gtk_box_append (GTK_BOX (hbox), drawing_area);

  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (drawing_area), draw_cb, NULL, NULL);

  darea = gtk_drawing_area_new ();
  gtk_widget_set_size_request (darea, 100, 100);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (darea), draw2_cb, NULL, NULL);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append (GTK_BOX (hbox), vbox);
  gtk_widget_set_halign (vbox, GTK_ALIGN_END);
  gtk_widget_set_valign (vbox, GTK_ALIGN_END);

  gtk_box_append (GTK_BOX (vbox), darea);
  button = gtk_button_new_with_label ("UP");
  g_signal_connect (button, "clicked", G_CALLBACK (upEvent), NULL);
  gtk_box_append (GTK_BOX (vbox), button);
  button = gtk_button_new_with_label ("DOWN");
  g_signal_connect (button, "clicked", G_CALLBACK (downEvent), NULL);
  gtk_box_append (GTK_BOX (vbox), button);
  button = gtk_button_new_with_label ("START");
  g_signal_connect (button, "clicked", G_CALLBACK (startEvent), NULL);
  gtk_box_append (GTK_BOX (vbox), button);
  button = gtk_button_new_with_label ("STOP");
  g_signal_connect (button, "clicked", G_CALLBACK (stopEvent), NULL);
  gtk_box_append (GTK_BOX (vbox), button);

  gtk_window_present (GTK_WINDOW (window));

  WS2812Init();
  controller_init();
  UI_Init();

  g_timeout_add(500, (GSourceFunc) time_handler, (gpointer) drawing_area);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("oven.control", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
